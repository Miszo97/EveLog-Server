//
// Created by Artur Spek on 20/05/2018.
//

#include <QtWidgets>
#include <QtNetwork>
#include <QtCore>
#include <iostream>

#include "Server.hpp"
#include "Request_Response.pb.h"

Server::Server() {

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


void Server::sendEvents() {

}

void Server::sendEvent() {

}

void Server::handleIncomingData(QTcpSocket* peer) {

    QByteArray input;
    input = peer->readAll();
    rrepro::Request request;
    request.ParseFromArray(input, input.size());

    std::cout << std::boolalpha;

    std::cout << "request.has_event() = " << request.has_event() << std::endl;
    std::cout << "request.kind() = " << request.kind() << std::endl;
    if (request.has_event()) {
        std::cout << "request.event().has_text()" << request.event().has_text() << std::endl;
        std::cout << "request.event().text()" << request.event().text() << std::endl;
    }

    if(request.kind() == rrepro::Request::GET)
    {
        std::cout<< "Request kind is GET" <<std::endl;
        sendEvents(peer);
    }
    if(request.kind() == rrepro::Request::ADD)
    {
        std::cout<< "Request kind is ADD" <<std::endl;
        //add new event and sent do others
        events.push_back(std::move(request.event()));
        for (auto&& peer : connections) {
            peer->write(input);
        }
    }


}

void Server::onNewConnection() {

    //get new connection
    QTcpSocket* newConnection = tcpServer->nextPendingConnection();

    //if new data will arrive from the new connection to the server(tcp socket) it's appropriate handled by Server::handleIncomingData function.
    connect(newConnection, &QIODevice::readyRead, [this, newConnection](){ Server::handleIncomingData(newConnection); });
    //delete this connection from list if it disconnects.
    connect(newConnection, &QAbstractSocket::disconnected, [this, newConnection](){ Server::deleteConnectionFromList(newConnection); });


#ifdef debug_cerr
    //if state of the socket changes, print it's current state with Server::displayState.
    connect(newConnection, &QIODevice::readyRead, [this, newConnection](){ Server::displayState(newConnection);  });
    //if an error with the socket occurred print it with Server::displayError.
    connect(newConnection , &QIODevice::readyRead, [this, newConnection](){ Server::displayError(newConnection);  });
#endif



    //store new connection for future.
    connections.push_back(newConnection);


    handling_connections = true;

}

int Server::connections_count() noexcept {
    return connections.size();
}

void Server::deleteConnectionFromList(QTcpSocket* sck_to_delete) {

    qInfo()<<"The following socket has just disconnected."<<sck_to_delete->peerName();
    connections.removeOne(sck_to_delete);
    if(connections.empty())
        handling_connections = false;
}

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
            qDebug() << "Error occurred!" << std::endl;

    }

}
void Server::displayState(QTcpSocket* socket) {

    auto socketState = socket->state();

    switch (socketState) {
        case QAbstractSocket::UnconnectedState    :
            std::cout << "The socket is not connected." << std::endl;
            break;
        case QAbstractSocket::HostLookupState    :
            std::cout << "The socket is performing a host name lookup." << std::endl;
            break;
        case QAbstractSocket::ConnectingState:
            std::cout << "The socket has started establishing a connection." << std::endl;
            break;
        case QAbstractSocket::ConnectedState:
            std::cout << "A connection is established." << std::endl;
            break;
        case QAbstractSocket::BoundState    :
            std::cout << "The socket is bound to an address and port." << std::endl;
            break;
        case QAbstractSocket::ClosingState        :
            std::cout << "The socket is about to close (data may still be waiting to be written)." << std::endl;
            break;
        case QAbstractSocket::ListeningState    :
            std::cout << "For internal use only." << std::endl;
            break;
        default:;
    }



}

