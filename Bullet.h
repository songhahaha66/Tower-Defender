#pragma once
#include "GameObject.h"
#include "Enemy.h"
#include <graphics.h>

// 子弹类
class Bullet : public GameObject
{
public:
    int damage;      // 伤害值
    double speed;    // 移动速度
    Enemy* target;   // 追踪目标
    
    static IMAGE bulletImage;
    static bool imageLoaded;

    Bullet(int startX, int startY, int dmg, double spd, Enemy* tar);

    static void loadImage();
    void draw() override;
    void update() override;
};