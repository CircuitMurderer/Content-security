//
// Created by Zhao Yuqi on 2022/4/14.
//

#include "hist.h"

void histCompare(const string& imgPath1, const string& imgPath2) {
    Mat img1, imgGray1, hist1, img2, imgGray2, hist2, imgEnhanced;   //定义修改图像，灰度修改图像, 修改直方图，目标图像，灰度目标图像，目标直方图, 规定化增强图像

    // 读取图片
    img1 = imread(imgPath1, 1);
    img2 = imread(imgPath2, 1);

    if (img1.empty() || img2.empty()) {
        cout << "Cannot load image..." << endl;
        return;
    }

    // 灰度化
    cvtColor(img1, imgGray1, COLOR_BGR2GRAY);
    cvtColor(img2, imgGray2, COLOR_BGR2GRAY);

    // 显示灰度图像
    imshow("Image 1 gray", imgGray1);
    imshow("Image 2 gray", imgGray2);

    // 均衡化处理
    equalizeHist(imgGray1, imgGray1);
    equalizeHist(imgGray2, imgGray2);

    // 获取两个均衡化图像的直方图
    int histSize = 256;
    float ranges[] = {0, 256};
    const float *histRanges = {ranges};
    calcHist(&imgGray1, 1, nullptr, Mat(), hist1, 1, &histSize, &histRanges, true, false);
    calcHist(&imgGray2, 1, nullptr, Mat(), hist2, 1, &histSize, &histRanges, true, false);

    // 进行直方图比较
    cout << "Correl: " << fabs(compareHist(hist1, hist2, HISTCMP_CORREL)) << endl; //相关性
    cout << "Intersect: " << compareHist(hist1, hist2, HISTCMP_INTERSECT) / (imgGray1.rows * imgGray1.cols) << endl; //十字
    cout << "Bhattacharyya: " << 1 - compareHist(hist1, hist2, HISTCMP_BHATTACHARYYA) << endl; //巴氏系数

    // 计算两个均衡化图像直方图的累积概率
    float histCdf1[256] = {hist1.at<float>(0)};
    float histCdf2[256] = {hist2.at<float>(0)};
    for (int i = 1; i < 256; i++) {
        histCdf1[i] = (histCdf1[i - 1] + hist1.at<float>(i));
        histCdf2[i] = (histCdf2[i - 1] + hist2.at<float>(i));
    }
    for (int i = 0; i < 256; i++) {
        histCdf1[i] = histCdf1[i] / (float)(imgGray1.rows * imgGray1.cols);
        histCdf2[i] = histCdf2[i] / (float)(imgGray2.rows * imgGray2.cols);
    }

    // 两个累计概率之间的差值，用于找到最接近的点
    float diffCdf[256][256];
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            diffCdf[i][j] = fabs(histCdf1[i] - histCdf2[j]);
        }
    }

    Mat lut(1, 256, CV_8U);
    for (int i = 0; i < 256; i++) {
        // 查找源灰度级为i的映射灰度和i的累积概率差最小（灰度接近）的规定化灰度
        float min = diffCdf[i][0];
        int index = 0;
        for (int j = 0; j < 256; j++) {
            if (min > diffCdf[i][j]) {
                min = diffCdf[i][j];
                index = j;
            }
        }
        lut.at<uchar>(i) = index;
    }
    LUT(imgGray1, lut, imgEnhanced);  //图像中进行映射
    imshow("Image enhanced", imgEnhanced);

    waitKey(0);
}

