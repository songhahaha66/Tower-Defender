#pragma once
#include "GameObject.h"
#include "Enemy.h"
#include <graphics.h>

class Bullet : public GameObject
{
public:
    int damage;      // 伤害值
    double speed;    // 移动速度
    Enemy* target;   // 追踪的目标敌人
    
    // 静态图片资源
    static IMAGE bulletImage;
    static bool imageLoaded;

    Bullet(int startX, int startY, int dmg, double spd, Enemy* tar);

    static void loadImage(); // 加载图片资源
    void draw() override;
    void update() override;
};