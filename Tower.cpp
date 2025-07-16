#include "Tower.h"
#include <cmath>

// 静态成员初始化
IMAGE Tower::towerWithBullet;
IMAGE Tower::towerWithoutBullet;
bool Tower::imagesLoaded = false;

Tower::Tower(int x, int y, int dmg, int rng, int rate)
    : GameObject(x, y, 40, 40), damage(dmg), range(rng), fireRate(rate), fireCountdown(0), hasBullet(true) {
    
    // 确保图片已加载
    if (!imagesLoaded) {
        loadImages();
    }
}

void Tower::loadImages() {
    if (!imagesLoaded) {
        loadimage(&towerWithBullet, _T("images/tower.png"));
        loadimage(&towerWithoutBullet, _T("images/tower_without_bullet.png"));
        imagesLoaded = true;
    }
}

void Tower::draw() {
    // 根据是否有子弹状态选择图片
    if (hasBullet) {
        putimage(x, y, &towerWithBullet);
    } else {
        putimage(x, y, &towerWithoutBullet);
    }

    // 可选：绘制攻击范围（调试用）
    // setlinecolor(LIGHTGRAY);
    // setlinestyle(PS_DOT);
    // circle(x + width / 2, y + height / 2, range);
    // setlinestyle(PS_SOLID);
}

void Tower::update(std::vector<Enemy*>& enemies, std::vector<Bullet*>& bullets) {
    if (fireCountdown > 0) {
        fireCountdown--;
    }

    if (fireCountdown <= 0) {
        Enemy* target = findTarget(enemies);
        if (target) {
            // 发射子弹时切换到无子弹状态
            hasBullet = false;
            bullets.push_back(new Bullet(x + width / 2, y + height / 2, damage, 10.0, target));
            fireCountdown = fireRate; // 重置开火倒计时
        }
    } else {
        // 倒计时过半时恢复有子弹状态
        if (fireCountdown <= fireRate / 2) {
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