# ****************************************************
# Lazarus Data Store
# Scripts
# 'pre-build.sh'
# Author: jcjuarez
# Description:
#      Script for building the build docker image.
# ****************************************************

#!/usr/bin/env bash
set -euo pipefail

BUILD_IMAGE_NAME="build-ubuntu-lazarus"
BUILD_DOCKERFILE="scripts/build.dockerfile"

echo "[PRE-BUILD] Checking for Docker build image: ${BUILD_IMAGE_NAME}"

if ! docker image inspect "$BUILD_IMAGE_NAME" >/dev/null 2>&1; then
    echo "[PRE-BUILD] Image not found — building it now..."
    docker build --no-cache -t "$BUILD_IMAGE_NAME" -f "$BUILD_DOCKERFILE" .
    echo "[PRE-BUILD] Build image created successfully."
else
    echo "[PRE-BUILD] Build image already exists."
fi