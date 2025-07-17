#include "Tower.h"
#include <cmath>

extern void putimage_alpha(int x, int y, IMAGE* img);

// 静态图片资源初始化
IMAGE Tower::towerWithBullet;
IMAGE Tower::towerWithoutBullet;
bool Tower::imagesLoaded = false;

// 防御塔构造函数
Tower::Tower(int x, int y, int dmg, int rng, int rate)
    : GameObject(x, y, 40, 40), damage(dmg), range(rng), fireRate(rate), fireCountdown(0), 
      hasBullet(true) {
    
    if (!imagesLoaded) {
        loadImages();
    }
}

// 加载防御塔图片资源
void Tower::loadImages() {
    loadimage(&towerWithBullet, _T("images/tower.png"),70,70);
    loadimage(&towerWithoutBullet, _T("images/tower_without_bullet.png"),70,70);
    
    // 检查图片是否加载成功
    if (towerWithBullet.getwidth() > 0 && towerWithBullet.getheight() > 0 && 
        towerWithoutBullet.getwidth() > 0 && towerWithoutBullet.getheight() > 0) {
        imagesLoaded = true;
    } else {
        TCHAR error[128];
        _stprintf_s(error, _T("防御塔图片加载失败，请检查图片路径！"));
        outtextxy(10, 10, error);
    }
}


// 绘制防御塔
void Tower::draw() {
    putimage_alpha(x, y, hasBullet ? &towerWithBullet : &towerWithoutBullet);
}

// 更新防御塔状态，处理射击逻辑
void Tower::update(std::vector<Enemy*>& enemies, std::vector<Bullet*>& bullets) {
    Enemy* target = findTarget(enemies);
    
    if (fireCountdown > 0) {
        fireCountdown--;
    }

    if (fireCountdown <= 0) {
        if (target) {
            // 从塔中心发射子弹
            bullets.push_back(new Bullet(x + width / 2, y + height / 2, damage, 10.0, target));
            fireCountdown = fireRate;
            hasBullet = false; // 射击后设置无子弹状态
        }
    } else {
        // 冷却过半时恢复子弹状态
        if (fireCountdown <= fireRate / 2 && !hasBullet) {
            hasBullet = true;
        }
    }
}

// 寻找攻击范围内最近的敌人
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