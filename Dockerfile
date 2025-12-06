FROM ubuntu:24.04

# Set noninteractive frontend
ENV DEBIAN_FRONTEND=noninteractive

# Install system dependencies
RUN apt update && apt install -y \
    build-essential \
    cmake \
    gcc \
    g++ \
    libjsoncpp-dev \
    uuid-dev \
    zlib1g-dev \
    openssl \
    libssl-dev \
    git \
    wget \
    curl \
    unzip \
    tar \
    net-tools \
    iputils-ping \
    pkg-config \
    libbson-dev \
    libmongoc-dev \
    libboost-all-dev \
    ca-certificates \
    nlohmann-json3-dev && \
    apt clean && \
    rm -rf /var/lib/apt/lists/*

# Permanently set LD_LIBRARY_PATH
ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# Set working directory
WORKDIR /home/ccsds_source

# Clone and build Drogon
RUN git clone https://github.com/drogonframework/drogon && \
    cd drogon && \
    git submodule update --init && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc) && \
    make install

# Download, build, and install MongoDB C++ Driver
RUN curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r4.1.0/mongo-cxx-driver-r4.1.0.tar.gz && \
    tar -xzf mongo-cxx-driver-r4.1.0.tar.gz && \
    mkdir -p mongo-cxx-driver-r3.10.1/build && \
    cd mongo-cxx-driver-r4.1.0/build && \
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DBSONCXX_POLY_USE_BOOST=1 \
        -DMONGOCXX_OVERRIDE_DEFAULT_INSTALL_PREFIX=OFF && \
    cmake --build . && \
    cmake --build . --target install 

RUN cp -r /usr/local/include/mongocxx/v_noabi/mongocxx /usr/local/include/
RUN cp -r /usr/local/include/bsoncxx/v_noabi/bsoncxx /usr/local/include

COPY . /home/ccsds_source/ccsds_final_project


RUN cd ccsds_final_project && \
    cd ws && \
    mkdir -p build && \
    cd build && \
    mkdir -p uploads && \
    cmake .. && \
    make -j$(nproc)


CMD ["/home/ccsds_source/ccsds_final_project/ws/build/final-project"]
