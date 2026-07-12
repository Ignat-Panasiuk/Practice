#include "game_engine.h"
#include <algorithm>
#include <cstdlib>

GameEngine::GameEngine() {
    startNewGame(Difficulty::Normal);
}

void GameEngine::startNewGame(Difficulty diff) {
    objects.clear();
    difficulty = diff;
    baseHp = 100;
    score = 0;
    rageMeter = 0;
    isRageActive = false;
    rageTimer = 0.0;
    activeWeapon = WeaponType::MachineGun;
    preRageWeapon = WeaponType::MachineGun;
    gunHeat = 0.0;
    isOverheated = false;
    cannonAmmo = (difficulty == Difficulty::Easy) ? 30 : 20;
    currentWave = 1;
    enemiesToSpawn = 10;
    spawnTimer = droneTimer = waveDelayTimer = 0.0;

    soundTriggerShootMG = soundTriggerShootCannon = soundTriggerExplosion = soundTriggerBaseDamage =
        soundTriggerRage = soundTriggerPickup = soundTriggerEnemyDeath = soundTriggerWaveComplete = false;
}

void GameEngine::addObject(std::unique_ptr<GameObject> obj) {
    objects.push_back(std::move(obj));
}

void GameEngine::activateRage() {
    if (rageMeter >= 100 && !isRageActive) {
        isRageActive = true;
        rageTimer = 6.0;
        rageMeter = 0;
        isOverheated = false;
        gunHeat = 0.0;
        preRageWeapon = activeWeapon;
        activeWeapon = WeaponType::HeavyCannon;
        soundTriggerRage = true;
    }
}

void GameEngine::fireWeapon(double targetX, double targetY) {
    double dx = targetX - BASE_X;
    double dy = targetY - BASE_Y;
    double angle = std::atan2(dy, dx) * 180.0 / M_PI;
    fireWeapon(angle);
}

void GameEngine::fireWeapon(double angleDegrees) {
    double rad = angleDegrees * M_PI / 180.0;
    double sx = std::cos(rad);
    double sy = std::sin(rad);

    if (activeWeapon == WeaponType::MachineGun) {
        if (isOverheated) return;
        addObject(std::make_unique<Projectile>(BASE_X + sx*28, BASE_Y + sy*40, sx * 500.0, sy * 500.0, 5));
        soundTriggerShootMG = true;
        if (!isRageActive) {
            gunHeat += 4.0;
            if (gunHeat >= 100.0) isOverheated = true;
        }
    } else if (activeWeapon == WeaponType::HeavyCannon) {
        if (cannonAmmo <= 0 && !isRageActive) return;
        double speed = isRageActive ? 500.0 : 320.0;
        addObject(std::make_unique<Projectile>(BASE_X + sx*38, BASE_Y + sy*38, sx * speed, sy * speed, 100));
        soundTriggerShootCannon = true;
        if (!isRageActive) cannonAmmo--;
    }
}

void GameEngine::updateWeapons(double deltaTime) {
    if (isRageActive) {
        rageTimer -= deltaTime;
        if (rageTimer <= 0.0) {
            isRageActive = false;
            activeWeapon = preRageWeapon;
        }
    }
    if (activeWeapon != WeaponType::MachineGun || isOverheated || gunHeat > 0.0) {
        double coolingRate = isOverheated ? 20.0 : 35.0;
        gunHeat -= coolingRate * deltaTime;
        if (gunHeat <= 0.0) { gunHeat = 0.0; isOverheated = false; }
    }
}

void GameEngine::spawnEnemyLogic(double deltaTime) {
    if (enemiesToSpawn <= 0) return;

    spawnTimer += deltaTime;
    double modifier = (difficulty == Difficulty::Easy) ? 2.5 : ((difficulty == Difficulty::Normal) ? 2.0 : 1.4);
    double spawnInterval = std::max(0.4, modifier - (currentWave * 0.15));

    if (spawnTimer >= spawnInterval) {
        spawnTimer = 0.0;
        enemiesToSpawn--;

        double sx = 0, sy = 0;
        int border = std::rand() % 4;
        if (border == 0) { sx = std::rand() % 800; sy = -30; }
        else if (border == 1) { sx = std::rand() % 800; sy = 550; }
        else if (border == 2) { sx = -30; sy = std::rand() % 550; }
        else { sx = 830; sy = std::rand() % 550; }

        double hpMod = (difficulty == Difficulty::Easy) ? 0.27 : ((difficulty == Difficulty::Normal) ? 0.53 : 1.0);

        int activeTanks = 0;
        for (const auto& obj : objects) {
            if (obj->getType() == ObjectType::Tank && !obj->getIsDead()) {
                activeTanks++;
            }
        }

        int maxTanks = (difficulty == Difficulty::Easy) ? 1 : 2;

        int r = std::rand() % 100;
        if (r < 15 && currentWave >= 2) {
            addObject(std::make_unique<Kamikaze>(sx, sy, hpMod));
        } else if (r >= 15 && r < 25 && currentWave >= 3 && activeTanks < maxTanks) {
            addObject(std::make_unique<Tank>(sx, sy, hpMod));
        } else {
            addObject(std::make_unique<Soldier>(sx, sy, hpMod));
        }
    }
}

void GameEngine::spawnDroneLogic(double deltaTime) {
    droneTimer += deltaTime;
    double droneInterval = (difficulty == Difficulty::Easy) ? 20.0 : 35.0;
    if (droneTimer >= droneInterval) {
        droneTimer = 0.0;

        double angle = (std::rand() % 360) * M_PI / 180.0;
        double distance = 160.0 + (std::rand() % 80);

        double targetX = BASE_X + std::cos(angle) * distance;
        double targetY = BASE_Y + std::sin(angle) * distance;

        targetX = std::max(50.0, std::min(750.0, targetX));
        targetY = std::max(50.0, std::min(500.0, targetY));

        double distLeft = targetX + 40.0;
        double distRight = 840.0 - targetX;
        double distTop = targetY + 40.0;
        double distBottom = 640.0 - targetY;

        double minDist = std::min({distLeft, distRight, distTop, distBottom});

        double startX = 0.0;
        double startY = 0.0;

        if (minDist == distLeft) {
            startX = -40.0;
            startY = targetY;
        } else if (minDist == distRight) {
            startX = 840.0;
            startY = targetY;
        } else if (minDist == distTop) {
            startX = targetX;
            startY = -40.0;
        } else {
            startX = targetX;
            startY = 640.0;
        }

        addObject(std::make_unique<Drone>(startX, startY, targetX, targetY));
    }
}

bool GameEngine::hasActiveEnemies() const {
    for (const auto& obj : objects) {
        if (obj->getType() == ObjectType::Soldier || obj->getType() == ObjectType::Kamikaze || obj->getType() == ObjectType::Tank) return true;
    }
    return false;
}

void GameEngine::update(double deltaTime) {
    updateWeapons(deltaTime);
    if (waveDelayTimer > 0.0) waveDelayTimer -= deltaTime;

    double speedModifier = (difficulty == Difficulty::Easy) ? 35.0 : ((difficulty == Difficulty::Normal) ? 50.0 : 65.0);

    for (auto& obj : objects) {
        if (obj->getType() == ObjectType::Tank) {
            if (waveDelayTimer <= 0.0) {
                bool collidingWithDrone = false;
                GameObject* droneObj = nullptr;

                for (auto& other : objects) {
                    if (other->getType() == ObjectType::Drone && !other->getIsDead()) {
                        if (obj->getRect().intersects(other->getRect())) {
                            collidingWithDrone = true;
                            droneObj = other.get();
                            break;
                        }
                    }
                }

                if (collidingWithDrone) {
                    double dx = obj->getX() - droneObj->getX();
                    double dy = obj->getY() - droneObj->getY();
                    double length = std::sqrt(dx * dx + dy * dy);

                    if (length > 0) {
                        double rx = dx / length;
                        double ry = dy / length;
                        double bx = BASE_X - obj->getX();
                        double by = BASE_Y - obj->getY();
                        double bLength = std::sqrt(bx * bx + by * by);
                        if (bLength > 0) { bx /= bLength; by /= bLength; }
                        double tx1 = -ry; double ty1 = rx;
                        double tx2 = ry; double ty2 = -rx;
                        double dot1 = tx1 * bx + ty1 * by;
                        double dot2 = tx2 * bx + ty2 * by;
                        double targetVx = 0, targetVy = 0;
                        if (dot1 > dot2) {
                            targetVx = (tx1 * 0.8 + rx * 0.3) * speedModifier * 0.6;
                            targetVy = (ty1 * 0.8 + ry * 0.3) * speedModifier * 0.6;
                        } else {
                            targetVx = (tx2 * 0.8 + rx * 0.3) * speedModifier * 0.6;
                            targetVy = (ty2 * 0.8 + ry * 0.3) * speedModifier * 0.6;
                        }
                        obj->setVelocity(targetVx, targetVy);
                    }
                } else {
                    static_cast<Tank*>(obj.get())->updateAI(BASE_X, BASE_Y, deltaTime, *this);
                }
            }
        }

        else if (obj->getType() == ObjectType::Drone) {
            static_cast<Drone*>(obj.get())->updateAI(deltaTime);
        }
        else if (obj->getType() != ObjectType::Projectile && obj->getType() != ObjectType::EnemyProjectile) {
            if (waveDelayTimer <= 0.0) {
                bool collidingWithDrone = false;
                GameObject* droneObj = nullptr;

                for (auto& other : objects) {
                    if (other->getType() == ObjectType::Drone && !other->getIsDead()) {
                        if (obj->getRect().intersects(other->getRect())) {
                            collidingWithDrone = true;
                            droneObj = other.get();
                            break;
                        }
                    }
                }

                if (collidingWithDrone) {
                    double dx = obj->getX() - droneObj->getX();
                    double dy = obj->getY() - droneObj->getY();
                    double length = std::sqrt(dx * dx + dy * dy);

                    if (length > 0) {
                        double rx = dx / length;
                        double ry = dy / length;
                        double bx = BASE_X - obj->getX();
                        double by = BASE_Y - obj->getY();
                        double bLength = std::sqrt(bx * bx + by * by);
                        if (bLength > 0) { bx /= bLength; by /= bLength; }
                        double tx1 = -ry; double ty1 = rx;
                        double tx2 = ry; double ty2 = -rx;
                        double dot1 = tx1 * bx + ty1 * by;
                        double dot2 = tx2 * bx + ty2 * by;
                        double targetVx = 0, targetVy = 0;
                        if (dot1 > dot2) {
                            targetVx = (tx1 * 0.8 + rx * 0.3) * speedModifier;
                            targetVy = (ty1 * 0.8 + ry * 0.3) * speedModifier;
                        } else {
                            targetVx = (tx2 * 0.8 + rx * 0.3) * speedModifier;
                            targetVy = (ty2 * 0.8 + ry * 0.3) * speedModifier;
                        }
                        obj->setVelocity(targetVx, targetVy);
                    }
                } else {
                    obj->moveTowards(BASE_X, BASE_Y, speedModifier);
                }
            }
        }
        obj->update(deltaTime);
    }

    if (waveDelayTimer <= 0.0) spawnEnemyLogic(deltaTime);
    spawnDroneLogic(deltaTime);

    checkCollisions();
    checkBaseDamage();
    cleanupDeadObjects();

    if (enemiesToSpawn == 0 && !hasActiveEnemies() && waveDelayTimer <= 0.0) {
        waveDelayTimer = 4.0;
        currentWave++;
        baseHp = std::min(100, baseHp + 20);
        enemiesToSpawn = 10 + currentWave * 3;
        soundTriggerWaveComplete = true;
    }
}

void GameEngine::checkCollisions() {
    for (auto& epObj : objects) {
        if (epObj->getType() != ObjectType::EnemyProjectile || epObj->getIsDead()) continue;
        Projectile* enemyBullet = static_cast<Projectile*>(epObj.get());

        for (auto& dObj : objects) {
            if (dObj->getType() == ObjectType::Drone && !dObj->getIsDead()) {
                if (enemyBullet->getRect().intersects(dObj->getRect())) {
                    dObj->takeDamage(enemyBullet->getDamage());
                    enemyBullet->setDead(true);
                    soundTriggerExplosion = true;

                    if (dObj->getIsDead()) {
                        baseHp = std::min(100, baseHp + 15);
                        cannonAmmo += 5;
                        soundTriggerPickup = true;
                    }
                    break;
                }
            }
        }
    }

    for (auto& bObj : objects) {
        if (bObj->getType() != ObjectType::Projectile || bObj->getIsDead()) continue;
        Projectile* bullet = static_cast<Projectile*>(bObj.get());

        for (auto& epObj : objects) {
            if (epObj->getType() != ObjectType::EnemyProjectile || epObj->getIsDead()) continue;
            if (bullet->getRect().intersects(epObj->getRect())) {
                bullet->setDead(true);
                epObj->setDead(true);
                soundTriggerExplosion = true;
                break;
            }
        }
    }

    for (auto& bulletObj : objects) {
        if (bulletObj->getType() != ObjectType::Projectile || bulletObj->getIsDead()) continue;
        Projectile* bullet = static_cast<Projectile*>(bulletObj.get());

        for (auto& enemyObj : objects) {
            if (enemyObj->getType() == ObjectType::Projectile ||
                enemyObj->getType() == ObjectType::EnemyProjectile ||
                enemyObj->getIsDead()) continue;

            if (bullet->getRect().intersects(enemyObj->getRect())) {

                if (enemyObj->getType() == ObjectType::Kamikaze && enemyObj->getHp() <= bullet->getDamage()) {
                    soundTriggerExplosion = true;
                    for (auto& neighbor : objects) {
                        if (neighbor.get() == enemyObj.get()) continue;
                        if (neighbor->getType() == ObjectType::Projectile ||
                            neighbor->getType() == ObjectType::EnemyProjectile ||
                            neighbor->getIsDead()) continue;

                        double dx = enemyObj->getX() - neighbor->getX();
                        double dy = enemyObj->getY() - neighbor->getY();
                        double dist = std::sqrt(dx * dx + dy * dy);
                        if (dist < 100.0) {
                            neighbor->takeDamage(40);
                            if (neighbor->getIsDead()) {
                                if (neighbor->getType() != ObjectType::Drone) {
                                    score += 100;
                                    int oldRage = rageMeter;
                                    rageMeter = std::min(100, rageMeter + 10);
                                    if (oldRage < 100 && rageMeter >= 100) soundTriggerRage = true;
                                    soundTriggerEnemyDeath = true;
                                } else {
                                    baseHp = std::min(100, baseHp + 15);
                                    cannonAmmo += 5;
                                    soundTriggerPickup = true;
                                }
                            }
                        }
                    }
                }

                if (enemyObj->getType() == ObjectType::Tank && enemyObj->getHp() <= bullet->getDamage()) {
                    soundTriggerExplosion = true;
                }

                if (enemyObj->getType() == ObjectType::Drone && enemyObj->getHp() <= bullet->getDamage()) {
                    baseHp = std::min(100, baseHp + 15);
                    cannonAmmo += 5;
                    soundTriggerPickup = true;
                }

                enemyObj->takeDamage(bullet->getDamage());
                bullet->setDead(true);

                if (enemyObj->getIsDead()) {
                    if (enemyObj->getType() != ObjectType::Drone) {
                        score += 100;
                        int oldRage = rageMeter;
                        rageMeter = std::min(100, rageMeter + 10);
                        if (oldRage < 100 && rageMeter >= 100) soundTriggerRage = true;
                        soundTriggerEnemyDeath = true;
                    }
                }
                break;
            }
        }
    }
}

void GameEngine::checkBaseDamage() {
    const double a = 40.0;
    const double b = 50.0;

    const double a2 = a * a;
    const double b2 = b * b;

    for (auto& obj : objects) {
        if (obj->getIsDead()) continue;

        double dx = obj->getX() - BASE_X;
        double dy = obj->getY() - BASE_Y;

        bool isInsideEllipse = (dx * dx) / a2 + (dy * dy) / b2 < 1.0;

        if (obj->getType() == ObjectType::EnemyProjectile) {
            if (isInsideEllipse) {
                baseHp -= static_cast<Projectile*>(obj.get())->getDamage();
                obj->setDead(true);
                soundTriggerBaseDamage = true;
            }
            continue;
        }

        if (obj->getType() != ObjectType::Projectile && obj->getType() != ObjectType::Drone) {
            if (isInsideEllipse) {
                int damageModifier = (difficulty == Difficulty::Hard) ? 2 : 1;
                baseHp -= ((obj->getType() == ObjectType::Kamikaze) ? 25 : 10) * damageModifier;
                obj->setDead(true);
                soundTriggerBaseDamage = true;
                if (obj->getType() == ObjectType::Kamikaze) {
                    soundTriggerExplosion = true;
                }
            }
        }
    }
}

void GameEngine::cleanupDeadObjects() {
    objects.erase(
        std::remove_if(objects.begin(), objects.end(),
                       [](const std::unique_ptr<GameObject>& obj) { return obj->getIsDead(); }),
        objects.end()
        );
}
