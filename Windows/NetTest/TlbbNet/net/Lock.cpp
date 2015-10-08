#include "Type.h"
#include "Lock.h"

#ifdef __LINUX__
#include <execinfo.h>
#include <signal.h>
#include <exception>
#include <setjmp.h>
#endif

INT					g_CmdArgv;
INT					g_LockTimes=0 ;
BOOL				g_LockTimeOutEnable=FALSE;


VOID	sm_lock(CHAR& flag,CHAR type)
{
	bgsm_loop:
	if(flag==SM_FREE)
	{
		flag=type;
		if(flag!=type)
		{
			MySleep(1);
#ifdef __LINUX__
			g_LockTimes ++ ;
			printf("lock fail %s,%d,%s",__FILE__,__LINE__,__PRETTY_FUNCTION__);
#endif
			if((g_LockTimes>100)&&g_LockTimeOutEnable)
			{
				g_LockTimes =0;
				return;
			}
			goto bgsm_loop;
		}
		return;
	}
	else
	{
		MySleep(1);
		goto	bgsm_loop;
	}
}



VOID	sm_unlock(CHAR& flag,CHAR type)
{
	edsm_loop:
		
		if(flag==SM_FREE)
		{
			return;
		}
		
		flag=SM_FREE;
	
		if(flag!=SM_FREE)
		{
			MySleep(1);
#ifdef __LINUX__
			printf("unlock fail %s,%s,%s",__FILE__,__LINE__,__PRETTY_FUNCTION__);
			if((g_LockTimes>100)&&g_LockTimeOutEnable)
			{
				g_LockTimes =0;
				return;
			}
#endif
			goto edsm_loop;
		}
		return;
	
}

BOOL sm_trylock(CHAR& flag,CHAR type)
{
	INT LockTimes;

bgsm_loop:

	if(flag==SM_FREE)
	{
		flag=type;
		if(flag!=type)
		{
			LockTimes++;
			MySleep(1);
			if(LockTimes>10)
			{
				return FALSE;
			}
			goto bgsm_loop;
		}
		return TRUE;
	}
	else
	{
		LockTimes++;
		MySleep(1);
		if(LockTimes>10)
		{
			return FALSE;
		}
		goto	bgsm_loop;
	}
}




VOID MySleep( UINT millionseconds )
{
#if defined(__WINDOWS__)
	Sleep( millionseconds ) ;
#elif defined(__LINUX__)
	usleep( millionseconds*1000 ) ;
#endif
}

TID MyGetCurrentThreadID( )
{
#if defined(__WINDOWS__)
	return GetCurrentThreadId( ) ;
#elif defined(__LINUX__)
	return pthread_self();
#endif
}
