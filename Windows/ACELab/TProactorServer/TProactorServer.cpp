#include "ace/Proactor.h"
#include "ace/Asynch_Acceptor.h"
#include "ServerHandler.h"
#include "TProactor/Asynch_Acceptor.h"
#include "TProactor/Proactor.h"

int main(int argc, char* argv[])
{
	ACE_INET_Addr port_to_accept(9999);
	TRB_Asynch_Acceptor<ServerHandler> server;
	if (server.open(port_to_accept,
		0,
		1,
		ACE_DEFAULT_ASYNCH_BACKLOG,
		1,
		TRB_Proactor::instance(),
		1) == -1)
	{
		return -1;
	}

	TRB_Proactor::instance()->proactor_run_event_loop();
	return 0;
}