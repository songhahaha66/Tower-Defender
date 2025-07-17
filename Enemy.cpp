#include "Enemy.h"
#include <cmath>

extern void putimage_alpha(int x, int y, IMAGE* img);

// 静态图片资源初始化
IMAGE Enemy::enemy1Image;
IMAGE Enemy::enemy2Image;
IMAGE Enemy::enemy3Image;
IMAGE Enemy::enemy4Image;
bool Enemy::imagesLoaded = false;

// 敌人构造函数
Enemy::Enemy(int startX, int startY, int w, int h, int hp, double spd, int type, int gold)
    : GameObject(startX, startY, w, h), health(hp), maxHealth(hp), speed(spd), pathIndex(0), enemyType(type), reward(gold) {
    
    if (!imagesLoaded) {
        loadImages();
    }
}

// 工厂方法：根据类型创建不同属性的敌人
Enemy* Enemy::createEnemy(int type, int startX, int startY) {
    Enemy* enemy = nullptr;
    
    switch (type) {
        case 1: // 普通敌人
            enemy = new Enemy(0, 0, 32, 32, 100, 1.5, 1, 10);
            break;
        case 2: // 快速敌人
            enemy = new Enemy(0, 0, 32, 32, 60, 3.0, 2, 15);
            break;
        case 3: // 重甲敌人
            enemy = new Enemy(0, 0, 32, 32, 200, 0.8, 3, 25);
            break;
        case 4: // 精英敌人
            enemy = new Enemy(0, 0, 32, 32, 300, 1.8, 4, 50);
            break;
        default:
            enemy = new Enemy(0, 0, 32, 32, 100, 1.5, 1, 10);
            break;
    }
    
    // 设置敌人位置，使其中心对齐到起始点
    if (enemy) {
        enemy->x = startX - 16;
        enemy->y = startY - 16;
    }
    
    return enemy;
}

// 加载敌人图片资源
void Enemy::loadImages() {
    if (!imagesLoaded) {
        loadimage(&enemy1Image, _T("images/enemy1.png")); 
        loadimage(&enemy2Image, _T("images/enemy2.png"));
        loadimage(&enemy3Image, _T("images/enemy3.png"));
        loadimage(&enemy4Image, _T("images/enemy4.png"));
        imagesLoaded = true;
    }
}

// 绘制敌人
void Enemy::draw() {
    if (!active) return;

    // 根据敌人类型选择对应图片
    IMAGE* currentImage = nullptr;
    switch (enemyType) {
        case 1:
            currentImage = &enemy1Image;
            break;
        case 2:
            currentImage = &enemy2Image;
            break;
        case 3:
            currentImage = &enemy3Image;
            break;
        case 4:
            currentImage = &enemy4Image;
            break;
        default:
            currentImage = &enemy1Image;
            break;
    }
    
    // 绘制敌人图片
    if (currentImage) {
        putimage_alpha(x, y, currentImage);
    }

    // 绘制血条背景
    setfillcolor(RGB(50, 50, 50));
    fillrectangle(x + 16 , y + 8, x + 16 + 32, y + 16 - 3);
    
    // 根据血量设置血条颜色
    double healthRatio = (double)health / maxHealth;
    COLORREF healthColor;
    if (healthRatio > 0.6) {
        healthColor = RGB(50, 200, 50);
    } else if (healthRatio > 0.3) {
        healthColor = RGB(255, 255, 50);
    } else {
        healthColor = RGB(255, 50, 50);
    }
    
    // 绘制当前血量
    setfillcolor(healthColor);
    fillrectangle(x + 16, y + 16 - 8, x + 16  + (int)(32 * healthRatio), y + 16 - 3);
    
    // 根据敌人类型绘制不同颜色边框
    switch (enemyType) {
        case 1:
            setlinecolor(RGB(255, 255, 255));
            break;
        case 2:
            setlinecolor(RGB(0, 255, 0));
            break;
        case 3:
            setlinecolor(RGB(0, 100, 255));
            break;
        case 4:
            setlinecolor(RGB(255, 215, 0));
            break;
        default:
            setlinecolor(RGB(255, 255, 255));
            break;
    }
    rectangle(x +16, y +16, x + 32 +16, y + 32 +16);
}

// 更新敌人位置，沿路径移动
void Enemy::update(const std::vector<PathPoint>& path) {
    if (!active) {
        return;
    }

    // 如果已经到达或超过倒数第二个路径点，说明目标是终点
    if (pathIndex >= path.size() - 1) {
        active = false; // 已经完成路径，直接标记为非活跃
        return;
    }

    PathPoint target = path[pathIndex + 1];
    
    // 计算敌人中心点到目标点的距离
    double centerX = x + 16;
    double centerY = y + 16;
    double dx = target.x - centerX;
    double dy = target.y - centerY;
    double distance = sqrt(dx * dx + dy * dy);

    // 检查是否到达当前目标点
    if (distance <= speed) {
        x = target.x - 16;
        y = target.y - 16;
        pathIndex++;
        
        // 如果刚刚到达了终点，立即标记为非活跃
        if (pathIndex >= path.size() - 1) {
            active = false;
        }
    }
    else {
        // 按速度向目标点移动
        double moveX = speed * dx / distance;
        double moveY = speed * dy / distance;
        
        x = static_cast<int>(x + moveX);
        y = static_cast<int>(y + moveY);
    }
}

// 敌人受到伤害
void Enemy::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        active = false; // 血量归零，标记为非活跃
    }
}