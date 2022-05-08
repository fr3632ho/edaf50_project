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

void printCommands()
{
        cout << "------------------------" << endl;
        cout << "1: List newsgroups" << endl;
        cout << "2: Create newsgroup" << endl;
        cout << "3: Delete newsgroup" << endl;
        cout << "4: List articles in newsgroup" << endl;
        cout << "5: Create article" << endl;
        cout << "6: Delete article" << endl;
        cout << "7: Get article" << endl;
        cout << "------------------------" << endl;
}

void listNewsgroupsResponse(MessageProtocol protocol, shared_ptr<Connection> conn)
{
        // Handle if wrong param
        Protocol par_num = protocol.readProtocol(conn);
        int nbrNewsgroups = protocol.readNumber(conn);
        cout << "Number of newsgroups: " << nbrNewsgroups << endl;
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
                        cout << "Error: Newsgroup already exists" << endl;
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
                        cout << "Error: Newsgroup does not exist" << endl;
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
                cout << "Number of articles: " << numArt << endl;
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
                        cout << "Error: Newsgroup does not exist" << endl;
                }
        }
        Protocol end = protocol.readProtocol(conn);
}

void createArticleResponse(MessageProtocol protocol, shared_ptr<Connection> conn)
{
        Protocol ack = protocol.readProtocol(conn);
        if (ack == Protocol::ANS_ACK)
        {
                cout << "Article created" << endl;
        }

        if (ack == Protocol::ANS_NAK)
        {
                Protocol err = protocol.readProtocol(conn);
                if (err == Protocol::ERR_NG_DOES_NOT_EXIST)
                {
                        cout << "Error: Newsgroup does not exist" << endl;
                }
        }

        Protocol end = protocol.readProtocol(conn);
}

void deleteArticleResponse(MessageProtocol protocol, shared_ptr<Connection> conn)
{
        Protocol ack = protocol.readProtocol(conn);
        if (ack == Protocol::ANS_ACK)
        {
                cout << "Article deleted" << endl;
        }

        if (ack == Protocol::ANS_NAK)
        {
                Protocol err = protocol.readProtocol(conn);
                if (err == Protocol::ERR_NG_DOES_NOT_EXIST)
                {
                        cout << "Error: Newsgroup does not exist" << endl;
                }

                if (err == Protocol::ERR_ART_DOES_NOT_EXIST)
                {
                        cout << "Error: Article does not exist" << endl;
                }
        }

        Protocol end = protocol.readProtocol(conn);
}

void getArticleResponse(MessageProtocol protocol, shared_ptr<Connection> conn)
{
        Protocol ack = protocol.readProtocol(conn);
        if (ack == Protocol::ANS_ACK)
        {
                Protocol par_string = protocol.readProtocol(conn);
                string title = protocol.readStringP(conn);
                cout << "************************" << endl;
                cout << title << "\n"
                     << endl;

                par_string = protocol.readProtocol(conn);
                string author = protocol.readStringP(conn);
                cout << "By: " << author << endl;
                cout << "************************" << endl;
                par_string = protocol.readProtocol(conn);
                string text = protocol.readStringP(conn);
                cout << text << endl;
                cout << "************************" << endl;
        }

        if (ack == Protocol::ANS_NAK)
        {
                Protocol err = protocol.readProtocol(conn);
                if (err == Protocol::ERR_NG_DOES_NOT_EXIST)
                {
                        cout << "Error: Newsgroup does not exist" << endl;
                }

                if (err == Protocol::ERR_ART_DOES_NOT_EXIST)
                {
                        cout << "Error: Article does not exist" << endl;
                }
        }

        Protocol end = protocol.readProtocol(conn);
}

int app(shared_ptr<Connection> conn)
{
        printCommands();
        cout << "Type a command: " << endl;
        MessageProtocol protocol = MessageProtocol(conn);
        string input = "0";
        while (getline(cin, input))
        // while (getline(cin, input))
        {
                Protocol command;
                try
                {
                        command = static_cast<Protocol>(stoi(input));
                }
                catch (std::invalid_argument e)
                {
                        command = Protocol::UNDEFINED;
                        cout << "Error: Enter a valid command" << endl;
                        printCommands();
                        continue;
                }

                string parameter_string = "";
                int p_num = 0;
                try
                {
                        switch (command)
                        {
                        case Protocol::COM_LIST_NG:
                                cout << "List newsgroups " << endl;
                                protocol.writeProtocol(conn, Protocol::COM_LIST_NG);
                                protocol.writeProtocol(conn, Protocol::COM_END);
                                break;
                        case Protocol::COM_CREATE_NG:
                                cout << "Create newsgroup " << endl;
                                protocol.writeProtocol(conn, Protocol::COM_CREATE_NG);
                                protocol.writeProtocol(conn, Protocol::PAR_STRING);
                                cout << "Enter newsgroup name: " << endl;
                                while (getline(cin, parameter_string))
                                {
                                        protocol.writeNumber(conn, parameter_string.size());
                                        protocol.writeString(conn, parameter_string);
                                        break;
                                }
                                protocol.writeProtocol(conn, Protocol::COM_END);
                                break;

                        case Protocol::COM_DELETE_NG:
                                cout << "Delete newsgroup " << endl;
                                protocol.writeProtocol(conn, Protocol::COM_DELETE_NG);
                                protocol.writeProtocol(conn, Protocol::PAR_NUM);
                                cout << "Enter newsgroup id: " << endl;
                                while (getline(cin, parameter_string))
                                {
                                        try
                                        {
                                                protocol.writeNumber(conn, stoi(parameter_string));
                                        }
                                        catch (std::invalid_argument e)
                                        {
                                                cout << "Enter valid newsgroup id:" << endl;
                                                continue;
                                        }

                                        break;
                                }
                                protocol.writeProtocol(conn, Protocol::COM_END);
                                break;

                        case Protocol::COM_LIST_ART:
                                cout << "List articles " << endl;
                                protocol.writeProtocol(conn, Protocol::COM_LIST_ART);
                                protocol.writeProtocol(conn, Protocol::PAR_NUM);
                                cout << "Enter newsgroup id: " << endl;
                                while (getline(cin, parameter_string))
                                {
                                        try
                                        {
                                                protocol.writeNumber(conn, stoi(parameter_string));
                                        }
                                        catch (std::invalid_argument e)
                                        {
                                                cout << "Enter valid newsgroup id:" << endl;
                                                continue;
                                        }

                                        break;
                                }
                                protocol.writeProtocol(conn, Protocol::COM_END);
                                break;

                        case Protocol::COM_CREATE_ART:
                                cout << "Create article " << endl;
                                protocol.writeProtocol(conn, Protocol::COM_CREATE_ART);
                                protocol.writeProtocol(conn, Protocol::PAR_NUM);
                                cout << "Enter newsgroup id: " << endl;
                                while (getline(cin, parameter_string))
                                {
                                        try
                                        {
                                                protocol.writeNumber(conn, stoi(parameter_string));
                                        }
                                        catch (std::invalid_argument e)
                                        {
                                                cout << "Enter valid newsgroup id:" << endl;
                                                continue;
                                        }

                                        break;
                                }
                                cout << "Enter article title: " << endl;
                                protocol.writeProtocol(conn, Protocol::PAR_STRING);
                                while (getline(cin, parameter_string))
                                {
                                        protocol.writeNumber(conn, parameter_string.size());
                                        protocol.writeString(conn, parameter_string);
                                        break;
                                }
                                cout << "Enter article author: " << endl;
                                protocol.writeProtocol(conn, Protocol::PAR_STRING);
                                while (getline(cin, parameter_string))
                                {
                                        protocol.writeNumber(conn, parameter_string.size());
                                        protocol.writeString(conn, parameter_string);
                                        break;
                                }
                                cout << "Enter article text: " << endl;
                                protocol.writeProtocol(conn, Protocol::PAR_STRING);
                                while (getline(cin, parameter_string))
                                {
                                        protocol.writeNumber(conn, parameter_string.size());
                                        protocol.writeString(conn, parameter_string);
                                        break;
                                }
                                protocol.writeProtocol(conn, Protocol::COM_END);
                                break;

                        case Protocol::COM_DELETE_ART:
                                cout << "Delete article " << endl;
                                protocol.writeProtocol(conn, Protocol::COM_DELETE_ART);
                                protocol.writeProtocol(conn, Protocol::PAR_NUM);
                                cout << "Enter newsgroup id: " << endl;
                                while (getline(cin, parameter_string))
                                {

                                        try
                                        {
                                                protocol.writeNumber(conn, stoi(parameter_string));
                                        }
                                        catch (std::invalid_argument e)
                                        {
                                                cout << "Enter valid newsgroup id:" << endl;
                                                continue;
                                        }

                                        break;
                                }
                                protocol.writeProtocol(conn, Protocol::PAR_NUM);
                                cout << "Enter article id: " << endl;
                                while (getline(cin, parameter_string))
                                {

                                        try
                                        {
                                                protocol.writeNumber(conn, stoi(parameter_string));
                                        }
                                        catch (std::invalid_argument e)
                                        {
                                                cout << "Enter valid article id:" << endl;
                                                continue;
                                        }

                                        break;
                                }
                                protocol.writeProtocol(conn, Protocol::COM_END);
                                break;

                        case Protocol::COM_GET_ART:
                                cout << "Get article " << endl;
                                protocol.writeProtocol(conn, Protocol::COM_GET_ART);
                                protocol.writeProtocol(conn, Protocol::PAR_NUM);
                                cout << "Enter newsgroup id: " << endl;
                                while (getline(cin, parameter_string))
                                {
                                        try
                                        {
                                                protocol.writeNumber(conn, stoi(parameter_string));
                                        }
                                        catch (std::invalid_argument e)
                                        {
                                                cout << "Enter valid newsgroup id:" << endl;
                                                continue;
                                        }

                                        break;
                                }
                                cout << "Enter article id: " << endl;
                                protocol.writeProtocol(conn, Protocol::PAR_NUM);
                                while (getline(cin, parameter_string))
                                {

                                        try
                                        {
                                                protocol.writeNumber(conn, stoi(parameter_string));
                                        }
                                        catch (std::invalid_argument e)
                                        {
                                                cout << "Enter valid article id:" << endl;
                                                continue;
                                        }

                                        break;
                                }
                                protocol.writeProtocol(conn, Protocol::COM_END);
                                break;

                        default:
                                cout << "Error: Enter a valid command" << endl;
                                printCommands();
                                continue;
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
                                break;
                        case Protocol::ANS_CREATE_ART:
                                createArticleResponse(protocol, conn);
                                // method to handle ack/nack
                                break;
                        case Protocol::ANS_DELETE_ART:
                                deleteArticleResponse(protocol, conn);
                                // method to handle ack/nack
                                break;
                        case Protocol::ANS_GET_ART:
                                getArticleResponse(protocol, conn);
                                break;
                        default:
                                cout << "default ANS FALLTHROUGH" << endl;
                                break;
                        }
                        cout << "------------------------" << endl;
                        cout << "Type a command: " << endl;
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
