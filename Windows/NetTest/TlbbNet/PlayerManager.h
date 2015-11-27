#ifndef __PLAYERMANAGER_H__
#define __PLAYERMANAGER_H__

#include "net/Type.h"
#include "net/Socket.h"
#include "Player.h"


//管理一群玩家功能的基本模块,一般为每个场景创建一个PlayerManager
//并且在一个独立的线程内执行
class PlayerManager
{
public :
	PlayerManager( ) ;
	~PlayerManager( ) ;

	//清除当前模块内的数据信息
	VOID				CleanUp( ) ;

	//增加一个玩家
	BOOL				AddPlayer( Player* pPlayer ) ;
	BOOL				AddPlayer( Player* pPlayer,UINT MaxPlayer);

	//删除一个玩家
	VOID				RemovePlayer( PlayerID_t pid ) ;

	PlayerID_t*			GetPlayers( ){ return m_pPlayers ; } ;

	uint				GetPlayerNumber( ){ return m_nPlayers ; } ;
	BOOL				HasPlayer( ){ return (BOOL)m_nPlayers ; } ;

	enum
	{
		MAX_PLAYER = 10000,
	};

	PlayerID_t	m_pPlayers[MAX_PLAYER];
	uint		m_nPlayers ;
};

extern PlayerManager* g_pPlayerManager;

#endif
