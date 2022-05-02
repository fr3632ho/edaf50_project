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

void process_request(std::shared_ptr<Connection> &conn, VolatileDatabase *db)
{

        int nbr = readNumber(conn);
        string result;

        if (nbr == 1)
        {
                db->createNewsgroup("Tech news");
                db->createNewsgroup("Finance");
                db->createNewsgroup("Crypto");
        }
        else if (nbr == 2)
        {
                result = "Delete newsgroup: ";
                db->deleteNewsgroup(2);
                map<int, Newsgroup> ng = db->getNewsgroups();
        }
        else if (nbr == 3)
        {
                result = "Create articles: ";
                db->writeArticle(1, "Hello, World!", "This is my first article", "Alexander");
                db->writeArticle(1, "Hello, again World!", "This is my second article", "Alexander");
                db->writeArticle(1, "Hello, 3x World!", "This is my 3rd article", "Alexander");
        }

        else if (nbr == 4)
        {
                map<int, Article> art = db->getNewsgroupArticles(1);

                std::stringstream buffer;
                buffer << "Articles in newsgroup 1: ";
                for (std::pair<int, Article> a : art)
                {
                        buffer << "\n"
                               << "a_id " << a.second.getId() << "\n"
                               << a.second.getTitle() << "\nAuthor: " << a.second.getAuthor()
                               << "\n"
                               << a.second.getText() << "\n";
                }
                buffer << endl;
                result = buffer.str();
        }

        else if (nbr == 5)
        {
                map<int, Newsgroup> ng = db->getNewsgroups();

                std::stringstream buffer;
                buffer << "Newsgroups: ";
                for (std::pair<int, Newsgroup> p : ng)
                {
                        buffer << "\n"
                               << p.first << " " << p.second.getTitle();
                }
                buffer << endl;
                result = buffer.str();
        }

        else if (nbr == 6)
        {
                std::stringstream buffer;
                db->writeArticle(2, "Article 1, Newsgroup 2", "This is getting rather boring", "Alexander");
                Article a = db->getArticle(2, 1);

                buffer << "Article 1 in newsgroup 2:\n"
                       << a.getId() << "\n"
                       << a.getTitle() << "\n"
                       << a.getAuthor() << "\n"
                       << a.getText() << endl;
                result = buffer.str();
        }

        else if (nbr == 7)
        {
                db->deleteArticle(2, 1);
                std::stringstream buffer2;
                map<int, Article> art = db->getNewsgroupArticles(2);
                buffer2 << "Articles in newsgroup 2:\n";
                for (std::pair<int, Article> a : art)
                {
                        buffer2 << "\n"
                                << "a_id " << a.second.getId() << "\n"
                                << a.second.getTitle() << "\nAuthor: " << a.second.getAuthor()
                                << "\n"
                                << a.second.getText() << "\n";
                }
                buffer2 << endl;
                result = buffer2.str();
        }

        writeString(conn, result);
}

void serve_one(Server &server, VolatileDatabase *db)
{
        auto conn = server.waitForActivity();
        if (conn != nullptr)
        {
                try
                {
                        process_request(conn, db);
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
        VolatileDatabase *db = new VolatileDatabase{};

        while (true)
        {
                serve_one(server, db);
        }
        return 0;
}
