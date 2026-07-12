#include <QtTest>
#include <QFile>
#include "../Source/client.h"

class Unit : public QObject {
    Q_OBJECT
private slots:
    void test_creation() {
        Client c(101, "Иван", "Дневной", 5000.0, 10, true);
        QCOMPARE(c.getFullName(), QString("Иван"));
    }
    void test_editing() {
        Client c;
        c.setFullName("Петр");
        QCOMPARE(c.getFullName(), QString("Петр"));
    }
    void test_validation() {
        Client c;
        QVERIFY(c.setFullName("") == false);
        QVERIFY(c.setPrice(-100.0) == false);
    }
    void test_text_io() {
        Client c1(101, "Иван", "Дневной", 5000.0, 10, true);
        QFile f("test.txt");
        f.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&f); out << c1; f.close();
        Client c2;
        f.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&f); in >> c2; f.close();
        QCOMPARE(c2.getFullName(), c1.getFullName());
        QFile::remove("test.txt");
    }
    void test_binary_io() {
        Client c1(101, "Иван", "Дневной", 5000.0, 10, true);
        QFile f("test.dat");
        f.open(QIODevice::WriteOnly);
        QDataStream out(&f); out << c1; f.close();
        Client c2;
        f.open(QIODevice::ReadOnly);
        QDataStream in(&f); in >> c2; f.close();
        QCOMPARE(c2.getCardNumber(), c1.getCardNumber());
        QFile::remove("test.dat");
    }
    void test_file_error() {
        QFile f("none.txt");
        QVERIFY(f.open(QIODevice::ReadOnly) == false);
    }
};
QTEST_APPLESS_MAIN(Unit)
#include "tst_unit.moc"