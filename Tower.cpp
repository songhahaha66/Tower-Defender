#include "Tower.h"
#include <iostream>
#include <cmath>
#define M_PI 3.14159265358979323846
// 声明透明图片显示函数
extern void putimage_alpha(int x, int y, IMAGE* img);

// 静态成员初始化
IMAGE Tower::towerWithBullet;
IMAGE Tower::towerWithoutBullet;
bool Tower::imagesLoaded = false;

Tower::Tower(int x, int y, int dmg, int rng, int rate)
    : GameObject(x, y, 40, 40), damage(dmg), range(rng), fireRate(rate), fireCountdown(0), 
      hasBullet(true) {
    
    // 确保图片已加载
    if (!imagesLoaded) {
        loadImages();
    }
}

void Tower::loadImages() {
    // 无论之前是否加载过，都重新加载以确保图像有效
    loadimage(&towerWithBullet, _T("images/tower.png"),70,70);
    loadimage(&towerWithoutBullet, _T("images/tower_without_bullet.png"),70,70);
    
    // 检查图像是否成功加载
    if (towerWithBullet.getwidth() > 0 && towerWithBullet.getheight() > 0 && 
        towerWithoutBullet.getwidth() > 0 && towerWithoutBullet.getheight() > 0) {
        imagesLoaded = true;
    } else {
        // 如果加载失败，输出错误信息（实际生产环境可以使用日志系统）
        TCHAR error[128];
        _stprintf_s(error, _T("加载防御塔图像失败，请检查图像文件路径！"));
        outtextxy(10, 10, error);
    }
}


void Tower::draw() {
    // 简单绘制原图，不旋转
    putimage_alpha(x, y, hasBullet ? &towerWithBullet : &towerWithoutBullet);
}

void Tower::update(std::vector<Enemy*>& enemies, std::vector<Bullet*>& bullets) {
    Enemy* target = findTarget(enemies);
    
    // 射击逻辑
    if (fireCountdown > 0) {
        fireCountdown--;
    }

    if (fireCountdown <= 0) {
        if (target) {
            // 简单从防御塔中心发射子弹
            bullets.push_back(new Bullet(x + width / 2, y + height / 2, damage, 10.0, target));
            fireCountdown = fireRate; // 重置开火倒计时
            hasBullet = false; // 发射后设置无子弹状态
        }
    } else {
        // 倒计时过半时恢复有子弹状态
        if (fireCountdown <= fireRate / 2 && !hasBullet) {
            hasBullet = true;
        }
    }
}

Enemy* Tower::findTarget(std::vector<Enemy*>& enemies) {
    Enemy* closestEnemy = nullptr;
    double minDistance = range + 1.0;

    for (auto& enemy : enemies) {
        if (enemy->active) {
            double dx = (enemy->x + enemy->width / 2) - (x + width / 2);
            double dy = (enemy->y + enemy->height / 2) - (y + height / 2);
            double distance = sqrt(dx * dx + dy * dy);

            if (distance <= range && distance < minDistance) {
                minDistance = distance;
                closestEnemy = enemy;
            }
        }
    }
    return closestEnemy;
}