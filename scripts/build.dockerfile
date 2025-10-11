# ****************************************************
# Lazarus Data Store
# Scripts
# 'build.dockerfile'
# Author: jcjuarez
# Description:
#      Ubuntu base image for building lazarus.
# ****************************************************

FROM ubuntu:24.04

# Install build essentials + dev libraries.
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    gdb \
    rsync \
    pkg-config \
    git \
    wget \
    curl \
    unzip \
    libboost-all-dev \
    libtbb-dev \
    libprotobuf-dev protobuf-compiler \
    libspdlog-dev \
    librocksdb-dev \
    libjsoncpp-dev \
    libssl-dev \
    libc-ares-dev \
    doxygen \
    uuid-dev \
    nlohmann-json3-dev \
    libjsoncpp-dev \
    && rm -rf /var/lib/apt/lists/*

# Drogon is not in Ubuntu repos, build from source.
RUN git clone --recursive https://github.com/drogonframework/drogon.git /tmp/drogon \
    && cd /tmp/drogon \
    && mkdir build && cd build \
    && cmake .. \
    && make -j$(nproc) \
    && make install \
    && rm -rf /tmp/drogon

# Install the ConcurrentQueue dependency.
RUN git clone https://github.com/cameron314/concurrentqueue.git /tmp/concurrentqueue \
    && mkdir -p /usr/local/include/moodycamel \
    && cp /tmp/concurrentqueue/concurrentqueue.h /usr/local/include/moodycamel/ \
    && cp /tmp/concurrentqueue/blockingconcurrentqueue.h /usr/local/include/moodycamel/ \
    && rm -rf /tmp/concurrentqueue

# Create a non-root user.
RUN useradd -ms /bin/bash dev && echo "dev:dev" | chpasswd
USER dev
WORKDIR /home/dev