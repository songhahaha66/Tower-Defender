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
    : GameObject(startX, startY, w, h), health(hp), maxHealth(hp), speed(spd), pathIndex(0), enemyType(type), reward(gold), preciseX(startX), preciseY(startY) {
    
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
    
    // 设置敌人位置，敌人中心对齐到路径点
    if (enemy) {
        enemy->preciseX = startX; // 敌人中心在路径点上
        enemy->preciseY = startY;
        enemy->x = (int)enemy->preciseX;
        enemy->y = (int)enemy->preciseY;
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
    
    // 绘制敌人图片 (中心点对齐到路径)
    if (currentImage) {
        putimage_alpha(x-32, y-32, currentImage);
    }

    // 绘制血条背景 (基于中心点坐标)
    setfillcolor(RGB(50, 50, 50));
    fillrectangle(x - 16, y - 24, x + 16, y - 19);
    
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
    fillrectangle(x - 16, y - 24, x - 16 + (int)(32 * healthRatio), y - 19);
    
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
    rectangle(x - 16, y - 16, x + 16, y + 16);
}

// 更新敌人位置，精确按路径移动
void Enemy::update(const std::vector<PathPoint>& path) {
    if (!active || path.empty()) {
        return;
    }

    // 如果已经到达终点
    if (pathIndex >= path.size() - 1) {
        active = false;
        return;
    }

    PathPoint target = path[pathIndex + 1];
    
    // 计算敌人中心点到目标点的距离和方向（现在preciseX,preciseY就是中心点）
    double dx = target.x - preciseX;
    double dy = target.y - preciseY;
    double distance = sqrt(dx * dx + dy * dy);

    // 如果距离小于等于速度，直接移动到目标点
    if (distance <= speed) {
        preciseX = target.x; // 敌人中心对齐到路径点
        preciseY = target.y;
        pathIndex++;
    } else {
        // 按比例移动，保持浮点精度
        double ratio = speed / distance;
        preciseX += dx * ratio;
        preciseY += dy * ratio;
    }
    
    // 更新整数坐标用于绘制
    x = (int)preciseX;
    y = (int)preciseY;
}

// 敌人受到伤害
void Enemy::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        active = false; // 血量归零，标记为非活跃
    }
}