#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "math_logic.h"
#include "date.h"
#include <cmath>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setStyleSheet("background-color: #2d2d2d; border: 1px solid #3f3f3f;");

    connect(ui->functions, QOverload<int>::of(&QComboBox::currentIndexChanged),
            ui->stackedWidget, &QStackedWidget::setCurrentIndex);
    ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool checkDoubleInput(QLineEdit* lineEdit, double& value, const QString& fieldName)
{
    if (lineEdit->text().trimmed().isEmpty())
    {
        QMessageBox::warning(nullptr, "Ошибка ввода", "Поле '" + fieldName + "' не заполнено!");
        return false;
    }
    bool ok;
    value = lineEdit->text().toDouble(&ok);
    if (!ok)
    {
        QMessageBox::warning(nullptr, "Ошибка ввода", "В поле '" + fieldName + "' должно быть введено число!");
        return false;
    }
    return true;
}


void MainWindow::on_datecalc1_clicked()
{
    QString inputStr = ui->date->text().trimmed();

    if (inputStr.isEmpty())
    {
        QMessageBox::warning(this, "Ошибка ввода", "Поле ввода даты пустое! Пожалуйста, введите дату.");
        return;
    }

    date::format_mode = ui->format1->currentIndex() + 1;

    date d;
    d.getdate(inputStr.toStdWString());

    int check_result = d.checkdate();
    if (check_result != 1)
    {
        QString error_msg;
        switch (check_result)
        {
        case 2:
            error_msg = "Ошибка ввода! Присутствуют буквы или год равен нулю.";
            break;
        case 3:
            error_msg = "Некорректный месяц! Номер месяца должен быть в диапазоне от 1 до 12.";
            break;
        case 4:
            error_msg = "Некорректный день! В выбранном месяце/году нет такого дня (например, 29 февраля в невисокосном году).";
            break;
        default:
            error_msg = "Введена некорректная дата.";
            break;
        }
        QMessageBox::warning(this, "Ошибка валидации даты", error_msg);
        return;
    }

    date now = date::today();
    long long diff = d - now;

    if (diff >= 0)
    {
        QMessageBox::warning(this, "Логическая ошибка", "Введенная дата должна быть меньше сегодняшней (в прошлом)!");
        return;
    }

    ui->date_res1->setText("Прошло дней с сегодня: " + QString::number(std::abs(diff)));
}

void MainWindow::on_datecalc2_clicked()
{
    QString inputStr1 = ui->date_2->text().trimmed();
    QString inputStr2 = ui->date_3->text().trimmed();

    if (inputStr1.isEmpty() || inputStr2.isEmpty())
    {
        QMessageBox::warning(this, "Ошибка ввода", "Оба поля ввода дат должны быть заполнены!");
        return;
    }

    date::format_mode = ui->format2->currentIndex() + 1;

    date d1, d2;
    d1.getdate(inputStr1.toStdWString());
    d2.getdate(inputStr2.toStdWString());

    int check1 = d1.checkdate();
    if (check1 != 1)
    {
        QString error_msg = "Ошибка в первой дате: ";
        if (check1 == 2) error_msg += "присутствуют буквы или год равен нулю.";
        else if (check1 == 3) error_msg += "месяц должен быть от 1 до 12.";
        else if (check1 == 4) error_msg += "некорректное число дней для этого месяца/года.";

        QMessageBox::warning(this, "Ошибка валидации первой даты", error_msg);
        return;
    }

    int check2 = d2.checkdate();
    if (check2 != 1)
    {
        QString error_msg = "Ошибка во второй дате: ";
        if (check2 == 2) error_msg += "присутствуют буквы или год равен нулю.";
        else if (check2 == 3) error_msg += "месяц должен быть от 1 до 12.";
        else if (check2 == 4) error_msg += "некорректное число дней для этого месяца/года.";

        QMessageBox::warning(this, "Ошибка валидации второй даты", error_msg);
        return;
    }

    long long diff = d1 - d2;
    ui->date_res2->setText("Разница между ними: " + QString::number(std::abs(diff)) + " дней");
}

void MainWindow::on_clear_date1_clicked()
{
    ui->date->clear();
    ui->date_res1->setText("Результат");
}

void MainWindow::on_clear_date2_clicked()
{
    ui->date_2->clear();
    ui->date_3->clear();
    ui->date_res2->setText("Результат");
}

void MainWindow::on_quadcalc_clicked()
{
    double a, b, c;
    if (!checkDoubleInput(ui->aq, a, "Коэффициент A") ||
        !checkDoubleInput(ui->bq, b, "Коэффициент B") ||
        !checkDoubleInput(ui->cq, c, "Коэффициент C"))
    {
        return;
    }

    if (a == 0)
    {
        QMessageBox::warning(this, "Математическая ошибка", "Коэффициент 'A' не может быть равен нулю в квадратном уравнении!");
        return;
    }

    quad_res res = math::quadratic(a, b, c);

    if (res.count == 0)
    {
        ui->quadres->setText("Действительных корней нет");
    }
    else if (res.count == 1)
    {
        ui->quadres->setText("Один корень:\nx = " + QString::number(res.x1, 'f', 3));
    }
    else
    {
        ui->quadres->setText("Два корня:\nx1 = " + QString::number(res.x1, 'f', 3) +
                             "\nx2 = " + QString::number(res.x2, 'f', 3));
    }
}

void MainWindow::on_clear_quad_clicked()
{
    ui->aq->clear();
    ui->bq->clear();
    ui->cq->clear();
    ui->quadres->setText("Результат");
}

void MainWindow::on_prog_calc_clicked()
{
    double first, diff;
    if (!checkDoubleInput(ui->first_el, first, "Первый член") ||
        !checkDoubleInput(ui->diff, diff, "Разность/Знаменатель"))
    {
        return;
    }

    if (ui->nn->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "Ошибка ввода", "Поле 'Количество членов (n)' не заполнено!");
        return;
    }

    bool ok;
    int n = ui->nn->text().toInt(&ok);
    if (!ok || n <= 0)
    {
        QMessageBox::warning(this, "Ошибка ввода", "Количество членов (n) должно быть целым положительным числом больше нуля!");
        return;
    }

    double res = 0;
    bool is_arith = (ui->prog_type->currentIndex() == 0);
    bool is_member = (ui->prog_mode->currentIndex() == 0);

    if (is_arith)
    {
        res = is_member ? math::arith_prog(first, diff, n) : math::arith_prog_sum(first, diff, n);
    }
    else
    {
        res = is_member ? math::geom_prog(first, diff, n) : math::geom_prog_sum(first, diff, n);
    }

    ui->prog_res->setText("Результат: " + QString::number(res));
}

void MainWindow::on_clear_prog_clicked()
{
    ui->first_el->clear();
    ui->diff->clear();
    ui->nn->clear();
    ui->prog_res->setText("Результат");
}

void MainWindow::on_tricalc_clicked()
{
    double a, b, c;
    if (!checkDoubleInput(ui->sideA, a, "Сторона A") ||
        !checkDoubleInput(ui->sideB, b, "Сторона B") ||
        !checkDoubleInput(ui->sideC, c, "Сторона C"))
    {
        return;
    }

    scene->clear();

    if (!math::is_triangle_exists(a, b, c))
    {
        QMessageBox::warning(this, "Математическая ошибка",
                             "Треугольник с такими сторонами не существует!\n"
                             "Сумма любых двух сторон должна быть строго больше третьей, а стороны должны быть больше нуля.");
        ui->triangle_res->setText("Ошибка существования");
        return;
    }

    double cosA = (b * b + c * c - a * a) / (2.0 * b * c);
    double sinA = std::sqrt(1.0 - cosA * cosA);

    double x1 = 0, y1 = 0;
    double x2 = c, y2 = 0;
    double x3 = b * cosA, y3 = b * sinA;

    double max_side = std::max({a, b, c});
    double view_size = std::min(ui->graphicsView->width(), ui->graphicsView->height()) - 40;
    double scale = view_size / max_side;

    QPointF p1(x1 * scale, -y1 * scale);
    QPointF p2(x2 * scale, -y2 * scale);
    QPointF p3(x3 * scale, -y3 * scale);

    QPolygonF polygon;
    polygon << p1 << p2 << p3;

    QPen pen(QColor(0, 191, 255), 2);
    QBrush brush(QColor(0, 191, 255, 45));

    scene->addPolygon(polygon, pen, brush);
    ui->graphicsView->setSceneRect(scene->itemsBoundingRect());
    ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);

    int op = ui->triop->currentIndex();
    QString res_text;
    double s = math::triangle_area(a, b, c);

    switch (op)
    {
    case 0:
        res_text = "Тип треугольника: " + QString::fromStdString(math::triangle_type(a, b, c));
        break;
    case 1:
        res_text = "Площадь S = " + QString::number(s, 'f', 3);
        break;
    case 2:
        res_text = "Периметр P = " + QString::number(a + b + c, 'f', 3);
        break;
    }
    ui->triangle_res->setText(res_text);
}

void MainWindow::on_clear_tri_clicked()
{
    ui->sideA->clear();
    ui->sideB->clear();
    ui->sideC->clear();
    scene->clear();
    ui->triangle_res->setText("Результат");
}