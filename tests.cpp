#define BOOST_TEST_MODULE module_test
#include <boost/test/included/unit_test.hpp>
#include <QApplication>
#include <iostream>
#include "Server.hpp"


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

  int argc{};
  QCoreApplication app(argc, nullptr);
  Server server;
  app.exec();






}






