# ****************************************************
# Lazarus Data Store
# Scripts
# 'post-build.sh'
# Author: jcjuarez
# Description:
#      Script for building the runtime docker image.
# ****************************************************

#!/usr/bin/env bash
set -euo pipefail

RUNTIME_IMAGE_NAME_AMD64="runtime-ubuntu-lazarus-amd64"
RUNTIME_IMAGE_NAME_ARM64="runtime-ubuntu-lazarus-arm64"
RUNTIME_DOCKERFILE_AMD64="scripts/runtime-amd64.dockerfile"
RUNTIME_DOCKERFILE_ARM64="scripts/runtime-arm64.dockerfile"
BINARY_PATH="cmake-build-docker-debug/lazarus"

echo "[POST-BUILD] Checking for compiled binary..."
if [[ ! -f "$BINARY_PATH" ]]; then
    echo "[ERROR] Binary not found: $BINARY_PATH"
    exit 1
fi

echo "[POST-BUILD] Building runtime Docker images."
docker buildx build --platform linux/amd64 -f "$RUNTIME_DOCKERFILE_AMD64" -t "$RUNTIME_IMAGE_NAME_AMD64" .
docker buildx build --platform linux/arm64 -f "$RUNTIME_DOCKERFILE_ARM64" -t "$RUNTIME_IMAGE_NAME_ARM64" .
echo "[POST-BUILD] Runtime image built successfully."