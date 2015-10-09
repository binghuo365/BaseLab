#include "PlayerPool.h"
#include "Player.h"

PlayerPool* g_pPlayerPool=NULL ;

PlayerPool::PlayerPool( )
{
	m_pPlayer = NULL ;
	m_Position = 0 ;
	m_PlayerCount = 0 ;
	m_MaxPlayerCount = 0 ;
}

PlayerPool::~PlayerPool( ) 
{
	SAFE_DELETE_ARRAY( m_pPlayer ) ;
}

BOOL PlayerPool::Init( uint MaxPlayerCount )
{
	m_pPlayer = new Player[MaxPlayerCount] ;

	for( UINT i=0; i<MaxPlayerCount; i++ )
	{
		m_pPlayer[i].SetPlayerID( (PlayerID_t)i ) ;
		m_pPlayer[i].SetEmpty(TRUE) ;
	}
	m_Position = 0 ;
	m_PlayerCount = MaxPlayerCount ;
	m_MaxPlayerCount = MaxPlayerCount ;

	return TRUE ;
}



Player *PlayerPool::NewPlayer()
{
	AutoLock_T autolock(m_Lock);

	INT iRet = 0 ;
	for( UINT i=0; i<m_MaxPlayerCount; i++ )
	{
		if( m_pPlayer[m_Position].IsEmpty() )
		{
			iRet = m_Position ;
			m_pPlayer[m_Position].SetEmpty( FALSE ) ;

			m_Position ++ ;
			if( m_Position >= m_MaxPlayerCount ) 
				m_Position = 0 ;

			m_PlayerCount -- ;

			return &(m_pPlayer[iRet]) ;
		}

		m_Position ++ ;
		if( m_Position >= m_MaxPlayerCount ) 
			m_Position = 0 ;
	}

	return NULL ;
}

Player* PlayerPool::NewPlayer( PlayerID_t PlayerID)
{
	AutoLock_T autolock(m_Lock);

	if(PlayerID>=(PlayerID_t)m_MaxPlayerCount)
	{
		return NULL;
	}


	if(m_pPlayer[PlayerID].IsEmpty())
	{
		Player* pPlayer = &(m_pPlayer[PlayerID]);
		pPlayer->SetEmpty(FALSE);
		m_PlayerCount--;
		m_Position = PlayerID;
		m_Position ++ ;
		if( m_Position >= m_MaxPlayerCount ) 
			m_Position = 0 ;

		return pPlayer;

	}
	return NULL ;
}

VOID PlayerPool::DelPlayer( PlayerID_t PlayerID )
{
	AutoLock_T autolock(m_Lock);

	if( (uint)PlayerID >= m_MaxPlayerCount )
	{
		return ;
	}
	m_pPlayer[PlayerID].SetEmpty( TRUE ) ;
	m_PlayerCount ++ ;
	return ;
}

