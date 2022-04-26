/* myserver.cc: sample server program */
#include "connection.h"
#include "connectionclosedexception.h"
#include "server.h"
#include "volatileDatabase.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>

using std::cerr;
using std::cout;
using std::endl;
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

void process_request(std::shared_ptr<Connection> &conn)
{

        VolatileDatabase db;
        db.createNewsgroup("Tech news");
        db.createNewsgroup("Finance");
        db.createNewsgroup("Crypto");
        vector<string> ng = db.getNewsgroups();

        int nbr = readNumber(conn);
        string result;
        std::stringstream buffer;
        if (nbr == 1)
        {
                buffer << "Newsgroups: ";
                for (string s : ng)
                {
                        buffer << "\n"
                               << s;
                }
                buffer << endl;
                result = buffer.str();
        }
        else if (nbr == 2)
        {
                result = "Create newsgroup: ";
        }
        else if (nbr == 3)
        {
                result = "Articles: ";
        }
        else if (nbr == 4)
        {
                result = "Read: ";
        }

        writeString(conn, result);
}

void serve_one(Server &server)
{
        auto conn = server.waitForActivity();
        if (conn != nullptr)
        {
                try
                {
                        process_request(conn);
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

        while (true)
        {
                serve_one(server);
        }
        return 0;
}
