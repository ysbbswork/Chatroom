

#include "Telnet.h"

namespace SocketLib
{

using std::string;


void Telnet::Translate( Connection<Telnet>& p_conn, char* p_buffer, int p_size )
{
    for( int i = 0; i < p_size; i++ )
    {
		

        char c = p_buffer[i];
		if (c == '\n' || c == '\r')
		{
		
			if (m_buffersize > 0 && p_conn.Handler() != 0)
			{	
				//every single command after translate will be send to conn's handler to handle
				p_conn.Handler()->Handle(string(m_buffer, m_buffersize));
			}
			m_buffersize = 0;
		}
        else if( m_buffersize < BUFFERSIZE )
        {
            m_buffer[m_buffersize] = c;
            m_buffersize++;
        }

        // else check if it's a backspace, number 8 is the backspace
        else if( c == 8 && m_buffersize > 0 )
        {
            // erase the last character
            m_buffersize--;
        }


       
    }
}



void Telnet::SendString( Connection<Telnet>& p_conn, string p_string )
{
    // just buffer the data on the connection
    p_conn.BufferData( p_string.data(), (int)p_string.size() );
}


}  
