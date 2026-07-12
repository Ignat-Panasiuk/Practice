#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "game_objects.h"
#include <vector>
#include <memory>

enum class WeaponType { MachineGun, HeavyCannon };
enum class Difficulty { Easy, Normal, Hard };

class GameEngine {
public:
    std::vector<std::unique_ptr<GameObject>> objects;

    const double BASE_X = 400.0;
    const double BASE_Y = 300.0;
    const double BASE_RADIUS = 40.0;

    int baseHp;
    int score;
    int rageMeter;
    bool isRageActive;
    double rageTimer;

    WeaponType activeWeapon;
    WeaponType preRageWeapon;
    double gunHeat;
    bool isOverheated;
    int cannonAmmo;

    int currentWave;
    int enemiesToSpawn;
    double spawnTimer;
    double droneTimer;
    double waveDelayTimer;
    Difficulty difficulty;

    bool soundTriggerShootMG;
    bool soundTriggerShootCannon;
    bool soundTriggerExplosion;
    bool soundTriggerBaseDamage;
    bool soundTriggerRage;
    bool soundTriggerPickup;
    bool soundTriggerEnemyDeath;
    bool soundTriggerWaveComplete;

    GameEngine();
    ~GameEngine() = default;

    void startNewGame(Difficulty diff);
    void update(double deltaTime);
    void addObject(std::unique_ptr<GameObject> obj);

    void fireWeapon(double targetX, double targetY);
    void fireWeapon(double angleDegrees);

    void checkCollisions();
    void checkBaseDamage();
    void spawnEnemyLogic(double deltaTime);
    void spawnDroneLogic(double deltaTime);
    void updateWeapons(double deltaTime);
    void activateRage();
    void cleanupDeadObjects();
    bool hasActiveEnemies() const;
};

#endif
