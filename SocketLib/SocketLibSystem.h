


#ifndef SOCKETLIBSYSTEM_H
#define SOCKETLIBSYSTEM_H



#include <string>
#include "SocketLibTypes.h"


namespace SocketLib
{


    ipaddress GetIPAddress( const std::string p_address );


    std::string GetIPString( ipaddress p_address );

    std::string GetHostNameString( ipaddress p_address );


    bool IsIPAddress( const std::string p_address );



}


#endif

