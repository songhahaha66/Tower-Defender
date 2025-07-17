#include "Game.h"
#include <conio.h>
#include <algorithm>
#include <cmath>
#pragma comment(lib, "MSIMG32.LIB")
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 透明图片显示函数
void putimage_alpha(int x, int y, IMAGE* img)
{
    int w = img->getwidth();
    int h = img->getheight();
    AlphaBlend(GetImageHDC(NULL), x, y, w, h, GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
}

// 游戏构造函数
Game::Game() : gameState(MENU), money(400), lives(5), wave(0), enemiesToSpawn(0), spawnTimer(0){}

// 游戏析构函数
Game::~Game() {
    for (auto p : enemies) delete p;
    for (auto p : towers) delete p;
    for (auto p : bullets) delete p;
}

// 初始化游戏
void Game::init() {
    initgraph(screenWidth, screenHeight);
    BeginBatchDraw();

    Tower::loadImages();
    Enemy::loadImages();
    Bullet::loadImage();

    generateRandomPath();
}

// 游戏主循环
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
            
            if (lives <= 0) {
                gameState = GAME_OVER;
            }
        } else if (gameState == WAVE_COMPLETE) {
            handleWaveCompleteInput();
            drawWaveComplete();
        }
        
        FlushBatchDraw();
        Sleep(16); // 约60FPS
    }

    // 游戏结束画面
    cleardevice();
    
    setfillcolor(RGB(20, 20, 20));
    fillrectangle(0, 0, screenWidth, screenHeight);
    
    // 居中绘制 "GAME OVER"
    settextcolor(RGB(255, 50, 50));
    settextstyle(80, 0, _T("Arial"));
    TCHAR gameOverText[] = _T("GAME OVER");
    int gameOverWidth = textwidth(gameOverText);
    outtextxy((screenWidth - gameOverWidth) / 2, 200, gameOverText);
    
    // 居中绘制最终统计信息
    settextcolor(RGB(200, 200, 200));
    settextstyle(24, 0, _T("Arial"));
    TCHAR finalStats[256];

    // 最终波数
    _stprintf_s(finalStats, _T("Final Wave: %d"), wave);
    int finalWaveWidth = textwidth(finalStats);
    outtextxy((screenWidth - finalWaveWidth) / 2, 300, finalStats);
    
    // 获得金钱
    _stprintf_s(finalStats, _T("Money Earned: $%d"), money);
    int moneyEarnedWidth = textwidth(finalStats);
    outtextxy((screenWidth - moneyEarnedWidth) / 2, 340, finalStats);
    
    // 居中绘制退出提示
    settextcolor(RGB(150, 150, 150));
    settextstyle(20, 0, _T("Arial"));
    TCHAR exitText[] = _T("Press Space to exit");
    int exitTextWidth = textwidth(exitText);
    outtextxy((screenWidth - exitTextWidth) / 2, 420, exitText);
    
    FlushBatchDraw();
    // 等待任意键按下
    while (!GetAsyncKeyState(VK_SPACE)) {
        Sleep(100); 
    }

    EndBatchDraw();
    closegraph();
}

// 处理游戏输入
void Game::handleInput() {
    static bool spacePressed = false;
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        if (!spacePressed) {
            if (enemiesToSpawn <= 0 && enemies.empty()) {
                gameState = WAVE_COMPLETE; // 切换到关卡完成状态
            }
            spacePressed = true;
        }
    } else {
        spacePressed = false;
    }

    // 处理鼠标输入
    MOUSEMSG m;
    while (MouseHit()) {
        m = GetMouseMsg();
        if (m.uMsg == WM_LBUTTONDOWN) {
            // 左键建塔
            if (money >= 100) {
                money -= 100;
                towers.push_back(new Tower(m.x - 20, m.y - 20, 10, 150, 30));
            }
        } else if (m.uMsg == WM_RBUTTONDOWN) {
            // 右键卖塔
            for (auto it = towers.begin(); it != towers.end(); ++it) {
                Tower* tower = *it;
                if (m.x >= tower->x && m.x <= tower->x + tower->width &&
                    m.y >= tower->y && m.y <= tower->y + tower->height) {
                    money += 50;
                    delete tower;
                    towers.erase(it);
                    break;
                }
            }
        }
    }
}

// 处理菜单输入
void Game::handleMenuInput() {
    if (GetAsyncKeyState(VK_SPACE) & 0x8000 || GetAsyncKeyState(VK_RETURN) & 0x8000) {
        // 初始化第一波（wave=0）
        wave = 0;
        enemiesToSpawn = 5 + wave * 2; // 第0波5个敌人
        spawnTimer = 60;
        gameState = PLAYING;
        return;
    }
    
    MOUSEMSG m;
    while (MouseHit()) {
        m = GetMouseMsg();
        if (m.uMsg == WM_LBUTTONDOWN) {
            // 初始化第一波（wave=0）
            wave = 0;
            enemiesToSpawn = 5 + wave * 2; // 第0波5个敌人
            spawnTimer = 60;
            gameState = PLAYING;
            return;
        }
    }
}

// 处理关卡完成输入
void Game::handleWaveCompleteInput() {
    if (GetAsyncKeyState(VK_SPACE) & 0x8000 || GetAsyncKeyState(VK_RETURN) & 0x8000) {
        // 进入下一关
        wave++;
        enemiesToSpawn = 5 + wave * 2;
        spawnTimer = 60;
        
        // 清空所有防御塔并返还原价
        for (auto tower : towers) {
            money += 100; // 返还建造费用
            delete tower;
        }
        towers.clear();
        
        // 清空所有子弹
        for (auto bullet : bullets) {
            delete bullet;
        }
        bullets.clear();
        
        if (wave > 1) {
            generateRandomPath();
        }
        
        gameState = PLAYING; // 返回游戏状态
        return;
    }
    
    MOUSEMSG m;
    while (MouseHit()) {
        m = GetMouseMsg();
        if (m.uMsg == WM_LBUTTONDOWN) {
            // 进入下一关
            wave++;
            enemiesToSpawn = 5 + wave * 2;
            spawnTimer = 60;
            
            // 清空所有防御塔并返还原价
            for (auto tower : towers) {
                money += 100; // 返还建造费用
                delete tower;
            }
            towers.clear();
            
            // 清空所有子弹
            for (auto bullet : bullets) {
                delete bullet;
            }
            bullets.clear();
            
            if (wave > 1) {
                generateRandomPath();
            }
            
            gameState = PLAYING; // 返回游戏状态
            return;
        }
    }
}

// 生成敌人波次
void Game::spawnWave() {
    if (enemiesToSpawn > 0) {
        spawnTimer--;
        if (spawnTimer <= 0) {
            // 根据波数生成不同类型敌人
            int enemyType = 1;
            
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
            spawnTimer = 60;
        }
    }
}

// 更新游戏状态
void Game::update() {
    spawnWave();

    for (auto& tower : towers) {
        tower->update(enemies, bullets);
    }
    for (auto& bullet : bullets) {
        bullet->update();
    }
    for (auto& enemy : enemies) {
        enemy->update(enemyPath);
        if (!enemy->active && enemy->health > 0) { // 敌人到达终点
            lives--;
        }
    }

    cleanup();
    
    // 检查关卡是否完成
    if (enemiesToSpawn <= 0 && enemies.empty() && gameState == PLAYING) {
        if (wave == 0) {
            // 第0波完成后直接进入下一波，不显示关卡完成界面
            wave++;
            enemiesToSpawn = 5 + wave * 2;
            spawnTimer = 60;
            
            // 清空所有防御塔并返还原价
            for (auto tower : towers) {
                money += 100; // 返还建造费用
                delete tower;
            }
            towers.clear();
            
            // 清空所有子弹
            for (auto bullet : bullets) {
                delete bullet;
            }
            bullets.clear();
            
            generateRandomPath();
        } else {
            gameState = WAVE_COMPLETE;
        }
    }
}

// 清理非活跃对象
void Game::cleanup() {
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet* b) {
        if (!b->active) {
            delete b;
            return true;
        }
        return false;
        }), bullets.end());

    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [&](Enemy* e) {
        if (!e->active) {
            if (e->health <= 0) { // 敌人被击杀
                money += e->getReward();
            }
            delete e;
            return true;
        }
        return false;
        }), enemies.end());
}

// 绘制游戏画面
void Game::draw() {
    cleardevice();
    
    drawBackground();
    drawPath();

    for (auto& tower : towers) tower->draw();
    for (auto& enemy : enemies) enemy->draw();
    for (auto& bullet : bullets) bullet->draw();

    drawUI();
}

// 绘制主菜单
void Game::drawMenu() {
    cleardevice();
    
    // 背景渐变
    for (int y = 0; y < screenHeight; y++) {
        int blue = 30 + (y * 50) / screenHeight;
        int green = 10 + (y * 20) / screenHeight;
        setlinecolor(RGB(10, green, blue));
        line(0, y, screenWidth, y);
    }
    
    // 游戏标题
    settextcolor(RGB(255, 255, 100));
    settextstyle(60, 0, _T("Arial"));
    TCHAR title[] = _T("Tower Defender");
    
    int titleWidth = textwidth(title);
    int titleX = (screenWidth - titleWidth) / 2;
    
    settextcolor(RGB(100, 100, 50));
    outtextxy(titleX + 3, 103, title);
    
    settextcolor(RGB(255, 255, 100));
    outtextxy(titleX, 100, title);
    
    // 副标题
    settextcolor(RGB(200, 200, 255));
    settextstyle(24, 0, _T("Arial"));
    TCHAR subtitle[] = _T("Defend your base from enemy waves!");
    int subtitleWidth = textwidth(subtitle);
    int subtitleX = (screenWidth - subtitleWidth) / 2;
    outtextxy(subtitleX, 180, subtitle);
    
    // 开始按钮
    setfillcolor(RGB(50, 50, 100));
    setlinecolor(RGB(100, 100, 200));
    int buttonX = screenWidth / 2 - 150;
    int buttonY = 300;
    int buttonWidth = 300;
    int buttonHeight = 60;
    
    fillrectangle(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
    rectangle(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
    
    settextcolor(RGB(255, 255, 255));
    settextstyle(32, 0, _T("Arial"));
    TCHAR startText[] = _T("Start Game");
    int startWidth = textwidth(startText);
    int startX = buttonX + (buttonWidth - startWidth) / 2;
    int startY = buttonY + (buttonHeight - textheight(startText)) / 2;
    outtextxy(startX, startY, startText);
    
    // 操作说明
    settextcolor(RGB(180, 180, 180));
    settextstyle(18, 0, _T("Arial"));
    outtextxy(250, 400, _T("Left Click: Build Tower ($100)"));
    outtextxy(250, 430, _T("Right Click: Sell Tower ($50)"));
    outtextxy(250, 460, _T("Space: Start/Next Wave"));
    
    // 闪烁提示
    settextcolor(RGB(255, 255, 150));
    settextstyle(20, 0, _T("Arial"));
    
    static int blinkTimer = 0;
    blinkTimer++;
    if ((blinkTimer / 30) % 2 == 0) {
        TCHAR prompt[] = _T("Press SPACE, ENTER or Click to Start");
        int promptWidth = textwidth(prompt);
        int promptX = (screenWidth - promptWidth) / 2;
        outtextxy(promptX, 520, prompt);
    }
}

// 绘制背景
void Game::drawBackground() {
    // 渐变背景
    for (int y = 0; y < screenHeight; y++) {
        int green = 20 + (y * 30) / screenHeight;
        setlinecolor(RGB(10, green, 10));
        line(0, y, screenWidth, y);
    }
    
    // 装饰性元素
    setfillcolor(RGB(34, 139, 34));
    
    for (int i = 0; i < 15; i++) {
        int x = (i * 73 + 50) % (screenWidth - 40);
        int y = (i * 97 + 80) % (screenHeight - 60);
        
        fillcircle(x, y, 8 + i % 5);
        setfillcolor(RGB(101, 67, 33));
        fillrectangle(x - 2, y + 5, x + 2, y + 15);
        setfillcolor(RGB(34, 139, 34));
    }
}

// 绘制敌人路径
void Game::drawPath() {
    // 路径阴影
    setlinecolor(RGB(80, 60, 40));
    setlinestyle(PS_SOLID, 25);
    for (size_t i = 0; i < enemyPath.size() - 1; ++i) {
        line(enemyPath[i].x + 2, enemyPath[i].y + 2, 
             enemyPath[i + 1].x + 2, enemyPath[i + 1].y + 2);
    }
    
    // 主路径
    setlinecolor(RGB(139, 119, 101));
    setlinestyle(PS_SOLID, 20);
    for (size_t i = 0; i < enemyPath.size() - 1; ++i) {
        line(enemyPath[i].x, enemyPath[i].y, enemyPath[i + 1].x, enemyPath[i + 1].y);
    }
    
    // 路径边框
    setlinecolor(RGB(160, 140, 120));
    setlinestyle(PS_SOLID, 2);
    for (size_t i = 0; i < enemyPath.size() - 1; ++i) {
        int dx = enemyPath[i + 1].x - enemyPath[i].x;
        int dy = enemyPath[i + 1].y - enemyPath[i].y;
        double length = sqrt(dx * dx + dy * dy);
        if (length > 0) {
            int offsetX = (int)(-dy * 10 / length);
            int offsetY = (int)(dx * 10 / length);
            
            line(enemyPath[i].x + offsetX, enemyPath[i].y + offsetY,
                 enemyPath[i + 1].x + offsetX, enemyPath[i + 1].y + offsetY);
            line(enemyPath[i].x - offsetX, enemyPath[i].y - offsetY,
                 enemyPath[i + 1].x - offsetX, enemyPath[i + 1].y - offsetY);
        }
    }
    
    // 路径点标记
    setfillcolor(RGB(255, 215, 0));
    for (size_t i = 0; i < enemyPath.size(); ++i) {
        fillcircle(enemyPath[i].x, enemyPath[i].y, 3);
    }
    
    setlinestyle(PS_SOLID, 1);
}

// 绘制用户界面
void Game::drawUI() {
    // UI背景
    setfillcolor(RGB(20, 20, 20));
    setcolor(RGB(80, 80, 80));
    fillrectangle(0, 0, 220, 200);
    rectangle(0, 0, 220, 200);

    // 金钱、生命、波数
    TCHAR s[256];
    settextcolor(RGB(255, 215, 0));
    settextstyle(22, 0, _T("Arial"));
    _stprintf_s(s, _T("$ %d"), money);
    outtextxy(20, 15, s);

    settextcolor(RGB(255, 100, 100));
    _stprintf_s(s, _T("Lives: %d"), lives);
    outtextxy(120, 15, s);

    settextcolor(RGB(100, 200, 255));
    _stprintf_s(s, _T("Wave: %d"), wave);
    outtextxy(20, 45, s);

    // 分隔线
    setcolor(RGB(80, 80, 80));
    line(10, 70, 210, 70);

    // 敌人统计
    settextcolor(RGB(200, 200, 200));
    settextstyle(16, 0, _T("Arial"));
    int enemyCount[5] = {0};
    for (auto& enemy : enemies) {
        if (enemy->active && enemy->enemyType >= 1 && enemy->enemyType <= 4) {
            enemyCount[enemy->enemyType]++;
        }
    }
    _stprintf_s(s, _T("Normal:%d Fast:%d Heavy:%d Elite:%d"), enemyCount[1], enemyCount[2], enemyCount[3], enemyCount[4]);
    outtextxy(20, 80, s);

    // 分隔线
    line(10, 105, 210, 105);

    // 建塔/卖塔提示
    settextcolor(money >= 100 ? RGB(100,255,100) : RGB(255,100,100));
    _stprintf_s(s, _T("Tower: $100  (Left click)"));
    outtextxy(20, 115, s);
    settextcolor(RGB(200,200,100));
    _stprintf_s(s, _T("Sell: $50  (Right click)"));
    outtextxy(20, 135, s);

    // 敌人数量/来袭
    settextcolor(RGB(255,200,100));
    _stprintf_s(s, _T("Enemies: %d"), (int)enemies.size());
    outtextxy(20, 160, s);
    if (enemiesToSpawn > 0) {
        _stprintf_s(s, _T("Incoming: %d"), enemiesToSpawn);
        outtextxy(20, 180, s);
    }
}

// 生成随机敌人路径
void Game::generateRandomPath() {
    enemyPath.clear();
    
    // 随机选择起始和结束边
    int startSide = rand() % 4; // 0:左, 1:上, 2:右, 3:下
    int endSide;
    do {
        endSide = rand() % 4;
    } while (endSide == startSide);
    
    // 设置起点和终点
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
    
    // 添加中间路径点
    int numWaypoints = rand() % 3 + 2; // 2-4个中间点
    PathPoint current = start;
    
    for (int i = 0; i < numWaypoints; i++) {
        PathPoint waypoint;
        waypoint.x = rand() % (screenWidth - 200) + 100;
        waypoint.y = rand() % (screenHeight - 200) + 100;
        
        if (abs(waypoint.x - current.x) > 100 || abs(waypoint.y - current.y) > 100) {
            enemyPath.push_back(waypoint);
            current = waypoint;
        }
    }
    
    enemyPath.push_back(end);
}

// 绘制关卡完成界面
void Game::drawWaveComplete() {
    cleardevice();
    
    // 背景渐变
    for (int y = 0; y < screenHeight; y++) {
        int green = 30 + (y * 40) / screenHeight;
        int blue = 20 + (y * 30) / screenHeight;
        setlinecolor(RGB(10, green, blue));
        line(0, y, screenWidth, y);
    }
    
    // 关卡完成标题
    settextcolor(RGB(100, 255, 100));
    settextstyle(60, 0, _T("Arial"));
    TCHAR waveCompleteText[] = _T("WAVE COMPLETE!");
    int waveCompleteWidth = textwidth(waveCompleteText);
    
    // 标题阴影效果
    settextcolor(RGB(50, 150, 50));
    outtextxy((screenWidth - waveCompleteWidth) / 2 + 3, 103, waveCompleteText);
    
    settextcolor(RGB(100, 255, 100));
    outtextxy((screenWidth - waveCompleteWidth) / 2, 100, waveCompleteText);
    
    // 当前关卡信息
    settextcolor(RGB(255, 255, 150));
    settextstyle(32, 0, _T("Arial"));
    TCHAR waveInfo[256];
    _stprintf_s(waveInfo, _T("Wave %d Completed!"), wave);
    int waveInfoWidth = textwidth(waveInfo);
    outtextxy((screenWidth - waveInfoWidth) / 2, 180, waveInfo);
    
    // 奖励信息
    settextcolor(RGB(255, 215, 0));
    settextstyle(24, 0, _T("Arial"));
    int towerRefund = (int)towers.size() * 100;
    _stprintf_s(waveInfo, _T("Tower Refund: $%d"), towerRefund);
    int refundWidth = textwidth(waveInfo);
    outtextxy((screenWidth - refundWidth) / 2, 230, waveInfo);
    
    _stprintf_s(waveInfo, _T("Current Money: $%d"), money);
    int moneyWidth = textwidth(waveInfo);
    outtextxy((screenWidth - moneyWidth) / 2, 260, waveInfo);
    
    // 下一关预告
    settextcolor(RGB(200, 200, 255));
    settextstyle(20, 0, _T("Arial"));
    _stprintf_s(waveInfo, _T("Next Wave: %d enemies incoming"), 5 + (wave + 1) * 2);
    int nextWaveWidth = textwidth(waveInfo);
    outtextxy((screenWidth - nextWaveWidth) / 2, 320, waveInfo);
    
    // 继续按钮
    setfillcolor(RGB(50, 100, 50));
    setlinecolor(RGB(100, 200, 100));
    int buttonX = screenWidth / 2 - 150;
    int buttonY = 380;
    int buttonWidth = 300;
    int buttonHeight = 60;
    
    fillrectangle(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
    rectangle(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
    
    settextcolor(RGB(255, 255, 255));
    settextstyle(28, 0, _T("Arial"));
    TCHAR continueText[] = _T("Continue to Next Wave");
    int continueWidth = textwidth(continueText);
    int continueX = buttonX + (buttonWidth - continueWidth) / 2;
    int continueY = buttonY + (buttonHeight - textheight(continueText)) / 2;
    outtextxy(continueX, continueY, continueText);
    
    // 闪烁提示
    settextcolor(RGB(255, 255, 150));
    settextstyle(18, 0, _T("Arial"));
    
    static int blinkTimer = 0;
    blinkTimer++;
    if ((blinkTimer / 30) % 2 == 0) {
        TCHAR prompt[] = _T("Press SPACE, ENTER or Click to Continue");
        int promptWidth = textwidth(prompt);
        int promptX = (screenWidth - promptWidth) / 2;
        outtextxy(promptX, 480, prompt);
    }
}
