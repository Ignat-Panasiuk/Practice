#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_currentZoom(1.0)
{
    ui->setupUi(this);

    connect(ui->actionUndo, &QAction::triggered, ui->canvas, &PaintCanvas::undo);
    connect(ui->actionRedo, &QAction::triggered, ui->canvas, &PaintCanvas::redo);
    connect(ui->actionCopy, &QAction::triggered, ui->canvas, &PaintCanvas::copy);
    connect(ui->actionPaste, &QAction::triggered, ui->canvas, &PaintCanvas::paste);

    ui->actionUndo->setShortcut(QKeySequence::Undo);
    ui->actionRedo->setShortcut(QKeySequence::Redo);
    ui->actionCopy->setShortcut(QKeySequence::Copy);
    ui->actionPaste->setShortcut(QKeySequence::Paste);

    ui->canvas->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnLine_clicked() { ui->canvas->setCurrentTool(TypeLine); }
void MainWindow::on_btnRect_clicked() { ui->canvas->setCurrentTool(TypeRect); }
void MainWindow::on_btnEllipse_clicked() { ui->canvas->setCurrentTool(TypeEllipse); }
void MainWindow::on_btnTriangle_clicked() { ui->canvas->setCurrentTool(TypeTriangle); }
void MainWindow::on_btnCurve_clicked() { ui->canvas->setCurrentTool(TypeCurve); }

void MainWindow::on_btnPenColor_clicked()
{
    QColor color = QColorDialog::getColor(ui->canvas->penColor(), this, "Выберите цвет линии");
    if (color.isValid()) {
        ui->canvas->setPenColor(color);
    }
}

void MainWindow::on_btnBrushColor_clicked()
{
    QColor color = QColorDialog::getColor(ui->canvas->brushColor(), this, "Выберите цвет заливки", QColorDialog::ShowAlphaChannel);
    if (color.isValid()) {
        ui->canvas->setBrushColor(color);
    }
}

void MainWindow::on_spinPenWidth_valueChanged(int width)
{
    ui->canvas->setPenWidth(width);
}

void MainWindow::on_btnClear_clicked()
{
    auto reply = QMessageBox::question(this, "Очистить холст", "Вы уверены, что хотите полностью стереть рисунок?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        ui->canvas->clearScene();
    }
}

void MainWindow::on_btnDelete_clicked()
{
    if (!ui->canvas->hasSelected()) {
        QMessageBox::information(this, "Удаление", "Сначала выделите фигуру на холсте кликом левой кнопки мыши!");
        return;
    }

    auto reply = QMessageBox::question(this, "Удалить фигуру", "Вы действительно хотите удалить выбранный объект?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        ui->canvas->deleteSelected();
    }
}

void MainWindow::on_actionOpen_triggered()
{
    if (!askToSave()) return;

    QString fileName = QFileDialog::getOpenFileName(this, "Открыть сцену", "", "Векторный файл (*.bin)");
    if (!fileName.isEmpty()) {
        if (ui->canvas->loadFromFile(fileName)) {
            QMessageBox::information(this, "Успех", "Векторный рисунок успешно загружен!");
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось прочесть файл сцены!");
        }
    }
}

void MainWindow::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить сцену", "", "Векторный файл (*.bin)");
    if (!fileName.isEmpty()) {
        if (ui->canvas->saveToFile(fileName)) {
            QMessageBox::information(this, "Успех", "Векторный рисунок успешно сохранен!");
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось сохранить файл сцены!");
        }
    }
}

bool MainWindow::askToSave()
{
    if (!ui->canvas->isModified()) return true;

    auto reply = QMessageBox::warning(this, "Несохраненные изменения",
                                      "В рисунке есть несохраненные изменения.\nХотите сохранить их перед выходом?",
                                      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (reply == QMessageBox::Save) {
        on_actionSave_triggered();
        return !ui->canvas->isModified();
    }
    return reply == QMessageBox::Discard;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (askToSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}
void MainWindow::on_actionBringToFront_triggered() { ui->canvas->bringToFront(); }
void MainWindow::on_actionSendToBack_triggered() { ui->canvas->sendToBack(); }
void MainWindow::on_actionToggleGrid_triggered(bool checked) { ui->canvas->setGridSnap(checked); }

void MainWindow::on_actionZoomIn_triggered()
{
    m_currentZoom += 0.1;
    ui->canvas->setZoom(m_currentZoom);
}

void MainWindow::on_actionZoomOut_triggered()
{
    if (m_currentZoom > 0.2) {
        m_currentZoom -= 0.1;
        ui->canvas->setZoom(m_currentZoom);
    }
}