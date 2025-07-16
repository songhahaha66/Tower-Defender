#include "Game.h"
#include <conio.h> // for _kbhit()
#include <algorithm> // for std::remove_if
#include <cmath> // for sin, sqrt
#pragma comment(lib, "MSIMG32.LIB")  // 需要这个库来支持AlphaBlend
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 显示透明图片的函数 - 完美解决黑边问题
void putimage_alpha(int x, int y, IMAGE* img)
{
    int w = img->getwidth();
    int h = img->getheight();
    AlphaBlend(GetImageHDC(NULL), x, y, w, h, GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
}

Game::Game() : money(20000), lives(10), wave(0), enemiesToSpawn(0), spawnTimer(0){}

Game::~Game() {
    // Release all dynamically allocated memory
    for (auto p : enemies) delete p;
    for (auto p : towers) delete p;
    for (auto p : bullets) delete p;
}

void Game::init() {
    // Initialize game graphics
    initgraph(screenWidth, screenHeight);
    BeginBatchDraw(); // Enable batch drawing mode to prevent flickering

    // Load all game images
    Tower::loadImages();
    Enemy::loadImages();
    Bullet::loadImage();

    // Initialize enemy movement path
    enemyPath.push_back({ 0, 100 });
    enemyPath.push_back({ 200, 100 });
    enemyPath.push_back({ 200, 400 });
    enemyPath.push_back({ 600, 400 });
    enemyPath.push_back({ 600, 200 });
    enemyPath.push_back({ screenWidth, 200 });
}

void Game::run() {
    init();

    while (lives > 0) {
        handleInput();
        update();
        draw();
        FlushBatchDraw(); // Flush all drawing commands to screen
        Sleep(16); // Approximately 60 FPS
    }

    // Enhanced game over screen
    cleardevice();
    
    // Draw dark background
    setfillcolor(RGB(20, 20, 20));
    fillrectangle(0, 0, screenWidth, screenHeight);
    
    // Draw "GAME OVER" with shadow effect
    settextcolor(RGB(80, 0, 0)); // Dark red shadow
    settextstyle(80, 0, _T("Arial"));
    outtextxy(202, 252, _T("GAME OVER"));
    
    settextcolor(RGB(255, 50, 50)); // Bright red
    outtextxy(200, 250, _T("GAME OVER"));
    
    // Draw final stats
    settextcolor(RGB(200, 200, 200));
    settextstyle(24, 0, _T("Arial"));
    TCHAR finalStats[256];
    _stprintf_s(finalStats, _T("Final Wave: %d"), wave);
    outtextxy(280, 320, finalStats);
    
    _stprintf_s(finalStats, _T("Money Earned: $%d"), money);
    outtextxy(260, 350, finalStats);
    
    settextcolor(RGB(150, 150, 150));
    settextstyle(20, 0, _T("Arial"));
    outtextxy(300, 400, _T("Press any key to exit"));
    
    FlushBatchDraw();
    _getch(); // Wait for key press to exit

    EndBatchDraw();
    closegraph();
}

void Game::handleInput() {
    // Use GetAsyncKeyState to detect space key press
    static bool spacePressed = false;
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) { // Check if space key is pressed
        if (!spacePressed) { // Only trigger once per press
            if (enemiesToSpawn <= 0 && enemies.empty()) {
                wave++;
                enemiesToSpawn = 5 + wave * 2; // Each wave adds 2 more enemies
                spawnTimer = 60; // Shorter preparation time
            }
            spacePressed = true;
        }
    } else {
        spacePressed = false; // Reset when key is released
    }

    // Handle mouse input for placing towers
    MOUSEMSG m;
    while (MouseHit()) { // Process all mouse messages
        m = GetMouseMsg();
        if (m.uMsg == WM_LBUTTONDOWN) {
            if (money >= 100) { // Tower cost is 100
                money -= 100;
                towers.push_back(new Tower(m.x - 20, m.y - 20, 10, 150, 30)); // damage:10, range:150, cooldown:30 frames
            }
        }
    }
}

void Game::spawnWave() {
    if (enemiesToSpawn > 0) {
        spawnTimer--;
        if (spawnTimer <= 0) {
            enemies.push_back(new Enemy(enemyPath[0].x, enemyPath[0].y, 30, 30, 50 + wave * 10, 2.0 + wave * 0.1));
            enemiesToSpawn--;
            spawnTimer = 60; // Spawn one enemy every 60 frames
        }
    }
}

void Game::update() {
    spawnWave();

    // Update towers, enemies and bullets
    for (auto& tower : towers) {
        tower->update(enemies, bullets);
    }
    for (auto& bullet : bullets) {
        bullet->update();
    }
    for (auto& enemy : enemies) {
        enemy->update(enemyPath);
        if (!enemy->active && enemy->health > 0) { // Enemy reached end
            lives--;
        }
    }

    cleanup(); // Remove inactive/dead objects
}

void Game::cleanup() {
    // Remove inactive bullets
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet* b) {
        if (!b->active) {
            delete b;
            return true;
        }
        return false;
        }), bullets.end());

    // Remove inactive enemies
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [&](Enemy* e) {
        if (!e->active) {
            if (e->health <= 0) { // Enemy was killed
                money += 10; // Add money reward
            }
            delete e;
            return true;
        }
        return false;
        }), enemies.end());
}

void Game::draw() {
    cleardevice(); // Clear screen
    
    // Draw background gradient
    drawBackground();
    
    // Draw enemy path with better visual effects
    drawPath();

    // Draw all game objects with enhanced effects
    for (auto& tower : towers) tower->draw();
    for (auto& enemy : enemies) enemy->draw();
    for (auto& bullet : bullets) bullet->draw();

    drawUI(); // Draw UI
}

void Game::drawBackground() {
    // Create a gradient background
    for (int y = 0; y < screenHeight; y++) {
        int green = 20 + (y * 30) / screenHeight; // Gradient from dark to light green
        setlinecolor(RGB(10, green, 10));
        line(0, y, screenWidth, y);
    }
    
    // Draw some decorative elements
    setfillcolor(RGB(34, 139, 34)); // Forest green
    
    // Draw trees/bushes randomly
    for (int i = 0; i < 15; i++) {
        int x = (i * 73 + 50) % (screenWidth - 40);
        int y = (i * 97 + 80) % (screenHeight - 60);
        
        // Avoid drawing on the path
        bool onPath = false;
        for (size_t j = 0; j < enemyPath.size() - 1; j++) {
            int px1 = enemyPath[j].x, py1 = enemyPath[j].y;
            int px2 = enemyPath[j + 1].x, py2 = enemyPath[j + 1].y;
            
            // Simple distance check from point to line segment
            if ((abs(y - py1) < 40 && x >= min(px1, px2) - 40 && x <= max(px1, px2) + 40) ||
                (abs(x - px1) < 40 && y >= min(py1, py2) - 40 && y <= max(py1, py2) + 40)) {
                onPath = true;
                break;
            }
        }
        
        if (!onPath) {
            // Draw a simple tree
            fillcircle(x, y, 8 + i % 5); // Tree crown
            setfillcolor(RGB(101, 67, 33)); // Brown
            fillrectangle(x - 2, y + 5, x + 2, y + 15); // Tree trunk
            setfillcolor(RGB(34, 139, 34)); // Reset to green
        }
    }
}

void Game::drawPath() {
    // Draw path shadow first
    setlinecolor(RGB(80, 60, 40));
    setlinestyle(PS_SOLID, 25);
    for (size_t i = 0; i < enemyPath.size() - 1; ++i) {
        line(enemyPath[i].x + 2, enemyPath[i].y + 2, 
             enemyPath[i + 1].x + 2, enemyPath[i + 1].y + 2);
    }
    
    // Draw main path
    setlinecolor(RGB(139, 119, 101)); // Sandy brown
    setlinestyle(PS_SOLID, 20);
    for (size_t i = 0; i < enemyPath.size() - 1; ++i) {
        line(enemyPath[i].x, enemyPath[i].y, enemyPath[i + 1].x, enemyPath[i + 1].y);
    }
    
    // Draw path borders
    setlinecolor(RGB(160, 140, 120));
    setlinestyle(PS_SOLID, 2);
    for (size_t i = 0; i < enemyPath.size() - 1; ++i) {
        // Calculate perpendicular offset for borders
        int dx = enemyPath[i + 1].x - enemyPath[i].x;
        int dy = enemyPath[i + 1].y - enemyPath[i].y;
        double length = sqrt(dx * dx + dy * dy);
        if (length > 0) {
            int offsetX = (int)(-dy * 10 / length);
            int offsetY = (int)(dx * 10 / length);
            
            // Top border
            line(enemyPath[i].x + offsetX, enemyPath[i].y + offsetY,
                 enemyPath[i + 1].x + offsetX, enemyPath[i + 1].y + offsetY);
            // Bottom border
            line(enemyPath[i].x - offsetX, enemyPath[i].y - offsetY,
                 enemyPath[i + 1].x - offsetX, enemyPath[i + 1].y - offsetY);
        }
    }
    
    // Draw waypoint markers
    setfillcolor(RGB(255, 215, 0)); // Gold
    for (size_t i = 0; i < enemyPath.size(); ++i) {
        fillcircle(enemyPath[i].x, enemyPath[i].y, 3);
    }
    
    setlinestyle(PS_SOLID, 1); // Restore default line style
}

void Game::drawUI() {
    // Draw semi-transparent UI background
    setfillcolor(RGB(0, 0, 0));
    setcolor(RGB(100, 100, 100));
    fillrectangle(0, 0, 200, 90);
    rectangle(0, 0, 200, 90);
    
    // Draw game stats with better formatting
    TCHAR s[256];
    settextcolor(RGB(255, 215, 0)); // Gold color
    settextstyle(20, 0, _T("Arial"));

    _stprintf_s(s, _T("$ %d"), money);
    outtextxy(15, 10, s);
    
    settextcolor(RGB(255, 100, 100)); // Light red
    _stprintf_s(s, _T("Lives: %d"), lives);
    outtextxy(15, 35, s);

    settextcolor(RGB(100, 200, 255)); // Light blue
    _stprintf_s(s, _T("Wave: %d"), wave);
    outtextxy(15, 60, s);

    // Draw enhanced start game prompt
    if (enemiesToSpawn <= 0 && enemies.empty()) {
        // Draw glowing effect for the prompt
        settextcolor(RGB(255, 255, 100)); // Bright yellow
        settextstyle(32, 0, _T("Arial"));
        
        // Create a pulsing effect
        static int pulseTimer = 0;
        pulseTimer++;
        int alpha = 150 + (int)(50 * sin(pulseTimer * 0.1));
        
        if (wave == 0) {
            // Draw background box for text
            setfillcolor(RGB(50, 50, 50));
            fillrectangle(200, 530, 600, 570);
            rectangle(200, 530, 600, 570);
            
            outtextxy(220, 540, _T("Press SPACE to Start Game"));
        } else {
            // Draw background box for text
            setfillcolor(RGB(50, 50, 50));
            fillrectangle(180, 530, 620, 570);
            rectangle(180, 530, 620, 570);
            
            outtextxy(200, 540, _T("Press SPACE for Next Wave"));
        }
    }
    
    // Draw tower cost indicator
    if (money >= 100) {
        settextcolor(RGB(100, 255, 100)); // Green
    } else {
        settextcolor(RGB(255, 100, 100)); // Red
    }
    settextstyle(16, 0, _T("Arial"));
    outtextxy(15, 100, _T("Tower Cost: $100"));
    outtextxy(15, 120, _T("Click to build"));
    
    // Draw mini map or game status
    if (!enemies.empty()) {
        settextcolor(RGB(255, 200, 100));
        settextstyle(14, 0, _T("Arial"));
        _stprintf_s(s, _T("Enemies: %d"), (int)enemies.size());
        outtextxy(15, 140, s);
        
        if (enemiesToSpawn > 0) {
            _stprintf_s(s, _T("Incoming: %d"), enemiesToSpawn);
            outtextxy(15, 160, s);
        }
    }
}
