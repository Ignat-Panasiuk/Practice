#include "clientmodel.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QColor>
#include <QFile>
#include <QIODevice>
#include <QTextStream>

ClientModel::ClientModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int ClientModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return m_clients.size();
}

int ClientModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return 6;
}

QVariant ClientModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    int row = index.row();
    int col = index.column();

    if (row < 0 || row >= m_clients.size())
    {
        return QVariant();
    }

    const Client &c = m_clients.at(row);

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (col)
        {
        case 0: return c.cardNumber;
        case 1: return c.fullName;
        case 2: return c.subscription;
        case 3: return c.price;
        case 4: return c.visitsLeft;
        case 5:
            {

                double cost = (c.visitsLeft > 0) ? (c.price / c.visitsLeft) : 0.0;
                return QString::number(cost, 'f', 2);
            }
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        if (col == 0 || col >= 3)
        {
            return Qt::AlignCenter;
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        if (c.visitsLeft < 3)
        {
            return QColor(255, 100, 100, 50);
        }
    }

    return QVariant();
}

QVariant ClientModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0: return "№ Карты";
        case 1: return "ФИО Клиента";
        case 2: return "Тариф";
        case 3: return "Оплачено";
        case 4: return "Остаток визитов";
        case 5: return "Цена 1 визита";
        }
    }
    else
    {
        return section + 1;
    }

    return QVariant();
}

bool ClientModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole)
    {
        return false;
    }

    int row = index.row();
    int col = index.column();

    Client &c = m_clients[row];

    switch (col)
    {
    case 0:
        c.cardNumber = value.toInt();
        break;
    case 1:
        c.fullName = value.toString();
        break;
    case 2:
        c.subscription = value.toString();
        break;
    case 3:
    {
        double val = value.toDouble();
        if (val < 0.0)
        {
            return false;
        }
        c.price = val;
        break;
    }
    case 4:
    {
        int val = value.toInt();
        if (val < 0)
        {
            return false;
        }
        c.visitsLeft = val;
        break;
    }
    default:
        return false;
    }

    emit dataChanged(index, index, {Qt::DisplayRole, Qt::BackgroundRole});
    if (col == 3 || col == 4)
    {
        emit dataChanged(this->index(row, 5), this->index(row, 5), {Qt::DisplayRole});
    }

    return true;
}

Qt::ItemFlags ClientModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    if (index.column() == 5)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void ClientModel::addClient(const Client &client)
{
    beginInsertRows(QModelIndex(), m_clients.size(), m_clients.size());
    m_clients.append(client);
    endInsertRows();
}

void ClientModel::removeClient(int row)
{
    if (row < 0 || row >= m_clients.size())
    {
        return;
    }
    beginRemoveRows(QModelIndex(), row, row);
    m_clients.removeAt(row);
    endRemoveRows();
}

void ClientModel::clear()
{
    beginResetModel();
    m_clients.clear();
    endResetModel();
}

FitnessStats ClientModel::getStatistics() const
{
    FitnessStats stats;
    stats.totalRevenue = 0.0;
    stats.totalVisits = 0;
    stats.totalClients = m_clients.size();
    stats.averagePrice = 0.0;
    stats.maxPrice = 0.0;
    stats.expiringCount = 0;

    if (m_clients.isEmpty())
    {
        return stats;
    }

    double maxPrice = -1.0;
    double priceSum = 0.0;

    for (const Client &c : m_clients)
    {
        stats.totalRevenue += c.price;
        stats.totalVisits += c.visitsLeft;
        priceSum += c.price;

        if (c.price > maxPrice)
        {
            maxPrice = c.price;
        }

        if (c.visitsLeft < 3)
        {
            stats.expiringCount++;
        }
    }

    stats.averagePrice = priceSum / m_clients.size();
    stats.maxPrice = (maxPrice < 0.0) ? 0.0 : maxPrice;

    return stats;
}

bool ClientModel::saveToCsv(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    for (const Client &c : m_clients)
    {
        out << c.cardNumber << ";"
            << c.fullName << ";"
            << c.subscription << ";"
            << c.price << ";"
            << c.visitsLeft << "\n";
    }

    file.close();
    return true;
}

bool ClientModel::loadFromCsv(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    beginResetModel();
    m_clients.clear();

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
        {
            continue;
        }

        QStringList fields = line.split(";");
        if (fields.size() < 5)
        {
            fields = line.split(",");
        }

        if (fields.size() >= 5)
        {
            Client c;
            c.cardNumber = fields[0].toInt();
            c.fullName = fields[1];
            c.subscription = fields[2];
            c.price = fields[3].toDouble();
            c.visitsLeft = fields[4].toInt();
            m_clients.append(c);
        }
    }

    file.close();
    endResetModel();
    return true;
}

bool ClientModel::saveToJson(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    QJsonArray array;
    for (const Client &c : m_clients)
    {
        QJsonObject obj;
        obj["cardNumber"] = c.cardNumber;
        obj["fullName"] = c.fullName;
        obj["subscription"] = c.subscription;
        obj["price"] = c.price;
        obj["visitsLeft"] = c.visitsLeft;
        array.append(obj);
    }

    QJsonDocument doc(array);
    file.write(doc.toJson());
    file.close();
    return true;
}

bool ClientModel::loadFromJson(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    beginResetModel();
    m_clients.clear();

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray array = doc.array();

    for (int i = 0; i < array.size(); ++i)
    {
        QJsonObject obj = array[i].toObject();
        Client c;
        c.cardNumber = obj["cardNumber"].toInt();
        c.fullName = obj["fullName"].toString();
        c.subscription = obj["subscription"].toString();
        c.price = obj["price"].toDouble();
        c.visitsLeft = obj["visitsLeft"].toInt();
        m_clients.append(c);
    }

    file.close();
    endResetModel();
    return true;
}