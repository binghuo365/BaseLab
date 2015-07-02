#include "Semaphore.h"
#include "time.h"
#include <iostream>

using namespace BaseLabWin;

std::list<Msg> CSemaphoreTest::MsgList;
int CSemaphoreTest::messageNum(0);
HANDLE CSemaphoreTest::hsem = ::CreateSemaphore(NULL, 1, 1, "Sem");
BaseLabWin::CSemaphoreTest::CSemaphoreTest()
{
	
}

BaseLabWin::CSemaphoreTest::~CSemaphoreTest()
{
}

DWORD WINAPI BaseLabWin::CSemaphoreTest::producer(LPVOID pParameter)
{
	Msg msgtmp;
	int i = 0;
	srand((unsigned)time(0));
	while (TRUE)
	{
		::WaitForSingleObject(hsem, INFINITE);
		if (MsgList.size() < MaxQueSize)
		{
			msgtmp.head = ++messageNum;
			MsgList.push_back(msgtmp);
			std::cout << "Semaphore product message :" << msgtmp.head << std::endl;
			std::cout << "Semaphore pool message num£º" << MsgList.size() << std::endl;
		}
		::ReleaseSemaphore(hsem, 1, NULL);
		Sleep(rand() % 1000);
	}
}

DWORD WINAPI BaseLabWin::CSemaphoreTest::consumer(LPVOID pParameter)
{
	std::list<Msg>::iterator it;
	Msg msgtmp;
	srand((unsigned)time(0));
	while (TRUE)
	{
		if (!MsgList.empty())
		{
			::WaitForSingleObject(hsem, INFINITE);
			if (!MsgList.empty())
			{
				it = MsgList.begin();
				std::cout << "Semaphore consume message :" << it->head << std::endl;
				std::cout << "Semaphore pool message num:" << MsgList.size() << std::endl;
				MsgList.pop_front();
			}
			::ReleaseSemaphore(hsem, 1, NULL);
		}
		Sleep(rand() % 1000);
	}
}

