#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

// 图像预处理
//cv::Mat preprocessImageBlue(
//    const cv::Mat& src,
//    double claheClip = 1.0,
//    cv::Size claheGrid = cv::Size(8, 8),
//    cv::Size gaussianKernel = cv::Size(3, 3),
//    cv::Size morphKernelSize = cv::Size(3, 3));

// 蓝色和红色筛选
cv::Mat chooseBlue(const cv::Mat& img);
cv::Mat chooseRed(const cv::Mat& img);
