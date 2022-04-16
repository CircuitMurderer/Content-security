cmake_minimum_required(VERSION 3.20)

set(CMAKE_MAKE_PROGRAMC "D:/Program Files/mingw-w64/bin/mingw32-make.exe")
set(CMAKE_C_COMPILER "D:/Program Files/mingw-w64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "D:/Program Files/mingw-w64/bin/g++.exe")

project(imgDetect)

set(CMAKE_CXX_STANDARD 14)

set(OpenCV_DIR "D:/opencv_inst/opencv/mingw-build/install/x64/mingw/lib")
find_package(OpenCV REQUIRED)
add_executable(imgDetect main.cpp utils/person.cpp utils/person.h utils/affine.cpp utils/affine.h utils/hist.cpp utils/hist.h utils/utils.h utils/sift.cpp utils/sift.h utils/yolo.cpp utils/yolo.h)

target_link_libraries(imgDetect ${OpenCV_LIBS})
