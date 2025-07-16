#pragma once
#include "GameObject.h"
#include "Enemy.h"
#include "Bullet.h"
#include <vector>
#include <graphics.h>

class Tower : public GameObject
{
public:
    int damage;          // 伤害值
    int range;           // 攻击范围
    int fireRate;        // 射速 (多少帧射击一次)
    int fireCountdown;   // 射击倒计时
    bool hasBullet;      // 是否有子弹状态
    
    // 静态图片资源
    static IMAGE towerWithBullet;
    static IMAGE towerWithoutBullet;
    static bool imagesLoaded;

    Tower(int x, int y, int dmg, int rng, int rate);
    
    static void loadImages(); // 加载图片资源
    void draw() override;
    void update(std::vector<Enemy*>& enemies, std::vector<Bullet*>& bullets);
    Enemy* findTarget(std::vector<Enemy*>& enemies); // 寻找目标
};