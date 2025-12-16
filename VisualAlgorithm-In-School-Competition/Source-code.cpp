/*
一、图像预处理

1.RGB转HSV

二、灯条检测

1.将画面中己方颜色灯条过滤
    1.1需要计算每个灯条区域像素快在原图中的 R 和 B 值总和，通过比较大小排除掉敌方颜色的灯条。
    1.2物理特征排除

灯条的视觉特征宽高比例值约为满足 0.1 - 0.4
灯条的倾斜角一般在 35.0°

三、灯条匹配

1.需要对这些灯条进行两两匹配

装甲板的灯条具有以下数学特征：

  **两灯条中心点之间的距离与灯条平均长度的比例满足一定的数学关系**

  **两灯条中点相连得到的线段倾角一般: < 35°**

  **两灯条比例一般满足：> 0.8**

  **两灯条之间一定不会出现灯条**


四、结果筛选

1.甲板区域进行面积计算，筛选出像素面积最大的装甲板区域

*/
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "head.h"

using namespace std;

/* 筛选颜色 */
cv::Mat chooseRed(const cv::Mat& img) {
    cv::Mat hsv;
    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);

    //// 视频测试颜色
    //cv::Scalar lower1(0, 100, 100);
    //cv::Scalar upper1(10, 255, 255);
    //// 实际测试颜色
    //cv::Scalar lower2(0, 0, 175);
    //cv::Scalar upper2(180, 40, 255);

    cv::Scalar lower1(0, 100, 100);
    cv::Scalar upper1(10, 255, 255);
    cv::Scalar lower2(170, 100, 100);
    cv::Scalar upper2(180, 255, 255);

    cv::Mat mask1, mask2, mask;
    cv::inRange(hsv, lower1, upper1, mask1);
    cv::inRange(hsv, lower2, upper2, mask2);
    cv::bitwise_or(mask1, mask2, mask);
    return mask;
}

cv::Mat chooseBlue(const cv::Mat& img) {
    cv::Mat hsv;
    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);

    //cv::Scalar lower(90, 100, 150);
    //cv::Scalar upper(135, 255, 255);

    cv::Scalar lower(95, 100, 160);
    cv::Scalar upper(135, 255, 255);

    cv::Mat mask;
    cv::inRange(hsv, lower, upper, mask);

    return mask;
}

/* 颜色选择 */
enum LightColor {
    UNKNOWN_COLOR = 0,
    RED = 1,
    BLUE = 2,
};

/* 灯条结构体 */
struct LightBar {
    cv::RotatedRect rect; // 旋转矩形
    cv::Point2f center;   // 中心点坐标
    float length;         // 灯条长度
    float angle;          // 灯条角度
    LightColor color;     // 灯条颜色
    // 构造灯条函数
    LightBar() : length(0), angle(0), color(UNKNOWN_COLOR) {} // 初始化
    LightBar(const cv::RotatedRect& r, LightColor c = UNKNOWN_COLOR) : rect(r), color(c) {
        center = r.center;
        length = max(r.size.width, r.size.height); // 取最长的为灯条长边
        angle = r.angle;
        // 转换角度 避免角度差问题
        if (r.size.width > r.size.height) {
            angle += 90;
        }
    }
};

/* 装甲板结构体 */
struct ArmorPlate {
    LightBar leftBar; // 左灯条
    LightBar rightBar; // 右灯条
    cv::Point2f center; // 装甲板中心坐标
    float width; // 宽度（长度）
    float height; // 高度
    LightColor color; // 颜色
    // 构造装甲板函数
    /*ArmorPlate(const LightBar left, const LightBar right)  // 值传递*/
    // 引用传递
    ArmorPlate(const LightBar& left, const LightBar& right)
        : leftBar(left), rightBar(right), color(left.color) { 
        center.x = (left.center.x + right.center.x) / 2; //计算装甲板中心X坐标
        center.y = (left.center.y + right.center.y) / 2; //计算装甲板中心Y坐标
        width = cv::norm(left.center - right.center); //计算宽度
        height = (left.length + right.length) / 2; //计算高度
    }
};

// 判断装甲板的四个顶点
void getArmorVertices(const ArmorPlate& armor,
    cv::Point2f vertices[4]) {
    const LightBar& left = armor.leftBar;
    const LightBar& right = armor.rightBar;

    cv::Point2f leftVertices[4], rightVertices[4];
    left.rect.points(leftVertices);
    right.rect.points(rightVertices);

    // 找到左灯条最右侧的两个点
    std::vector<cv::Point2f> leftPoints(leftVertices, leftVertices + 4);
    std::sort(leftPoints.begin(), leftPoints.end(),
        [](const cv::Point2f& a, const cv::Point2f& b) {
            return a.x < b.x;
        });

    // 找到右灯条最左侧的两个点
    std::vector<cv::Point2f> rightPoints(rightVertices, rightVertices + 4);
    std::sort(rightPoints.begin(), rightPoints.end(),
        [](const cv::Point2f& a, const cv::Point2f& b) {
            return a.x < b.x;
        });

    if (leftPoints[2].y < leftPoints[3].y) {
        vertices[0] = leftPoints[2]; // 左上
        vertices[3] = leftPoints[3]; // 左下
    }
    else {
        vertices[0] = leftPoints[3]; // 左上
        vertices[3] = leftPoints[2]; // 左下
    }

    if (rightPoints[0].y < rightPoints[1].y) {
        vertices[1] = rightPoints[0]; // 右上
        vertices[2] = rightPoints[1]; // 右下
    }
    else {
        vertices[1] = rightPoints[1]; // 右上
        vertices[2] = rightPoints[0]; // 右下
    }
}

int main() {
    // 打开摄像头
    //cv::VideoCapture cap(0);

    //测试视频 蓝色装甲板
    cv::VideoCapture cap("D:/中北大学/卓创校内赛测试视频/BlueArmorPlus.mp4");

    //测试视频 红色装甲板
    //cv::VideoCapture cap("D:/中北大学/卓创校内赛测试视频/RedArmorPlus.mp4");
    //cv::VideoCapture cap("D:/中北大学/卓创校内赛测试视频/RED.mp4");

    // 检查摄像头
    if (!cap.isOpened()) {
        cerr << "无法打开摄像头！" << endl;
        return -1;
    }

    //定义变量
    cv::Mat frame;
    cv::Mat image;
    cv::Mat result;

    while (true) {

        cap >> frame;

        if (frame.empty()) {
            std::cerr << "读取帧失败" << std::endl;
            break;
        }

        /* 图像预处理 */
        cv::Mat originalClone = frame.clone();
        image = chooseBlue(originalClone);
        // 形态学处理
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        // 闭运算
        //cv::morphologyEx(image, result, cv::MORPH_CLOSE, kernel);
        // 膨胀
        cv::dilate(image, result, kernel);
        cv::Mat result_Copy = result.clone();
        // 二值化
        cv::threshold(result_Copy, result_Copy, 128, 255, cv::THRESH_BINARY);

        // 从二值图提取轮廓
        vector<vector<cv::Point>> contours;
        cv::findContours(result_Copy, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
       
        vector<LightBar> lightBars;

        /* 灯条判断 */
        // 遍历轮廓
        for (const auto& contour : contours) {
            if (contour.size() < 5) continue;
            // 轮廓面积
            double area = cv::contourArea(contour);
            if (area < 1e-5) continue;
            // 最小旋转矩形
            cv::RotatedRect rect = cv::minAreaRect(contour);
            // 确保height为长边
            float height = rect.size.height;   //高
            float width = rect.size.width;    //宽
            if (height < width)
            {
                float temp = height;
                height = width;
                width = temp;
            }

            cv::Point2f center = rect.center;

            float angle=rect.angle;
            angle = abs(angle);

            if (angle > 90) angle = 180 - angle;

            // 长宽比
            float aspectRatio = height / width;
            if (aspectRatio > 15.0 || aspectRatio < 2.0)
            {
                continue;
            }
            
            /* 颜色判断 */
            LightColor lightColor = UNKNOWN_COLOR;
            cv::Rect bRect = rect.boundingRect();
            if (bRect.x >= 0 && bRect.y >= 0 &&
                bRect.x + bRect.width <= frame.cols &&
                bRect.y + bRect.height <= frame.rows) {

                // 提取ROI区域 
                cv::Mat roi = frame(bRect);

                int sum_r = 0, sum_b = 0, sum_g = 0;
                for (int i = 0; i < roi.rows; i++) 
                {
                    for (int j = 0; j < roi.cols; j++) 
                    {
                        cv::Vec3b pixel = roi.at<cv::Vec3b>(i, j);
                        sum_r += pixel[2];
                        sum_b += pixel[0];
                    }
                }
                lightColor = sum_r > sum_b ? RED : BLUE;
                //cout << lightColor << endl;
            }

            // 获取四个顶点
            cv::Point2f vertices[4];
            rect.points(vertices);
            /*
                // 绘制旋转矩形
                for (int i = 0; i < 4; i++) {
                    cv::line(frame, vertices[i], vertices[(i + 1) % 4],
                        cv::Scalar(0, 255, 0), 2);
                }
            */
            // 保留合理大小的轮廓
            LightBar lightBar(rect, lightColor);
            lightBars.push_back(lightBar);
            }
        /*cout << "过滤后剩余 " << validContours.size() << " 个轮廓" << endl;*/

        /* 装甲板判断 */
        vector<ArmorPlate> armorPlates;
        // size_t 无符号，与 size() 匹配，无警告
        for (size_t i = 0; i < lightBars.size(); i++) {
            for (size_t j = i + 1; j < lightBars.size(); j++) {
                // 1.判断俩灯条是否平行
                if ((lightBars[i].angle - lightBars[j].angle) > 10.0)
                {
                    continue;
                }
                // 2.长度比较
                float lengthRatio = max(lightBars[i].length, lightBars[j].length) / min(lightBars[i].length, lightBars[j].length);
                if (lengthRatio > 1.2) {
                    continue;
                }
                // 3.中点连线倾角小于35°
                /*两个点的坐标差，然后使用反正切函数atan2得到角度*/
                float dx = lightBars[i].center.x - lightBars[j].center.x;
                float dy = lightBars[i].center.y - lightBars[j].center.y;

                float Langle = std::atan2(dy, dx) * 180.0 / CV_PI;

                // 取绝对值，转换到0-90度范围
                Langle = std::abs(Langle);
                if (Langle > 90.0f) Langle = 180.0f - Langle;

                if (Langle > 35.0f) {
                    continue;
                }
                // 4.装甲板俩灯条之间不会出现第三个灯条
                cv::Point2f centerI = lightBars[i].center;
                cv::Point2f centerJ = lightBars[j].center;
                cv::Point2f cIJ = centerJ - centerI;

                float dx_rect = abs(centerI.x - centerJ.x);
                float dy_rect = abs(centerI.y - centerJ.y);

                float minX = (centerI.x < centerJ.x ? centerI.x : centerJ.x) - dx_rect * 0.5f;
                float maxX = (centerI.x > centerJ.x ? centerI.x : centerJ.x) + dx_rect * 0.5f;
                float minY = (centerI.y < centerJ.y ? centerI.y : centerJ.y) - dy_rect * 0.5f;
                float maxY = (centerI.y > centerJ.y ? centerI.y : centerJ.y) + dy_rect * 0.5f;

                bool light3 = false;
                for (int k = 0; k < lightBars.size(); k++)
                {
                    if (k == i || k == j)
                    {
                        continue;
                    }
                    cv::Point2f otherCenter = lightBars[k].center;
                    if (otherCenter.x >= minX && otherCenter.x <= maxX &&
                        otherCenter.y >= minY && otherCenter.y <= maxY)
                    {
                        light3 = true;
                        break;
                    }
                }

                if (light3) {
                    continue;
                }
                

                // 5.高度差 俩灯条y坐标差
                float centerYi = lightBars[i].center.y;
                float centerYj = lightBars[j].center.y;
                float heightDiff = abs(centerYi - centerYj);

                float aveLength = (lightBars[i].rect.size.height + lightBars[j].rect.size.height) / 2;
                if (heightDiff > aveLength * 0.5) {
                    continue;
                }
                // 6.左右灯条颜色相同
                if (lightBars[i].color != lightBars[j].color) {
                    continue;
                }
                // 确定左右灯条
                const LightBar& leftBar = (centerI.x < centerJ.x) ? lightBars[i] : lightBars[j];
                const LightBar& rightBar = (centerI.x < centerJ.x) ? lightBars[j] : lightBars[i];
                armorPlates.push_back(ArmorPlate(leftBar, rightBar));
            }
        }

        /* 绘制装甲板 */
        for (size_t i = 0; i < armorPlates.size(); i++)
        {
            // 绘制左侧灯条
            cv::Point2f vertices1[4];
            armorPlates[i].leftBar.rect.points(vertices1);
            for (int j = 0; j < 4; j++) {
                cv::line(frame, vertices1[j], vertices1[(j + 1) % 4], cv::Scalar(0, 255, 0), 2);
            }

            // 绘制右侧灯条
            cv::Point2f vertices2[4];
            armorPlates[i].rightBar.rect.points(vertices2);
            for (int j = 0; j < 4; j++) {
                cv::line(frame, vertices2[j], vertices2[(j + 1) % 4], cv::Scalar(0, 255, 0), 2);
            }

            // 绘制装甲板外框
            //cv::Point2f topLeft = vertices1[0];
            //cv::Point2f topRight = vertices2[1];
            //cv::Point2f bottomRight = vertices2[2];
            //cv::Point2f bottomLeft = vertices1[3];

            //cv::line(frame, topLeft, topRight, cv::Scalar(255, 0, 0), 2);
            //cv::line(frame, topRight, bottomRight, cv::Scalar(255, 0, 0), 2);
            //cv::line(frame, bottomRight, bottomLeft, cv::Scalar(255, 0, 0), 2);
            //cv::line(frame, bottomLeft, topLeft, cv::Scalar(255, 0, 0), 2);

            cv::Point2f armorVertices[4];
            getArmorVertices(armorPlates[i], armorVertices);
            for (int j = 0; j < 4; j++) {
                cv::line(frame, armorVertices[j], armorVertices[(j + 1) % 4],
                    cv::Scalar(255, 0, 0), 2);
            }

            // 显示装甲板数量
            std::string stats = "Armors: " + std::to_string(armorPlates.size());    
            cv::putText(frame, stats, cv::Point(10, 30),
                cv::FONT_HERSHEY_SIMPLEX, 1.0,
                cv::Scalar(0, 255, 255), 2);

            // 标记装甲板颜色
            string colorText = (armorPlates[i].color == RED) ? "COLOR:RED" : "COLOR:BLUE";
            cv::putText(frame, colorText, cv::Point2f(10, 60),
                cv::FONT_HERSHEY_SIMPLEX, 1.0,
                (armorPlates[i].color == RED) ? cv::Scalar(0, 0, 255) : cv::Scalar(255, 0, 0), 2);
        }

        cv::resize(result, result, cv::Size(640, 640));
        cv::resize(frame, frame, cv::Size(640, 640));
        cv::imshow("图像处理", result);
        cv::imshow("检测结果", frame);

        // 按 ESC 退出
        char c = (char)cv::waitKey(1);
        if (c == 27) {
            break;
        }
    }

    // 释放资源
    cap.release();
    cv::destroyAllWindows();

    return 0;
}