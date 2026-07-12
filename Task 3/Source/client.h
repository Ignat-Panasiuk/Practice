#ifndef CLIENT_H
#define CLIENT_H

#include <QString>
#include <QTextStream>
#include <QDataStream>

class Client
{
private:
    int m_cardNumber;
    QString m_fullName;
    QString m_subscription;
    double m_price;
    int m_visits;
    bool m_isActive;

public:
    Client();
    Client(int card, QString name, QString sub, double price, int visits, bool active);

    int getCardNumber() const { return m_cardNumber; }
    QString getFullName() const { return m_fullName; }
    QString getSubscription() const { return m_subscription; }
    double getPrice() const { return m_price; }
    int getVisits() const { return m_visits; }
    bool getIsActive() const { return m_isActive; }

    bool setCardNumber(int card);
    bool setFullName(const QString& name);
    bool setSubscription(const QString& sub);
    bool setPrice(double price);
    bool setVisits(int visits);
    void setIsActive(bool active);

    bool isValid() const;

    friend QTextStream& operator<<(QTextStream& out, const Client& c);
    friend QTextStream& operator>>(QTextStream& in, Client& c);
    friend QDataStream& operator<<(QDataStream& out, const Client& c);
    friend QDataStream& operator>>(QDataStream& in, Client& c);
};

#endif