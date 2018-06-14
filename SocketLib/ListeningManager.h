
#ifndef LISTENINGMANAGER_H
#define LISTENINGMANAGER_H


#include "SocketLibTypes.h"
#include "SocketLibSocket.h"
#include "SocketSet.h"
#include "SocketLibErrors.h"
#include "ConnectionManager.h"
#include <vector>

namespace SocketLib
{


template<typename protocol, typename defaulthandler>
class ConnectionManager;



template<typename protocol, typename defaulthandler>
class ListeningManager
{
public:

    ListeningManager();

    ~ListeningManager();

    void AddPort( port p_port );


    void SetConnectionManager( ConnectionManager<protocol, defaulthandler>* p_manager );//listeningManger之所以带个defaulthandler模板参数全是因为connectionManger需要，而connectionmanger是listeningmanger的一个参数。protocol也是相同原因


    void Listen();

protected:

    std::vector<ListeningSocket> m_sockets;

    SocketSet m_set;


    ConnectionManager<protocol, defaulthandler>* m_manager;
};



template<typename protocol, typename defaulthandler>
ListeningManager<protocol, defaulthandler>::ListeningManager()
{
    m_manager = 0;
}


template<typename protocol, typename defaulthandler>
ListeningManager<protocol, defaulthandler>::~ListeningManager()
{   
  
    for( size_t i = 0; i < m_sockets.size(); i++ )
    {
        m_sockets[i].Close();
    }
}



template<typename protocol, typename defaulthandler>
void ListeningManager<protocol, defaulthandler>::AddPort( port p_port )
{
    if( m_sockets.size() == MAX )
    {
        Exception e( ESocketLimitReached );
        throw( e );
    }


    ListeningSocket lsock;

    lsock.Listen( p_port );

    lsock.SetBlocking( false );


    m_sockets.push_back( lsock );//

    // ，实现set实现多路复用，对多个接口进行监听也OK
    m_set.AddSocket( lsock );//m_set不是一个set，传进去文件描述符后，这个m_set就是一个用poll的对象，可以返回select结果，但是轮询还是得去socket的vector里进行
}



// 他的功能告诉监听管理器，每当接受一个新的套接字时，应该使用哪个连接管理器。
template<typename protocol, typename defaulthandler>
void ListeningManager<protocol, defaulthandler>::
SetConnectionManager( ConnectionManager<protocol, defaulthandler>* p_manager )
{

    m_manager = p_manager;
}


template<typename protocol, typename defaulthandler>
void ListeningManager<protocol, defaulthandler>::Listen()
{	
   
    DataSocket datasock;

  
    if( m_set.Poll() > 0 )//利用select获得套接字是否有活动的，但不能告诉你具体是哪个在活动。。所以需要下面的遍历
    {

        for( size_t s = 0; s < m_sockets.size(); s++ )//将所有接口的监听套接字遍历一遍
        {

            if( m_set.HasActivity( m_sockets[s] ) )//挨个去询问这个套接字在不在fd_set集合中
            {
                try
                {
                    // accept the connection
                    datasock = m_sockets[s].Accept();
					std::cout << "监听到数据套接字 " << datasock.GetSock() << " 安排进manager了 " << std::endl;
                    //把监听到的新的活动连接
                    m_manager->NewConnection( datasock );//将某一个监听接口获得的新连接添加到ConnectionManager里
                }


                catch( Exception& e )
                {
                    if( e.ErrorCode() != EOperationWouldBlock )//如果=EWOULDBLOCK时是为阻塞了，不用管，要不是就抛出错误
                    {
                        throw e;
                    }
                }
            }   
        }   
    }  

}




}  


#endif
