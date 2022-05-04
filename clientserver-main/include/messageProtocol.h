#ifndef MESSAGE_PROTOCOL_H
#define MESSAGE_PROTOCOL_H

#include "protocol.h"
#include "volatileDatabase.h"
#include "connection.h"
#include <memory>

using std::shared_ptr;

class MessageProtocol
{
public:
    MessageProtocol(shared_ptr<Connection> &conn, VolatileDatabase *db);
    MessageProtocol(shared_ptr<Connection> &conn);
    int readNumber(const shared_ptr<Connection> &conn);
    Protocol readProtocol(const shared_ptr<Connection> &conn);
    string readString(const std::shared_ptr<Connection> &conn);
    void writeString(const shared_ptr<Connection> &conn, const string &s);
    void writeInt(const shared_ptr<Connection> &conn, const int &value);
    void writeProtocol(const shared_ptr<Connection> &conn, const Protocol c);
    void writeChar(const shared_ptr<Connection> &conn, const char &c);
    void process_request();

private:
    void listNewsgroups();
    void createNewsgroup();
    void deleteNewsgroup();
    shared_ptr<Connection> conn;
    Database *db;
};

#endif