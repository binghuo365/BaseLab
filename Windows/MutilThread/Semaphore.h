#ifndef _SEMAPHORE_TEST_
#define  _SEMAPHORE_TEST_
#include "PublicDef.h"
#include <list>

namespace BaseLabWin
{
	class CSemaphoreTest
	{
	public:
		CSemaphoreTest();
		~CSemaphoreTest();

		static DWORD WINAPI producer(LPVOID pParameter);
		static DWORD WINAPI consumer(LPVOID pParameter);

	private:
		static std::list<Msg> MsgList;
		static int messageNum;
		static HANDLE hsem;
	};
}

#endif