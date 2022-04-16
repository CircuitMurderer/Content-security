# 内容安全-图像处理
## 包含功能

> affine transfer: 仿射变换
> 
> histogram compare: 直方图匹配
>
> person detect: 行人检测
>
> SIFT match: SIFT匹配
> 
> YOLO detect: YOLO目标检测

## 使用指南

第一次使用时，请确保安装了`Mingw(8.1.0)`、`cmake(3.22)`以及`opencv(4.5.4)`，需要修改的有以下条目：

In `CMakeLists.txt`:
```cmake
# 请将下面三条改成自己的Mingw目录
set(CMAKE_MAKE_PROGRAMC "D:/Program Files/mingw-w64/bin/mingw32-make.exe")
set(CMAKE_C_COMPILER "D:/Program Files/mingw-w64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "D:/Program Files/mingw-w64/bin/g++.exe")

# 请将下面这条修改成自己的opencv编译后的lib目录
set(OpenCV_DIR "D:/opencv_inst/opencv/mingw-build/install/x64/mingw/lib")

```

In `main.cpp` (optional) :
```c++
// 定义模型和图片目录
const string yoloPath = R"(.\model\)";
const string imgPath = R"(.\images\)";

// 定义图片路径
const string aImgPath = imgPath + "aff.png";
const string hImgPath1 = imgPath + "h1.png", hImgPath2 = imgPath + "h2.png";
const string pImgPath = imgPath + "per.png";
const string sImgPath1 = imgPath + "alice1.png", sImgPath2 = imgPath + "alice2.png";
const string yImgPath = imgPath + "street.png";
```

`build.bat`文件用来编译，`clean.bat`文件用来清除编译后的文件。
