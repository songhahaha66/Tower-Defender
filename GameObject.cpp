#include "GameObject.h"

GameObject::GameObject(int x, int y, int w, int h) : x(x), y(y), width(w), height(h), active(true) {}

GameObject::~GameObject() {}

void GameObject::update() {
    // 基类中没有特定的更新逻辑
}