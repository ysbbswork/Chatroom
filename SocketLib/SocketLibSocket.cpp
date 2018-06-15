
// socket classes: Basic, Data, and Listening.


#include "SocketLibSocket.h"
namespace SocketLib
{


    void Socket::Close()
    {

    #ifdef WIN32
        closesocket( m_sock );
    #else
        close( m_sock );
    #endif

        // invalidate the socket
        m_sock = -1;
    }


    void Socket::SetBlocking( bool p_blockmode )
    {
        int err;

        #ifdef WIN32
            unsigned long mode = !p_blockmode;
            err = ioctlsocket( m_sock, FIONBIO, &mode );
        #else
            
            // get the flags
            int flags = fcntl( m_sock, F_GETFL, 0 );

            // set or clear the non-blocking flag
            if( p_blockmode == false )
            {
                flags |= O_NONBLOCK;
            }
            else
            {
                flags &= ~O_NONBLOCK;
            }
            err = fcntl( m_sock, F_SETFL, flags );
        #endif

        if( err == -1 )
        {
            throw( Exception( GetError() ) );
        }

        m_isblocking = p_blockmode;
    }



    Socket::Socket( sock p_socket ): m_sock( p_socket )
    {
        if( p_socket != -1 )
        {
            socklen_t s = sizeof(m_localinfo);
            getsockname( p_socket, (sockaddr*)(&m_localinfo), &s );
        }

        // the socket is blocking by default
        m_isblocking = true;
    }




    DataSocket::DataSocket( sock p_socket ) : Socket( p_socket ),m_connected( false )
    {
        if( p_socket != -1 )
        {
            socklen_t s = sizeof(m_remoteinfo);
            getpeername( p_socket, (sockaddr*)(&m_remoteinfo), &s );
            m_connected = true;
        }
    }



    void DataSocket::Connect( ipaddress p_addr, port p_port )
    {
        int err;

        // if the socket is already connected...
        if( m_connected == true )
        {
            throw Exception( EAlreadyConnected );
        }

        // first try to obtain a socket descriptor from the OS, if
        // there isn't already one.
        if( m_sock == -1 )
        {
            m_sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

            // throw an exception if the socket could not be created
            if( m_sock == -1 )
            {
                throw Exception( GetError() );
            }
        }

        // set up the socket address structure
        m_remoteinfo.sin_family = AF_INET;
        m_remoteinfo.sin_port = htons( p_port );
        m_remoteinfo.sin_addr.s_addr = p_addr;
        memset( &(m_remoteinfo.sin_zero), 0, 8 );

        // now the socket is created, so connect it.
        socklen_t s = sizeof(struct sockaddr);
        err = connect( m_sock, (struct sockaddr*)(&m_remoteinfo), s );
        if( err == -1 )
        {
            throw Exception( GetError() );
        }

        m_connected = true;

        // to get the local port, you need to do a little more work
        err = getsockname( m_sock, (struct sockaddr*)(&m_localinfo), &s );
        if( err != 0 )
        {
            throw Exception( GetError() );
        }
    }


    int DataSocket::Send( const char* p_buffer, int p_size )
    {
        int err;

        // make sure the socket is connected first.
        if( m_connected == false )
        {
            throw Exception( ENotConnected );
        }

        // attempt to send the data
        err = send( m_sock, p_buffer, p_size, 0 );
        if( err == -1 )
        {
            Error e = GetError();
            if( e != EOperationWouldBlock )
            {
                throw Exception( e );
            }


            err = 0;
        }


        return err;
    }


    int DataSocket::Receive( char* p_buffer, int p_size )
    {
        int err;


        if( m_connected == false )
        {
            throw Exception( ENotConnected );
        }

        err = recv( m_sock, p_buffer, p_size, 0 );
        if( err == 0 )
        {
            throw Exception( EConnectionClosed );
        }
        if( err == -1 )
        {
            throw Exception( GetError() );
        }

        return err;
    }


    void DataSocket::Close()
    {
        if( m_connected == true )
        {
            shutdown( m_sock, 2 );
        }

        // close the socket
        Socket::Close();

        m_connected = false;
    }


    ListeningSocket::ListeningSocket()
    {
        m_listening = false;
    }


    void ListeningSocket::Listen( port p_port )
    {
        int err;


        if( m_sock == -1 )
        {
            m_sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

            // throw an exception if the socket could not be created
            if( m_sock == -1 )
            {
                throw Exception( GetError() );
            }
        }


        int reuse = 1;
        err = setsockopt( m_sock, SOL_SOCKET, SO_REUSEADDR,
                          (char*)(&reuse), sizeof( reuse ) );
        if( err != 0 )
        {
            throw Exception( GetError() );
        }

        m_localinfo.sin_family = AF_INET;
        m_localinfo.sin_port = htons( p_port );//整个socket address structure需要动态定义的是端口号
        m_localinfo.sin_addr.s_addr = htonl( INADDR_ANY );//绑定到任何地址，= inet_addr("127.0.0.1");时是绑定到具体的IP地址
        memset( &(m_localinfo.sin_zero), 0, 8 );


        err = bind( m_sock, (struct sockaddr*)&m_localinfo,
            sizeof(struct sockaddr));
        if( err == -1 )
        {
            throw Exception( GetError() );
        }


        err = listen( m_sock, 8 );
        if( err == -1 )
        {
            throw Exception( GetError() );
        }

        m_listening = true;
    }



    DataSocket ListeningSocket::Accept()
    {
        sock s;
        struct sockaddr_in socketaddress;

        // try to accept a connection
        socklen_t size = sizeof(struct sockaddr);
        s = accept( m_sock, (struct sockaddr*)&socketaddress, &size );
        if( s == -1 )
        {
            throw Exception( GetError() );
        }

        // return the newly created socket.
        return DataSocket( s );
    }



    void ListeningSocket::Close()
    {

        Socket::Close();


        m_listening = false;
    }

}
