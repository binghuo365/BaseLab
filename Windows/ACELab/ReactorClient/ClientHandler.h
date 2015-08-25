#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include "ace/Svc_Handler.h"
#include "ace/SOCK_Stream.h"	

namespace BaseLabWin
{
	namespace ACELab
	{
		class ClientHandler : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
		{
			typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> super;

		public:
			virtual int open(void * = 0);
			virtual int handle_timeout(const ACE_Time_Value &current_time, const void *act = 0);
			virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);

		private:
			enum { ITERATIONS = 5 };
			int iterations_;
		};
	}
}
#endif