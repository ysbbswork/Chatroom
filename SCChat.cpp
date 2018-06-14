

#include "SCChat.h"
#include <exception>

using namespace SocketLib;


void SCChat::Handle( string p_data )
{

    string name = UserDatabase::find( m_connection )->name;


    if( p_data[0] == '/' )
    {
        string command = BasicLib::ParseWord( p_data, 0 );
        string data = BasicLib::RemoveWord( p_data, 0 );

        if( command == "/who" )
        {
            string wholist = magenta + bold + "Who is in the room: ";

            UserDatabase::iterator itr = UserDatabase::begin();
            while( itr != UserDatabase::end() )
            {
                wholist += (*itr).name;

                ++itr;
                if( itr != UserDatabase::end() )
                {
                    wholist += ", ";
                }
            }
            wholist += newline;

            m_connection->Protocol().SendString( *m_connection, wholist );
        }

        else if( command == "/quit" )
        {
            CloseConnection( "has quit. Message: " + data );
            m_connection->Close();
			m_connection->ClearHandlers();
        }
		else if (command == "/leave")
		{
			CloseConnection("has quit. Message: " + data);
			m_connection->RemoveHandler();
		}
    }


    else
    {
        if( BasicLib::TrimWhitespace( p_data ).size() > 0 )
        {
            SendAll( green + bold + "<" + name + "> " + reset + p_data );
        }
    }

}


void SCChat::Enter()
{
    SendAll( bold + yellow + UserDatabase::find( m_connection )->name +
             " has entered the room." );
}


void SCChat::Leave()
{
	SendAll(bold + yellow + UserDatabase::find(m_connection)->name +
		" has leave the room.");
    UserDatabase::DeleteUser( m_connection );
}


void SCChat::Hungup()
{	SendAll( bold + yellow + UserDatabase::find( m_connection )->name +
             " has entered the room." );
    CloseConnection( "has hung up!" );
}

void SCChat::Flooded()
{
    CloseConnection( "has been kicked for flooding!" );
}

void SCChat::CloseConnection( const string& p_reason )
{
    SendAll( bold + red + UserDatabase::find( m_connection )->name +
        " " + p_reason );
}


void SCChat::SendAll( const string& p_message )
{
    UserDatabase::iterator itr = UserDatabase::begin();

    // loop through every connection and send the message.
    while( itr != UserDatabase::end() )
    {
        itr->connection->Protocol().SendString( *itr->connection, p_message + newline );
        ++itr;
    }
}

