#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QHeaderView>

WarehouseFilterProxyModel::WarehouseFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void WarehouseFilterProxyModel::setSearchText(const QString &text)
{
    m_searchText = text.trimmed();
    invalidateFilter();
}

void WarehouseFilterProxyModel::setSelectedCategory(const QString &category)
{
    m_selectedCategory = category;
    invalidateFilter();
}

bool WarehouseFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex indexName = sourceModel()->index(source_row, 1, source_parent);
    QModelIndex indexSub = sourceModel()->index(source_row, 2, source_parent);

    QString name = sourceModel()->data(indexName).toString();
    QString sub = sourceModel()->data(indexSub).toString();

    bool nameMatch = name.contains(m_searchText, Qt::CaseInsensitive);
    bool categoryMatch = (m_selectedCategory == "Все тарифы" || m_selectedCategory.isEmpty() || sub == m_selectedCategory);

    return nameMatch && categoryMatch;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_model = new ClientModel(this);
    m_proxyModel = new WarehouseFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);

    ui->tableView->setModel(m_proxyModel);
    ui->tableView->setSortingEnabled(true);

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    ui->filter_category->addItems({
        "Все тарифы",
        "Дневной",
        "Безлимитный",
        "Выходного дня",
        "Индивидуальный"
    });

    loadDefaultData();
    updateStatistics();

    connect(m_proxyModel, &QAbstractItemModel::dataChanged, this, &MainWindow::updateStatistics);
    connect(m_proxyModel, &QAbstractItemModel::rowsInserted, this, &MainWindow::updateStatistics);
    connect(m_proxyModel, &QAbstractItemModel::rowsRemoved, this, &MainWindow::updateStatistics);
    connect(m_proxyModel, &QAbstractItemModel::modelReset, this, &MainWindow::updateStatistics);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadDefaultData()
{
    m_model->addClient(Client(1001, "Иванов Игнат Сергеевич", "Безлимитный", 150.0, 30));
    m_model->addClient(Client(1002, "Петров Петр Петрович", "Дневной", 80.0, 2));
    m_model->addClient(Client(1003, "Сидорова Анна Ивановна", "Индивидуальный", 250.0, 10));
    m_model->addClient(Client(1004, "Дмитриев Дмитрий Дмитриевич", "Выходного дня", 60.0, 1));
    m_model->addClient(Client(1005, "Козлова Мария Витальевна", "Безлимитный", 150.0, 12));
    m_model->addClient(Client(1006, "Васильев Василий Васильевич", "Дневной", 80.0, 5));
    m_model->addClient(Client(1007, "Смирнов Алексей Александрович", "Безлимитный", 150.0, 1));
    m_model->addClient(Client(1008, "Павлова Ольга Петровна", "Выходного дня", 60.0, 8));
    m_model->addClient(Client(1009, "Федоров Федор Федорович", "Индивидуальный", 300.0, 15));
    m_model->addClient(Client(1010, "Тарасова Елена Тимофеевна", "Дневной", 80.0, 0));
}

void MainWindow::updateStatistics()
{
    double totalValue = 0.0;
    int totalQuantity = 0;
    int uniqueProducts = m_proxyModel->rowCount();
    double priceSum = 0.0;
    double maxPrice = -1.0;
    int lowStockCount = 0;

    for (int i = 0; i < uniqueProducts; ++i)
    {
        QModelIndex indexName = m_proxyModel->index(i, 1);
        QModelIndex indexPrice = m_proxyModel->index(i, 3);
        QModelIndex indexQuantity = m_proxyModel->index(i, 4);

        double price = m_proxyModel->data(indexPrice).toDouble();
        int quantity = m_proxyModel->data(indexQuantity).toInt();

        totalValue += price;
        totalQuantity += quantity;
        priceSum += price;

        if (price > maxPrice)
        {
            maxPrice = price;
        }

        if (quantity < 3)
        {
            lowStockCount++;
        }
    }

    double averagePrice = (uniqueProducts > 0) ? (priceSum / uniqueProducts) : 0.0;
    double displayMaxPrice = (maxPrice < 0.0) ? 0.0 : maxPrice;

    ui->total_value->setText(QString::number(totalValue, 'f', 2) + " руб.");
    ui->total_quantity->setText(QString::number(totalQuantity) + " шт.");
    ui->unique_products->setText(QString::number(uniqueProducts));
    ui->average_price->setText(QString::number(averagePrice, 'f', 2) + " руб.");
    ui->most_expensive->setText(QString::number(displayMaxPrice, 'f', 2) + " руб.");
    ui->low_stock_count->setText(QString::number(lowStockCount));
}

void MainWindow::on_add_clicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Добавление клиента");

    QFormLayout form(&dialog);

    QSpinBox *sbId = new QSpinBox(&dialog);
    sbId->setRange(1, 999999);
    form.addRow("Номер карты:", sbId);

    QLineEdit *leName = new QLineEdit(&dialog);
    form.addRow("ФИО Клиента:", leName);

    QComboBox *cbCategory = new QComboBox(&dialog);
    cbCategory->addItems({
        "Дневной",
        "Безлимитный",
        "Выходного дня",
        "Индивидуальный"
    });
    form.addRow("Тариф абонемента:", cbCategory);

    QDoubleSpinBox *dsbPrice = new QDoubleSpinBox(&dialog);
    dsbPrice->setRange(0.01, 1000000.0);
    dsbPrice->setDecimals(2);
    form.addRow("Размер оплаты (руб.):", dsbPrice);

    QSpinBox *sbQuantity = new QSpinBox(&dialog);
    sbQuantity->setRange(0, 10000);
    form.addRow("Остаток визитов:", sbQuantity);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted)
    {
        if (leName->text().trimmed().isEmpty())
        {
            QMessageBox::warning(this, "Ошибка ввода", "ФИО клиента не может быть пустым!");
            return;
        }

        Client c(
            sbId->value(),
            leName->text().trimmed(),
            cbCategory->currentText(),
            dsbPrice->value(),
            sbQuantity->value()
            );

        m_model->addClient(c);
        updateStatistics();
    }
}

void MainWindow::on_remove_clicked()
{
    QModelIndex proxyIndex = ui->tableView->currentIndex();
    if (!proxyIndex.isValid())
    {
        QMessageBox::warning(this, "Ошибка удаления", "Пожалуйста, выберите клиента для удаления!");
        return;
    }

    QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);
    m_model->removeClient(sourceIndex.row());
    updateStatistics();
}

void MainWindow::on_save_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Сохранить базу клиентов",
        "",
        "CSV файлы (*.csv);;JSON файлы (*.json)"
        );

    if (filePath.isEmpty())
    {
        return;
    }

    bool success = false;
    if (filePath.endsWith(".csv", Qt::CaseInsensitive))
    {
        success = m_model->saveToCsv(filePath);
    }
    else if (filePath.endsWith(".json", Qt::CaseInsensitive))
    {
        success = m_model->saveToJson(filePath);
    }

    if (success)
    {
        QMessageBox::information(this, "Сохранение", "Данные успешно сохранены в файл!");
    }
    else
    {
        QMessageBox::critical(this, "Ошибка сохранения", "Не удалось открыть файл для записи!");
    }
}

void MainWindow::on_load_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Загрузить данные склада",
        "",
        "CSV файлы (*.csv);;JSON файлы (*.json)"
        );

    if (filePath.isEmpty())
    {
        return;
    }

    bool success = false;
    if (filePath.endsWith(".csv", Qt::CaseInsensitive))
    {
        success = m_model->loadFromCsv(filePath);
    }
    else if (filePath.endsWith(".json", Qt::CaseInsensitive))
    {
        success = m_model->loadFromJson(filePath);
    }

    if (success)
    {
        QMessageBox::information(this, "Загрузка", "Данные успешно загружены!");
        updateStatistics();
    }
    else
    {
        QMessageBox::critical(this, "Ошибка загрузки", "Не удалось прочитать файл!");
    }
}

void MainWindow::on_search_button_clicked()
{
    m_proxyModel->setSearchText(ui->search->text());
    updateStatistics();
}

void MainWindow::on_filter_category_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    m_proxyModel->setSelectedCategory(ui->filter_category->currentText());
    updateStatistics();
}