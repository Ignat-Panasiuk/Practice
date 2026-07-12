#include <QTest>
#include "math_logic.h"
#include "date.h"

class Unit : public QObject
{
    Q_OBJECT

public:
    Unit();
    ~Unit() override;

private slots:
    void test_quadratic()
    {
        quad_res res1 = math::quadratic(1, -3, 2);
        QCOMPARE(res1.count, 2);
        QVERIFY(qFuzzyCompare(res1.x1, 1.0) || qFuzzyCompare(res1.x2, 1.0));
        QVERIFY(qFuzzyCompare(res1.x1, 2.0) || qFuzzyCompare(res1.x2, 2.0));

        quad_res res2 = math::quadratic(1, -2, 1);
        QCOMPARE(res2.count, 1);
        QVERIFY(qFuzzyCompare(res2.x1, 1.0));
        QVERIFY(qFuzzyCompare(res2.x2, 1.0));

        quad_res res3 = math::quadratic(1, 1, 1);
        QCOMPARE(res3.count, 0);
        QVERIFY(qFuzzyCompare(res3.x1, 0.0));
        QVERIFY(qFuzzyCompare(res3.x2, 0.0));
    }

    void test_triangle()
    {
        QVERIFY(math::is_triangle_exists(3,4,5) == true);
        QCOMPARE(math::triangle_area(3,4,5), 6.0);
        QCOMPARE(math::perimeter(3,4,5), 12.0);
        QCOMPARE(math::triangle_type(3,3,3), std::string("равносторонний"));
        QCOMPARE(math::triangle_type(3,4,5), std::string("разносторонний"));
        QCOMPARE(math::triangle_type(3,5,5), std::string("равнобедренный"));
        QVERIFY(math::is_triangle_exists(1,2,10) == false);
        QVERIFY(math::is_triangle_exists(0,5,5) == false);
        QVERIFY(math::is_triangle_exists(-3,4,5) == false);
    }

    void test_prog()
    {
        QCOMPARE(math::arith_prog(2, 3, 4), 11.0);
        QCOMPARE(math::arith_prog(10, 5, 1), 10.0);
        QCOMPARE(math::arith_prog(10, -2, 3), 6.0);
        QCOMPARE(math::arith_prog(10, 0, 5), 10.0);
        QCOMPARE(math::arith_prog_sum(2, 3, 4), 26);
        QCOMPARE(math::arith_prog_sum(10, 0, 5), 50.0);

        QCOMPARE(math::geom_prog(2, 3, 3), 18.0);
        QCOMPARE(math::geom_prog(5, 1, 10), 5.0);
        QCOMPARE(math::geom_prog(10, -2, 2), -20.0);
        QCOMPARE(math::geom_prog(10, -2, 3), 40.0);
        QCOMPARE(math::geom_prog_sum(2, 3, 4), 80.0);
        QCOMPARE(math::geom_prog_sum(2, -3, 4), -40.0);
        QCOMPARE(math::geom_prog_sum(10, 1, 5), 50.0);
    }

    void test_date_logic()
    {
        date wrong_year (30, 1, 0);
        QCOMPARE(wrong_year.checkdate(), 2);

        date wrong_month1 (30, -1, 2018);
        QCOMPARE(wrong_month1.checkdate(), 3);

        date wrong_month2 (30, 13, 2018);
        QCOMPARE(wrong_month2.checkdate(), 3);

        date wrong_day1 (29, 2, 2018);
        QCOMPARE(wrong_day1.checkdate(), 4);

        date wrong_day2 (-1, 1, 2018);
        QCOMPARE(wrong_day2.checkdate(), 4);

        date real (29, 2, 2000);
        QCOMPARE(real.checkdate(), 1);

        date first1 (1, 1, 2000);
        date second1 (1, 1, 2000);
        QCOMPARE(second1 - first1, 0LL);

        date first2 (31, 12, 2000);
        date second2 (1, 1, 2001);
        QCOMPARE(second2 - first2, 1LL);

        date first3 (1, 1, 2000);
        date second3 (1, 2, 2000);
        QCOMPARE(second3 - first3, 31LL);

        date first4 (31, 12, -1);
        date second4 (1, 1, 1);
        QCOMPARE(second4 - first4, 1LL);

        date first5 (1, 1, 1999);
        date second5 (1, 1, 2000);
        QCOMPARE(second5 - first5, 365LL);

        date first6 (1, 1, 2000);
        date second6 (1, 1, 2001);
        QCOMPARE(second6 - first6, 366LL);

        date third1;
        date::format_mode = 1;
        third1.getdate(L"10.05.2023");
        QCOMPARE(third1.getday(), 10);
        QCOMPARE(third1.getmonth(), 5);
        QCOMPARE(third1.getyear(), 2023);

        date third2;
        date::format_mode = 2;
        third2.getdate(L"двадцать девятое февраля 2000 года до нашей эры");
        QCOMPARE(third2.getday(), 29);
        QCOMPARE(third2.getmonth(), 2);
        QCOMPARE(third2.getyear(), -2000);

        date current = date::today();
        QVERIFY(current.getyear() != 0);
        QVERIFY(current.getmonth() >= 1 && current.getmonth() <= 12);
        QVERIFY(current.getday() >= 1 && current.getday() <= 31);

    }
};

Unit::Unit() {}

Unit::~Unit() = default;

QTEST_APPLESS_MAIN(Unit)

#include "tst_unit.moc"
