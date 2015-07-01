#include <queue>
#include <list>
#include<vector>
#include <algorithm>
#include <iostream>
#include <Windows.h>
#include "PublicDef.h"
#include "CriticalSection.h"

using namespace BaseLabWin;

void main()
{
	HANDLE producerThread[100];
	HANDLE consumeThread[100];
	for (int i = 0; i < 20; ++i)
	{
		producerThread[i] = ::CreateThread(NULL, 0, CCRITICAL_SECTION_TEST::producer, NULL, 0, NULL);
	}
	for (int i = 0; i < 21; ++i)
	{
		consumeThread[i] = ::CreateThread(NULL, 0, CCRITICAL_SECTION_TEST::consumer, NULL, 0, NULL);
	}
	::WaitForSingleObject(producerThread[0], INFINITE);
}
