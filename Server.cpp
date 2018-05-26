//
// Created by Artur Spek on 20/05/2018.
//

#include <QtWidgets>
#include <QtNetwork>
#include <QtCore>
#include <iostream>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include "Server.hpp"
#include <QSqlError>
#include "Request_Response.pb.h"

Server::Server() {

    initDB();
    fetchAllEventsFromDBToVector(events);

    std::cout << "events[0].text() returns" << events[0].text() << std::endl;


    //std::cout << "query.lastError() returns" << query.lastError() << std::endl;


    setUpNetConf();


    connect(tcpServer, &QTcpServer::newConnection, this, &Server::onNewConnection);
}


void Server::sessionOpened() {

    // Save the used configuration
    if (networkSession) {
        QNetworkConfiguration config = networkSession->configuration();
        QString id;
        if (config.type() == QNetworkConfiguration::UserChoice)
            id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
        else
            id = config.identifier();

        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
        settings.endGroup();
    }

    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(QHostAddress::LocalHost, 4888)) {
        std::cerr << "Unable to start the server" << std::endl;
        return;
    }

}


void Server::sendEvents(QTcpSocket* peer) {

    rrepro::Response response;
    rrepro::Event ev;

    for (auto&& ev : events) {
        auto event = response.add_events();
        *event = ev;
    }

    std::string string = response.SerializeAsString();


    peer->write(string.c_str());

}
void Server::sendEvent(QTcpSocket* peer, rrepro::Event event) {

    rrepro::Response response;
    auto ev = response.add_events();
    *ev = event;

    std::string string = response.SerializeAsString();

    peer->write(string.c_str());
}


void Server::handleIncomingData(QTcpSocket* peer) {

    QByteArray input;
    input = peer->readAll();
    rrepro::Request request;
    request.ParseFromArray(input, input.size());

    std::cout << std::boolalpha;

#ifdef debug_cerr
    std::cout << "request.has_event() = " << request.has_event() << std::endl;
    std::cout << "request.kind() = " << request.kind() << std::endl;
    if (request.has_event()) {
        std::cout << "request.event().has_text()" << request.event().has_text() << std::endl;
        std::cout << "request.event().text()" << request.event().text() << std::endl;
    }
#endif

    if(request.kind() == rrepro::Request::GET)
    {
        std::cout<< "Request kind is GET" <<std::endl;
        sendEvents(peer);
    }
    if(request.kind() == rrepro::Request::ADD)
    {

        std::cout<< "Request kind is ADD" <<std::endl;
        addEventToDB(request.event());
        //add new event and sent do others
        events.push_back(std::move(request.event()));
        for (auto&& other_peer : connections) {
            if (other_peer->socketDescriptor() != peer->socketDescriptor()) {
                sendEvent(peer, request.event());
            }
        }
    }


}

/*!
 *  This slot is called when &QTcpServer::newConnection signal is emitted.
 */
void Server::onNewConnection() {

    
    std::cout<< "new connection" <<std::endl;
    //get new connection
    QTcpSocket* newConnection = tcpServer->nextPendingConnection();

    //if new data will arrive from the new connection to the server(tcp socket) it's appropriate handled by Server::handleIncomingData function.
    connect(newConnection, &QIODevice::readyRead, [this, newConnection](){ Server::handleIncomingData(newConnection); });
    //delete this connection from list if it disconnects.
    connect(newConnection, &QAbstractSocket::disconnected, [this, newConnection](){ Server::onDeleteConnectionFromList(newConnection); });


#ifdef debug_cerr
    //if state of the socket changes, print it's current state with Server::displayState.
    connect(newConnection, &QAbstractSocket::stateChanged, [this, newConnection](){ Server::displayState(newConnection);  });
    //if an error with the socket occurred print it with Server::displayError.
    connect(newConnection , QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), [this, newConnection](){ Server::displayError(newConnection);  });
#endif



    //store new connection for future.
    connections.push_back(newConnection);


    handling_connections = true;

}

size_t Server::connections_count() noexcept {
    return connections.size();
}

/*!
 * This function will remove the given tcp socket from Server's list Connections.
 * If debug_cerr macro is defined it will print the name of the socket.
 * If it's the last socket from the list handling_connection flag will be set to false.
 * @param sck_to_delete QTcpSocket which will be removed from list.
 */
void Server::deleteConnectionFromList(QTcpSocket* sck_to_delete) {

    qInfo()<<"The following socket has just disconnected."<<sck_to_delete->peerName();
    connections.removeOne(sck_to_delete);
    if(connections.empty())
        handling_connections = false;
}
/*!
 * Auxiliary function to displays possible errors affecting given tcp socket.
 * This function uses QAbstractSocket::SocketError enum class to identify error which affected
 * the tcp socket. Displaying information about a problem is handled by qDebug() object.
 *
 */
void Server::displayError(QTcpSocket* socket) {

    auto socketError = socket->error();


    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
            break;
        case QAbstractSocket::HostNotFoundError:
            qDebug()<<"HostNotFoundError, check host name and port settings.";
            break;
        case QAbstractSocket::ConnectionRefusedError:
            qDebug()<<"The connection was refused by the peer. "
                      "Make sure the Events server is running, "
                      "and check that the host name and port "
                      "settings are correct.";
            break;
        default:
            qDebug() << "Error occurred! "<< socketError;

    }

}

/*!
 * Auxiliary function to displays all possible states in which a given tcp socket is.
 * This function uses QAbstractSocket::SocketState enum class to identify error which affected
 * the tcp socket. Displaying information about the current state is handled by qInfo() object.
 *
 */
void Server::displayState(QTcpSocket* socket) {

    auto socketState = socket->state();

    switch (socketState) {
        case QAbstractSocket::UnconnectedState    :
            qInfo() << "The socket is not connected.";
            break;
        case QAbstractSocket::HostLookupState    :
            qInfo() << "The socket is performing a host name lookup.";
            break;
        case QAbstractSocket::ConnectingState:
            qInfo() << "The socket has started establishing a connection.";
            break;
        case QAbstractSocket::ConnectedState:
            qInfo() << "A connection is established.";
            break;
        case QAbstractSocket::BoundState    :
            qInfo() << "The socket is bound to an address and port.";
            break;
        case QAbstractSocket::ClosingState        :
            qInfo() << "The socket is about to close (data may still be waiting to be written).";
            break;
        case QAbstractSocket::ListeningState    :
            qInfo() << "For internal use only.";
            break;
        default:;
    }



}
Server::~Server() {

}
void Server::setUpNetConf() {

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) != QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, &QNetworkSession::opened, this, &Server::sessionOpened);

        networkSession->open();
    } else {
        sessionOpened();
    }

}

bool Server::is_handling_connections() {

    return handling_connections;
}
bool Server::connected_to_database() noexcept {
    return db_connection.isOpen();
}
void Server::initDB() {

    db_connection = QSqlDatabase::addDatabase("QSQLITE");
    db_connection.setHostName("host_name");
    db_connection.setDatabaseName("events_db");
    db_connection.setUserName("user");
    db_connection.setPassword("password");
    bool ok = db_connection.open();



    if (!ok) {
        std::cerr<< "Could not open a connection to Database";
    } else {
        std::cout<< "Connection to server was established" << std::endl;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE Events(id UNIQUE, event TEXT, timestamp TEXT, priority INT)");

}
void Server::addEventToDB(rrepro::Event event) {

    QSqlQuery query;

    std::string name = event.text();
    std::string timestamp = std::to_string(event.timestamp());
    std::string q = "INSERT INTO Events (event, timestamp, priority) "
                    "VALUES ("+name+", "+timestamp+", 1)";

    query.exec(QString::fromStdString(q));

    qInfo() << "query.lastError().databaseText() returns" << query.lastError().databaseText();

}
void Server::fetchAllEventsFromDBToVector(std::vector<rrepro::Event>& vec) {

    QSqlQuery query;
    query.exec("SELECT * FROM Events");
    qInfo() << "query.lastError().databaseText() returns" << query.lastError().databaseText();

    while (query.next()) {
        QString name = query.value(0).toString();
        QString timestamp = query.value(1).toString();
        int priority = query.value(2).toInt();
        qDebug() << name << timestamp << priority;
        rrepro::Event event;
        event.set_text(name.toStdString());
        event.set_timestamp(timestamp.toInt());
        vec.push_back(event);
    }




}
int Server::event_number() {

    return events.size();
}

void Server::onDeleteConnectionFromList(QTcpSocket* socket) {

    deleteConnectionFromList(socket);
}


