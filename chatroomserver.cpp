


#include "SocketLib/SocketLib.h"
#include "SCUserDB.h"
#include "SCLogon.h"
#include "SCChat.h"
#include <Windows.h>

#pragma comment(lib,"ws2_32.lib")//VS in Windows need include ws2_32.dll


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
		Sleep(5);
	}

}
