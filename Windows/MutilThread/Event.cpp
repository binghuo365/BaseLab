#include "Event.h"
#include "time.h"
#include <iostream>

using namespace BaseLabWin;

std::list<Msg> CEventTest::MsgList;
int CEventTest::messageNum(0);

BaseLabWin::CEventTest::CEventTest()
{
	event = ::CreateEvent(NULL, false, true, "event");
}

BaseLabWin::CEventTest::~CEventTest()
{
	::CloseHandle(event);
}

DWORD WINAPI BaseLabWin::CEventTest::producer(LPVOID pParameter)
{
	Msg msgtmp;
	int i = 0;
	srand((unsigned)time(0));
	HANDLE tevent = ::OpenEvent(MUTEX_ALL_ACCESS, false, "event");
	while (TRUE)
	{
		::WaitForSingleObject(tevent, INFINITE);
		if (MsgList.size() < MaxQueSize)
		{
			msgtmp.head = ++messageNum;
			MsgList.push_back(msgtmp);
			std::cout << "product message :" << msgtmp.head << std::endl;
			std::cout << "pool message num£º" << MsgList.size() << std::endl;
		}
		SetEvent(tevent);
		Sleep(rand() % 1000);
	}
}

DWORD WINAPI BaseLabWin::CEventTest::consumer(LPVOID pParameter)
{
	std::list<Msg>::iterator it;
	Msg msgtmp;
	srand((unsigned)time(0));
	HANDLE tevent = ::OpenEvent(MUTEX_ALL_ACCESS, false, "event");
	while (TRUE)
	{
		if (!MsgList.empty())
		{
			::WaitForSingleObject(tevent, INFINITE);
			if (!MsgList.empty())
			{
				it = MsgList.begin();
				std::cout << "consume message :" << it->head << std::endl;
				std::cout << "pool message num:" << MsgList.size() << std::endl;
				MsgList.pop_front();
			}
			::SetEvent(tevent);
		}
		Sleep(rand() % 1000);
	}
}

