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
    /// \brief Construct a Server instance.
    Server();
    /// \brief Destroy the Server instance.
    virtual ~Server();
    int connections_count() noexcept ;
    bool handling_connections;

private slots:

    /// \brief Handle incoming data arrived to a given socket. It's called when &QIODevice::readyRead signal is emitted.
    void handleIncomingData(QTcpSocket*);
    void sessionOpened();

    /// \brief Send events from connections list to a given socket.
    void sendEvents(QTcpSocket*);

    /// \brief Send particular event to a given socket.
    void sendEvent(QTcpSocket*, rrepro::Event event);

    /// \brief This slot is called when &QTcpServer::newConnection signal is emitted.
    /// It prepares new socket and store in connections list.
    void onNewConnection();

    /// /brief This function will remove the given tcp socket from Server's list Connections.
    void deleteConnectionFromList(QTcpSocket*);

    /*!
     * /brief Auxilary function to display possible errors affecting given tcp socket.
     */
    void displayError(QTcpSocket* );
    /*!
     * /brief Auxilary function to display states in which tcpSocket can be.
     */
    void displayState(QTcpSocket* );

private:

    QList<QTcpSocket*> connections; /*!< Each new client connection is referred as socket and stored in this list.*/
    std::vector<rrepro::Event> events; /*!< This container contains all events fetched from DB and is updated whenever new event arrives.*/
    QTcpServer *tcpServer = nullptr;
    QNetworkSession *networkSession = nullptr;
    QDataStream input;

};


#endif //SERVER_SERVER_HPP
