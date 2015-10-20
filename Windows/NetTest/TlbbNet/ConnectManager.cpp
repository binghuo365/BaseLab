#include "ConnectManager.h"
#include "PlayerManager.h"
#include "PlayerPool.h"

ConnectManager* g_pConnectManager = NULL;

ConnectManager::ConnectManager( )
{

	if (!g_pConnectManager)
	{
		g_pConnectManager = new ConnectManager();
	}
	m_Active = FALSE ;
}

ConnectManager::~ConnectManager( )
{
	if (g_pConnectManager)
	{
		delete g_pConnectManager;
	}
}

BOOL ConnectManager::Init( )
{

	m_Active = TRUE ;

	INT LoginPort = 8888;

	m_pServerSocket = new ServerSocket(LoginPort);

	m_pServerSocket->setNonBlocking();

	m_SocketID = m_pServerSocket->getSOCKET();

	FD_SET(m_SocketID, &m_ReadFDs[SELECT_BAK]);
	FD_SET(m_SocketID, &m_ExceptFDs[SELECT_BAK]);

	m_MinFD = m_MaxFD = m_SocketID;

	m_Timeout[SELECT_BAK].tv_sec = 0;
	m_Timeout[SELECT_BAK].tv_usec = 0;

	return TRUE ;
}

VOID ConnectManager::run( )
{
	while( IsActive() )
	{
		BOOL ret = FALSE ;
		MySleep(100);

		_MY_TRY
		{
			Select( ) ;
			ProcessExceptions( ) ;
			ProcessInputs( ) ;
			ProcessOutputs( ) ;
		}
		_MY_CATCH
		{
		}

		_MY_TRY
		{
			ProcessCommands( ) ;
		}
		_MY_CATCH
		{
		}

		_MY_TRY
		{
			ProcessCacheCommands( ) ;
		}
		_MY_CATCH
		{
		}

		_MY_TRY
		{
			HeartBeat( ) ;
		}
		_MY_CATCH
		{
		}
	};

	Quit( ) ;

}

VOID ConnectManager::Quit( )
{
	RemoveAllPlayer( ) ;
}

BOOL ConnectManager::Select()
{
	m_Timeout[SELECT_USE].tv_sec = m_Timeout[SELECT_BAK].tv_sec;
	m_Timeout[SELECT_USE].tv_usec = m_Timeout[SELECT_BAK].tv_usec;

	m_ReadFDs[SELECT_USE] = m_ReadFDs[SELECT_BAK];
	m_WriteFDs[SELECT_USE] = m_WriteFDs[SELECT_BAK];
	m_ExceptFDs[SELECT_USE] = m_ExceptFDs[SELECT_BAK];

	MySleep(100);

	_MY_TRY
	{
		INT iRet = SocketAPI::select_ex((INT)m_MaxFD + 1,
		&m_ReadFDs[SELECT_USE],
		&m_WriteFDs[SELECT_USE],
		&m_ExceptFDs[SELECT_USE],
		&m_Timeout[SELECT_USE]);
	}
	_MY_CATCH
	{
	}

	return TRUE;
}

BOOL ConnectManager::ProcessInputs()
{
	BOOL ret = FALSE;

	if (m_MinFD == INVALID_SOCKET && m_MaxFD == INVALID_SOCKET) // no player exist
	{
		return TRUE;
	}

	//新连接接入：
	if (FD_ISSET(m_SocketID, &m_ReadFDs[SELECT_USE]))
	{
		for (INT i = 0; i < 50; i++)
		{
			if (!AcceptNewConnection())
				break;
		}
	}

	//数据读取
	uint nPlayerCount = g_pPlayerManager->GetPlayerNumber();
	for (uint i = 0; i < nPlayerCount; i++)
	{
		if (g_pPlayerManager->m_pPlayers[i] == INVALID_ID)
			continue;

		Player* pPlayer = g_pPlayerPool->GetPlayer(g_pPlayerManager->m_pPlayers[i]);

		SOCKET s = pPlayer->GetSocket()->getSOCKET();
		if (s == m_SocketID)
			continue;

		if (FD_ISSET(s, &m_ReadFDs[SELECT_USE]))
		{
			if (pPlayer->GetSocket()->isSockError())
			{//连接出现错误
				RemovePlayer(pPlayer);
			}
			else
			{//连接正常
				_MY_TRY
				{
				ret = pPlayer->ProcessInput();
				if (!ret)
				{
					RemovePlayer(pPlayer);
				}
			}
				_MY_CATCH
				{
					RemovePlayer(pPlayer);
				}
			}
		}
	}


	return TRUE;
}

BOOL ConnectManager::ProcessOutputs()
{

	BOOL ret = FALSE;

	if (m_MinFD == INVALID_SOCKET && m_MaxFD == INVALID_SOCKET) // no player exist
	{
		return TRUE;
	}

	//新连接接入：
	if (FD_ISSET(m_SocketID, &m_ReadFDs[SELECT_USE]))
	{
		for (INT i = 0; i < 10; i++)
		{
			if (!AcceptNewConnection())
				break;
		}
	}

	//数据读取
	uint nPlayerCount = g_pPlayerManager->GetPlayerNumber();
	for (uint i = 0; i < nPlayerCount; i++)
	{
		if (g_pPlayerManager->m_pPlayers[i] == INVALID_ID)
			continue;

		Player* pPlayer = g_pPlayerPool->GetPlayer(g_pPlayerManager->m_pPlayers[i]);

		SOCKET s = pPlayer->GetSocket()->getSOCKET();
		if (s == m_SocketID)
			continue;

		if (FD_ISSET(s, &m_ReadFDs[SELECT_USE]))
		{
			if (pPlayer->GetSocket()->isSockError())
			{//连接出现错误
				RemovePlayer(pPlayer);
			}
			else
			{//连接正常
				_MY_TRY
				{
				ret = pPlayer->ProcessInput();
				if (!ret)
				{
					RemovePlayer(pPlayer);
				}
			}
				_MY_CATCH
				{
					RemovePlayer(pPlayer);
				}
			}
		}
	}


	return TRUE;
}

BOOL ConnectManager::ProcessExceptions()
{
	if (m_MinFD == INVALID_SOCKET && m_MaxFD == INVALID_SOCKET) // no player exist
	{
		return TRUE;
	}

	uint nPlayerCount = g_pPlayerManager->GetPlayerNumber();
	for (uint i = 0; i < nPlayerCount; i++)
	{
		if (g_pPlayerManager->m_pPlayers[i] == INVALID_ID)
			continue;

		//某个玩家断开网络连接

		Player* pPlayer = g_pPlayerPool->GetPlayer(g_pPlayerManager->m_pPlayers[i]);

		SOCKET s = pPlayer->GetSocket()->getSOCKET();
		if (s == m_SocketID)
		{//侦听句柄出现问题，难。。。
			continue;
		}

		if (FD_ISSET(s, &m_ExceptFDs[SELECT_USE]))
		{
			RemovePlayer(pPlayer);
		}
	}

	return TRUE;
}

BOOL ConnectManager::ProcessCommands()
{

	BOOL ret;

	if (m_MinFD == INVALID_SOCKET && m_MaxFD == INVALID_SOCKET) // no player exist
	{
		return TRUE;
	}

	uint nPlayerCount = g_pPlayerManager->GetPlayerNumber();
	for (uint i = 0; i < nPlayerCount; i++)
	{
		if (g_pPlayerManager->m_pPlayers[i] == INVALID_ID)
			continue;

		Player* pPlayer = g_pPlayerPool->GetPlayer(g_pPlayerManager->m_pPlayers[i]);
		SOCKET s = pPlayer->GetSocket()->getSOCKET();
		if (s == m_SocketID)
			continue;

		if (pPlayer->GetSocket()->isSockError())
		{//连接出现错误
			RemovePlayer(pPlayer);
		}
		else
		{//连接正常
			_MY_TRY
			{
			ret = pPlayer->ProcessCommand(FALSE);
			if (!ret)
			{
				RemovePlayer(pPlayer);
			}
		}
			_MY_CATCH
			{
				RemovePlayer(pPlayer);
			}
		}
	}
	return TRUE;

}

BOOL ConnectManager::AcceptNewConnection()
{
	INT iStep = 0;
	BOOL ret = FALSE;

	//从玩家池中找出一个空闲的玩家数据集
	Player* client = g_pPlayerPool->NewPlayer();
	//	Assert( client ) ;
	if (client == NULL)
		return FALSE;

	iStep = 5;
	client->CleanUp();

	INT fd = INVALID_SOCKET;

	iStep = 10;
	_MY_TRY
	{
		//接受客户端接入Socket句柄
		ret = m_pServerSocket->accept(client->GetSocket());
		if (!ret)
		{
			client->CleanUp();
			g_pPlayerPool->DelPlayer(client->PlayerID());
			return FALSE;
		}
	}
	_MY_CATCH
	{
		iStep += 1000;
		goto EXCEPTION;
	}

	_MY_TRY
	{
		iStep = 30;
		fd = (INT)client->GetSocket()->getSOCKET();
		if (fd == INVALID_SOCKET)
		{
			goto EXCEPTION;
		}

		iStep = 40;
		ret = client->GetSocket()->setNonBlocking();
		if (!ret)
		{
			goto EXCEPTION;
		}

		iStep = 50;
		if (client->GetSocket()->getSockError())
		{
			goto EXCEPTION;
		}

		iStep = 60;
		ret = client->GetSocket()->setLinger(0);
		if (!ret)
		{
			goto EXCEPTION;
		}

		iStep = 70;
		//初始化基本玩家信息
		//client->Init();
		//设置当前客户端连接的状态
		//client->SetPlayerStatus(PS_LOGIN_CONNECT);
		//client->m_ConnectTime = g_pTimeManager->CurrentTime();

		iStep = 80;
		_MY_TRY
		{
			ret = AddPlayer(client);
			if (!ret)
			{
				goto EXCEPTION;
			}
		}
		_MY_CATCH
		{
			iStep += 10000;
			goto EXCEPTION;
		}
	}
	_MY_CATCH
	{
		iStep += 100000;
	}

	return TRUE;


EXCEPTION:
	//client->GetSocket()->getSOCKET(), client->GetSocket()->m_Host, iStep);

	client->CleanUp();
	g_pPlayerPool->DelPlayer(client->PlayerID());

	return FALSE;
}
