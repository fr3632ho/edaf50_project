/* myserver.cc: sample server program */
#include "connection.h"
#include "connectionclosedexception.h"
#include "server.h"
#include "volatileDatabase.h"
#include "database.h"
#include "diskDatabase.h"
#include "messageProtocol.h"
#include "protocol.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include <map>

using std::cerr;
using std::cout;
using std::endl;
using std::map;
using std::string;
using std::vector;

/*
 * Read an integer from a client.
 */
int readNumber(const std::shared_ptr<Connection> &conn)
{
        unsigned char byte1 = conn->read();
        unsigned char byte2 = conn->read();
        unsigned char byte3 = conn->read();
        unsigned char byte4 = conn->read();
        return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}

/*
 * Send a string to a client.
 */
void writeString(const std::shared_ptr<Connection> &conn, const string &s)
{
        for (char c : s)
        {
                conn->write(c);
        }
        conn->write('$');
}

Server init(int argc, char *argv[])
{
        if (argc != 2)
        {
                cerr << "Usage: myserver port-number" << endl;
                exit(1);
        }

        int port = -1;
        try
        {
                port = std::stoi(argv[1]);
        }
        catch (std::exception &e)
        {
                cerr << "Wrong format for port number. " << e.what() << endl;
                exit(2);
        }

        Server server(port);
        if (!server.isReady())
        {
                cerr << "Server initialization error." << endl;
                exit(3);
        }
        return server;
}

void serve_one(Server &server, Database *db)
{
        auto conn = server.waitForActivity();
        MessageProtocol protocol = MessageProtocol(conn, db);

        if (conn != nullptr)
        {
                try
                {
                        protocol.process_request();
                }
                catch (ConnectionClosedException &)
                {
                        server.deregisterConnection(conn);
                        cout << "Client closed connection" << endl;
                }
        }
        else
        {
                conn = std::make_shared<Connection>();
                server.registerConnection(conn);
                cout << "New client connects" << endl;
        }
}

int main(int argc, char *argv[])
{
        auto server = init(argc, argv);
        Database *db = new DiskDatabase{};

        while (true)
        {
                serve_one(server, db);
        }
        return 0;
}
