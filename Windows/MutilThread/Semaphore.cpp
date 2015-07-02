#include "Semaphore.h"
#include "time.h"
#include <iostream>

using namespace BaseLabWin;

std::list<Msg> CSemaphoreTest::MsgList;
int CSemaphoreTest::messageNum(0);

BaseLabWin::CSemaphoreTest::CSemaphoreTest()
{
	hsem = ::CreateSemaphore(NULL, 1, 1, "Sem");
}

BaseLabWin::CSemaphoreTest::~CSemaphoreTest()
{
	::CloseHandle(hsem);
}

DWORD WINAPI BaseLabWin::CSemaphoreTest::producer(LPVOID pParameter)
{
	Msg msgtmp;
	int i = 0;
	srand((unsigned)time(0));
	HANDLE thsem = ::OpenSemaphore(MUTEX_ALL_ACCESS, false, "Sem");
	while (TRUE)
	{
		::WaitForSingleObject(thsem, INFINITE);
		if (MsgList.size() < MaxQueSize)
		{
			msgtmp.head = ++messageNum;
			MsgList.push_back(msgtmp);
			std::cout << "product message :" << msgtmp.head << std::endl;
			std::cout << "pool message num£º" << MsgList.size() << std::endl;
		}
		::ReleaseSemaphore(thsem, 1, NULL);
		Sleep(rand() % 1000);
	}
}

DWORD WINAPI BaseLabWin::CSemaphoreTest::consumer(LPVOID pParameter)
{
	std::list<Msg>::iterator it;
	Msg msgtmp;
	srand((unsigned)time(0));
	HANDLE thsem = ::OpenSemaphore(MUTEX_ALL_ACCESS, false, "Sem");
	while (TRUE)
	{
		if (!MsgList.empty())
		{
			::WaitForSingleObject(thsem, INFINITE);
			if (!MsgList.empty())
			{
				it = MsgList.begin();
				std::cout << "consume message :" << it->head << std::endl;
				std::cout << "pool message num:" << MsgList.size() << std::endl;
				MsgList.pop_front();
			}
			::ReleaseSemaphore(thsem, 1, NULL);
		}
		Sleep(rand() % 1000);
	}
}

