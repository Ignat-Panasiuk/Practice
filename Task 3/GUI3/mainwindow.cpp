#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QCloseEvent>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_isModified(false)
{
    ui->setupUi(this);

    setupTable();
    loadSettings();

    logAction("Приложение успешно запущено.");

    if (!m_currentFile.isEmpty() && QFile::exists(m_currentFile)) {
        QFile file(m_currentFile);
        if (file.open(QIODevice::ReadOnly)) {
            m_clients.clear();
            if (m_currentFile.endsWith(".txt")) {
                QTextStream in(&file);
                while (!in.atEnd()) {
                    Client c;
                    in >> c;
                    if (c.isValid()) m_clients.append(c);
                }
            } else {
                QDataStream in(&file);
                int count;
                in >> count;
                for (int i = 0; i < count; ++i) {
                    Client c;
                    in >> c;
                    m_clients.append(c);
                }
            }
            updateTable();
            m_isModified = false;
            logAction("Автоматически загружен последний рабочий файл: " + m_currentFile);
        }
    } else if (m_clients.isEmpty()) {
        m_clients.append(Client(1001, "Иванов Иван Иванович", "Безлимитный", 5000.0, 10, true));
        m_clients.append(Client(1002, "Петров Петр Петрович", "Дневной", 3000.0, 5, true));
        m_clients.append(Client(1003, "Сидорова Анна Сергеевна", "Вечерний", 4000.0, 0, false));
        updateTable();
        m_isModified = false;
    }

    clearForm();
}

MainWindow::~MainWindow()
{
    logAction("Приложение закрыто.");
    delete ui;
}

void MainWindow::logAction(const QString &msg)
{
    QString logLine = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + " - " + msg;

    QString logPath = QDir::homePath() + "/app_journal.txt";
    QFile logFile(logPath);
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&logFile);
        out << logLine << "\n";
    }

    ui->logEdit->append(logLine);
}

void MainWindow::setupTable()
{
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setHorizontalHeaderLabels({"Номер карты", "ФИО Клиента", "Тариф", "Оплата", "Остаток визитов", "Статус"});
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSortingEnabled(true);
}

void MainWindow::updateTable()
{
    ui->tableWidget->blockSignals(true);

    ui->tableWidget->setRowCount(m_clients.size());

    for (int i = 0; i < m_clients.size(); ++i) {
        const Client &c = m_clients[i];

        QTableWidgetItem *itemNum = new QTableWidgetItem();
        itemNum->setData(Qt::EditRole, c.getCardNumber());

        QTableWidgetItem *itemPrice = new QTableWidgetItem();
        itemPrice->setData(Qt::EditRole, c.getPrice());

        QTableWidgetItem *itemVisits = new QTableWidgetItem();
        itemVisits->setData(Qt::EditRole, c.getVisits());

        ui->tableWidget->setItem(i, 0, itemNum);
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(c.getFullName()));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(c.getSubscription()));
        ui->tableWidget->setItem(i, 3, itemPrice);
        ui->tableWidget->setItem(i, 4, itemVisits);
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(c.getIsActive() ? "Активен" : "Закрыт"));
    }

    ui->tableWidget->blockSignals(false);

    on_btnSearch_clicked();
}

void MainWindow::clearForm()
{
    int maxCard = 1000;
    for (const Client &c : m_clients) {
        if (c.getCardNumber() > maxCard) {
            maxCard = c.getCardNumber();
        }
    }
    ui->spinCard->setValue(maxCard + 1);

    ui->editName->clear();
    ui->spinPrice->setValue(0.0);
    ui->spinVisits->setValue(0);
    ui->checkActive->setChecked(true);

    ui->tableWidget->blockSignals(true);
    ui->tableWidget->clearSelection();
    ui->tableWidget->setCurrentCell(-1, -1);
    ui->tableWidget->blockSignals(false);

    ui->btnSaveRecord->setText("Добавить клиента");
}

void MainWindow::on_actionAdd_triggered()
{
    clearForm();
    ui->editName->setFocus();
    logAction("Форма сброшена для ввода новой записи.");
}

void MainWindow::on_btnSaveRecord_clicked()
{
    QString name = ui->editName->text().trimmed();

    if (name.isEmpty()) {
        QMessageBox::warning(this, "Ошибка валидации", "Поле 'ФИО клиента' должно быть заполнено!");
        return;
    }
    if (ui->spinPrice->value() < 0) {
        QMessageBox::warning(this, "Ошибка валидации", "Оплата не может быть отрицательной!");
        return;
    }

    Client c(ui->spinCard->value(), name, ui->comboSub->currentText(),
             ui->spinPrice->value(), ui->spinVisits->value(), ui->checkActive->isChecked());

    int row = ui->tableWidget->currentRow();
    bool isEditing = (row >= 0 && !ui->tableWidget->selectedItems().isEmpty());

    if (isEditing) {
        int id = ui->tableWidget->item(row, 0)->data(Qt::EditRole).toInt();
        for (int i = 0; i < m_clients.size(); ++i) {
            if (m_clients[i].getCardNumber() == id) {
                m_clients[i] = c;
                logAction("Изменены данные клиента: " + name);
                break;
            }
        }
    } else {
        m_clients.append(c);
        logAction("Добавлен новый клиент: " + name);
    }

    m_isModified = true;
    updateTable();
    clearForm();
}

void MainWindow::on_tableWidget_itemSelectionChanged()
{
    int row = ui->tableWidget->currentRow();
    if (row < 0 || ui->tableWidget->selectedItems().isEmpty()) return;

    int id = ui->tableWidget->item(row, 0)->data(Qt::EditRole).toInt();
    for (const Client &c : m_clients) {
        if (c.getCardNumber() == id) {
            ui->spinCard->setValue(c.getCardNumber());
            ui->editName->setText(c.getFullName());
            ui->comboSub->setCurrentText(c.getSubscription());
            ui->spinPrice->setValue(c.getPrice());
            ui->spinVisits->setValue(c.getVisits());
            ui->checkActive->setChecked(c.getIsActive());

            ui->btnSaveRecord->setText("Сохранить изменения");
            break;
        }
    }
}

void MainWindow::on_actionDelete_triggered()
{
    int row = ui->tableWidget->currentRow();
    if (row < 0 || ui->tableWidget->selectedItems().isEmpty()) {
        QMessageBox::warning(this, "Удаление записи", "Выберите запись в таблице для удаления.");
        return;
    }

    if (QMessageBox::question(this, "Удаление записи", "Вы уверены, что хотите удалить выбранного клиента?") == QMessageBox::Yes) {
        int id = ui->tableWidget->item(row, 0)->data(Qt::EditRole).toInt();
        for (int i = 0; i < m_clients.size(); ++i) {
            if (m_clients[i].getCardNumber() == id) {
                logAction("Удален клиент из базы: " + m_clients[i].getFullName());
                m_clients.removeAt(i);
                break;
            }
        }
        m_isModified = true;
        updateTable();
        clearForm();
    }
}

void MainWindow::on_btnSearch_clicked()
{
    QString text = ui->editSearch->text().trimmed();

    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        bool match = ui->tableWidget->item(i, 1)->text().contains(text, Qt::CaseInsensitive);
        ui->tableWidget->setRowHidden(i, !match);
    }

    logAction("Выполнен поиск клиентов. Запрос: " + (text.isEmpty() ? "[Сброшен]" : text));
}

void MainWindow::on_actionOpen_triggered()
{
    if (!askToSave()) return;

    QString filePath = QFileDialog::getOpenFileName(this, "Открыть базу данных клиентов", m_currentFile, "Текстовые файлы (*.txt);;Бинарные файлы (*.dat)");
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Ошибка чтения", "Не удалось открыть файл для чтения.");
        return;
    }

    m_clients.clear();

    if (filePath.endsWith(".txt")) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            Client c;
            in >> c;
            if (c.isValid()) m_clients.append(c);
        }
    } else {
        QDataStream in(&file);
        int count;
        in >> count;
        for (int i = 0; i < count; ++i) {
            Client c;
            in >> c;
            m_clients.append(c);
        }
    }

    m_currentFile = filePath;
    m_isModified = false;
    logAction("База данных успешно загружена из файла: " + filePath);
    updateTable();
    clearForm();
}

void MainWindow::on_actionSave_triggered()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Сохранить базу данных клиентов", m_currentFile, "Текстовые файлы (*.txt);;Бинарные файлы (*.dat)");
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Ошибка записи", "Не удалось сохранить файл.");
        return;
    }

    if (filePath.endsWith(".txt")) {
        QTextStream out(&file);
        for (const Client &c : m_clients) out << c;
    } else {
        QDataStream out(&file);
        out << m_clients.size();
        for (const Client &c : m_clients) out << c;
    }

    m_currentFile = filePath;
    m_isModified = false;
    logAction("База данных успешно сохранена в файл: " + filePath);
    QMessageBox::information(this, "Сохранение", "Данные успешно записаны.");
}

bool MainWindow::askToSave()
{
    if (!m_isModified) return true;
    auto reply = QMessageBox::warning(this, "Несохраненные изменения",
                                      "В базе данных есть изменения.\nСохранить их перед выходом?",
                                      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (reply == QMessageBox::Save) {
        on_actionSave_triggered();
        return !m_isModified;
    }
    return reply == QMessageBox::Discard;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (askToSave()) {
        saveSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::loadSettings()
{
    QSettings settings("StudentDevTeam", "GymClientsApp");
    restoreGeometry(settings.value("geometry").toByteArray());
    m_currentFile = settings.value("lastFile", "").toString();
}

void MainWindow::saveSettings()
{
    QSettings settings("StudentDevTeam", "GymClientsApp");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("lastFile", m_currentFile);
}

void MainWindow::on_actionAbout_2_triggered()
{
    QMessageBox::about(this, "О программе",
                       "Вариант 8: Карточка клиента");
}