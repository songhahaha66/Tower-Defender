#pragma once
#include "Enemy.h"
#include "Tower.h"
#include "Bullet.h"
#include <vector>
#include <graphics.h>

class Game
{
public:
    Game();
    ~Game();

    void run(); // 游戏主循环
    
private:
    // 游戏状态枚举
    enum GameState {
        MENU,    // 开局画面
        PLAYING, // 游戏中
        GAME_OVER // 游戏结束
    };
    
    // 游戏窗口尺寸
    const int screenWidth = 800;
    const int screenHeight = 600;

    // 游戏对象容器
    std::vector<Enemy*> enemies;
    std::vector<Tower*> towers;
    std::vector<Bullet*> bullets;
    std::vector<PathPoint> enemyPath; // 敌人移动路径

    // 游戏状态
    GameState gameState;
    int money;
    int lives;
    int wave;
    int enemiesToSpawn;
    int spawnTimer;

    // Private methods
    void init();        // Initialize game
    void handleInput(); // Handle user input
    void handleMenuInput(); // Handle menu input
    void update();      // Update game state
    void draw();        // Draw game graphics
    void drawMenu();    // Draw menu screen
    void spawnWave();   // Spawn enemy wave
    void cleanup();     // Clean up inactive objects
    void drawUI();      // Draw user interface
    void drawBackground(); // Draw background with decorations
    void drawPath();    // Draw enhanced enemy path
    void generateRandomPath(); // Generate random enemy path
};