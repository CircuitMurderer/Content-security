//
// Created by Zhao Yuqi on 2022/4/15.
//

#ifndef UNTITLED_YOLO_H
#define UNTITLED_YOLO_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace dnn;
using namespace std;

struct netConfig {
    float confThreshold; // Confidence threshold
    float nmsThreshold;  // Non-maximum suppression threshold
    int inpWidth;  // Width of network's input image
    int inpHeight; // Height of network's input image
    string classesFile;
    string modelConfiguration;
    string modelWeights;
    string netName;
};

class Yolo {
public:
    explicit Yolo(const netConfig& config);

    void detect(Mat &frame);

private:
    float confThreshold;
    float nmsThreshold;
    int inpWidth;
    int inpHeight;
    char netName[20]{};
    vector<string> classes;
    Net net;

    void postProcess(Mat &frame, const vector<Mat> &outs);

    void drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat &frame);
};

void yolo(const string& modelPath, const string& imgPath);

#endif //UNTITLED_YOLO_H
