#pragma once

#include <graphics.h>

// 游戏对象基类
class GameObject
{
public:
    int x, y;           // 对象位置
    int width, height;  // 对象尺寸
    bool active;        // 是否活跃

    GameObject(int x, int y, int w, int h);
    virtual ~GameObject();

    // 纯虚函数，所有派生类必须实现绘制方法
    virtual void draw() = 0;
    // 虚函数，派生类可根据需要重写更新逻辑
    virtual void update();
};