//
// Created by Zhao Yuqi on 2022/4/14.
//

#include "affine.h"

// 显示窗口
void showWindow(const Mat& m, const string& winName) {
    namedWindow(winName, WINDOW_AUTOSIZE);
    imshow(winName, m);
}

// 平移
void translate(const Mat& src, Mat& dst, const string& winName) {
    // 定义平移数组，水平平移量40，竖直平移量20
    float m[2][3] = {{1, 0, 40}, {0, 1, 20}};

    // 通过平移数组得到平移矩阵，进行变换
    warpAffine(src, dst, Mat(2, 3,CV_32FC1, m), dst.size());
    showWindow(dst, winName);
}

// 翻转
void reflect(const Mat& src, Mat& dst, const string& winName) {
    // 定义翻转数组，这里需要平移一个size，否则看不到图（因为在负轴）
    float m[2][3] = {{-1, 0, (float) dst.cols},
                     {0, -1, (float) dst.rows}};

    // 通过翻转数组得到翻转矩阵，进行变换
    warpAffine(src, dst, Mat(2, 3,CV_32FC1, m), dst.size());
    showWindow(dst, winName);
}

// 错切 + 缩放
void shearScale(const Mat& src, Mat& dst, const string& winName) {
    // 定义错切 + 缩放数组，缩放0.6倍
    float m[2][3] = {{0.6, 0.2, 0},
                     {0.2, 0.6, 0}};

    // 通过数组得到矩阵，进行变换
    warpAffine(src, dst, Mat(2, 3,CV_32FC1, m), dst.size());
    showWindow(dst, winName);
}

// 旋转 + 缩放
void rotateScale(const Mat& src, Mat& dst, const string& winName) {
    // 计算绕图像中点顺时针旋转50度、缩放因子为0.6的旋转矩阵
    Point center = Point(dst.cols / 2, dst.rows / 2);
    double angle = -50.0, scale = 0.6;

    // 通过旋转细节信息求得旋转矩阵，旋转图像
    warpAffine(src, dst, getRotationMatrix2D(center, angle, scale), dst.size());
    showWindow(dst, winName);
}

// 仿射变换
void affine(const Mat& src, Mat& dst, const string& winName) {
    auto col = (float) dst.cols, row = (float) dst.rows;

    // 设置源图像和目标图像上的三组点以计算仿射变换
    Point2f srcTri[3] = {
            Point2f(0, 0),
            Point2f(col - 1, 0),
            Point2f(0, row - 1)
    };
    Point2f dstTri[3] = {
            Point2f(col * 0.0f, row * 0.33f),
            Point2f(col * 0.85f, row * 0.25f),
            Point2f(col * 0.15f, row * 0.7f)
    };

    // 对源图像应用上面的三组点以求得仿射变换
    warpAffine(src, dst, getAffineTransform(srcTri, dstTri), dst.size());
    showWindow(dst, winName);
}

void affineTrans(const string &imgPath) {
    // 读取图片
    Mat src = imread(imgPath, 1);

    // 设置目标图像的大小和类型与源图像一致
    Mat dst = Mat::zeros(src.rows, src.cols, src.type());

    showWindow(src, "Source image");
    translate(src, dst, "Translate image");
    reflect(src, dst, "Reflect image");
    shearScale(src, dst, "Shear + scale image");
    rotateScale(src, dst, "Rotate + scale image");
    affine(src, dst, "Affine image");

    waitKey(0);
}
