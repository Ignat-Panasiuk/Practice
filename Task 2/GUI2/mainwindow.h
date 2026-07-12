#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include "clientmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class WarehouseFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

private:
    QString m_searchText;
    QString m_selectedCategory;

public:
    explicit WarehouseFilterProxyModel(QObject *parent = nullptr);
    void setSearchText(const QString &text);
    void setSelectedCategory(const QString &category);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    ClientModel *m_model;
    WarehouseFilterProxyModel *m_proxyModel;

    void updateStatistics();
    void loadDefaultData();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_add_clicked();
    void on_remove_clicked();
    void on_save_clicked();
    void on_load_clicked();
    void on_search_button_clicked();
    void on_filter_category_currentIndexChanged(int index);
};

#endif // MAINWINDOW_H