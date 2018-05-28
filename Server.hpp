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
#include <QSqlDatabase>
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

    /// \brief Return the current amount of connected hosts.
    size_t connections_count() noexcept ;

    /// \brief Return the true if Server is connected to Database otherwise return false.
    bool connected_to_database() noexcept;

    /// \brief Return true if Sever is handling any connections(sessions) otherwise return false.
    bool is_handling_connections();

    /// \brief Return the current amount of residing events fetched from DB and updated with each ADD request.
    int event_number();

    /// \brief Send particular event to a given socket.
    void sendEvent(QTcpSocket*, rrepro::Event event);

    /// \brief Add given event to Database that Sever is connected with.
    /// \param event Event to be added.
    void addEventToDB(rrepro::Event event);

    /// \brief Fetch all events from database and put them to passed std::vector<rrepro::Event>.
    /// \param vec Vector to which all events from database should be put.
    void fetchAllEventsFromDBToVector(std::vector<rrepro::Event>& vec);

    /// \brief This function will remove the given tcp socket from Server's list Connections.
    void deleteConnectionFromList(QTcpSocket*);

    /// \brief This function tries to make a connection with Database and create a table.
    void initDB();


private slots:

    /// \brief Handle incoming data arrived to a given socket. It's called when &QIODevice::readyRead signal is emitted.
    void handleIncomingData(QTcpSocket*);
    void sessionOpened();

    /// \brief Send events from connections list to a given socket.
    void sendEvents(QTcpSocket*);


    /// \brief This slot is called when &QTcpServer::newConnection signal is emitted.
    /// It prepares new socket and store in connections list.
    void onNewConnection();


    /// \brief This function slot will invoke Server::deleteConnectionFromList function which
    /// remove the passed tcp socket from Server's list Connections.
    void onDeleteConnectionFromList(QTcpSocket*);



    /*!
     * /brief Auxiliary function to display possible errors affecting given tcp socket.
     */
    void displayError(QTcpSocket* );
    /*!
     * /brief Auxiliary function to display states in which tcpSocket can be.
     */
    void displayState(QTcpSocket* );


private:

    QList<QTcpSocket*> connections; /*!< Each new client connection is referred as socket and stored in this list.*/
    std::vector<rrepro::Event> events; /*!< This container contains all events fetched from DB and is updated whenever new event arrives.*/
    QTcpServer *tcpServer = nullptr;
    QNetworkSession *networkSession = nullptr;
    QSqlDatabase db_connection;
    bool handling_connections;


    void setUpNetConf();

};


#endif //SERVER_SERVER_HPP
