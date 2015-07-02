#include "Mutex.h"
#include "PublicDef.h"
#include "time.h"
#include <iostream>

using namespace BaseLabWin;

std::list<Msg> CMutexTest::MsgList;
int CMutexTest::messageNum(0);
HANDLE CMutexTest::mutex = ::CreateMutex(NULL, FALSE, "mutex");

BaseLabWin::CMutexTest::CMutexTest()
{
	mutex = ::CreateMutex(NULL, FALSE, "mutex");
}

BaseLabWin::CMutexTest::~CMutexTest()
{
}

DWORD WINAPI BaseLabWin::CMutexTest::producer(LPVOID pParameter)
{
	Msg msgtmp;
	int i = 0;
	srand((unsigned)time(0));
	while (TRUE)
	{
		WaitForSingleObject(mutex, INFINITE);
		if (MsgList.size() < MaxQueSize)
		{
			msgtmp.head = ++messageNum;
			MsgList.push_back(msgtmp);
			std::cout << "Mutex product message :" << msgtmp.head << std::endl;
			std::cout << "Mutex pool message num£º" << MsgList.size() << std::endl;
		}
		ReleaseMutex(mutex);
		Sleep(rand() % 1000);
	}

}

DWORD WINAPI BaseLabWin::CMutexTest::consumer(LPVOID pParameter)
{
	std::list<Msg>::iterator it;
	Msg msgtmp;
	srand((unsigned)time(0));
	while (TRUE)
	{
		if (!MsgList.empty())
		{
			WaitForSingleObject(mutex, INFINITE);
			if (!MsgList.empty())
			{
				it = MsgList.begin();
				std::cout << "Mutex consume message :" << it->head << std::endl;
				std::cout << "Mutex pool message num:" << MsgList.size() << std::endl;
				MsgList.pop_front();
			}
			ReleaseMutex(mutex);
		}
		Sleep(rand() % 1000);
	}
}

