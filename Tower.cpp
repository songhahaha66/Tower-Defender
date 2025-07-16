#include "Tower.h"
#include <iostream>
#include <cmath>
#define M_PI 3.14159265358979323846
// Declare transparent image display function
extern void putimage_alpha(int x, int y, IMAGE* img);

// Static member initialization
IMAGE Tower::towerWithBullet;
IMAGE Tower::towerWithoutBullet;
bool Tower::imagesLoaded = false;

Tower::Tower(int x, int y, int dmg, int rng, int rate)
    : GameObject(x, y, 40, 40), damage(dmg), range(rng), fireRate(rate), fireCountdown(0), 
      hasBullet(true) {
    
    // Ensure images are loaded
    if (!imagesLoaded) {
        loadImages();
    }
}

void Tower::loadImages() {
    // Reload images regardless of previous loading status to ensure validity
    loadimage(&towerWithBullet, _T("images/tower.png"),70,70);
    loadimage(&towerWithoutBullet, _T("images/tower_without_bullet.png"),70,70);
    
    // Check if images loaded successfully
    if (towerWithBullet.getwidth() > 0 && towerWithBullet.getheight() > 0 && 
        towerWithoutBullet.getwidth() > 0 && towerWithoutBullet.getheight() > 0) {
        imagesLoaded = true;
    } else {
        // If loading fails, output error message (can use logging system in production)
        TCHAR error[128];
        _stprintf_s(error, _T("Failed to load tower images, please check image file paths!"));
        outtextxy(10, 10, error);
    }
}


void Tower::draw() {
    // Simple drawing of original image without rotation
    putimage_alpha(x, y, hasBullet ? &towerWithBullet : &towerWithoutBullet);
}

void Tower::update(std::vector<Enemy*>& enemies, std::vector<Bullet*>& bullets) {
    Enemy* target = findTarget(enemies);
    
    // Shooting logic
    if (fireCountdown > 0) {
        fireCountdown--;
    }

    if (fireCountdown <= 0) {
        if (target) {
            // Simply fire bullet from tower center
            bullets.push_back(new Bullet(x + width / 2, y + height / 2, damage, 10.0, target));
            fireCountdown = fireRate; // Reset fire countdown
            hasBullet = false; // Set no bullet state after firing
        }
    } else {
        // Restore bullet state when countdown is halfway through
        if (fireCountdown <= fireRate / 2 && !hasBullet) {
            hasBullet = true;
        }
    }
}

Enemy* Tower::findTarget(std::vector<Enemy*>& enemies) {
    Enemy* closestEnemy = nullptr;
    double minDistance = range + 1.0;

    for (auto& enemy : enemies) {
        if (enemy->active) {
            double dx = (enemy->x + enemy->width / 2) - (x + width / 2);
            double dy = (enemy->y + enemy->height / 2) - (y + height / 2);
            double distance = sqrt(dx * dx + dy * dy);

            if (distance <= range && distance < minDistance) {
                minDistance = distance;
                closestEnemy = enemy;
            }
        }
    }
    return closestEnemy;
}