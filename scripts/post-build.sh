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

RUNTIME_IMAGE_NAME="runtime-ubuntu-lazarus"
RUNTIME_DOCKERFILE="scripts/runtime.dockerfile"
BINARY_PATH="cmake-build-docker-debug/lazarus"

echo "[POST-BUILD] Checking for compiled binary..."
if [[ ! -f "$BINARY_PATH" ]]; then
    echo "[ERROR] Binary not found: $BINARY_PATH"
    echo "[HINT] Build the project in CLion first."
    exit 1
fi

echo "[POST-BUILD] Building runtime Docker image: ${RUNTIME_IMAGE_NAME}"
docker build -f "$RUNTIME_DOCKERFILE" -t "$RUNTIME_IMAGE_NAME" .
echo "[POST-BUILD] Runtime image built successfully."