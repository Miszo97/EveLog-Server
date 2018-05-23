#include <QApplication>
#include <iostream>
#include "Server.hpp"

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
    Server server;
    return app.exec();

}