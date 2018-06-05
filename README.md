# Server

Server is a program intended to be run along with Client.
If you are intersted how client is impelemted please visit
[Client repository](https://github.com/Miszo97/Client/tree/Development).

### Roadmap

1. Add capability for running Server as a service on platform supporting this feature.
2. SSL implementation.
3. Add logging system.
3. Add capability to remove particular entry from database.

#### Deatils

Currently server supports connections with many clients at the same time. 
The server internally starts listen on port **4888** and ipv4 127.0.0.1 which is basically _localhost_'.
For more information visit [documentation section](documentation).

#### Documentation
The documentation is available in docs/html folder. Download this folder and open [index.html](docs/html/index.html) file with your browser. 
