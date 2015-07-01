#include <queue>
#include <list>
#include<vector>
#include <algorithm>
#include <iostream>
#include <Windows.h>
#include "PublicDef.h"
#include "CriticalSection.h"
#include "Mutex.h"

using namespace BaseLabWin;

void main()
{
	CMutexTest mutesTest;

	HANDLE producerThread[100];
	HANDLE consumeThread[100];
	for (int i = 0; i < 20; ++i)
	{
		//1.利用临界区
		//producerThread[i] = ::CreateThread(NULL, 0, CCRITICAL_SECTION_TEST::producer, NULL, 0, NULL);

		//2.利用互斥量
		producerThread[i] = ::CreateThread(NULL, 0, CMutexTest::producer, NULL, 0, NULL);
	}
	for (int i = 0; i < 20; ++i)
	{
		//1.利用临界区
		//consumeThread[i] = ::CreateThread(NULL, 0, CCRITICAL_SECTION_TEST::consumer, NULL, 0, NULL);

		//2.利用互斥量
		consumeThread[i] = ::CreateThread(NULL, 0, CMutexTest::consumer, NULL, 0, NULL);
	}
	::WaitForSingleObject(producerThread[0], INFINITE);
}
