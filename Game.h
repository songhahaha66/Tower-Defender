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

    void run(); // ��Ϸ��ѭ��
private:
    // ��Ϸ���ڳߴ�
    const int screenWidth = 800;
    const int screenHeight = 600;

    // ��Ϸ��������
    std::vector<Enemy*> enemies;
    std::vector<Tower*> towers;
    std::vector<Bullet*> bullets;
    std::vector<PathPoint> enemyPath; // �����ƶ�·��

    // ��Ϸ״̬
    int money;
    int lives;
    int wave;
    int enemiesToSpawn;
    int spawnTimer;

    // Private methods
    void init();        // Initialize game
    void handleInput(); // Handle user input
    void update();      // Update game state
    void draw();        // Draw game graphics
    void spawnWave();   // Spawn enemy wave
    void cleanup();     // Clean up inactive objects
    void drawUI();      // Draw user interface
    void drawBackground(); // Draw background with decorations
    void drawPath();    // Draw enhanced enemy path
};