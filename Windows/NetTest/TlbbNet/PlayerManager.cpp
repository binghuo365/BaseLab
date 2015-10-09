
#include "PlayerManager.h"
#include "Player.h"

PlayerManager* g_pPlayerManager = NULL;

PlayerManager::PlayerManager( )
{
	m_nPlayers = 0 ;

	for( INT i=0; i<MAX_PLAYER; i++ )
	{
		m_pPlayers[i] = INVALID_ID ;
	}
}

PlayerManager::~PlayerManager( )
{
	CleanUp( ) ;
}

VOID PlayerManager::CleanUp( )
{
	m_nPlayers = 0 ;
	for( INT i=0; i<MAX_PLAYER; i++ )
	{
		m_pPlayers[i] = INVALID_ID ;
	}
}

BOOL PlayerManager::AddPlayer( Player* pPlayer )
{
	if( m_pPlayers[m_nPlayers] == INVALID_ID )
	{
		m_pPlayers[m_nPlayers] = pPlayer->PlayerID() ;
		pPlayer->SetPlayerManagerID( m_nPlayers ) ;

		m_nPlayers ++ ;
	}

	return TRUE ;
}

BOOL PlayerManager::AddPlayer( Player* pPlayer,UINT MaxPlayer )
{
	if(m_nPlayers<MaxPlayer)
	{
		if( m_pPlayers[m_nPlayers] == INVALID_ID )
		{
			m_pPlayers[m_nPlayers] = pPlayer->PlayerID() ;
			pPlayer->SetPlayerManagerID( m_nPlayers ) ;

			m_nPlayers ++ ;
		}
		
		return TRUE;
	}
}

