#ifndef __PLAYERPOOL_H__
#define __PLAYERPOOL_H__

#include "net/Type.h"
#include "net/Lock.h"
#include "Player.h"

//分配玩家数据的数据池

class PlayerPool
{

	
public :
	PlayerPool( ) ;
	~PlayerPool( ) ;

	//初始化数据池
	BOOL				Init( uint MaxPlayerCount = 10000 ) ;
	
	//根据玩家的PlayerID取得数据指针
	Player*			GetPlayer( PlayerID_t PlayerID )
	{
		if( PlayerID==INVALID_ID || PlayerID>=(PlayerID_t)m_MaxPlayerCount || PlayerID<0 )
			return NULL ;
		return &(m_pPlayer[PlayerID]) ;
	};

	//从空闲数据池里分配一个玩家数据
	Player*			NewPlayer();
	Player*			NewPlayer(PlayerID_t PlayerID);
	//将一个玩家数据清空回收
	VOID				DelPlayer( PlayerID_t PlayerID ) ;

private :
	Player*		    m_pPlayer;
	uint				m_Position ;

	MyLock			m_Lock ;
	uint				m_PlayerCount ;
	uint				m_MaxPlayerCount ;

	VOID				SetPosition(uint pos);
};


extern PlayerPool* g_pPlayerPool ;


#endif
