#include <QtTest>
#include "clientmodel.h"

class unit : public QObject
{
    Q_OBJECT

private slots:
    void test_initial_state()
    {
        ClientModel model;
        QCOMPARE(model.rowCount(), 0);
        QCOMPARE(model.columnCount(), 6);
    }

    void test_add_client()
    {
        ClientModel model;
        Client c(1001, "Иванов Игнат", "Безлимитный", 15000.0, 10);

        model.addClient(c);

        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.data(model.index(0, 0)).toInt(), 1001);
        QCOMPARE(model.data(model.index(0, 1)).toString(), QString("Иванов Игнат"));
        QCOMPARE(model.data(model.index(0, 5)).toDouble(), 1500.0);
    }

    void test_remove_client()
    {
        ClientModel model;
        model.addClient(Client(1001, "Клиент 1", "Дневной", 5000.0, 5));
        model.addClient(Client(1002, "Клиент 2", "Безлимитный", 10000.0, 10));

        QCOMPARE(model.rowCount(), 2);

        model.removeClient(0);

        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.data(model.index(0, 0)).toInt(), 1002);
    }

    void test_edit_and_validation()
    {
        ClientModel model;
        model.addClient(Client(1001, "Иван", "Дневной", 5000.0, 8));

        QVERIFY(model.setData(model.index(0, 1), "Иван Сергеевич") == true);
        QCOMPARE(model.data(model.index(0, 1)).toString(), QString("Иван Сергеевич"));

        QVERIFY(model.setData(model.index(0, 3), -500.0) == false);
        QCOMPARE(model.data(model.index(0, 3)).toDouble(), 5000.0);

        QVERIFY(model.setData(model.index(0, 4), -2) == false);
        QCOMPARE(model.data(model.index(0, 4)).toInt(), 8);
    }

    void test_statistics()
    {
        ClientModel model;
        model.addClient(Client(1001, "Клиент А", "Дневной", 1000.0, 10));
        model.addClient(Client(1002, "Клиент Б", "Безлимитный", 5000.0, 2));

        FitnessStats stats = model.getStatistics();

        QCOMPARE(stats.totalRevenue, 6000.0);
        QCOMPARE(stats.totalVisits, 12);
        QCOMPARE(stats.totalClients, 2);
        QCOMPARE(stats.averagePrice, 3000.0);
        QCOMPARE(stats.maxPrice, 5000.0);
        QCOMPARE(stats.expiringCount, 1);
    }

    void test_csv_file_io()
    {
        ClientModel modelSave;
        modelSave.addClient(Client(1001, "Игнат", "Дневной", 5000.0, 10));
        modelSave.addClient(Client(1002, "Мария", "Безлимитный", 12000.0, 15));

        QString testPath = "test_fitness.csv";

        QVERIFY(modelSave.saveToCsv(testPath) == true);

        ClientModel modelLoad;
        QVERIFY(modelLoad.loadFromCsv(testPath) == true);

        QCOMPARE(modelLoad.rowCount(), 2);
        QCOMPARE(modelLoad.data(modelLoad.index(0, 1)).toString(), QString("Игнат"));
        QCOMPARE(modelLoad.data(modelLoad.index(1, 4)).toInt(), 15);

        QFile::remove(testPath);
    }

    void test_json_file_io()
    {
        ClientModel modelSave;
        modelSave.addClient(Client(1001, "Иван", "Дневной", 5000.0, 10));

        QString testPath = "test_fitness.json";

        QVERIFY(modelSave.saveToJson(testPath) == true);

        ClientModel modelLoad;
        QVERIFY(modelLoad.loadFromJson(testPath) == true);

        QCOMPARE(modelLoad.rowCount(), 1);
        QCOMPARE(modelLoad.data(modelLoad.index(0, 1)).toString(), QString("Иван"));

        QFile::remove(testPath);
    }
};

QTEST_APPLESS_MAIN(unit)
#include "tst_unit.moc"