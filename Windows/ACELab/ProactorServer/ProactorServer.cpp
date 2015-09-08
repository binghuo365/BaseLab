#include "ace/Proactor.h"
#include "ace/Asynch_Acceptor.h"
#include "ServerHandler.h"

int main(int argc, char* argv[])
{
	ACE_INET_Addr port_to_accept(9999);
	ACE_Asynch_Acceptor<ServerHandler> server;
	if (server.open(port_to_accept) == -1)
	{
		return -1;
	}

	ACE_Proactor::instance()->proactor_run_event_loop();
	return 0;
}