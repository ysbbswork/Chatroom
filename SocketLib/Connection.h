



#ifndef CONNECTION_H
#define CONNECTION_H

#include <stack>
#include "BasicLib/BasicLib.h"
#include "SocketLibTypes.h"
#include "ConnectionManager.h"
#include "SocketLibSocket.h"

namespace SocketLib
{


const static int BUFFERSIZE = 1024;
const static int TIMECHUNK = 16;

template< class protocol >
class Connection: public DataSocket
{

public:

    Connection();
    Connection( DataSocket& p_socket );

protected:
    void Initialize();//将所有成员复位


public:


    BasicLib::sint64 GetLastSendTime() const;
    

    inline BasicLib::sint64 GetLastReceiveTime() const
    {
        return m_lastReceiveTime;
    }


    inline void Close()     { m_closed = true; }


    inline void CloseSocket() 
    { 
        DataSocket::Close(); 


        ClearHandlers();
    }


    void BufferData( const char* p_buffer, int p_size );


    void SendBuffer();

    void Receive();

    inline int GetDataRate() const
    {
        return m_lastdatarate;//上次时间块的信息速率
    }

 
    inline int GetCurrentDataRate() const
    {
        return m_datarate / TIMECHUNK;//信息和除以时间块为信息速率，当前时间块的信息速率
    }


    inline int GetBufferedBytes() const
    {
        return (int)m_sendbuffer.size();
    }

    inline BasicLib::sint64 GetCreationTime() const
    {
        return m_creationtime;
    }

    inline protocol& Protocol()         { return m_protocol; }
    inline bool Closed()                { return m_closed; }

    inline void SwitchHandler( typename protocol::handler* p_handler )
    {
        if( Handler() )
        {
            Handler()->Leave();     
            delete Handler();       
            m_handlerstack.pop();  
        }

        m_handlerstack.push( p_handler );
        p_handler->Enter();    
    }

    inline void AddHandler( typename protocol::handler* p_handler )
    {   
        if( Handler() )
            Handler()->Leave();
        m_handlerstack.push( p_handler );
        p_handler->Enter();   
    }

    inline void RemoveHandler()
    {
        Handler()->Leave();     
        delete Handler();       
        m_handlerstack.pop();   
        if( Handler() )          
            Handler()->Enter(); 
    }


    inline typename protocol::handler* Handler()
    {
        if( m_handlerstack.size() == 0 )
            return 0;
        return m_handlerstack.top();
    }


    void ClearHandlers()
    {
       
        if( Handler() )  
            Handler()->Leave();

    
        while( Handler() )
        {
            delete Handler();
            m_handlerstack.pop();
        }
    }



protected:


    
    protocol m_protocol;


    std::stack< typename protocol::handler* > m_handlerstack;

    //用来存储要发送的缓存区，之所以用string来存，是因为在末尾加东西很简单，
	//用.data()方法很容易转换成char*数组，不是固定大小，可以缓存很多慢慢发送，
	//通过connectionmanger来控制这个缓存阀值，如果循环了一段时间仍然缓存了很多数据没发出去，是有问题的。
    std::string m_sendbuffer;
    
    int m_datarate;

 
    int m_lastdatarate;


    BasicLib::sint64 m_lastReceiveTime;

    BasicLib::sint64 m_lastSendTime;

    BasicLib::sint64 m_creationtime;


    bool m_checksendtime;

    char m_buffer[BUFFERSIZE];

    bool m_closed;
};





template< class protocol >
Connection<protocol>::Connection()
{
    Initialize();
}

template< class protocol >
Connection<protocol>::Connection( DataSocket& p_socket )
    : DataSocket( p_socket )
{
    Initialize();
}


template< class protocol >
void Connection<protocol>::Initialize()
{
    m_datarate = 0;
    m_lastdatarate = 0;
    m_lastReceiveTime = 0;
    m_lastSendTime = 0;
    m_checksendtime = false;
    m_creationtime = BasicLib::GetTimeMS();
    m_closed = false;
}


template< class protocol >
BasicLib::sint64 Connection<protocol>::GetLastSendTime() const
{
    if( m_checksendtime )
    {
        return BasicLib::GetTimeS() - m_lastSendTime;
    }

    return 0;
}



template< class protocol >
void Connection<protocol>::BufferData( const char* p_buffer, int p_size )
{
    m_sendbuffer.append( p_buffer, p_size );
}

template< class protocol >
void Connection<protocol>::SendBuffer()
{
    if( m_sendbuffer.size() > 0 )
    {
        
        int sent = Send( m_sendbuffer.data(), (int)m_sendbuffer.size() );
        m_sendbuffer.erase( 0, sent );//缓存区发出去多少为send，然后把这些发出去的部分从头部去掉

        if( sent > 0 )
        {
            
            m_lastSendTime = BasicLib::GetTimeS();
            m_checksendtime = false;
        }
        else
        {
           
            if( !m_checksendtime )
            {
                
                m_checksendtime = true;
                m_lastSendTime = BasicLib::GetTimeS();

            }
        }   

    }  
}


template< class protocol >
void Connection<protocol>::Receive()
{

    int bytes = DataSocket::Receive( m_buffer, BUFFERSIZE );

    BasicLib::sint64 t = BasicLib::GetTimeS();

    //利用截断除法来判断是否和上一条数据在一个时间块中。比如：TIMECHUNK=16，从0~15 /16 都为0，都在一个时间块中，16以上都不相等了。
    if( (m_lastReceiveTime / TIMECHUNK) != (t / TIMECHUNK) )
    {
        m_lastdatarate = m_datarate / TIMECHUNK;//记录上一次的信息速率
        m_datarate = 0;//复位信息和
        m_lastReceiveTime = t;//不在一个时间块时，记录最新时间
    }

    m_datarate += bytes;//在同一个时间块中的信息合

    // 协议主要在这里起作用，起翻译作用
    m_protocol.Translate( *this, m_buffer, bytes );
}


}   


#endif
