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
        listArticles();
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
        cout << "Recieved COM_END" << endl;
    }

    map<int, Newsgroup> ngs = db->getNewsgroups();
    size_t numberOfNewsgroups = ngs.size();

    writeProtocol(conn, Protocol::ANS_LIST_NG);
    writeProtocol(conn, Protocol::PAR_NUM);
    writeNumber(conn, numberOfNewsgroups);

    // for (size_t n = 1; n <= numberOfNewsgroups; n++)
    for (std::pair<int, Newsgroup> p : ngs)
    {
        writeProtocol(conn, Protocol::PAR_NUM);
        writeNumber(conn, p.first);
        writeProtocol(conn, Protocol::PAR_STRING);
        writeNumber(conn, ngs.at(p.first).getTitle().size());
        for (char c : ngs.at(p.first).getTitle())
        {
            conn->write(c);
        }
    }
    writeProtocol(conn, Protocol::ANS_END);
}

void MessageProtocol::createNewsgroup()
{
    Protocol proto = readProtocol(conn);

    std::stringstream ng_name;
    while (proto != Protocol::COM_END)
    {
        int size = readNumber(conn);

        for (int i = 0; i < size; i++)
            ng_name << conn->read();

        cout << "name: " << ng_name.str() << endl;

        proto = readProtocol(conn);
    }
    //  TODO: Handle if already exists
    bool result = db->createNewsgroup(ng_name.str());

    cout << "Recieved COM_END" << endl;
    writeProtocol(conn, Protocol::ANS_CREATE_NG);

    if (result)
    {
        writeProtocol(conn, Protocol::ANS_ACK);
    }
    else
    {
        writeProtocol(conn, Protocol::ANS_NAK);
        writeProtocol(conn, Protocol::ERR_NG_ALREADY_EXISTS);
        cout << "ng already exists!!" << endl;
    }
    writeProtocol(conn, Protocol::ANS_END);
}

void MessageProtocol::deleteNewsgroup()
{
    /*
    Protocol parameter_num = readProtocol(conn);
    if (parameter_num == Protocol::PAR_NUM)
    {
        int id = readNumber(conn);
        db->deleteNewsgroup(id);
    }
    */
    Protocol p = readProtocol(conn);
    int id{};

    while (p != Protocol::COM_END)
    {
        id = readNumber(conn);

        cout << "id : " << id << endl;

        p = readProtocol(conn);
    }
    db->deleteNewsgroup(id);
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

        Protocol p = readProtocol(conn);
        if (p == Protocol::COM_END)
            cout << "com_end recieved " << endl;

        writeProtocol(conn, Protocol::ANS_LIST_ART);
        writeProtocol(conn, Protocol::ANS_ACK);
        writeProtocol(conn, Protocol::PAR_NUM);
        writeNumber(conn, 0);
        writeProtocol(conn, Protocol::ANS_END);
    }
}