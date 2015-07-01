#include "CriticalSection.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace BaseLabWin;

std::list<Msg> CCRITICAL_SECTION_TEST::MsgList;
int CCRITICAL_SECTION_TEST::messageNum(0);

CCRITICAL_SECTION::CCRITICAL_SECTION()
{
	//::InitializeCriticalSection(&_lock);
	::InitializeCriticalSectionAndSpinCount(&_lock, 10);	//×ÔÐýËø
}

CCRITICAL_SECTION::~CCRITICAL_SECTION()
{
	::DeleteCriticalSection(&_lock);
}

void CCRITICAL_SECTION::lock()
{
	::EnterCriticalSection(&_lock);
}

void CCRITICAL_SECTION::unlock()
{
	::LeaveCriticalSection(&_lock);
}

CCRITICAL_SECTION& CCRITICAL_SECTION::getInstance()
{
	static CCRITICAL_SECTION inst;
	return inst;
}

DWORD WINAPI CCRITICAL_SECTION_TEST::producer(LPVOID pParameter)
{
	Msg msgtmp;
	int i = 0;
	srand((unsigned)time(0));
	while (TRUE)
	{
		CCRITICAL_SECTION::getInstance().lock();
		if (MsgList.size() < MaxQueSize)
		{
			msgtmp.head = ++messageNum;
			MsgList.push_back(msgtmp);
			std::cout << "product message :" << msgtmp.head << std::endl;
			std::cout << "pool message num£º" << MsgList.size() << std::endl;
		}
		CCRITICAL_SECTION::getInstance().unlock();
		Sleep(rand()%1000);
	}
}

DWORD WINAPI CCRITICAL_SECTION_TEST::consumer(LPVOID pParameter)
{
	std::list<Msg>::iterator it;
	Msg msgtmp;
	srand((unsigned)time(0));
	while (TRUE)
	{
		if (!MsgList.empty())
		{
			CCRITICAL_SECTION::getInstance().lock();
			if (!MsgList.empty())
			{
				it = MsgList.begin();
				std::cout << "consume message :" << it->head << std::endl;
				std::cout << "pool message num:" << MsgList.size() << std::endl;
				MsgList.pop_front();
			}
			CCRITICAL_SECTION::getInstance().unlock();
		}
		Sleep(rand() % 1000);
	}
}
