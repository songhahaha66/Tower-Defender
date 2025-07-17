#include "GameObject.h"

// 游戏对象构造函数
GameObject::GameObject(int x, int y, int w, int h) : x(x), y(y), width(w), height(h), active(true) {}

// 游戏对象析构函数
GameObject::~GameObject() {}

// 默认更新函数，派生类可重写
void GameObject::update() {
    // 基类默认无特定更新逻辑
}