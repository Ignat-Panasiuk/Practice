#ifndef CLIENTMODEL_H
#define CLIENTMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QFile>
#include <QTextStream>
#include "client.h"

struct FitnessStats
{
    double totalRevenue;
    int totalVisits;
    int totalClients;
    double averagePrice;
    double maxPrice;
    int expiringCount;
};

class ClientModel : public QAbstractTableModel
{
    Q_OBJECT

private:
    QList<Client> m_clients;

public:
    explicit ClientModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void addClient(const Client& client);
    void removeClient(int row);

    bool saveToCsv(const QString& filePath);
    bool loadFromCsv(const QString& filePath);
    bool saveToJson(const QString& filePath);
    bool loadFromJson(const QString& filePath);

    FitnessStats getStatistics() const;

    const QList<Client>& getClients() const { return m_clients; }
    void clear();
};

#endif // CLIENTMODEL_H