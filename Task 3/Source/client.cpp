#include "client.h"
#include <QStringList>

Client::Client() : m_cardNumber(0), m_fullName(""), m_subscription(""), m_price(0.0), m_visits(0), m_isActive(true) {}

Client::Client(int card, QString name, QString sub, double price, int visits, bool active)
    : m_cardNumber(card), m_fullName(name), m_subscription(sub), m_price(price), m_visits(visits), m_isActive(active) {}

bool Client::setCardNumber(int card) { if(card < 0) return false; m_cardNumber = card; return true; }
bool Client::setFullName(const QString& name) { if(name.isEmpty()) return false; m_fullName = name; return true; }
bool Client::setSubscription(const QString& sub) { if(sub.isEmpty()) return false; m_subscription = sub; return true; }
bool Client::setPrice(double price) { if(price < 0) return false; m_price = price; return true; }
bool Client::setVisits(int visits) { if(visits < 0) return false; m_visits = visits; return true; }
void Client::setIsActive(bool active) { m_isActive = active; }

bool Client::isValid() const { return !m_fullName.isEmpty() && m_cardNumber > 0 && m_price >= 0; }

QTextStream& operator<<(QTextStream& out, const Client& c)
{
    out << c.m_cardNumber << ";" << c.m_fullName << ";" << c.m_subscription << ";"
        << c.m_price << ";" << c.m_visits << ";" << (c.m_isActive ? 1 : 0) << "\n";
    return out;
}

QTextStream& operator>>(QTextStream& in, Client& c)
{
    QString line = in.readLine();

    // ВАЖНОЕ ИСПРАВЛЕНИЕ: Защита от пустых строк и конца файла
    if (line.isNull() || line.isEmpty()) return in;

    QStringList p = line.split(";");
    if(p.size() == 6) {
        c.m_cardNumber = p[0].toInt();
        c.m_fullName = p[1];
        c.m_subscription = p[2];
        c.m_price = p[3].toDouble();
        c.m_visits = p[4].toInt();
        c.m_isActive = (p[5].toInt() == 1);
    }
    return in;
}

QDataStream& operator<<(QDataStream& out, const Client& c)
{
    out << c.m_cardNumber << c.m_fullName << c.m_subscription << c.m_price << c.m_visits << c.m_isActive;
    return out;
}

QDataStream& operator>>(QDataStream& in, Client& c)
{
    in >> c.m_cardNumber >> c.m_fullName >> c.m_subscription >> c.m_price >> c.m_visits >> c.m_isActive;
    return in;
}