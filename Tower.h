#pragma once
#include "GameObject.h"
#include "Enemy.h"
#include "Bullet.h"
#include <vector>
#include <graphics.h>

// 防御塔类
class Tower : public GameObject
{
public:
    int damage;          // 伤害值
    int range;           // 攻击范围
    int fireRate;        // 射速（帧数间隔）
    int fireCountdown;   // 射击冷却
    bool hasBullet;      // 子弹状态
    
    static IMAGE towerWithBullet;
    static IMAGE towerWithoutBullet;
    static bool imagesLoaded;

    Tower(int x, int y, int dmg, int rng, int rate);
    
    static void loadImages();
    void draw() override;
    void update(std::vector<Enemy*>& enemies, std::vector<Bullet*>& bullets);
    Enemy* findTarget(std::vector<Enemy*>& enemies);
};