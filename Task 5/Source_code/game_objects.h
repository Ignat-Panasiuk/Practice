#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

#include <QRectF>
#include <cmath>

enum class ObjectType { Soldier, Kamikaze, Tank, Projectile, EnemyProjectile, Drone };

class GameObject {
protected:
    double x, y, width, height, speedX, speedY;
    int hp, maxHp;
    bool isDead;
    ObjectType type;
    double animAccumulator;
public:
    GameObject(double x, double y, double w, double h, int hp, ObjectType type);
    virtual ~GameObject() = default;
    virtual void update(double deltaTime);
    void moveTowards(double targetX, double targetY, double speed);
    QRectF getRect() const;
    bool getIsDead() const;
    void setDead(bool dead);
    int getHp() const;
    void takeDamage(int damage);
    ObjectType getType() const;
    double getX() const;
    double getY() const;
    void setVelocity(double sx, double sy);
    double getAnimOffset() const;
};

class Projectile : public GameObject {
    int damage;
public:
    Projectile(double x, double y, double sx, double sy, int dmg, ObjectType type = ObjectType::Projectile);
    int getDamage() const;
};

class Soldier : public GameObject { public: Soldier(double x, double y, double hpModifier); };
class Kamikaze : public GameObject { public: Kamikaze(double x, double y, double hpModifier); };
class Tank : public GameObject {
    double shootTimer;
public:
    Tank(double x, double y, double hpModifier);
    void updateAI(double targetX, double targetY, double deltaTime, class GameEngine& engine);
};
class Drone : public GameObject {
    double targetX, targetY;
public:
    Drone(double x, double y, double tx, double ty);
    void updateAI(double deltaTime);
};

#endif