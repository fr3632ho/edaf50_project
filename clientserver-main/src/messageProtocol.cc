#include "messageProtocol.h"
#include "protocol.h"

MessageProtocol::MessageProtocol(shared_ptr<Connection> &conn, VolatileDatabase *db) : conn(conn), db(db) {}

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

Protocol MessageProtocol::readProtocol(const shared_ptr<Connection> &conn)
{
    unsigned char protocol_byte = conn->read();
    return static_cast<Protocol>(protocol_byte);
}