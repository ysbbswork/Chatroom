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
//" typename "��һ��C++������������еĹؼ��֡� �����ڷ��ͱ��ʱ����һ����" class "��ͬ��ʡ� ����ؼ�������ָ��ģ�����������壩�еķǶ������ƣ�dependent names���������������Ǳ�������
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
        // defaulthandlerҪΪConnectionManager���NoRoom��һ������
        defaulthandler::NoRoom( conn );

	
        conn.CloseSocket();
    }
    else
    {

        //STL��push_back��ֵ���ݵ�(copy-by-value)��ʱconn������vector������Ǹ�����������Ǹ�����һ����
        m_connections.push_back( conn );

        //����Ҫ��vector������Ǹ��������ã��ٲ�����
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


//����ÿ���Ϳͻ��˳ɹ��������ӵ������׽��ִ���������Ϣ
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

                // �����κ��쳣����˳��ȥ�ر�conn
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
void ConnectionManager<protocol, defaulthandler>::Send()//��buffer�е�������һ�·���
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
            // �ܿ��ܾ͹ҵ���
            if( c->GetBufferedBytes() > m_maxbuffered || //һֱû����ȥ
                c->GetLastSendTime() > m_sendtimeout )//GetLastSendTime()�ǵ�ǰʱ��-��һ�η��͵�ʱ�䣬���ǹ��˶�����û�з��Ͷ���������
            {
                c->Close();
                c->Handler()->Hungup();//�ҵ���
                Close( c );
            }
        }
        catch( ... )//���������������
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

    while( itr != m_connections.end() )//ͳһ����ÿ���׽��ֵı�־λ����־Ϊ�رյģ���ʱ��һ�¹ر�
    {
        // retain the value of the iterator in "c" while moving "itr" forward
        c = itr++;

        if( c->Closed() )
            Close( c );
    }
}


}   

#endif
