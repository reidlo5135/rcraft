#!/bin/bash

TARGET=$1
BUILD_TYPE=$2

if [ -z "$BUILD_TYPE" ]; then
  BUILD_TYPE=Debug
fi

echo "[INFO] Build type: $BUILD_TYPE"
echo "[INFO] Target: ${TARGET:-<ALL>}"

cmake -B build -S . -G "MinGW Makefiles" \
  -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
  -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DVCPKG_TARGET_TRIPLET=x64-windows \
  -DOpenCV_DIR="C:/vcpkg/installed/x64-windows/share/opencv4" \
  -DProtobuf_DIR="C:/vcpkg/installed/x64-windows/share/protobuf" \
  -Dquirc_DIR="C:/vcpkg/installed/x64-windows/share/quirc"


if [ -z "$TARGET" ]; then
  cmake --build build
else
  cmake --build build --target "$TARGET"
fi