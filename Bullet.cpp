#include "Bullet.h"
#include <cmath>

extern void putimage_alpha(int x, int y, IMAGE* img);

// 静态图片资源初始化
IMAGE Bullet::bulletImage;
bool Bullet::imageLoaded = false;

// 子弹构造函数
Bullet::Bullet(int startX, int startY, int dmg, double spd, Enemy* tar)
    : GameObject(startX, startY, 15, 15), damage(dmg), speed(spd), target(tar){
    
    if (!imageLoaded) {
        loadImage();
    }
}

// 加载子弹图片
void Bullet::loadImage() {
    if (!imageLoaded) {
        loadimage(&bulletImage, _T("images/bullet.png"),15,15);
        imageLoaded = true;
    }
}

// 绘制子弹
void Bullet::draw() {
    if (!active) return;
    putimage_alpha(x - width/2, y - height/2, &bulletImage);
}

// 更新子弹位置，追踪目标
void Bullet::update() {
    if (!active || target == nullptr || !target->active) {
        active = false;
        return;
    }

    // 计算到目标的距离和方向
    double dx = target->x + target->width / 2 - x;
    double dy = target->y + target->height / 2 - y;
    double distance = sqrt(dx * dx + dy * dy);

    if (distance < speed) {
        target->takeDamage(damage);
        active = false; // 击中目标，子弹消失
    }
    else {
        // 向目标移动
        x += static_cast<int>(speed * dx / distance);
        y += static_cast<int>(speed * dy / distance);
    }
}