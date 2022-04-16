//
// Created by Zhao Yuqi on 2022/4/14.
//

#include "utils/utils.h"

int main() {
    // 定义模型和图片目录
    const string yoloPath = R"(.\model\)";
    const string imgPath = R"(.\images\)";

    // 定义图片路径
    const string aImgPath = imgPath + "aff.png";
    const string hImgPath1 = imgPath + "h1.png", hImgPath2 = imgPath + "h2.png";
    const string pImgPath = imgPath + "per.png";
    const string sImgPath1 = imgPath + "alice1.png", sImgPath2 = imgPath + "alice2.png";
    const string yImgPath = imgPath + "street.png";

    cout << "Input a, h, p, s, y to choose the function:\n"
         << "\ta: affine transfer" << endl
         << "\th: histogram compare" << endl
         << "\tp: person detect" << endl
         << "\ts: SIFT match" << endl
         << "\ty: YOLO detect" << endl;

    // 根据输入执行不同功能
    switch (getchar()) {
        case 'a':
            affineTrans(aImgPath);
            break;
        case 'h':
            histCompare(hImgPath1, hImgPath2);
            break;
        case 'p':
            personDetect(pImgPath);
            break;
        case 's':
            sift(sImgPath1, sImgPath2);
            break;
        case 'y':
            yolo(yoloPath, yImgPath);
            break;
        default:
            cout << "Invalid option." << endl;
    }
    return 0;
}













