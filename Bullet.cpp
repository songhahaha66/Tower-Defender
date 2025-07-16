#include "Bullet.h"
#include <cmath>
#define M_PI 3.14159265358979323846

// 声明透明图片显示函数
extern void putimage_alpha(int x, int y, IMAGE* img);

// 静态成员初始化
IMAGE Bullet::bulletImage;
bool Bullet::imageLoaded = false;

Bullet::Bullet(int startX, int startY, int dmg, double spd, Enemy* tar)
    : GameObject(startX, startY, 15, 15), damage(dmg), speed(spd), target(tar){
    
    // 确保图片已加载
    if (!imageLoaded) {
        loadImage();
    }
}

void Bullet::loadImage() {
    if (!imageLoaded) {
        loadimage(&bulletImage, _T("images/bullet.png"),15,15);
        imageLoaded = true;
    }
}

void Bullet::draw() {
    if (!active) return;
    putimage_alpha(x - width/2, y - height/2, &bulletImage);
}

void Bullet::update() {
    if (!active || target == nullptr || !target->active) {
        active = false;
        return;
    }

    // 追踪敌人并计算旋转角度
    double dx = target->x + target->width / 2 - x;
    double dy = target->y + target->height / 2 - y;
    double distance = sqrt(dx * dx + dy * dy);


    if (distance < speed) {
        target->takeDamage(damage);
        active = false; // 击中目标，消失
    }
    else {
        x += static_cast<int>(speed * dx / distance);
        y += static_cast<int>(speed * dy / distance);
    }
}