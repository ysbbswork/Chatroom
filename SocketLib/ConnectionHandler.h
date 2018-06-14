


#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H

#include "SocketLibTypes.h"
#include "Connection.h"


namespace SocketLib
{

template<typename protocol, typename command>
class ConnectionHandler
{

public:

    typedef Connection<protocol> conn;//ĳһ������


    ConnectionHandler( conn& p_conn ) : m_connection( &p_conn ) {}
    virtual ~ConnectionHandler() {};

    //Handle ���Ƕ����е�����������д���
    virtual void Handle( command p_data ) = 0;


    virtual void Enter() = 0;


    virtual void Leave() = 0;


    virtual void Hungup() = 0;


    virtual void Flooded() = 0;

protected:
    conn* m_connection;

};


}   
#endif
