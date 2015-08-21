#ifndef _SERVERHANDLER_
#define _SERVERHANDLER_

#include "ace/Svc_Handler.h"
#include "ace/SOCK_Stream.h"	

namespace BaseLab
{
	namespace ACELab
	{
		class ServerHandler : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
		{
		public:
			ServerHandler(){}
			virtual ~ServerHandler(){}

		public:
			virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);
		};
	}
}

#endif
