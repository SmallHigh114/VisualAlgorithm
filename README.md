# VisualAlgorithm-In-School-Competition




# Lambda 表达式：C++ 的匿名函数工具

Lambda 表达式是 C++11 引入的匿名函数对象，它允许你在需要函数的地方内联定义函数，而不必单独声明。

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
