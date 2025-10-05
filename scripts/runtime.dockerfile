# Lazarus Data Store - Runtime Dockerfile (Ubuntu 24.04)
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
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY cmake-build-docker-debug/lazarus /usr/local/bin/lazarus

ENTRYPOINT ["/usr/local/bin/lazarus"]