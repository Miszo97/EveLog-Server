//
// Created by Artur Spek on 20/05/2018.
//

#ifndef SERVER_SERVER_HPP
#define SERVER_SERVER_HPP

#include <QObject>
#include <QString>
#include <QDataStream>
#include <QVector>
#include <QTcpSocket>
#include <QTcpServer>
#include <QNetworkSession>
#include <QSignalMapper>
#include "Request_Response.pb.h"


class Server : public QObject
{
    Q_OBJECT

public:
     Server();
    int connections_count() noexcept ;
    bool handling_connections;

private slots:
    void handleIncomingData(QTcpSocket*);
    void sessionOpened();
    void sendEvents();
    void sendEvent();
    void onNewConnection();
    void deleteConnectionFromList(QTcpSocket*);

private:

    QList<QTcpSocket*> connections; /*!< Each new client connection is referred as socket and stored in this list.*/
    std::vector<rrepro::Event> events; /*!< This container contains all events fetched from DB and is updated whenever new event arrives.*/
    QTcpServer *tcpServer = nullptr;
    QNetworkSession *networkSession = nullptr;
    QDataStream input;
    QSignalMapper* signalMapper;

};


#endif //SERVER_SERVER_HPP
