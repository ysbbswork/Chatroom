


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

    typedef Connection<protocol> conn;//某一个连接


    ConnectionHandler( conn& p_conn ) : m_connection( &p_conn ) {}
    virtual ~ConnectionHandler() {};

    //Handle 就是对所有的输入命令进行处理
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
