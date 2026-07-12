#include "game_objects.h"
#include "game_engine.h"

GameObject::GameObject(double x, double y, double w, double h, int hp, ObjectType type)
    : x(x), y(y), width(w), height(h), speedX(0), speedY(0), hp(hp), maxHp(hp), isDead(false), type(type), animAccumulator(0.0) {
    animAccumulator = (std::rand() % 100) / 10.0;
}

void GameObject::update(double deltaTime) {
    x += speedX * deltaTime; y += speedY * deltaTime;
    if (speedX != 0  || speedY != 0) animAccumulator += deltaTime * 12.0;
}

double GameObject::getAnimOffset() const { return std::abs(std::sin(animAccumulator)) * 4.0; }

void GameObject::moveTowards(double targetX, double targetY, double speed) {
    double dx = targetX - x; double dy = targetY - y;
    double distance = std::sqrt(dx * dx + dy * dy);
    if (distance > 1.0) {
        speedX = (dx / distance) * speed; speedY = (dy / distance) * speed;
    } else {
        speedX = speedY = 0;
    }
}

QRectF GameObject::getRect() const {
    double k = (type == ObjectType::Projectile || type == ObjectType::EnemyProjectile) ? 1.0 : 0.5;
    return QRectF(x - (width * k) / 2, y - (height * k) / 2, width * k, height * k);
}

bool GameObject::getIsDead() const { return isDead; }
void GameObject::setDead(bool dead) { isDead = dead; }
int GameObject::getHp() const { return hp; }

void GameObject::takeDamage(int damage) {
    hp -= damage;
    if (hp <= 0) isDead = true;
}

ObjectType GameObject::getType() const { return type; }
double GameObject::getX() const { return x; }
double GameObject::getY() const { return y; }
void GameObject::setVelocity(double sx, double sy) { speedX = sx; speedY = sy; }

Projectile::Projectile(double x, double y, double sx, double sy, int dmg, ObjectType type)
    : GameObject(x, y, 10, 10, 1, type), damage(dmg) { setVelocity(sx, sy); }
int Projectile::getDamage() const { return damage; }

Soldier::Soldier(double x, double y, double hpModifier)
    : GameObject(x, y, 36, 36, static_cast<int>(10 * hpModifier), ObjectType::Soldier) {}

Kamikaze::Kamikaze(double x, double y, double hpModifier)
    : GameObject(x, y, 36, 36, static_cast<int>(15 * hpModifier), ObjectType::Kamikaze) {}

Tank::Tank(double x, double y, double hpModifier)
    : GameObject(x, y, 80, 80, static_cast<int>(150 * hpModifier), ObjectType::Tank), shootTimer(0.0) {}

void Tank::updateAI(double targetX, double targetY, double deltaTime, GameEngine& engine) {
    double dx = targetX - x;
    double dy = targetY - y;
    double distance = std::sqrt(dx * dx + dy * dy);

    if (distance > 250.0) {
        moveTowards(targetX, targetY, 20.0);
    } else {
        speedX = 0;
        speedY = 0;
    }

    shootTimer += deltaTime;
    if (shootTimer >= 3.0) {
        shootTimer = 0;
        double sx = (dx / distance) * 120.0;
        double sy = (dy / distance) * 120.0;
        engine.addObject(std::make_unique<Projectile>(x, y, sx, sy, 15, ObjectType::EnemyProjectile));
    }
}

Drone::Drone(double x, double y, double tx, double ty)
    : GameObject(x, y, 60, 60, 35, ObjectType::Drone), targetX(tx), targetY(ty) {}

void Drone::updateAI(double deltaTime) {
    double dx = targetX - x; double dy = targetY - y;
    if (std::sqrt(dx * dx + dy * dy) > 5.0) moveTowards(targetX, targetY, 45.0);
    else speedX = speedY = 0;
}