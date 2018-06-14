


#ifndef SCLOGON_H
#define SCLOGON_H


#include "SocketLib/SocketLib.h"
#include "SCUserDB.h"
#include <string>

using SocketLib::Connection;
using SocketLib::Telnet;
using std::string;


class SCLogon : public Telnet::handler
{
    typedef Telnet::handler thandler;
public:
    
    SCLogon( Connection<Telnet>& p_conn )
        : thandler( p_conn ) {};


    void Handle( string p_data );


    static void NoRoom( Connection<Telnet>& p_connection )
    {
        static string msg = "Sorry, there is no more room on this server.\r\n";
        try
        {
            p_connection.Send( msg.c_str(), (int)msg.size() );
        }
        catch( SocketLib::Exception )
        {

        }
    }


    void Enter();


    void Leave() {};

    void Hungup() {};

    void Flooded() {};
};




#endif
