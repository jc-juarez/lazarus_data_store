# ****************************************************
# Lazarus Data Store
# Scripts
# 'runtime-arm64.dockerfile'
# Author: jcjuarez
# Description:
#      ARM64 Ubuntu base image for running lazarus.
# ****************************************************

FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    libboost-system1.83.0 \
    libtbb12 \
    libprotobuf32 \
    libspdlog1.12 \
    librocksdb-dev \
    libjsoncpp25 \
    libssl3 \
    libc-ares2 \
    libuuid1 \
    nlohmann-json3-dev \
    file \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY cmake-build-docker-debug-arm64/lazarus /usr/local/bin/lazarus
COPY config.json /app/config.json

RUN useradd -ms /bin/bash runtime
USER runtime
WORKDIR /home/runtime

ENTRYPOINT ["/usr/local/bin/lazarus", "/app/config.json"]