#pragma once // 防止头文件被重复包含

#include <graphics.h> // 包含 EasyX 图形库

// 游戏对象基类
class GameObject
{
public:
    int x, y; // 对象的坐标
    int width, height; // 对象的宽高
    bool active; // 对象是否活动/存活

    GameObject(int x, int y, int w, int h);
    virtual ~GameObject(); // 虚析构函数，保证派生类对象能被正确销毁

    // 纯虚函数，要求所有派生类都必须实现自己的绘制方法
    virtual void draw() = 0;
    // 虚函数，派生类可以根据需要重写更新逻辑
    virtual void update();
};