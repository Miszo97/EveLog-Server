//
// Created by Artur Spek on 20/05/2018.
//

#ifndef SERVER_SERVER_HPP
#define SERVER_SERVER_HPP

#include <QObject>
#include <QString>
#include <QVector>

class QTcpServer;
class QNetworkSession;

class Server : public QObject
{
    Q_OBJECT

public:
     Server();

private slots:
    void sessionOpened();
    void sendEvents();
    void sendEvent();

private:
    QTcpServer *tcpServer = nullptr;
    QVector<QString> fortunes; //TODO fetched events from DB?
    QNetworkSession *networkSession = nullptr;
};


#endif //SERVER_SERVER_HPP
