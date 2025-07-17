#include "Enemy.h"
#include <cmath>
#include <iostream>
// 声明透明图片显示函数
extern void putimage_alpha(int x, int y, IMAGE* img);

// 静态成员初始化
IMAGE Enemy::enemy1Image;
IMAGE Enemy::enemy2Image;
IMAGE Enemy::enemy3Image;
IMAGE Enemy::enemy4Image;
bool Enemy::imagesLoaded = false;

Enemy::Enemy(int startX, int startY, int w, int h, int hp, double spd, int type, int gold)
    : GameObject(startX, startY, w, h), health(hp), maxHealth(hp), speed(spd), pathIndex(0), enemyType(type), reward(gold) {
    
    // 确保图片已加载
    if (!imagesLoaded) {
        loadImages();
    }
}

// 静态工厂方法创建不同类型的敌人
Enemy* Enemy::createEnemy(int type, int startX, int startY) {
    Enemy* enemy = nullptr;
    // 统一使用32x32的基础尺寸，后续在draw中根据图片调整
    switch (type) {
        case 1: // 普通敌人 - 平衡型
            enemy = new Enemy(0, 0, 32, 32, 100, 1.5, 1, 10);
            break;
        case 2: // 快速敌人 - 速度快但血量少
            enemy = new Enemy(0, 0, 32, 32, 60, 3.0, 2, 15);
            break;
        case 3: // 重甲敌人 - 血量高但速度慢
            enemy = new Enemy(0, 0, 32, 32, 200, 0.8, 3, 25);
            break;
        case 4: // 精英敌人 - 各项属性都很强
            enemy = new Enemy(0, 0, 32, 32, 300, 1.8, 4, 50);
            break;
        default:
            enemy = new Enemy(0, 0, 32, 32, 100, 1.5, 1, 10);
            break;
    }
    
    // 设置敌人位置，使其中心对齐到起始点
    if (enemy) {
        enemy->x = startX - 16; // 32/2 = 16
        enemy->y = startY - 16;
    }
    
    return enemy;
}

void Enemy::loadImages() {
    if (!imagesLoaded) {
        loadimage(&enemy1Image, _T("images/enemy1.png")); 
        loadimage(&enemy2Image, _T("images/enemy2.png"));
        loadimage(&enemy3Image, _T("images/enemy3.png"));
        loadimage(&enemy4Image, _T("images/enemy4.png"));
        imagesLoaded = true;
    }
}

void Enemy::draw() {
    if (!active) return;

    // 根据类型绘制不同的敌人图片 - 使用透明渲染
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
    
    if (currentImage) {
        putimage_alpha(x, y, currentImage);
    }

    // 绘制血条背景 - 调整位置使其显示在敌人正上方
    setfillcolor(RGB(50, 50, 50)); // 深灰色背景
    fillrectangle(x + 16 , y + 8, x + 16 + 32, y + 16 - 3); // 血条在敌人中心上方
    
    // 血条颜色根据血量变化
    double healthRatio = (double)health / maxHealth;
    COLORREF healthColor;
    if (healthRatio > 0.6) {
        healthColor = RGB(50, 200, 50);  // 绿色
    } else if (healthRatio > 0.3) {
        healthColor = RGB(255, 255, 50); // 黄色
    } else {
        healthColor = RGB(255, 50, 50);  // 红色
    }
    
    setfillcolor(healthColor);
    fillrectangle(x + 16, y + 16 - 8, x + 16  + (int)(32 * healthRatio), y + 16 - 3);
    
    // 为不同类型的敌人绘制不同颜色的边框
    switch (enemyType) {
        case 1: // 普通敌人 - 白色边框
            setlinecolor(RGB(255, 255, 255));
            break;
        case 2: // 快速敌人 - 绿色边框
            setlinecolor(RGB(0, 255, 0));
            break;
        case 3: // 重甲敌人 - 蓝色边框
            setlinecolor(RGB(0, 100, 255));
            break;
        case 4: // 精英敌人 - 金色边框
            setlinecolor(RGB(255, 215, 0));
            break;
        default:
            setlinecolor(RGB(255, 255, 255));
            break;
    }
    rectangle(x +16, y +16, x + 32 +16, y + 32 +16);
}

void Enemy::update(const std::vector<PathPoint>& path) {
    if (!active || pathIndex >= path.size() - 1) {
        if (pathIndex >= path.size() - 1) {
            active = false; // �����յ㣬����Ϊ���
        }
        return;
    }

    // 移动到下一个路径点
    PathPoint target = path[pathIndex + 1];
    
    // 计算敌人中心点到目标点的距离
    double centerX = x + 16; // 使用固定的16像素偏移(32/2)
    double centerY = y + 16;
    double dx = target.x - centerX;
    double dy = target.y - centerY;
    double distance = sqrt(dx * dx + dy * dy);

    // 如果距离很小或者已经超过了目标点，直接跳到下一个路径点
    if (distance <= speed + 1) { // 增加一点容错范围
        // 到达路径点，将敌人中心对齐到路径点
        x = target.x - 16;
        y = target.y - 16;
        pathIndex++;
    }
    else {
        // 按照速度移动，确保移动方向正确
        double moveX = speed * dx / distance;
        double moveY = speed * dy / distance;
        
        // 使用浮点数计算确保精度
        x = static_cast<int>(x + moveX + 0.5); // +0.5 用于四舍五入
        y = static_cast<int>(y + moveY + 0.5);
    }
}

void Enemy::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        active = false; // ����ֵ�ľ�������
    }
}