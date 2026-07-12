#include <QTest>
#include <QFile>
#include <QBuffer>
#include "shape.h"

class UnitShapeTest : public QObject
{
    Q_OBJECT

private slots:
    void test_shape_properties();
    void test_shape_movement();
    void test_rect_containment();
    void test_ellipse_containment();
    void test_shape_serialization();
    void test_scene_disk_io();
};

void UnitShapeTest::test_shape_properties()
{
    RectShape rect;
    QCOMPARE(rect.type(), TypeRect);

    rect.setStartPoint(QPoint(10, 20));
    rect.setEndPoint(QPoint(100, 200));
    QCOMPARE(rect.startPoint(), QPoint(10, 20));
    QCOMPARE(rect.endPoint(), QPoint(100, 200));

    rect.setPenColor(Qt::red);
    rect.setBrushColor(Qt::blue);
    rect.setPenWidth(5);
    QCOMPARE(rect.penColor(), QColor(Qt::red));
    QCOMPARE(rect.brushColor(), QColor(Qt::blue));
    QCOMPARE(rect.penWidth(), 5);

    rect.setSelected(true);
    QVERIFY(rect.isSelected() == true);
}

void UnitShapeTest::test_shape_movement()
{
    LineShape line;
    line.setStartPoint(QPoint(50, 50));
    line.setEndPoint(QPoint(150, 150));

    line.move(10, -20);

    QCOMPARE(line.startPoint(), QPoint(60, 30));
    QCOMPARE(line.endPoint(), QPoint(160, 130));
}

void UnitShapeTest::test_rect_containment()
{
    RectShape rect;
    rect.setStartPoint(QPoint(0, 0));
    rect.setEndPoint(QPoint(100, 100));

    QVERIFY(rect.contains(QPoint(50, 50)) == true);
    QVERIFY(rect.contains(QPoint(5, 5)) == true);
    QVERIFY(rect.contains(QPoint(-10, 50)) == false);
    QVERIFY(rect.contains(QPoint(101, 100)) == false);
}

void UnitShapeTest::test_ellipse_containment()
{
    EllipseShape ellipse;
    ellipse.setStartPoint(QPoint(0, 0));
    ellipse.setEndPoint(QPoint(100, 100));

    QVERIFY(ellipse.contains(QPoint(50, 50)) == true);
    QVERIFY(ellipse.contains(QPoint(10, 10)) == false);
    QVERIFY(ellipse.contains(QPoint(90, 90)) == false);
}

void UnitShapeTest::test_shape_serialization()
{
    RectShape rectWrite;
    rectWrite.setStartPoint(QPoint(15, 25));
    rectWrite.setEndPoint(QPoint(115, 125));
    rectWrite.setPenColor(Qt::green);
    rectWrite.setBrushColor(Qt::yellow);
    rectWrite.setPenWidth(4);

    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    rectWrite.serialize(out);

    RectShape rectRead;
    QDataStream in(&buffer, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_15);
    rectRead.deserialize(in);

    QCOMPARE(rectRead.startPoint(), QPoint(15, 25));
    QCOMPARE(rectRead.endPoint(), QPoint(115, 125));
    QCOMPARE(rectRead.penColor(), QColor(Qt::green));
    QCOMPARE(rectRead.brushColor(), QColor(Qt::yellow));
    QCOMPARE(rectRead.penWidth(), 4);
}

void UnitShapeTest::test_scene_disk_io()
{
    QVector<Shape*> sceneWrite;

    RectShape* rect = new RectShape();
    rect->setStartPoint(QPoint(10, 10));
    rect->setEndPoint(QPoint(50, 50));
    rect->setPenColor(Qt::red);
    rect->setBrushColor(Qt::blue);
    rect->setPenWidth(3);
    sceneWrite.append(rect);

    LineShape* line = new LineShape();
    line->setStartPoint(QPoint(0, 0));
    line->setEndPoint(QPoint(100, 100));
    line->setPenColor(Qt::green);
    line->setPenWidth(1);
    sceneWrite.append(line);

    QFile file("test_scene.bin");
    QVERIFY(file.open(QIODevice::WriteOnly));

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_15);
    out << (int)sceneWrite.size();
    for (Shape *shape : sceneWrite) {
        out << (int)shape->type();
        shape->serialize(out);
    }
    file.close();

    qDeleteAll(sceneWrite);
    sceneWrite.clear();

    QVERIFY(file.open(QIODevice::ReadOnly));
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_15);
    int size = 0;
    in >> size;
    QCOMPARE(size, 2);

    QVector<Shape*> sceneRead;
    for (int i = 0; i < size; ++i) {
        int typeVal = 0;
        in >> typeVal;
        ShapeType type = (ShapeType)typeVal;
        Shape *shape = nullptr;
        switch (type) {
        case TypeLine: shape = new LineShape(); break;
        case TypeRect: shape = new RectShape(); break;
        default: break;
        }
        if (shape) {
            shape->deserialize(in);
            sceneRead.append(shape);
        }
    }
    file.close();

    QCOMPARE(sceneRead.size(), 2);
    QCOMPARE(sceneRead[0]->type(), TypeRect);
    QCOMPARE(sceneRead[0]->startPoint(), QPoint(10, 10));
    QCOMPARE(sceneRead[0]->penColor(), QColor(Qt::red));
    QCOMPARE(sceneRead[0]->brushColor(), QColor(Qt::blue));
    QCOMPARE(sceneRead[0]->penWidth(), 3);

    QCOMPARE(sceneRead[1]->type(), TypeLine);
    QCOMPARE(sceneRead[1]->startPoint(), QPoint(0, 0));
    QCOMPARE(sceneRead[1]->endPoint(), QPoint(100, 100));

    qDeleteAll(sceneRead);
    QFile::remove("test_scene.bin");
}

QTEST_APPLESS_MAIN(UnitShapeTest)
#include "tst_unit.moc"