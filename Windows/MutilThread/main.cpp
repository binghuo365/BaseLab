#include <queue>
#include <list>
#include<vector>
#include <algorithm>
#include <iostream>
#include <Windows.h>
using namespace std;

DWORD WINAPI producer(LPVOID n);//生产者
DWORD WINAPI consumer(LPVOID lpParaneter);//消费者
struct Msg
{
	HANDLE id;
	int type;
	char words[BUFSIZ];
};
#define MaxQueSize  100
list<Msg> MsgList;
CRITICAL_SECTION g_cs;
int m_count = 0;


void main()
{
	//初始化临界资源
	InitializeCriticalSection(&g_cs);
	HANDLE producerThread[20];
	HANDLE consumeThread;
	for (int i = 0; i < 20; ++i)
	{
		producerThread[i] = CreateThread(NULL, 0, producer, NULL, 0, NULL);
	}
	consumeThread = CreateThread(NULL, 0, consumer, NULL, 0, NULL);
	while (TRUE)
	{
	}
	DeleteCriticalSection(&g_cs);
}
DWORD WINAPI producer(LPVOID lpParaneter)
{
	Msg msgtmp;
	int i = 0;
	while (TRUE)
	{

		EnterCriticalSection(&g_cs);//进入临界资源
		msgtmp.type = 1;
		char temp[1024];
		sprintf_s(temp, "%d", ++m_count);
		strcpy_s(msgtmp.words, "message count :");
		strcat_s(msgtmp.words, temp);
		if (MsgList.size() < MaxQueSize)
		{
			MsgList.push_back(msgtmp);
			std::cout << "消息队列消息数：" << MsgList.size() << std::endl;
		}
		LeaveCriticalSection(&g_cs);//退出临界资源
		Sleep(300);
	}

	return 0;
}

DWORD WINAPI consumer(LPVOID lpParaneter)
{
	list<Msg>::iterator i, end;
	Msg msgtmp;
	while (TRUE)
	{
		while (MsgList.empty() == 0)
		{
			i = MsgList.begin();
			cout << (*i).type << (*i).words << endl;
			EnterCriticalSection(&g_cs);
			MsgList.pop_front();//删除对头
			LeaveCriticalSection(&g_cs);
			Sleep(100);
		}
		cout << "消息队列为空" << endl;
		Sleep(30);
	}
	return 0;
}