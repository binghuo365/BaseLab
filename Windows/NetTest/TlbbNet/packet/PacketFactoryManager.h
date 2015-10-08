


#ifndef __PACKETFACTORYMANAGER_H__
#define __PACKETFACTORYMANAGER_H__

#include "../net/Type.h"
#include "../net/Packet.h"
#include "PacketFactory.h"
#include "../net/Lock.h"


class PacketFactoryManager 
{
public :
	
	PacketFactoryManager( ) ;
	
	~PacketFactoryManager( ) ;

private :
	VOID				AddFactory( PacketFactory* pFactory ) ;
	
public :
	//�ⲿ����ͨ�ýӿ�

	//��ʼ���ӿ�
	BOOL				Init( ) ;
	//������Ϣ���ʹ��ڴ��������Ϣʵ�����ݣ��������߳�ͬʱ���ã�
	Packet*				CreatePacket( PacketID_t packetID ) ;
	//������Ϣ����ȡ�ö�Ӧ��Ϣ�����ߴ磨�������߳�ͬʱ���ã�
	UINT				GetPacketMaxSize( PacketID_t packetID ) ;
	//ɾ����Ϣʵ�壨�������߳�ͬʱ���ã�
	VOID				RemovePacket( Packet* pPacket ) ;
	
	VOID				Lock( ){ m_Lock.Lock() ; } ;
	VOID				Unlock( ){ m_Lock.Unlock() ; } ;

private :
	
	PacketFactory **	m_Factories ;
	
	USHORT				m_Size ;

	MyLock				m_Lock ;

public :
	UINT*				m_pPacketAllocCount ;

};

extern PacketFactoryManager* g_pPacketFactoryManager ;







#endif