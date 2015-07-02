#include "Event.h"
#include "time.h"
#include <iostream>

using namespace BaseLabWin;

std::list<Msg> CEventTest::MsgList;
int CEventTest::messageNum(0);
HANDLE CEventTest::event = ::CreateEvent(NULL, false, true, "event");

BaseLabWin::CEventTest::CEventTest()
{
}

BaseLabWin::CEventTest::~CEventTest()
{
}

DWORD WINAPI BaseLabWin::CEventTest::producer(LPVOID pParameter)
{
	Msg msgtmp;
	int i = 0;
	srand((unsigned)time(0));
	while (TRUE)
	{
		::WaitForSingleObject(event, INFINITE);
		if (MsgList.size() < MaxQueSize)
		{
			msgtmp.head = ++messageNum;
			MsgList.push_back(msgtmp);
			std::cout << "Event product message :" << msgtmp.head << std::endl;
			std::cout << "Event pool message num£º" << MsgList.size() << std::endl;
		}
		SetEvent(event);
		Sleep(rand() % 1000);
	}
}

DWORD WINAPI BaseLabWin::CEventTest::consumer(LPVOID pParameter)
{
	std::list<Msg>::iterator it;
	Msg msgtmp;
	srand((unsigned)time(0));
	while (TRUE)
	{
		if (!MsgList.empty())
		{
			::WaitForSingleObject(event, INFINITE);
			if (!MsgList.empty())
			{
				it = MsgList.begin();
				std::cout << "Event consume message :" << it->head << std::endl;
				std::cout << "Event pool message num:" << MsgList.size() << std::endl;
				MsgList.pop_front();
			}
			::SetEvent(event);
		}
		Sleep(rand() % 1000);
	}
}

