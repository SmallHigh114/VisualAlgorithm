# RM视觉代码

部分内容注释

1.Scalar：存储四个 double 类型的值：(val[0], val[1], val[2], val[3])
cv::Scalar lower1(0, 100, 100);
调用：lower1[1]

2.enum：枚举 (enumeration, enum) 是 C++ 中的一种用户定义数据类型，用于定义一组命名的常量。
enum Color{Red=0,blue=1};
Color color=Red;

3.LightBar(const cv::RotatedRect& r, LightColor c = UNKNOWN_COLOR) : rect(r), color(c) 
初始化值，避免const类型无法定义

4.// 转换角度 避免角度差问题
        if (r.size.width > r.size.height) {
            angle += 90;
        }
高度是长边，但 angle 表示的是短边方向
需要旋转 90 度来得到长边方向

5.pair
vector<pair<float, cv::Point2f>> leftDist;
std::pair 是 C++ 标准库中的模板类，用于存储两个值（一对值），这两个值可以是不同类型。

6.norm
float d = cv::norm(lv[i] - armor.rightBar.rect.center);
norm(点1-点2) = 距离
计算欧几里得距离

7.
sort(leftDist.begin(), leftDist.end(),
        [](auto& a, auto& b) { return a.first > b.first; });

7.1 迭代器
    // 基本用法：升序排序
    std::sort(numbers.begin(), numbers.end());

7.2 Lambda 表达式：C++ 的匿名函数工具

Lambda 表达式是 C++11 引入的匿名函数对象，它允许你在需要函数的地方内联定义函数，而不必单独声明。

Lambda 表达式语法
完整表达：[capture-list](parameters) mutable exception -> return-type { body }
                 [捕获列表](参数) 可变异常 -> 返回类型 { 方法体 }
详解：


A. 捕获列表 []
指定 Lambda 可以访问的外部变量：

int threshold = 100;
// 1. 不捕获任何外部变量
[]() { /* 不能访问 threshold */ };
// 2. 按值捕获
[threshold]() { /* 可以使用 threshold，但不能修改 */ };
// 3. 按引用捕获
[&threshold]() { /* 可以修改 threshold */ };
// 4. 捕获所有变量
[=]() { /* 按值捕获所有 */ };
[&]() { /* 按引用捕获所有 */ };
[]表示不捕获任何变量

B. 参数列表 (auto& a, auto& b)
引入auto参数

C. 返回类型
[](auto& a, auto& b) { return a.first > b.first; }  // 推导为 bool

D. 比较逻辑 a.first > b.first
// leftDist 的元素类型是 pair<float, cv::Point2f>
// a.first = 距离值，a.second = 顶点坐标
// 升序排序（默认）：a.first < b.first
// 降序排序（示例）：a.first > b.first

8.camera >> frame
从camera（cv::VideoCapture）对象中获取每一帧

9.picName = frame.clone();
克隆frame（原图像）

10.cv::getStructuringElement
kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
创建一个 3×3 的矩形结构元素（内核）

10.1 cv::MORPH_RECT - 矩形形状
// 矩形结构元素的特点：
// - 所有元素都是 1
// - 形状为矩形
// - 最常用的结构元素

10.2 cv::Size(3, 3) - 内核大小
// Size(width, height) 参数说明：
// - width: 内核宽度（列数）
// - height: 内核高度（行数）
// - 3x3 是最常用的尺寸 

11.cv::dilate() 膨胀操作

函数原型：
void dilate(
    InputArray src,      // 输入图像（支持多通道）
    OutputArray dst,     // 输出图像
    InputArray kernel,   // 结构元素（内核）
    Point anchor = Point(-1,-1),  // 锚点，默认中心
    int iterations = 1,           // 迭代次数
    int borderType = BORDER_CONSTANT,  // 边界类型
    const Scalar& borderValue = morphologyDefaultBorderValue()  // 边界值
);

cv::dilate(image, result, kernel);
             ↑       ↑       ↑
            输入    输出     内核

12.cv::threshold 二值化

函数原型：
double threshold(
    InputArray src,       // 输入图像（单通道）
    OutputArray dst,      // 输出图像
    double thresh,        // 阈值
    double maxval,        // 最大值
    int type              // 阈值类型
);

cv::threshold(result_Copy, result_Copy, 128, 255, cv::THRESH_BINARY);
                   ↑             ↑       ↑    ↑          ↑
                  输入          输出    阈值 最大值       类型
灰度值分布：
0 ─── 128 ─── 255
黑       灰       白

13.vector
外层：vector - 存储所有轮廓
内层：vector<cv::Point> - 存储单个轮廓的点

std::vector<std::vector<cv::Point>> contours;
// 可以理解为：
// contours = [
//     [Point, Point, Point, ...],  // 第1个轮廓
//     [Point, Point, Point, ...],  // 第2个轮廓  
//     [Point, Point, Point, ...],  // 第3个轮廓
//     ...
// ]

cv::findContours()



































