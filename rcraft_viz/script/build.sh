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

if [ -z "$TARGET" ]; then
  cmake --build build
else
  cmake --build build --target "$TARGET"
fi

/c/Qt/Qt5.12.4/5.12.4/mingw73_64/bin/windeployqt.exe ./build/rcraft_viz.exe