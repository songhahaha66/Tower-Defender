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

Game::Game() : gameState(MENU), money(400), lives(10), wave(0), enemiesToSpawn(0), spawnTimer(0){}

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
    generateRandomPath(); // 生成随机路径
}

void Game::run() {
    init();

    while (gameState != GAME_OVER) {
        if (gameState == MENU) {
            handleMenuInput();
            drawMenu();
        } else if (gameState == PLAYING) {
            handleInput();
            update();
            draw();
            
            // 检查游戏结束条件
            if (lives <= 0) {
                gameState = GAME_OVER;
            }
        }
        
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
                
                // 只有在wave > 1时才生成新的随机路径（第一波保持初始路径）
                if (wave > 1) {
                    generateRandomPath();
                }
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
            // 左键建塔
            if (money >= 100) { // Tower cost is 100
                money -= 100;
                towers.push_back(new Tower(m.x - 20, m.y - 20, 10, 150, 30)); // damage:10, range:150, cooldown:30 frames
            }
        } else if (m.uMsg == WM_RBUTTONDOWN) {
            // 右键卖塔
            for (auto it = towers.begin(); it != towers.end(); ++it) {
                Tower* tower = *it;
                // 检查鼠标点击是否在塔的范围内
                if (m.x >= tower->x && m.x <= tower->x + tower->width &&
                    m.y >= tower->y && m.y <= tower->y + tower->height) {
                    money += 50; // 卖塔回收一半价格
                    delete tower;
                    towers.erase(it);
                    break; // 只删除一个塔
                }
            }
        }
    }
}

void Game::handleMenuInput() {
    // 检测鼠标点击或按键开始游戏
    if (GetAsyncKeyState(VK_SPACE) & 0x8000 || GetAsyncKeyState(VK_RETURN) & 0x8000) {
        gameState = PLAYING;
        return;
    }
    
    // 检测鼠标点击
    MOUSEMSG m;
    while (MouseHit()) {
        m = GetMouseMsg();
        if (m.uMsg == WM_LBUTTONDOWN) {
            gameState = PLAYING;
            return;
        }
    }
}

void Game::spawnWave() {
    if (enemiesToSpawn > 0) {
        spawnTimer--;
        if (spawnTimer <= 0) {
            // 根据当前波数和随机性生成不同类型的敌人
            int enemyType = 1; // 默认普通敌人
            
            // 随着波数增加，高级敌人出现概率增加
            int randomValue = rand() % 100;
            if (wave >= 5) {
                if (randomValue < 10) {
                    enemyType = 4; // 10% 精英敌人
                } else if (randomValue < 30) {
                    enemyType = 3; // 20% 重甲敌人
                } else if (randomValue < 60) {
                    enemyType = 2; // 30% 快速敌人
                } else {
                    enemyType = 1; // 40% 普通敌人
                }
            } else if (wave >= 3) {
                if (randomValue < 5) {
                    enemyType = 3; // 5% 重甲敌人
                } else if (randomValue < 40) {
                    enemyType = 2; // 35% 快速敌人
                } else {
                    enemyType = 1; // 60% 普通敌人
                }
            } else {
                if (randomValue < 30) {
                    enemyType = 2; // 30% 快速敌人
                } else {
                    enemyType = 1; // 70% 普通敌人
                }
            }
            
            enemies.push_back(Enemy::createEnemy(enemyType, enemyPath[0].x, enemyPath[0].y));
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
                money += e->getReward(); // 根据敌人类型获得不同奖励
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

void Game::drawMenu() {
    cleardevice();
    
    // 绘制背景渐变
    for (int y = 0; y < screenHeight; y++) {
        int blue = 30 + (y * 50) / screenHeight; // 蓝色渐变
        int green = 10 + (y * 20) / screenHeight;
        setlinecolor(RGB(10, green, blue));
        line(0, y, screenWidth, y);
    }
    
    // 绘制游戏标题
    settextcolor(RGB(255, 255, 100)); // 金黄色
    settextstyle(60, 0, _T("Arial"));
    TCHAR title[] = _T("Tower Defender");
    
    // 计算标题居中位置
    int titleWidth = textwidth(title);
    int titleX = (screenWidth - titleWidth) / 2;
    
    // 绘制标题阴影
    settextcolor(RGB(100, 100, 50));
    outtextxy(titleX + 3, 103, title);
    
    // 绘制标题
    settextcolor(RGB(255, 255, 100));
    outtextxy(titleX, 100, title);
    
    // 绘制副标题
    settextcolor(RGB(200, 200, 255));
    settextstyle(24, 0, _T("Arial"));
    TCHAR subtitle[] = _T("Defend your base from enemy waves!");
    int subtitleWidth = textwidth(subtitle);
    int subtitleX = (screenWidth - subtitleWidth) / 2;
    outtextxy(subtitleX, 180, subtitle);
    
    // 绘制开始按钮框
    setfillcolor(RGB(50, 50, 100));
    setlinecolor(RGB(100, 100, 200));
    int buttonX = screenWidth / 2 - 150;
    int buttonY = 300;
    int buttonWidth = 300;
    int buttonHeight = 60;
    
    fillrectangle(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
    rectangle(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
    
    // 绘制开始按钮文字
    settextcolor(RGB(255, 255, 255));
    settextstyle(32, 0, _T("Arial"));
    TCHAR startText[] = _T("Start Game");
    int startWidth = textwidth(startText);
    int startX = buttonX + (buttonWidth - startWidth) / 2;
    int startY = buttonY + (buttonHeight - textheight(startText)) / 2;
    outtextxy(startX, startY, startText);
    
    // 绘制操作说明
    settextcolor(RGB(180, 180, 180));
    settextstyle(18, 0, _T("Arial"));
    outtextxy(250, 400, _T("Left Click: Build Tower ($100)"));
    outtextxy(250, 430, _T("Right Click: Sell Tower ($50)"));
    outtextxy(250, 460, _T("Space: Start/Next Wave"));
    
    // 绘制提示文字
    settextcolor(RGB(255, 255, 150));
    settextstyle(20, 0, _T("Arial"));
    
    // 创建闪烁效果
    static int blinkTimer = 0;
    blinkTimer++;
    if ((blinkTimer / 30) % 2 == 0) { // 每秒闪烁一次
        TCHAR prompt[] = _T("Press SPACE, ENTER or Click to Start");
        int promptWidth = textwidth(prompt);
        int promptX = (screenWidth - promptWidth) / 2;
        outtextxy(promptX, 520, prompt);
    }
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
        
        // Draw a simple tree
        fillcircle(x, y, 8 + i % 5); // Tree crown
        setfillcolor(RGB(101, 67, 33)); // Brown
        fillrectangle(x - 2, y + 5, x + 2, y + 15); // Tree trunk
        setfillcolor(RGB(34, 139, 34)); // Reset to green
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
    
    // 显示当前存活的敌人类型统计
    settextcolor(RGB(200, 200, 200)); // Light gray
    int enemyCount[5] = {0}; // 索引0不使用，1-4对应敌人类型
    for (auto& enemy : enemies) {
        if (enemy->active && enemy->enemyType >= 1 && enemy->enemyType <= 4) {
            enemyCount[enemy->enemyType]++;
        }
    }
    
    settextstyle(16, 0, _T("Arial"));
    _stprintf_s(s, _T("Enemies: Normal:%d Fast:%d Heavy:%d Elite:%d"), 
                enemyCount[1], enemyCount[2], enemyCount[3], enemyCount[4]);
    outtextxy(15, 85, s);
    
    // 恢复字体大小
    settextstyle(20, 0, _T("Arial"));

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
    outtextxy(15, 120, _T("Left click to build"));
    
    // 卖塔说明
    settextcolor(RGB(200, 200, 100));
    outtextxy(15, 140, _T("Right click to sell ($50)"));
    
    // Draw mini map or game status
    if (!enemies.empty()) {
        settextcolor(RGB(255, 200, 100));
        settextstyle(14, 0, _T("Arial"));
        _stprintf_s(s, _T("Enemies: %d"), (int)enemies.size());
        outtextxy(15, 160, s);
        
        if (enemiesToSpawn > 0) {
            _stprintf_s(s, _T("Incoming: %d"), enemiesToSpawn);
            outtextxy(15, 180, s);
        }
    }
}

void Game::generateRandomPath() {
    enemyPath.clear();
    
    // 随机选择起始边
    int startSide = rand() % 4; // 0:左, 1:上, 2:右, 3:下
    int endSide;
    do {
        endSide = rand() % 4;
    } while (endSide == startSide); // 确保起点和终点不在同一边
    
    // 设置起点
    PathPoint start, end;
    switch (startSide) {
        case 0: // 左边
            start = {0, rand() % (screenHeight - 200) + 100};
            break;
        case 1: // 上边
            start = {rand() % (screenWidth - 200) + 100, 0};
            break;
        case 2: // 右边
            start = {screenWidth, rand() % (screenHeight - 200) + 100};
            break;
        case 3: // 下边
            start = {rand() % (screenWidth - 200) + 100, screenHeight};
            break;
    }
    
    // 设置终点
    switch (endSide) {
        case 0: // 左边
            end = {0, rand() % (screenHeight - 200) + 100};
            break;
        case 1: // 上边
            end = {rand() % (screenWidth - 200) + 100, 0};
            break;
        case 2: // 右边
            end = {screenWidth, rand() % (screenHeight - 200) + 100};
            break;
        case 3: // 下边
            end = {rand() % (screenWidth - 200) + 100, screenHeight};
            break;
    }
    
    enemyPath.push_back(start);
    
    // 添加2-4个中间点创建弯曲路径
    int numWaypoints = rand() % 3 + 2; // 2-4个中间点
    PathPoint current = start;
    
    for (int i = 0; i < numWaypoints; i++) {
        PathPoint waypoint;
        waypoint.x = rand() % (screenWidth - 200) + 100;
        waypoint.y = rand() % (screenHeight - 200) + 100;
        
        // 确保路径点不会太接近
        if (abs(waypoint.x - current.x) > 100 || abs(waypoint.y - current.y) > 100) {
            enemyPath.push_back(waypoint);
            current = waypoint;
        }
    }
    
    enemyPath.push_back(end);
}
