#ifndef _MUTEX_TEST_
#define _MUTEX_TEST_
#include <windows.h>
#include<list>
#include "PublicDef.h"

namespace BaseLabWin
{
	class CMutexTest
	{
	public:
		CMutexTest();
		~CMutexTest();

		static DWORD WINAPI producer(LPVOID pParameter);
		static DWORD WINAPI consumer(LPVOID pParameter);

	private:
		static std::list<Msg> MsgList;
		static int messageNum;
		static HANDLE mutex;
	};
}

#endif