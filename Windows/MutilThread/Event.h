#ifndef _EVENT_TEST_
#define _EVENT_TEST_
#include "PublicDef.h"
#include <list>

namespace BaseLabWin
{
	class CEventTest
	{
	public:
		CEventTest();
		~CEventTest();

	public:
		static DWORD WINAPI producer(LPVOID pParameter);
		static DWORD WINAPI consumer(LPVOID pParameter);

	private:
		static std::list<Msg> MsgList;
		static int messageNum;
		static HANDLE event;
	};
}
#endif