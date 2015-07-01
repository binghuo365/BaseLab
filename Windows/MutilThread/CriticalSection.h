#ifndef _CRITICALSECTION_
#define _CRITICALSECTION_

#include "PublicDef.h"
#include <list>

namespace BaseLabWin
{
	class CCRITICAL_SECTION
	{
	public:
		CCRITICAL_SECTION();
		~CCRITICAL_SECTION();

		inline void lock();
		inline void unlock();

		static CCRITICAL_SECTION& getInstance();

	private:
		CRITICAL_SECTION _lock;
	};

	class CCRITICAL_SECTION_TEST
	{
	public:
		CCRITICAL_SECTION_TEST();
		~CCRITICAL_SECTION_TEST();

		static DWORD WINAPI producer(LPVOID pParameter);
		static DWORD WINAPI consumer(LPVOID pParameter);

	private:
		static std::list<Msg> MsgList;
		static int messageNum;
	};
}


#endif