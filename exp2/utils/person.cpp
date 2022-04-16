//
// Created by Zhao Yuqi on 2022/4/14.
//

#include "person.h"

void personDetect(const string& imgPath) {
    // 读取图片
    Mat src = imread(imgPath);
    if (src.empty()) {
        cout << "Cannot load image..." << endl;
        return;
    }

    namedWindow("Input image", WINDOW_AUTOSIZE);
    imshow("Input image", src);

    HOGDescriptor hog = HOGDescriptor();
    // 设置hog的SVM算子为默认的行人检测算子
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

    vector<Rect> foundLocations;   //定个一个变量行人矩形框
    hog.detectMultiScale(src,                              // 源图像矩阵
                         foundLocations,                     // 检测出的物体的边缘
                         0,                          // 阀值，特征向量和SVM划分超平面的距离，默认为0
                         Size(4, 4),     // 窗口步长，必须是block步长的整数倍
                         Size(16, 16),    // 图像边缘扩充参数
                         1.05,                             // 检测窗口增长参数
                         2);                        // 检测结果聚类参数
    // 多个尺度的行人的寻找，采用不同的开窗大小检测
    // winStride参数越大，检测窗口移动的步长越大，检测的目标个数越小。
    // padding有(8, 8), (16, 16), (24, 24), (32, 32)等。适当的pad可以提高检测的准确率。
    // scale参数可以具体控制金字塔的层数，参数越小，层数越多，检测时间越长。通常scale在1.01-1.5这个区间。
    for (auto &foundLocation: foundLocations) {
        rectangle(src, foundLocation, Scalar(0, 0, 255), 2, 8, 0);
    }
    namedWindow("HOG(SVM) detect", WINDOW_AUTOSIZE);
    imshow("HOG(SVM) detect", src);

    waitKey(0);
}
