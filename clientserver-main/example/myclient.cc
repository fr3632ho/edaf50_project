/* myclient.cc: sample client program */
#include "connection.h"
#include "connectionclosedexception.h"
#include "messageProtocol.h"
#include "protocol.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::string;

void writeString(const Connection &conn, const string &s)
{
        for (char c : s)
        {
                conn.write(c);
        }
        conn.write('$');
}

/*
 * Send an integer to the server as four bytes.
 */
void writeNumber(const Connection &conn, int value)
{
        conn.write((value >> 24) & 0xFF);
        conn.write((value >> 16) & 0xFF);
        conn.write((value >> 8) & 0xFF);
        conn.write(value & 0xFF);
}

void writeProtocol(const Connection &conn, Protocol value)
{
        conn.write(static_cast<char>(value));
}

/*
 * Read a string from the server.
 */
string readString(const Connection &conn)
{
        string s;
        char ch;
        while ((ch = conn.read()) != '$')
        {
                s += ch;
        }
        return s;
}

char readChar(const Connection &conn)
{
        return conn.read();
}

/* Creates a client for the given args, if possible.
 * Otherwise exits with error code.
 */
Connection init(int argc, char *argv[])
{
        if (argc != 3)
        {
                cerr << "Usage: myclient host-name port-number" << endl;
                exit(1);
        }

        int port = -1;
        try
        {
                port = std::stoi(argv[2]);
        }
        catch (std::exception &e)
        {
                cerr << "Wrong port number. " << e.what() << endl;
                exit(2);
        }

        Connection conn(argv[1], port);
        if (!conn.isConnected())
        {
                cerr << "Connection attempt failed" << endl;
                exit(3);
        }

        return conn;
}

void listNewsgroupsResponse(MessageProtocol protocol, shared_ptr<Connection> conn)
{
        // Handle if wrong param
        Protocol par_num = protocol.readProtocol(conn);
        int nbrNewsgroups = protocol.readNumber(conn);
        cout << nbrNewsgroups << endl;
        for (int i = 0; i < nbrNewsgroups; i++)
        {
                Protocol par_num = protocol.readProtocol(conn);
                int nbr = protocol.readNumber(conn);
                Protocol par_string = protocol.readProtocol(conn);
                string reply = protocol.readStringP(conn);
                cout << nbr << " " << reply << endl;
        }
        Protocol end_byte = protocol.readProtocol(conn);
}

void createNewsgroupResponse(MessageProtocol protocol, shared_ptr<Connection> conn)
{
        Protocol ack = protocol.readProtocol(conn);
        if (ack == Protocol::ANS_ACK)
        {
                cout << "Newsgroup created" << endl;
        }

        if (ack == Protocol::ANS_NAK)
        {
                Protocol err = protocol.readProtocol(conn);
                if (err == Protocol::ERR_NG_ALREADY_EXISTS)
                {
                        cout << "Newsgroup already exists" << endl;
                }
        }
        Protocol end = protocol.readProtocol(conn);
}

void deleteNewsgroupResponse(MessageProtocol protocol, shared_ptr<Connection> conn)
{
        Protocol ack = protocol.readProtocol(conn);
        if (ack == Protocol::ANS_ACK)
        {
                cout << "Newsgroup deleted" << endl;
        }

        if (ack == Protocol::ANS_NAK)
        {
                Protocol err = protocol.readProtocol(conn);
                if (err == Protocol::ERR_NG_DOES_NOT_EXIST)
                {
                        cout << "Newsgroup does not exist" << endl;
                }
        }
        Protocol end = protocol.readProtocol(conn);
}

void listArticlesResponse(MessageProtocol protocol, shared_ptr<Connection> conn)
{
        Protocol ack = protocol.readProtocol(conn);
        if (ack == Protocol::ANS_ACK)
        {
                Protocol p_num = protocol.readProtocol(conn);
                int numArt = protocol.readNumber(conn);
                for (int i = 0; i < numArt; i++)
                {
                        Protocol p_num = protocol.readProtocol(conn);
                        int id = protocol.readNumber(conn);
                        Protocol p_string = protocol.readProtocol(conn);
                        string title = protocol.readStringP(conn);
                        cout << id << " " << title << endl;
                }
        }

        if (ack == Protocol::ANS_NAK)
        {
                Protocol err = protocol.readProtocol(conn);
                if (err == Protocol::ERR_NG_DOES_NOT_EXIST)
                {
                        cout << "Newsgroup does not exist" << endl;
                }
        }
}

int app(shared_ptr<Connection> conn)
{
        cout << "Type a command: ";
        MessageProtocol protocol = MessageProtocol(conn);
        int input;
        while (cin >> input)
        // while (getline(cin, input))
        {
                Protocol command = static_cast<Protocol>(input);
                string parameter_string = "";
                int p_num = 0;
                try
                {
                        switch (command)
                        {
                        case Protocol::COM_LIST_NG:
                                protocol.writeProtocol(conn, Protocol::COM_LIST_NG);
                                protocol.writeProtocol(conn, Protocol::COM_END);
                                break;
                        case Protocol::COM_CREATE_NG:
                                protocol.writeProtocol(conn, Protocol::COM_CREATE_NG);
                                protocol.writeProtocol(conn, Protocol::PAR_STRING);
                                while (getline(cin, parameter_string))
                                {
                                        protocol.writeNumber(conn, parameter_string.size());
                                        protocol.writeString(conn, parameter_string);
                                        break;
                                }
                                protocol.writeProtocol(conn, Protocol::COM_END);
                                break;

                        case Protocol::COM_DELETE_NG:
                                protocol.writeProtocol(conn, Protocol::COM_DELETE_NG);
                                protocol.writeProtocol(conn, Protocol::PAR_NUM);
                                while (cin >> p_num)
                                {
                                        protocol.writeNumber(conn, p_num);
                                        break;
                                }
                                protocol.writeProtocol(conn, Protocol::COM_END);
                                break;

                        case Protocol::COM_LIST_ART:
                                protocol.writeProtocol(conn, Protocol::COM_LIST_NG);
                                protocol.writeProtocol(conn, Protocol::PAR_NUM);
                                while (cin >> p_num)
                                {
                                        protocol.writeNumber(conn, p_num);
                                        break;
                                }
                                protocol.writeProtocol(conn, Protocol::COM_END);
                                break;
                        default:
                                cout << "default fall through" << endl;
                                break;
                        }

                        Protocol reply = protocol.readProtocol(conn);
                        switch (reply)
                        {
                        case Protocol::ANS_LIST_NG:
                                listNewsgroupsResponse(protocol, conn);
                                break;
                        case Protocol::ANS_CREATE_NG:
                                // method to handle ack/nack
                                createNewsgroupResponse(protocol, conn);
                                break;
                        case Protocol::ANS_DELETE_NG:
                                // method to handle ack/nack
                                deleteNewsgroupResponse(protocol, conn);
                                break;
                        case Protocol::ANS_LIST_ART:
                                listArticlesResponse(protocol, conn);
                                // method to handle ack/nack
                                cout << "COM_LIST_ART Success" << endl;
                                break;
                        default:
                                cout << "default ANS FALLTHROUGH" << endl;
                                break;
                        }
                }
                catch (ConnectionClosedException &)
                {
                        cout << " no reply from server. Exiting." << endl;
                        return 1;
                }
        }
        cout << "\nexiting.\n";
        return 0;
}

int main(int argc, char *argv[])
{
        auto conn = std::make_shared<Connection>(init(argc, argv));
        // Connection conn = ;
        return app(conn);
}
