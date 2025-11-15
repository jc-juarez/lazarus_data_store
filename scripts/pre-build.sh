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

BUILD_IMAGE_NAME_AMD64="build-ubuntu-lazarus-amd64"
BUILD_IMAGE_NAME_ARM64="build-ubuntu-lazarus-arm64"
BUILD_DOCKERFILE="build.dockerfile"

echo "[PRE-BUILD] Checking for Docker build image: ${BUILD_IMAGE_NAME_AMD64}"
if ! docker image inspect "$BUILD_IMAGE_NAME_AMD64" >/dev/null 2>&1; then
    echo "[PRE-BUILD] Image not found — building it now..."
    docker buildx build --platform linux/amd64 --no-cache -t "$BUILD_IMAGE_NAME_AMD64" -f "$BUILD_DOCKERFILE" .
    echo "[PRE-BUILD] Build image created successfully."
else
    echo "[PRE-BUILD] Build image already exists."
fi

echo "[PRE-BUILD] Checking for Docker build image: ${BUILD_IMAGE_NAME_ARM64}"
if ! docker image inspect "$BUILD_IMAGE_NAME_ARM64" >/dev/null 2>&1; then
    echo "[PRE-BUILD] Image not found — building it now..."
    docker buildx build --platform linux/arm64 --no-cache -t "$BUILD_IMAGE_NAME_ARM64" -f "$BUILD_DOCKERFILE" .
    echo "[PRE-BUILD] Build image created successfully."
else
    echo "[PRE-BUILD] Build image already exists."
fi