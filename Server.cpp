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

    signalMapper = new QSignalMapper(this);

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

void Server::handleIncomingData(QTcpSocket* newSocket) {

    QByteArray input;
    input = newSocket->readAll();
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
        sendEvents();
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



    //store new connection for future.
    connections.push_back(newConnection);


    handling_connections = true;

}

int Server::connections_count() noexcept {
    return connections.size();
}

void Server::deleteConnectionFromList(QTcpSocket* sck_to_delete) {

    connections.removeOne(sck_to_delete);
    if(connections.empty())
        handling_connections = false;
}
