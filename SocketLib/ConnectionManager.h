// The class that will manage connections.

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <list>
#include <iostream>
#include "SocketLibTypes.h"
#include "SocketLibErrors.h"
#include "SocketSet.h"
#include "Connection.h"



namespace SocketLib
{

// Forward Declarations
template<typename protocol>
class Connection;




template<typename protocol, typename defaulthandler>
class ConnectionManager
{
//" typename "是一个C++程序设计语言中的关键字。 当用于泛型编程时是另一术语" class "的同义词。 这个关键字用于指出模板声明（或定义）中的非独立名称（dependent names）是类型名，而非变量名。
    typedef std::list< Connection<protocol> > clist;
    typedef typename std::list< Connection<protocol> >::iterator clistitr; 

public:

    ConnectionManager( int p_maxdatarate = 1024,    // 1 kbyte/second
                       int p_sentimeout = 60,       // 60 seconds
                       int p_maxbuffered = 8192 );  // 8 kbytes


    ~ConnectionManager();

    void NewConnection( DataSocket& p_socket );

    inline int AvailableConnections() const
    {
        return MAX - (int)m_connections.size();
    }

    inline int TotalConnections() const
    {
        return (int)m_connections.size();
    }

    void CloseConnections();

    void Listen();

    void Send();

    inline void Manage()
    {
        Listen();
        Send();
        CloseConnections();
    }

protected:

    void Close( clistitr p_itr );



protected:


    clist m_connections;

    int m_maxdatarate;

    int m_sendtimeout;

    int m_maxbuffered;

    SocketSet m_set;
};



template<typename protocol, typename defaulthandler>
ConnectionManager<protocol, defaulthandler>::
ConnectionManager( int p_maxdatarate,    
                   int p_sentimeout,
                   int p_maxbuffered )
{
    m_maxdatarate = p_maxdatarate;
    m_sendtimeout = p_sentimeout;
    m_maxbuffered = p_maxbuffered;
}


template<typename protocol, typename defaulthandler>
ConnectionManager<protocol, defaulthandler>::~ConnectionManager()
{
    
    clistitr itr;

    for( itr = m_connections.begin(); itr != m_connections.end(); ++itr )
        itr->CloseSocket();
}




template<typename protocol, typename defaulthandler>
void ConnectionManager<protocol, defaulthandler>::
NewConnection( DataSocket& p_socket )
{
    // turn the socket into a connection
    Connection<protocol> conn( p_socket );

    if(AvailableConnections() == 0)
    {
        // defaulthandler要为ConnectionManager设计NoRoom这一个方法
        defaulthandler::NoRoom( conn );

	
        conn.CloseSocket();
    }
    else
    {

        //STL的push_back是值传递的(copy-by-value)此时conn不是是vector外面的那个，和里面的那个不是一个，
        m_connections.push_back( conn );

        //所以要对vector里面的那个创建引用，再操作。
        Connection<protocol>& c = *m_connections.rbegin();


        c.SetBlocking( false );


        m_set.AddSocket( c );


        c.AddHandler( new defaulthandler( c ) );
    }
}


template<typename protocol, typename defaulthandler>
void ConnectionManager<protocol, defaulthandler>::Close( clistitr p_itr )
{
   
    m_set.RemoveSocket( *p_itr );
   
    p_itr->CloseSocket();

    m_connections.erase( p_itr );
}


//监听每个和客户端成功建立连接的数据套接字传回来的消息
template<typename protocol, typename defaulthandler>
void ConnectionManager<protocol, defaulthandler>::Listen()
{
    int socks = 0;
    if( TotalConnections() > 0 )
    {
        //socks=1;
        socks = m_set.Poll(1);
    }

   
    if( socks != 0 )
    {


        clistitr itr = m_connections.begin();
        clistitr c;


        while( itr != m_connections.end() )
        {

            c = itr++;
            
            
            if( m_set.HasActivity( *c ) )
            {
                try
                {
                    
                    c->Receive();

                    // check to see if the connection is flooding.
                    if( c->GetCurrentDataRate() > m_maxdatarate )
                    {
                        // too much data was sent, tell the protocol handler
                        c->Close();
                        c->Handler()->Flooded();

                        // close the connection
                        Close( c );
                    }
                }

                // 其它任何异常都按顺序去关闭conn
                catch( ... )
                {
                    c->Close();
                    c->Handler()->Hungup();
                    Close( c );
                }

            }  

        }  

    }   
}



template<typename protocol, typename defaulthandler>
void ConnectionManager<protocol, defaulthandler>::Send()//将buffer中的内容作一下发送
{

    clistitr itr = m_connections.begin();
    clistitr c;

    // loop through every connection
    while( itr != m_connections.end() )
    {
        // move "itr" to the next connection, but keep "c" on the current

        c = itr++;

        // now try to send all buffered data on the socket.
        try
        {
            c->SendBuffer();

            // 
            // 很可能就挂掉了
            if( c->GetBufferedBytes() > m_maxbuffered || //一直没发出去
                c->GetLastSendTime() > m_sendtimeout )//GetLastSendTime()是当前时间-上一次发送的时间，就是过了多少秒没有发送东西给他了
            {
                c->Close();
                c->Handler()->Hungup();//挂掉了
                Close( c );
            }
        }
        catch( ... )//如果发生其他意外
        {
            c->Close();
            c->Handler()->Hungup();
            Close( c );
        }
    }  
}




template<typename protocol, typename defaulthandler>
void ConnectionManager<protocol, defaulthandler>::CloseConnections()
{
    clistitr itr = m_connections.begin();
    clistitr c;

    while( itr != m_connections.end() )//统一查找每个套接字的标志位，标志为关闭的，此时作一下关闭
    {
        // retain the value of the iterator in "c" while moving "itr" forward
        c = itr++;

        if( c->Closed() )
            Close( c );
    }
}


}   

#endif
