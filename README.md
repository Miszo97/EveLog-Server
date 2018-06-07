# Server

Server is a program intended to be run along with Client.
If you are intersted how client is impelemted please visit
[Client repository](https://github.com/Miszo97/Client/tree/Development).

### Roadmap

1. Add capability for running Server as a service on platform supporting this feature.
2. SSL implementation.
3. Add logging system.
3. Add capability to remove particular entry from database.

#### Dependencies
Server isn't a standalone application (i.e is not linked statically) and as a result it utilizes external dynamic libraries.
Please make sure you have installed all dependent libraries listed below:

| Library          | Purpose                      | Source                                                             |
|------------------|------------------------------|--------------------------------------------------------------------|
| Qt libraries     | Network level implementation | http://doc.qt.io/qt-5/gettingstarted.html                          |
| Protocol Buffers | Protocol implementation      | https://developers.google.com/protocol-buffers/                    |
| Boost log        | Logging system               | https://www.boost.org/doc/libs/1_67_0/libs/log/doc/html/index.html |

If you are missing any of above libraries install them with your packet manger (recommended way)
or build manually following stpes from authors sites.

 
#### Deatils

Currently server supports connections with many clients at the same time. 
The server internally starts listen on port **4888** and ipv4 127.0.0.1 which is basically _localhost_'.
For more information visit [documentation section](documentation).

#### Documentation
The documentation is available in docs/html folder. Download this folder and open [index.html](docs/html/index.html) file with your browser. 
