#include "ThreadedParser.h"

#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include "../logics/CCSDS_Packet.h"
#include <chrono>

using namespace std::chrono;

ThreadedParser::ThreadedParser() : _stopParser(false) {
}

ThreadedParser::~ThreadedParser() { {
        std::lock_guard<std::mutex> lock(_bufferMutex);
        _stopParser = true;
    }
    _cv.notify_one();
    if (_parserThread.joinable()) {
        _parserThread.join();
    }
}

void ThreadedParser::start() {
    _stopParser = false;
    _parserThread = std::thread(&ThreadedParser::parsingThreadFunction, this);
}

void ThreadedParser::receiveData(const std::vector<uint8_t> &data) { {
        std::lock_guard<std::mutex> lock(_bufferMutex);
        _activeBuffer.insert(_activeBuffer.end(), data.begin(), data.end());
    }
    _cv.notify_one();
}

void ThreadedParser::parsingThreadFunction() {
    std::vector<uint8_t> leftoverData;

    while (true) {
        std::vector<uint8_t> localBuffer; {
            std::unique_lock<std::mutex> lock(_bufferMutex);
            _cv.wait(lock, [this] { return !_activeBuffer.empty() || _stopParser; });

            if (_stopParser) break;

            if (!leftoverData.empty()) {
                localBuffer.insert(localBuffer.end(), leftoverData.begin(), leftoverData.end());
            }

            localBuffer.insert(localBuffer.end(), _activeBuffer.begin(), _activeBuffer.end());
            _activeBuffer.clear();
        }

        leftoverData = parseData(localBuffer);
    }
}

std::vector<uint8_t> ThreadedParser::parseData(std::vector<uint8_t> &data) {
    size_t i = 0;


    while (i < data.size()) {
        cout << packets.size() << endl;
        if (i != data.size() - 1 && data[i] == 'H' && data[i + 1] == 'H' && i + 128 <= data.size()) {
            std::vector<uint8_t> chunk(data.begin() + i, data.begin() + i + 128);
            auto start = high_resolution_clock::now();
            CCSDS_Packet packet = packet.deserialize_packet(chunk);
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(end - start);
            cout << "deserialize_packet took " << duration.count() << " ms" << endl;
            printPacket(packet);
            packets.push_back(packet);

            data.erase(data.begin(), data.begin() + i + 128);
            i--;
        } else if (i != data.size() - 1 && data[i] == 'H' && data[i + 1] == 'H' && i + 128 > data.size()) {
            data.erase(data.begin(), data.begin() + i);
            return data;
        } else if (i == data.size() - 1 && data[i] == 'H') {
            data.erase(data.begin(), data.begin() + i);
            return data;
        }
        i++;
    }

    return {};
}

void ThreadedParser::printPacket(CCSDS_Packet packet) {
    std::cout << "main frame header = " << std::hex << std::uppercase << std::setfill('0')
            << std::setw(2) << (int) ((packet.main_frame_header >> 8) & 0xFF) << " "
            << std::setw(2) << (int) (packet.main_frame_header & 0xFF) << std::endl;
    std::cout << "packet id = " << std::hex << std::uppercase << std::setfill('0')
            << std::setw(2) << (int) ((packet.packet_id >> 8) & 0xFF) << " "
            << std::setw(2) << (int) (packet.packet_id & 0xFF) << std::endl;
    std::cout << "packet sequence control = " << std::hex << std::uppercase << std::setfill('0')
            << std::setw(2) << (int) ((packet.packet_sequence_control >> 8) & 0xFF) << " "
            << std::setw(2) << (int) (packet.packet_sequence_control & 0xFF) << std::endl;
    std::cout << "packet length = " << std::hex << std::uppercase << std::setfill('0')
            << std::setw(2) << (int) ((packet.packet_length >> 8) & 0xFF) << " "
            << std::setw(2) << (int) (packet.packet_length & 0xFF) << std::endl;
    std::cout << "packet sequence control = " << std::hex << std::uppercase << std::setfill('0')
            << std::setw(2) << (int) ((packet.packet_sequence_control >> 8) & 0xFF) << " "
            << std::setw(2) << (int) (packet.packet_length & 0xFF) << std::endl;
    std::cout << "data field header = " << std::hex << std::uppercase << std::setfill('0')
            << std::setw(2) << (int) (packet.data_field_header & 0xFF) << std::endl;
    std::cout << "service type = " << std::hex << std::uppercase << std::setfill('0')
            << std::setw(2) << (int) (packet.service_type & 0xFF) << std::endl;
    std::cout << "sub service type = " << std::hex << std::uppercase << std::setfill('0')
            << std::setw(2) << (int) (packet.sub_service_type & 0xFF) << std::endl;
    std::cout << "sid = " << std::hex << std::uppercase << std::setfill('0')
            << std::setw(2) << (int) (packet.sub_service_type & 0xFF) << std::endl;
    std::cout << "timestamp = " << std::hex << std::uppercase << std::setfill('0')
            << std::setw(2) << (int) ((packet.timestamp >> 24) & 0xFF) << " "
            << std::setw(2) << (int) ((packet.timestamp >> 16) & 0xFF) << " "
            << std::setw(2) << (int) ((packet.timestamp >> 8) & 0xFF) << " "
            << std::setw(2) << (int) (packet.timestamp & 0xFF) << std::endl;
    std::cout << "crc_fail_upload_map = " << std::hex << std::uppercase << std::setfill('0')
            << std::setw(2) << (int) ((packet.crc_fail_upload_map >> 56) & 0xFF) << " "
            << std::setw(2) << (int) ((packet.crc_fail_upload_map >> 48) & 0xFF) << " "
            << std::setw(2) << (int) ((packet.crc_fail_upload_map >> 40) & 0xFF) << " "
            << std::setw(2) << (int) ((packet.crc_fail_upload_map >> 32) & 0xFF) << " "
            << std::setw(2) << (int) ((packet.crc_fail_upload_map >> 24) & 0xFF) << " "
            << std::setw(2) << (int) ((packet.crc_fail_upload_map >> 16) & 0xFF) << " "
            << std::setw(2) << (int) ((packet.crc_fail_upload_map >> 8) & 0xFF) << " "
            << std::setw(2) << (int) (packet.crc_fail_upload_map & 0xFF) << std::endl;
    std::cout << "flash_address = " << std::hex << std::uppercase << std::setfill('0')
            << std::setw(2) << (int) ((packet.flash_address >> 24) & 0xFF) << " "
            << std::setw(2) << (int) ((packet.flash_address >> 16) & 0xFF) << " "
            << std::setw(2) << (int) ((packet.flash_address >> 8) & 0xFF) << " "
            << std::setw(2) << (int) (packet.flash_address & 0xFF) << std::endl;
    std::cout << "......................................." << std::endl;
}
