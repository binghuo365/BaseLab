#include "PacketFactoryManager.h"
#include "PacketDefine.h"

using namespace Packets;


PacketFactoryManager* g_pPacketFactoryManager = NULL ;


PacketFactoryManager::PacketFactoryManager( ) 
{

	m_Factories = NULL ;
	m_Size = PACKET_MAX ;
	
	m_Factories = new PacketFactory*[ m_Size ];
	m_pPacketAllocCount = new UINT[m_Size] ;
	
	for( INT i=0; i<m_Size; i++ ) 
	{
		m_Factories[i] = NULL ;
		m_pPacketAllocCount[i] = 0 ;
	}
}

PacketFactoryManager::~PacketFactoryManager( ) 
{
	for( INT i=0; i<m_Size; i++ ) 
	{
		SAFE_DELETE(m_Factories[i]) ;
	}
	
	SAFE_DELETE_ARRAY(m_Factories) ;
	SAFE_DELETE_ARRAY(m_pPacketAllocCount) ;
}

BOOL PacketFactoryManager::Init( )
{

	//AddFactory( new WBRetValidateUserFactory()) ;

	return TRUE ;
}

VOID PacketFactoryManager::AddFactory( PacketFactory* pFactory ) 
{
		
	if( m_Factories[pFactory->GetPacketID()]!=NULL ) 
	{//重复设定
		return ;
	}
	
	m_Factories[pFactory->GetPacketID()] = pFactory ;
			
}

Packet* PacketFactoryManager::CreatePacket( PacketID_t packetID ) 
{

	if( packetID>=m_Size || m_Factories[packetID]==NULL ) 
	{
		return NULL ;
	}

	Packet* pPacket = NULL ;
	Lock() ;
	_MY_TRY
	{
		pPacket = m_Factories[packetID]->CreatePacket();
		m_pPacketAllocCount[packetID]++ ;
	}
	_MY_CATCH
	{
		pPacket = NULL ;
	}
	Unlock() ;

	return pPacket ;
}

UINT PacketFactoryManager::GetPacketMaxSize( PacketID_t packetID ) 
{
	if( packetID>=m_Size || m_Factories[packetID]==NULL ) 
	{
		char buff[256] = {0};
		sprintf(buff,"PacketID= %d 消息没有注册到PacketFactoryManager上",packetID);
		return 0 ;
	}

	Lock() ;
	UINT iRet = m_Factories[packetID]->GetPacketMaxSize( ) ;
	Unlock() ;

	return iRet ;
}

VOID PacketFactoryManager::RemovePacket( Packet* pPacket )
{
	if( pPacket==NULL )
	{
		return ;
	}

	PacketID_t packetID = pPacket->GetPacketID() ;
	if( packetID>=m_Size ) 
	{
		return ;
	}

	Lock() ;
	_MY_TRY
	{
		SAFE_DELETE( pPacket ) ;
		m_pPacketAllocCount[packetID] -- ;
	}
	_MY_CATCH
	{
	}
	Unlock() ;
	return ;
}



