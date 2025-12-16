#include "head.h"

//// 图像预处理
//cv::Mat preprocessImageRed(
//    const cv::Mat& src,
//    double claheClip,
//    cv::Size claheGrid,
//    cv::Size gaussianKernel,
//    cv::Size morphKernelSize)
//{   
//    // RGB转HSV
//    cv::Mat hsv;
//    cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);
//
//    // V通道 CLAHE 处理
//    std::vector<cv::Mat> hsvChannels;// 创建一个向量来存放分离后的通道
//    cv::split(hsv, hsvChannels);
//
//    cv::Mat vChannel = hsvChannels[2]; //提取V通道
//
//    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
//    clahe->setClipLimit(claheClip);
//    clahe->setTilesGridSize(claheGrid);
//
//    cv::Mat V_after;
//
//    clahe->apply(vChannel, V_after);
//
//    hsvChannels[2] = V_after;    //V通道
//    cv::merge(hsvChannels, hsv);
//
//    // 阶段测试
//    //cv::imshow("CLAHE增强图像_enhanced", enhanced);
//
//    // 高斯模糊
//    cv::Mat blurred;
//    cv::GaussianBlur(hsv, blurred, gaussianKernel, 0, 0);
//
//    // 二值化
//
//    // 红色
//    cv::Scalar lower1(0, 100, 100);
//    cv::Scalar upper1(10, 255, 255);
//    cv::Scalar lower2(170, 100, 100);
//    cv::Scalar upper2(180, 255, 255);
//
//    cv::Mat mask1, mask2, mask;
//    cv::inRange(hsv, lower1, upper1, mask1);
//    cv::inRange(hsv, lower2, upper2, mask2);
//    cv::bitwise_or(mask1, mask2, mask);
//
//    // 形态学
//    cv::Mat morphKernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
//
//    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, morphKernel);
//    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, morphKernel);
//
//    cv::Mat threshImg;
//    cv::Mat gray = hsvChannels[2];
//
//    cv::threshold(gray, threshImg, 200, 255, cv::THRESH_BINARY);
//
//    return threshImg;
//}

