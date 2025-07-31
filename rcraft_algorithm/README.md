
# Install OpenCV
## Windows
Clone OpenCV
```bash
cd C:/
git clone https://github.com/opencv/opencv
cd opencv/
```
Open CMake GUI
* Where is the source code
  * C:/opencv
* Where to build the binaries
  * C:/opencv/build
* Specify the generator for this project
  * MinGW Makefiles
* Configure
  * OPENCV_ENABLE_ALLOCATOR_STATS <- Except this Checkbox
* Generate

Build
```bash
cd C:/opencv/build
mingw32-make
mingw32-make install
```

Add build/bin into the System Path
* C:/opencv/build/bin