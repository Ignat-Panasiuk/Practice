#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include "../Source/client.h"

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
    void on_btnSaveRecord_clicked();
    void on_btnSearch_clicked();
    void on_tableWidget_itemSelectionChanged();
    void on_actionAbout_2_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionAdd_triggered();
    void on_actionDelete_triggered();

private:
    Ui::MainWindow *ui;
    QVector<Client> m_clients;
    QString m_currentFile;
    bool m_isModified;

    void setupTable();
    void updateTable();
    void clearForm();

    bool askToSave();
    void loadSettings();
    void saveSettings();
    void logAction(const QString &msg);
};

#endif