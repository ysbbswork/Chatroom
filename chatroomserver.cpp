


#include "SocketLib/SocketLib.h"
#include "SCUserDB.h"
#include "SCLogon.h"
#include "SCChat.h"
#include <unistd.h>
int main()
{	

    
	using SocketLib::Telnet;

	SocketLib::ListeningManager<Telnet, SCLogon> lm;
    
	SocketLib::ConnectionManager<Telnet, SCLogon> cm(128);

	lm.SetConnectionManager(&cm);
	lm.AddPort(5099);
    lm.AddPort(5000);    
	while (1)
	{
		lm.Listen();
		cm.Manage();
        usleep(100);
	}

}
