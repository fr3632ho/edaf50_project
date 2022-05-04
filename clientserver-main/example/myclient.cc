/* myclient.cc: sample client program */
#include "connection.h"
#include "connectionclosedexception.h"
#include "messageProtocol.h"
#include "protocol.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

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

void listNewsgroupsResponse(const Connection &conn)
{
        // Handle if wrong param
        Protocol par_num = static_cast<Protocol>(readChar(conn));
        int nbr;
        string reply;
        nbr = stoi(readString(conn));
        Protocol par_string = static_cast<Protocol>(readChar(conn));
        reply = readString(conn);
        cout << nbr << " " << reply << endl;
        Protocol end_byte = static_cast<Protocol>(readChar(conn));
}

int app(const Connection &conn)
{
        cout << "Type a command: ";

        int input;
        while (cin >> input)
        // while (getline(cin, input))
        {
                Protocol command = static_cast<Protocol>(input);
                string parameter_string;
                int p_num = 0;
                try
                {
                        switch (command)
                        {
                        case Protocol::COM_LIST_NG:
                                writeProtocol(conn, Protocol::COM_LIST_NG);
                                writeProtocol(conn, Protocol::COM_END);
                                break;
                        case Protocol::COM_CREATE_NG:
                                writeProtocol(conn, Protocol::COM_CREATE_NG);
                                writeProtocol(conn, Protocol::PAR_STRING);
                                while (getline(cin, parameter_string))
                                {
                                        writeString(conn, parameter_string);
                                        break;
                                }
                                writeProtocol(conn, Protocol::COM_END);
                                break;
                        case Protocol::COM_DELETE_NG:
                                writeProtocol(conn, Protocol::COM_DELETE_NG);
                                writeProtocol(conn, Protocol::PAR_NUM);
                                while (cin >> p_num)
                                {
                                        writeNumber(conn, p_num);
                                        break;
                                }
                                writeProtocol(conn, Protocol::COM_END);
                                break;
                        default:
                                cout << "default fall through" << endl;
                                break;
                        }
                }
                catch (ConnectionClosedException &)
                {
                        cout << " no reply from server. Exiting." << endl;
                        return 1;
                }

                char reply = readChar(conn);
                switch (static_cast<Protocol>(reply))
                {
                case Protocol::ANS_LIST_NG:
                        listNewsgroupsResponse(conn);
                        cout << "COM_LIST_NG" << endl;
                        break;
                case Protocol::ANS_CREATE_NG:
                        // method to handle ack/nack
                        cout << "COM_CREATE_NG Success" << endl;
                        break;
                case Protocol::ANS_DELETE_NG:
                        // method to handle ack/nack
                        cout << "COM_DELETE_NG Success" << endl;
                        break;
                default:
                        cout << "default ANS FALLTHROUGH" << endl;
                        break;
                }
        }
        cout << "\nexiting.\n";
        return 0;
}

int main(int argc, char *argv[])
{
        Connection conn = init(argc, argv);
        return app(conn);
}
