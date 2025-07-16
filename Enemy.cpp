#include "Enemy.h"
#include <cmath>

// 声明透明图片显示函数
extern void putimage_alpha(int x, int y, IMAGE* img);

// 静态成员初始化
IMAGE Enemy::enemy1Image;
IMAGE Enemy::enemy2Image;
bool Enemy::imagesLoaded = false;

Enemy::Enemy(int startX, int startY, int w, int h, int hp, double spd)
    : GameObject(startX, startY, w, h), health(hp), maxHealth(hp), speed(spd), pathIndex(0) {
    
    // 随机选择敌人类型
    enemyType = (rand() % 2) + 1; // 1 或 2
    
    // 确保图片已加载
    if (!imagesLoaded) {
        loadImages();
    }
}

void Enemy::loadImages() {
    if (!imagesLoaded) {
        loadimage(&enemy1Image, _T("images/ememy1.png"));  // 注意：文件名是ememy1，不是enemy1
        loadimage(&enemy2Image, _T("images/enemy2.png"));
        imagesLoaded = true;
    }
}

void Enemy::draw() {
    if (!active) return;

    // 根据类型绘制不同的敌人图片 - 使用透明渲染
    if (enemyType == 1) {
        putimage_alpha(x, y, &enemy1Image);
    } else {
        putimage_alpha(x, y, &enemy2Image);
    }

    // 绘制血条
    setfillcolor(RGB(50, 50, 50)); // 深灰色背景
    fillrectangle(x, y - 8, x + width, y - 3);
    
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
    fillrectangle(x, y - 8, x + (int)(width * healthRatio), y - 3);
}

void Enemy::update(const std::vector<PathPoint>& path) {
    if (!active || pathIndex >= path.size() - 1) {
        if (pathIndex >= path.size() - 1) {
            active = false; // �����յ㣬����Ϊ���
        }
        return;
    }

    // �ƶ�����һ��·����
    PathPoint target = path[pathIndex + 1];
    double dx = target.x - x;
    double dy = target.y - y;
    double distance = sqrt(dx * dx + dy * dy);

    if (distance < speed) {
        x = target.x;
        y = target.y;
        pathIndex++;
    }
    else {
        x += static_cast<int>(speed * dx / distance);
        y += static_cast<int>(speed * dy / distance);
    }
}

void Enemy::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        active = false; // ����ֵ�ľ�������
    }
}