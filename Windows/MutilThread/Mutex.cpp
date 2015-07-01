#include "Mutex.h"
#include "PublicDef.h"
#include "time.h"
#include <iostream>

using namespace BaseLabWin;

std::list<Msg> CMutexTest::MsgList;
int CMutexTest::messageNum(0);

BaseLabWin::CMutexTest::CMutexTest()
{
	mutex = CreateMutex(NULL, FALSE, "mutex");
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
		HANDLE hCounterIn = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "mutex");
		WaitForSingleObject(hCounterIn, INFINITE);
		if (MsgList.size() < MaxQueSize)
		{
			msgtmp.head = ++messageNum;
			MsgList.push_back(msgtmp);
			std::cout << "product message :" << msgtmp.head << std::endl;
			std::cout << "pool message num£º" << MsgList.size() << std::endl;
		}
		ReleaseMutex(hCounterIn);
		CloseHandle(hCounterIn);
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
			HANDLE hCounterIn = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "mutex");
			WaitForSingleObject(hCounterIn, INFINITE);
			if (!MsgList.empty())
			{
				it = MsgList.begin();
				std::cout << "consume message :" << it->head << std::endl;
				std::cout << "pool message num:" << MsgList.size() << std::endl;
				MsgList.pop_front();
			}
			ReleaseMutex(hCounterIn);
			CloseHandle(hCounterIn);
		}
		Sleep(rand() % 1000);
	}
}

