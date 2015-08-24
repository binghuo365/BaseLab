#include "ace/Reactor.h"
#include "ace/Acceptor.h"
#include "ace/SOCK_Acceptor.h"	
#include "ServerHandler.h"

using namespace BaseLabWin::ACELab;

int main(int argc, char* argv[])
{
	ACE_INET_Addr port_to_accept(9999);
	ACE_Acceptor<ServerHandler, ACE_SOCK_ACCEPTOR> server;
	if (server.open(port_to_accept) == -1)
	{
		return -1;
	}

	ACE_Reactor::instance()->run_reactor_event_loop();
	return 0;
}