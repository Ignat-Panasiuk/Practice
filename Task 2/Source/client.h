#ifndef CLIENT_H
#define CLIENT_H

#include <QString>

struct Client
{
    int cardNumber;
    QString fullName;
    QString subscription;
    double price;
    int visitsLeft;

    Client() : cardNumber(0), price(0.0), visitsLeft(0) {}
    Client(int card, const QString& name, const QString& sub, double p, int visits)
        : cardNumber(card), fullName(name), subscription(sub), price(p), visitsLeft(visits) {}
};

#endif // CLIENT_H