//
// Created by Zhao Yuqi on 2022/4/14.
//

#include "sift.h"

// 获取特征点信息
void getDescriptors(int numFeatures,
                    Mat &img,
                    Mat &descriptors,
                    vector<KeyPoint> &keyPoints,
                    const string& winName) {

    Ptr<SIFT> detector = SIFT::create(numFeatures);
    if (detector.empty())
        throw runtime_error("fail to create detector!");

    // 检测关键点
    detector->detect(img, keyPoints);
    cout << "img:" << keyPoints.size() << endl;

    // Sift特征描述提取
    cout << "Computing descriptors for keyPoints from images..." << endl;

    detector->compute(img, keyPoints, descriptors);

    cout << endl << "Descriptors Size: " << descriptors.size() << endl;
    cout << endl << "Descriptor's Column: " << descriptors.cols << endl
         << "Descriptor's Row: " << descriptors.rows << endl;

    // 绘制并匹配两个图片的关键点
    Mat imgKeyPoints;
    drawKeypoints(img, keyPoints, imgKeyPoints,
                  Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    imshow(winName, imgKeyPoints);
}

// 画匹配的特征点对
void drawMatch(Mat &img1,
               vector<KeyPoint> &keyPoints1,
               Mat &descriptors1,
               Mat &img2,
               vector<KeyPoint> &keyPoints2,
               Mat &descriptors2,
               int numLines) {
    // 使用暴力匹配模式
    Ptr<DescriptorMatcher> descriptorMatcher = DescriptorMatcher::create("BruteForce");
    vector<DMatch> matches;
    descriptorMatcher->match(descriptors1, descriptors2, matches);

    // 挑选匹配的最好的前numLines个
    nth_element(matches.begin(), matches.begin() + numLines - 1, matches.end());
    matches.erase(matches.begin() + numLines - 1, matches.end());

    // 绘制匹配点连线
    Mat imgMatches;
    drawMatches(img1, keyPoints1, img2, keyPoints2, matches,
                imgMatches, Scalar::all(-1), Scalar::all(-1),
                vector<char>(), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    imshow("SIFT result", imgMatches);
    waitKey(0);
}

void sift(const string& imgPath1, const string& imgPath2) {
    vector<KeyPoint> keyPoints1, keyPoints2;
    Mat img1, img2, descriptors1, descriptors2;

    // 读取图片
    img1 = imread(imgPath1);
    img2 = imread(imgPath2);
    if (img1.empty() || img2.empty()) {
        cout << "Cannot load image..." << endl;
        return;
    }

    int numFeatures = 500;  // 特征点最大个数
    int numLines = 30;      // 前多少个匹配最好的点

    getDescriptors(numFeatures, img1, descriptors1, keyPoints1, "Image 1"); // 绘制图片1特征点
    getDescriptors(numFeatures, img2, descriptors2, keyPoints2, "Image 2"); // 绘制图片2特征点
    drawMatch(img1, keyPoints1, descriptors1, img2, keyPoints2, descriptors2, numLines); // 绘制匹配点
}
