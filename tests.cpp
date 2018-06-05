#define BOOST_TEST_MODULE module_test
#include <boost/test/included/unit_test.hpp>
#include <QApplication>
#include <iostream>
#include <chrono>
#include <thread>
#include "Server.hpp"
#include "Client.hpp"


namespace utf = boost::unit_test;


BOOST_AUTO_TEST_CASE( initial_state )
{
  Server server;
  BOOST_CHECK_EQUAL(server.connected_to_database(), true);
  BOOST_CHECK_EQUAL(server.is_handling_connections(), false);
  BOOST_CHECK_EQUAL(server.connections_count(), 0);

}

BOOST_AUTO_TEST_CASE( test )
{
    int c{};
    char** argv;
    QApplication app(c, argv);
    Client client;
    Server server;
    BOOST_CHECK_EQUAL(server.is_handling_connections(), false);
    client.connectToServer(QHostAddress("127.0.0.1"), 4888);
    std::this_thread::sleep_for(std::chrono::seconds(4));
    BOOST_CHECK_EQUAL(client.isConnectedToServer(), true);

    BOOST_CHECK_EQUAL(server.is_handling_connections(), true);
    BOOST_CHECK_EQUAL(server.connections_count(), 1);



}






