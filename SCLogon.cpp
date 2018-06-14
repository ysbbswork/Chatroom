


#include "SCLogon.h"
#include "SCChat.h"
#include "SCUserDB.h"

using namespace SocketLib;

void SCLogon::Handle( string p_data )
{

    Connection<Telnet>* conn = m_connection;

    if( !UserDatabase::IsValidName( p_data ) )
    {
        conn->Protocol().SendString( *conn, red + bold + 
            "Sorry, that is an invalid username.\r\n" +
            "Please enter another username: " + reset + bold );
        return;
    }

    if( UserDatabase::HasUser( p_data ) )
    {
        conn->Protocol().SendString( *conn, red + bold + 
            "Sorry, that name is already in use.\r\n" +
            "Please enter another username: " + reset );
        return;
    }


    UserDatabase::AddUser( conn, p_data );
    conn->Protocol().SendString( *conn, "Thank you for joining us, " + 
                                 p_data + newline );

    conn->AddHandler( new SCChat( *conn ) );
	
}




void SCLogon::Enter()
{
    // welcome the new user
    m_connection->Protocol().SendString( *m_connection,
        green + bold + "Welcome To SimpleChat!\r\n" + 
        "Please enter your username: " + reset + bold );
}



