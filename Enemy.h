#pragma once
#include "GameObject.h"
#include <vector>
#include <graphics.h>

// 敌人路径点
struct PathPoint {
    int x, y;
};

class Enemy : public GameObject
{
public:
    int health;       // 生命值
    int maxHealth;    // 最大生命值
    double speed;     // 移动速度
    int pathIndex;    // 当前路径点索引
    int reward;       // 击杀奖励金币
    
    // 静态图片资源
    static IMAGE enemy1Image;
    static IMAGE enemy2Image;
    static IMAGE enemy3Image;
    static IMAGE enemy4Image;
    static bool imagesLoaded;
    
    int enemyType;    // 敌人类型 (1-4)

    Enemy(int startX, int startY, int w, int h, int hp, double spd, int type, int gold);

    // 静态工厂方法创建不同类型的敌人
    static Enemy* createEnemy(int type, int startX, int startY);
    
    static void loadImages(); // 加载图片资源
    void draw() override;
    void update(const std::vector<PathPoint>& path); // 根据路径更新位置
    void takeDamage(int damage); // 受到伤害
    int getReward() const { return reward; } // 获取奖励金币
};