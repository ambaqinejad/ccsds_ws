//
// Created by ambaqinejad on 4/19/25.
//

#include "CCSDSPacketFileHelper.h"
#include <iostream>
#include <fstream>

#include "helpers/ClientCommunicationHelper.h"
#include <drogon/drogon.h>
#include <chrono>
#include "database/MongoDBHandler.h"

using namespace std::chrono;


using namespace std;

void CCSDSPacketFileHelper::processFile(const string &filePath, const std::string &fileUUID) {
    ifstream file(filePath, ios::binary);
    if (!file) {
        LOG_INFO << "Error opening file!\n";
        return;
    }
    file.seekg(0, ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, ios::beg);

    // Allocate buffer and read data
    std::vector<unsigned char> buffer(fileSize);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        ClientCommunicationHelper::notifyClients(-1);
        LOG_INFO << "Error reading file!\n";
        return;
    }

    int count_of_valid_chunks = 0;
    std::vector<std::vector<uint8_t>> chunks;
    for (size_t i = 0; i < buffer.size() - 1; ++i) {
        if (buffer[i] == 0x48 && buffer[i + 1] == 0x48) {
            size_t j = 0;
            bool is_chunked_in_inner_for = false;
            for (j = i+2; j < buffer.size() - 1; ++j) {
                if (buffer[j] == 0x48 && buffer[j + 1] == 0x48) {
                    if (j - i == 128) {
                        vector<uint8_t> chunk;
                        chunk.insert(chunk.begin(), buffer.begin() + i, buffer.begin() + j);
                        chunks.push_back(chunk);
                        count_of_valid_chunks ++;
                        is_chunked_in_inner_for = true;
                        i = j - 1;
                    }
                    break;
                }
            }
            if (j - i == 127 && !is_chunked_in_inner_for) {
                vector<uint8_t> chunk;
                chunk.insert(chunk.begin(), buffer.begin() + i, buffer.begin() + j);
                chunks.push_back(chunk);
                count_of_valid_chunks ++;
                i = j - 1;
            }
        }
    }
    parseData(chunks, count_of_valid_chunks, fileUUID);
}

std::map<std::string, std::vector<CCSDS_Packet>> CCSDSPacketFileHelper::uuidToSavedPacketsMapper;
std::map<std::string, std::set<uint8_t >> CCSDSPacketFileHelper::uuidToSids;
void CCSDSPacketFileHelper::parseData(std::vector<std::vector<uint8_t>> chunks, int count_of_valid_chunks, const std::string &fileUUID) {
    const size_t BATCH_SIZE = 50000; // Process 50K packets at a time
    int eachTimeNotifyClients = count_of_valid_chunks / ClientCommunicationHelper::progressDivider != 0 ?
                                count_of_valid_chunks / ClientCommunicationHelper::progressDivider : 1;

    LOG_INFO << "Starting batch processing of " << chunks.size() << " chunks";

    // This will hold ALL packets in memory eventually
    std::vector<CCSDS_Packet> allPackets;
    allPackets.reserve(chunks.size()); // Reserve space for ALL packets upfront

    for (size_t start = 0; start < chunks.size(); start += BATCH_SIZE) {
        size_t end = std::min(start + BATCH_SIZE, chunks.size());
        size_t currentBatchSize = end - start;

        LOG_INFO << "Processing batch " << (start / BATCH_SIZE) << " (" << start << " to " << end << ")";

        // Temporary batch vector
        std::vector<CCSDS_Packet> batch;
        batch.reserve(currentBatchSize);

        // Process current batch
        for (size_t i = start; i < end; ++i) {
            auto startTime = high_resolution_clock::now();
            CCSDS_Packet packet = packet.deserialize_packet(chunks[i]);
            auto endTime = high_resolution_clock::now();
            auto duration_us = duration_cast<microseconds>(endTime - startTime);

            batch.push_back(packet);
            CCSDSPacketFileHelper::uuidToSids[fileUUID].insert(packet.sid);

            // Progress notification
            if (i % eachTimeNotifyClients == 0) {
                int progress = std::ceil(((double)(i+1) / (double)count_of_valid_chunks) * 100);
                ClientCommunicationHelper::notifyClients(progress);
            }
        }

        // Move the batch into the main collection
        allPackets.insert(allPackets.end(),
                          std::make_move_iterator(batch.begin()),
                          std::make_move_iterator(batch.end()));

        LOG_INFO << "Batch completed. Total packets in memory: " << allPackets.size();
        LOG_INFO << "All packets capacity: " << allPackets.capacity();

        // Clear the batch to free temporary memory
        batch.clear();
        batch.shrink_to_fit(); // Free the batch memory
    }

    // Now store the complete collection in the global map
    LOG_INFO << "All batches processed. Storing " << allPackets.size() << " packets in global map";

    CCSDSPacketFileHelper::uuidToSavedPacketsMapper[fileUUID] = std::move(allPackets);

    LOG_INFO << "Storage completed. Verified count: "
             << CCSDSPacketFileHelper::uuidToSavedPacketsMapper[fileUUID].size();
}
