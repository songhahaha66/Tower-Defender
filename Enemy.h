#pragma once
#include "GameObject.h"
#include <vector>
#include <graphics.h>

// 路径点结构
struct PathPoint {
    int x, y;
};

// 敌人类
class Enemy : public GameObject
{
public:
    int health;
    int maxHealth;
    double speed;
    int pathIndex;
    int reward;
    int enemyType;
    
    static IMAGE enemy1Image;
    static IMAGE enemy2Image;
    static IMAGE enemy3Image;
    static IMAGE enemy4Image;
    static bool imagesLoaded;

    Enemy(int startX, int startY, int w, int h, int hp, double spd, int type, int gold);

    static Enemy* createEnemy(int type, int startX, int startY);
    static void loadImages();
    void draw() override;
    void update(const std::vector<PathPoint>& path);
    void takeDamage(int damage);
    int getReward() const { return reward; }
};