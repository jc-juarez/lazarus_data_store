# ****************************************************
# PandoraDB
# Scripts
# 'post-build.sh'
# Author: jcjuarez
# Description:
#      Script for building the runtime docker image.
# ****************************************************

#!/usr/bin/env bash
set -euo pipefail

RUNTIME_IMAGE_NAME_AMD64="runtime-ubuntu-pandora-amd64"
RUNTIME_NO_ENTRY_IMAGE_NAME_AMD64="runtime-no-entry-ubuntu-pandora-amd64"
RUNTIME_IMAGE_NAME_ARM64="runtime-ubuntu-pandora-arm64"
RUNTIME_NO_ENTRY_IMAGE_NAME_ARM64="runtime-no-entry-ubuntu-pandora-arm64"
RUNTIME_DOCKERFILE_AMD64="scripts/runtime-amd64.dockerfile"
RUNTIME_NO_ENTRY_DOCKERFILE_AMD64="scripts/runtime-no-entry-amd64.dockerfile"
RUNTIME_DOCKERFILE_ARM64="scripts/runtime-arm64.dockerfile"
RUNTIME_NO_ENTRY_DOCKERFILE_ARM64="scripts/runtime-no-entry-arm64.dockerfile"
BINARY_PATH_AMD64="cmake-build-docker-debug-amd64/pandora"
BINARY_PATH_ARM64="cmake-build-docker-debug-arm64/pandora"

echo "[POST-BUILD] Checking for compiled binary..."
if [[ ! -f "$BINARY_PATH_AMD64" ]]; then
    echo "[ERROR] Binary not found: $BINARY_PATH_AMD64"
    exit 1
fi

echo "[POST-BUILD] Checking for compiled binary..."
if [[ ! -f "$BINARY_PATH_ARM64" ]]; then
    echo "[ERROR] Binary not found: $BINARY_PATH_ARM64"
    exit 1
fi

echo "[POST-BUILD] Building runtime Docker images."
docker buildx build --platform linux/amd64 -f "$RUNTIME_DOCKERFILE_AMD64" -t "$RUNTIME_IMAGE_NAME_AMD64" .
docker buildx build --platform linux/amd64 -f "$RUNTIME_NO_ENTRY_DOCKERFILE_AMD64" -t "$RUNTIME_NO_ENTRY_IMAGE_NAME_AMD64" .
docker buildx build --platform linux/arm64 -f "$RUNTIME_DOCKERFILE_ARM64" -t "$RUNTIME_IMAGE_NAME_ARM64" .
docker buildx build --platform linux/arm64 -f "$RUNTIME_NO_ENTRY_DOCKERFILE_ARM64" -t "$RUNTIME_NO_ENTRY_IMAGE_NAME_ARM64" .
echo "[POST-BUILD] Runtime image built successfully."