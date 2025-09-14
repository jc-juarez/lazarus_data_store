# ****************************************************
# Lazarus Data Store
# Build
# 'Dockerfile'
# Author: jcjuarez
# Description:
#      Ubuntu base image for running
#      lazarus along the CLion IDE. Build with:
#      `docker build -t clion-lazarus-ubuntu .`
# ****************************************************

FROM ubuntu:22.04

# Install build essentials + dev libraries
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
    libtbb-dev \
    libprotobuf-dev protobuf-compiler \
    libspdlog-dev \
    librocksdb-dev \
    libjsoncpp-dev \
    libssl-dev \
    libc-ares-dev \
    doxygen \
    uuid-dev \
    && rm -rf /var/lib/apt/lists/*

# Drogon is not in Ubuntu repos, build from source
RUN git clone --recursive https://github.com/drogonframework/drogon.git /tmp/drogon \
    && cd /tmp/drogon \
    && mkdir build && cd build \
    && cmake .. \
    && make -j$(nproc) \
    && make install \
    && rm -rf /tmp/drogon

# Create a non-root user CLion can map to
RUN useradd -ms /bin/bash dev && echo "dev:dev" | chpasswd
USER dev
WORKDIR /home/dev