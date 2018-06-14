

#ifndef SCCHAT_H
#define SCCHAT_H

#include "SocketLib/SocketLib.h"
#include "SCUserDB.h"
#include <string>

using SocketLib::Connection;
using SocketLib::Telnet;
using std::string;

//�̳����й̶�Э��ĺ������ʽ��ConnectionHandler
class SCChat : public Telnet::handler
{
    typedef Telnet::handler thandler;
public:

    SCChat( Connection<Telnet>& p_conn )
        : thandler( p_conn ) {};

    void Handle( string p_data );


    void Enter();

    void Leave();


    void Hungup();

    void Flooded();

    void CloseConnection( const string& p_reason );


    void SendAll( const string& p_message );

};

#endif
