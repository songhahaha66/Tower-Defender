#include "Bullet.h"
#include <cmath>

// 静态成员初始化
IMAGE Bullet::bulletImage;
bool Bullet::imageLoaded = false;

Bullet::Bullet(int startX, int startY, int dmg, double spd, Enemy* tar)
    : GameObject(startX, startY, 10, 10), damage(dmg), speed(spd), target(tar) {
    
    // 确保图片已加载
    if (!imageLoaded) {
        loadImage();
    }
}

void Bullet::loadImage() {
    if (!imageLoaded) {
        loadimage(&bulletImage, _T("images/bullet.png"));
        imageLoaded = true;
    }
}

void Bullet::draw() {
    if (!active) return;
    
    // 使用图片绘制子弹
    putimage(x - width/2, y - height/2, &bulletImage);
}

void Bullet::update() {
    if (!active || target == nullptr || !target->active) {
        active = false;
        return;
    }

    // ׷�ٵ���
    double dx = target->x + target->width / 2 - x;
    double dy = target->y + target->height / 2 - y;
    double distance = sqrt(dx * dx + dy * dy);

    if (distance < speed) {
        target->takeDamage(damage);
        active = false; // ����Ŀ�����ʧ
    }
    else {
        x += static_cast<int>(speed * dx / distance);
        y += static_cast<int>(speed * dy / distance);
    }
}