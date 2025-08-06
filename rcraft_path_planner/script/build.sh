if [ -z "$BUILD_TYPE" ]; then
  BUILD_TYPE=Debug
fi

echo "[INFO] Build type: $BUILD_TYPE"
echo "[INFO] Target: ${TARGET:-<ALL>}"

rm -rf build
cmake -B build -S . -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_INSTALL_PREFIX=../rcraft_libs
cmake --build build --target install

if [ -z "$TARGET" ]; then
  cmake --build build
else
  cmake --build build --target "$TARGET"
fi
