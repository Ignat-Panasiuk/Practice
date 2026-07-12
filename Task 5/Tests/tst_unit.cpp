#include <QtTest>
#include <memory>
#include "../Source_code/game_engine.h"
#include "../Source_code/game_objects.h"

class UnitGameTest : public QObject
{
    Q_OBJECT

private slots:
    void test_object_creation()
    {
        Soldier soldier(100.0, 200.0, 1.0);
        QCOMPARE(soldier.getX(), 100.0);
        QCOMPARE(soldier.getY(), 200.0);
        QCOMPARE(soldier.getHp(), 10);
        QCOMPARE(soldier.getType(), ObjectType::Soldier);
        QVERIFY(soldier.getIsDead() == false);
    }

    void test_object_movement()
    {
        GameObject obj(0.0, 0.0, 10, 10, 100, ObjectType::Soldier);
        obj.setVelocity(100.0, 50.0);

        double deltaTime = 0.1;
        obj.update(deltaTime);

        QCOMPARE(obj.getX(), 10.0);
        QCOMPARE(obj.getY(), 5.0);
    }

    void test_object_damage()
    {
        Soldier soldier(100.0, 100.0, 1.0);
        QCOMPARE(soldier.getHp(), 10);

        soldier.takeDamage(4);
        QCOMPARE(soldier.getHp(), 6);
        QVERIFY(soldier.getIsDead() == false);

        soldier.takeDamage(10);
        QVERIFY(soldier.getIsDead() == true);
    }

    void test_projectile_collision()
    {
        GameEngine engine;
        engine.startNewGame(Difficulty::Normal);

        engine.addObject(std::make_unique<Soldier>(400.0, 400.0, 1.0));
        engine.addObject(std::make_unique<Projectile>(400.0, 400.0, 0.0, 0.0, 5));

        QCOMPARE(engine.objects.size(), 2);

        engine.checkCollisions();

        QVERIFY(engine.objects[1]->getIsDead() == true);
        QCOMPARE(engine.objects[0]->getHp(), 5);
    }

    void test_cleanup_dead()
    {
        GameEngine engine;
        engine.startNewGame(Difficulty::Normal);

        engine.addObject(std::make_unique<Soldier>(100.0, 100.0, 1.0));
        QCOMPARE(engine.objects.size(), 1);

        engine.objects[0]->setDead(true);
        engine.cleanupDeadObjects();

        QCOMPARE(engine.objects.size(), 0);
    }
};

QTEST_APPLESS_MAIN(UnitGameTest)
#include "tst_unit.moc"