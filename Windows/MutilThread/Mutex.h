#ifndef _MUTEX_
#define _MUTEX_
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
		HANDLE mutex;
	};
}

#endif