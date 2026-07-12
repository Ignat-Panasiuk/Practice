#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "shape.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_btnLine_clicked();
    void on_btnRect_clicked();
    void on_btnEllipse_clicked();
    void on_btnTriangle_clicked();
    void on_btnCurve_clicked();

    void on_btnPenColor_clicked();
    void on_btnBrushColor_clicked();
    void on_spinPenWidth_valueChanged(int arg1);

    void on_btnClear_clicked();
    void on_btnDelete_clicked();

    void on_actionOpen_triggered();
    void on_actionSave_triggered();

    void on_actionBringToFront_triggered();
    void on_actionSendToBack_triggered();
    void on_actionToggleGrid_triggered(bool checked);
    void on_actionZoomIn_triggered();
    void on_actionZoomOut_triggered();

private:
    Ui::MainWindow *ui;
    double m_currentZoom;
    bool askToSave();
};

#endif