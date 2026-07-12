#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_datecalc1_clicked();

    void on_datecalc2_clicked();

    void on_quadcalc_clicked();

    void on_prog_calc_clicked();

    void on_tricalc_clicked();

    void on_clear_date1_clicked();

    void on_clear_date2_clicked();

    void on_clear_quad_clicked();

    void on_clear_prog_clicked();

    void on_clear_tri_clicked();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
};
#endif // MAINWINDOW_H
