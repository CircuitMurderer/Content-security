//
// Created by Zhao Yuqi on 2022/4/15.
//

#include "yolo.h"

// 初始化
Yolo::Yolo(const netConfig& config) {
    // 初始化参数
    cout << "Net use " << config.netName << endl;
    this->confThreshold = config.confThreshold;
    this->nmsThreshold = config.nmsThreshold;
    this->inpWidth = config.inpWidth;
    this->inpHeight = config.inpHeight;
    strcpy_s(this->netName, config.netName.c_str());

    // 从类别文件中读取类名
    ifstream ifs(config.classesFile.c_str());
    string line;
    while (getline(ifs, line)) this->classes.push_back(line);

    // 读取训练好的yolo模型
    this->net = readNetFromDarknet(config.modelConfiguration, config.modelWeights);
    this->net.setPreferableBackend(DNN_BACKEND_OPENCV);
    this->net.setPreferableTarget(DNN_TARGET_CPU);
}

// 边框处理：使用非极大值抑制算法移除置信度较低的边框
void Yolo::postProcess(Mat &frame, const vector<Mat> &outs) {
    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;

    for (const auto & out : outs) {
        // 扫描网络输出的所有边框，只保留置信度高的边框
        // 将边框的类标签指定为该边框得分最高的类
        auto *data = (float *) out.data;
        for (int j = 0; j < out.rows; ++j, data += out.cols) {
            Mat scores = out.row(j).colRange(5, out.cols);
            Point classIdPoint;
            double confidence;
            // 获取得分最高的值和位置
            minMaxLoc(scores, nullptr, &confidence, nullptr, &classIdPoint);
            if (confidence > this->confThreshold) {
                int centerX = (int) (data[0] * (float) frame.cols);
                int centerY = (int) (data[1] * (float) frame.rows);
                int width = (int) (data[2] * (float) frame.cols);
                int height = (int) (data[3] * (float) frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;

                classIds.push_back(classIdPoint.x);
                confidences.push_back((float) confidence);
                boxes.emplace_back(left, top, width, height);
            }
        }
    }

    // 执行非极大值抑制算法来消除置信度较低的冗余重叠边框
    vector<int> indices;
    NMSBoxes(boxes, confidences, this->confThreshold, this->nmsThreshold, indices);
    for (int i : indices) {
        Rect box = boxes[i];
        this->drawPred(classIds[i], confidences[i], box.x, box.y,
                       box.x + box.width, box.y + box.height, frame);
    }
}

// 绘制预测边框
void Yolo::drawPred(int classId, float conf, int left,
                    int top, int right, int bottom, Mat &frame) {
    // 绘制边框矩形
    rectangle(frame, Point(left, top), Point(right, bottom),
              Scalar(255, 0, 0), 2);

    // 获取类别名及其置信度的标签
    string label = format("%.2f", conf);
    if (!this->classes.empty()) {
        CV_Assert(classId < (int) this->classes.size());
        label = this->classes[classId] + ":" + label;
    }

    // 在边框顶部显示标签
    int baseLine;
    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    top = max(top, labelSize.height);
    rectangle(frame, Point(left, top - int(1.5 * labelSize.height)),
              Point(left + int(1.5 * labelSize.width), top + baseLine), Scalar(255, 0, 0), FILLED);
    putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.75,
            Scalar(255, 255, 255), 2);
}

// 进行检测
void Yolo::detect(Mat &frame) {
    Mat blob;

    // 使用yolo网络进行目标检测，并绘制边框
    // 预处理
    blobFromImage(frame, blob, 1 / 255.0, Size(this->inpWidth, this->inpHeight),
                  Scalar(0, 0, 0), true, false);
    this->net.setInput(blob);
    vector<Mat> outs;

    // 前向传播
    this->net.forward(outs, this->net.getUnconnectedOutLayersNames());
    this->postProcess(frame, outs);

    // 显示检测用时
    vector<double> layersTimes;
    double freq = getTickFrequency() / 1000;
    double t = (double) net.getPerfProfile(layersTimes) / freq;
    string label = format("%s Inference time : %.2f ms", this->netName, t);
    putText(frame, label, Point(0, 30), FONT_HERSHEY_SIMPLEX,
            1, Scalar(0, 0, 255), 2);
}

void yolo(const string& modelPath, const string& imgPath) {
    // 读取图片
    Mat src = imread(imgPath);
    cout << src.cols << " x " << src.rows << endl;

    // 网络配置
    netConfig conf = {0.5,
                      0.4,
                      480,
                      480,
                      modelPath + "classes.txt",
                      modelPath + "yolo-fastest-xl.cfg",
                      modelPath + "yolo-fastest-xl.weights",
                      "yolo-fastest-xl"};

    Yolo yoloModel(conf);
    yoloModel.detect(src);

    namedWindow("Yolo detect result", WINDOW_AUTOSIZE);
    imshow("Yolo detect result", src);
    waitKey(0);
}
