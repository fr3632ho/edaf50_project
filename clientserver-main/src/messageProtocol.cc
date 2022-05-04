#include "messageProtocol.h"
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>

using std::cout;
using std::endl;

MessageProtocol::MessageProtocol(shared_ptr<Connection> &conn, Database *db) : conn(conn), db(db) {}
MessageProtocol::MessageProtocol(shared_ptr<Connection> &conn) : conn(conn) {}

void MessageProtocol::writeNumber(const shared_ptr<Connection> &conn, int value)
{
    conn->write((value >> 24) & 0xFF);
    conn->write((value >> 16) & 0xFF);
    conn->write((value >> 8) & 0xFF);
    conn->write(value & 0xFF);
}

int MessageProtocol::readNumber(const std::shared_ptr<Connection> &conn)
{
    unsigned char byte1 = conn->read();
    unsigned char byte2 = conn->read();
    unsigned char byte3 = conn->read();
    unsigned char byte4 = conn->read();
    return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}

void MessageProtocol::writeString(const std::shared_ptr<Connection> &conn, const string &s)
{
    for (char c : s)
    {
        conn->write(c);
    }
    conn->write('$');
}

string MessageProtocol::readString(const std::shared_ptr<Connection> &conn)
{
    string s;
    char ch;
    while ((ch = conn->read()) != '$')
    {
        s += ch;
    }
    return s;
}

void MessageProtocol::writeProtocol(const shared_ptr<Connection> &conn, Protocol p)
{
    conn->write(static_cast<char>(p));
}

Protocol MessageProtocol::readProtocol(const shared_ptr<Connection> &conn)
{
    unsigned char protocol_byte = conn->read();
    return static_cast<Protocol>(protocol_byte);
}

void MessageProtocol::process_request()
{
    Protocol command = readProtocol(conn);
    switch (command)
    {
    case Protocol::COM_LIST_NG:
        cout << "Recieved COM_LIST_NG" << endl;
        listNewsgroups();
        break;
    case Protocol::COM_CREATE_NG:
        cout << "Recieved COM_CREATE_NG" << endl;
        createNewsgroup();
        break;
    case Protocol::COM_DELETE_NG:
        deleteNewsgroup();
        break;
    case Protocol::COM_LIST_ART:
        break;
    case Protocol::COM_CREATE_ART:
        break;
    case Protocol::COM_DELETE_ART:
        break;
    case Protocol::COM_GET_ART:
        break;
    default:
        cout << "Error..." << endl;
        break;
    }
}

void MessageProtocol::listNewsgroups()
{
    Protocol end_byte = readProtocol(conn);
    if (end_byte == Protocol::COM_END)
    {
        cout << "end byte" << endl;
    }

    map<int, Newsgroup> ngs = db->getNewsgroups();
    size_t numberOfNewsgroups = ngs.size();

    std::stringstream buffer;
    for (std::pair<int, Newsgroup> p : ngs)
    {
        buffer << " "
               << p.first << " " << p.second.getTitle();
    }
    buffer << endl;
    string result = buffer.str();

    cout << "res: " << result << endl;

    writeProtocol(conn, Protocol::ANS_LIST_NG);
    writeProtocol(conn, Protocol::PAR_NUM);
    writeNumber(conn, numberOfNewsgroups);
    for (size_t n = 0; n < numberOfNewsgroups; n++)
    {
        writeProtocol(conn, Protocol::PAR_NUM);
        writeNumber(conn, numberOfNewsgroups);
        writeProtocol(conn, Protocol::PAR_STRING);
        writeString(conn, result);
    }
    writeProtocol(conn, Protocol::ANS_END);
}

void MessageProtocol::createNewsgroup()
{
    Protocol parameter_type = readProtocol(conn);

    std::stringstream buffer;
    int num = readNumber(conn);
    cout << num << endl;
    for (int i = 0; i < num; i++)
    {
        buffer << conn->read();
    }
    // string parameter_value = readString(conn);
    //  TODO: Handle if already exists
    cout << buffer.str() << endl;
    string ng_name = buffer.str();
    db->createNewsgroup(ng_name);

    Protocol end_byte = readProtocol(conn);
    if (end_byte == Protocol::COM_END)
    {
        cout << "end byte" << endl;
    }
    writeProtocol(conn, Protocol::ANS_CREATE_NG);
    writeProtocol(conn, Protocol::ANS_ACK);
    /* if newsgroup already exists:
    writeProtocol(conn, Protocol::ANS_NAK);
    writeProtocol(conn, Protocol::ERR_NG_ALREADY_EXISTS);
    */
    writeProtocol(conn, Protocol::ANS_END);
}

void MessageProtocol::deleteNewsgroup()
{
    Protocol parameter_num = readProtocol(conn);
    if (parameter_num == Protocol::PAR_NUM)
    {
        int id = readNumber(conn);
        db->deleteNewsgroup(id);
    }
    writeProtocol(conn, Protocol::ANS_DELETE_NG);
    writeProtocol(conn, Protocol::ANS_ACK);
    /* if newsgroup does not exist:
    writeProtocol(conn, Protocol::ANS_NAK);
    writeProtocol(conn, Protocol::ERR_NG_DOES_NOT_EXIST);
    */
    writeProtocol(conn, Protocol::ANS_END);
}

void MessageProtocol::listArticles()
{
    Protocol parameter_num = readProtocol(conn);
    if (parameter_num == Protocol::PAR_NUM)
    {
        int id = readNumber(conn);
        map<int, Article> articles = db->getNewsgroupArticles(id);
        std::stringstream buffer;
        for (std::pair<int, Article> p : articles)
        {
            buffer << " "
                   << p.first << " " << p.second.getTitle();
        }
        buffer << endl;
        string result = buffer.str();
    }
}