#pragma once
#include "Enemy.h"
#include "Tower.h"
#include "Bullet.h"
#include <vector>
#include <graphics.h>

// 游戏主类
class Game
{
public:
    Game();
    ~Game();

    void run();
    
private:
    // 游戏状态枚举
    enum GameState {
        MENU,
        PLAYING,
        GAME_OVER
    };
    
    // 窗口尺寸
    const int screenWidth = 800;
    const int screenHeight = 600;

    // 游戏对象容器
    std::vector<Enemy*> enemies;
    std::vector<Tower*> towers;
    std::vector<Bullet*> bullets;
    std::vector<PathPoint> enemyPath;

    // 游戏状态变量
    GameState gameState;
    int money;
    int lives;
    int wave;
    int enemiesToSpawn;
    int spawnTimer;

    // 私有方法
    void init();
    void handleInput();
    void handleMenuInput();
    void update();
    void draw();
    void drawMenu();
    void spawnWave();
    void cleanup();
    void drawUI();
    void drawBackground();
    void drawPath();
    void generateRandomPath();
};