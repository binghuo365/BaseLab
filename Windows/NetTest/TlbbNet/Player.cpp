#include "Player.h"
#include "net/Socket.h"
#include "packet/PacketDefine.h"
#include "packet/PacketFactoryManager.h"

using namespace Packets;

Player::Player( BOOL bIsServer )
{

	m_PID = INVALID_ID ;
	m_UID = INVALID_ID ;
	m_PlayerManagerID = INVALID_ID ;


	m_pSocket = new Socket ;

	if( bIsServer )
	{
		m_pSocketInputStream = new SocketInputStream( m_pSocket,DEFAULTSOCKETINPUTBUFFERSIZE,64*1024*1024 ) ;

		m_pSocketOutputStream = new SocketOutputStream( m_pSocket,DEFAULTSOCKETOUTPUTBUFFERSIZE,64*1024*1024 ) ;
	}
	else
	{
		m_pSocketInputStream = new SocketInputStream( m_pSocket ) ;

		m_pSocketOutputStream = new SocketOutputStream( m_pSocket ) ;
	}

	m_IsEmpty		= TRUE ;
	m_IsDisconnect	= FALSE ;

	m_PacketIndex	 = 0 ;

}

Player::~Player( )
{

	SAFE_DELETE( m_pSocketInputStream ) ;
	SAFE_DELETE( m_pSocketOutputStream ) ;

	SAFE_DELETE( m_pSocket ) ;

}

VOID Player::CleanUp( )
{
	m_pSocket->close() ;
	m_pSocketInputStream->CleanUp() ;
	m_pSocketOutputStream->CleanUp() ;
	SetPlayerManagerID( INVALID_ID ) ;
	SetUserID( INVALID_ID ) ;
	m_PacketIndex = 0 ;
	SetDisconnect(FALSE) ;
}

VOID Player::Disconnect( )
{
	_MY_TRY
	{
		m_pSocket->close() ;
	}
	_MY_CATCH
	{
	}
}

BOOL Player::IsValid( )
{

	if( m_pSocket==NULL ) return FALSE ;

	if( !m_pSocket->isValid() ) return FALSE ;


	return TRUE ;

}

BOOL Player::ProcessInput( )
{

	if( IsDisconnect() )
		return TRUE ;

	_MY_TRY 
	{
		uint ret = m_pSocketInputStream->Fill( ) ;
		if( (INT)ret <= SOCKET_ERROR )
		{
			return FALSE ;
		}
	} 
	_MY_CATCH
	{
		return FALSE ;
	}


	return TRUE ;

}

BOOL Player::ProcessCommand( BOOL Option )
{

	BOOL ret ;

	CHAR header[PACKET_HEADER_SIZE];
	PacketID_t packetID;
	uint packetuint, packetSize, packetIndex;
	WORD packetTick;
	Packet* pPacket = NULL ;

	if( IsDisconnect( ) )
		return TRUE ;

	_MY_TRY
	{
		if( Option ) 
		{//ִ�в���ѡ�����
		}

		for( ;; )
		{
			if( !m_pSocketInputStream->Peek(&header[0], PACKET_HEADER_SIZE) )
			{//���ݲ��������Ϣͷ
				break ;
			}

			this->DecryptHead_CS(header);//���ͷ����

			memcpy( &packetID, &header[0], sizeof(PacketID_t) ) ;	
			memcpy( &packetTick, &header[sizeof(WORD)], sizeof(WORD) ) ;
			memcpy( &packetuint, &header[sizeof(WORD)+sizeof(PacketID_t)], sizeof(uint) );
			packetSize = GET_PACKET_LEN(packetuint) ;
			packetIndex = GET_PACKET_INDEX(packetuint) ;

			if( packetID >= (PacketID_t)PACKET_MAX )
			{//��Ч����Ϣ����
				return FALSE ;
			}

			//��Ϣ���ܴ���--Begin
			{
				UINT t_uSize = packetSize+PACKET_HEADER_SIZE;
				UINT t_uHead = m_pSocketInputStream->GetHead();
				UINT t_uTail = m_pSocketInputStream->GetTail();
				UINT t_uBufferLen = m_pSocketInputStream->GetBuffLen();
				CHAR* t_szBuffer = m_pSocketInputStream->GetBuff();
				if ( t_uHead < t_uTail ) 
				{
					this->Decrypt_CS(&t_szBuffer[t_uHead], t_uSize, 0);
				} 
				else 
				{
					UINT rightLen = t_uBufferLen - t_uHead ;
					if( t_uSize <= rightLen ) 
					{
						this->Decrypt_CS(&t_szBuffer[t_uHead], t_uSize, 0);
					} 
					else 
					{
						this->Decrypt_CS(&t_szBuffer[t_uHead], rightLen, 0);
						this->Decrypt_CS(t_szBuffer, t_uSize-rightLen, rightLen);
					}
				}
			}
			//��Ϣ���ܴ���--End

			_MY_TRY
			{

				if( m_pSocketInputStream->Length()<PACKET_HEADER_SIZE+packetSize )
				{//��Ϣû�н���ȫ
					break;
				}

				if( packetSize>g_pPacketFactoryManager->GetPacketMaxSize(packetID) )
				{
					//��Ϣ�Ĵ�С�����쳣���յ�����Ϣ��Ԥ������Ϣ�����ֵ��Ҫ��
//					m_pSocketInputStream->Skip( PACKET_HEADER_SIZE+packetSize ) ;
					return FALSE ;
				}

				Packet* pPacket = g_pPacketFactoryManager->CreatePacket( packetID ) ;
				if( pPacket==NULL )
				{//���ܷ��䵽�㹻���ڴ�
//					m_pSocketInputStream->Skip( PACKET_HEADER_SIZE+packetSize ) ;
					return FALSE ;
				}

				//������Ϣ���к�
				pPacket->SetPacketIndex( packetIndex ) ;

				ret = m_pSocketInputStream->ReadPacket( pPacket ) ;
				if( ret==FALSE )
				{//��ȡ��Ϣ���ݴ���
					g_pPacketFactoryManager->RemovePacket( pPacket ) ;
					return FALSE ;
				}

				BOOL bNeedRemove = TRUE ;

				_MY_TRY
				{
					//����m_KickTime��Ϣ��m_KickTime��Ϣ�е�ֵΪ�ж��Ƿ���Ҫ�ߵ�
					//�ͻ��˵�����
					this->ResetKick( ) ;

					uint uret = pPacket->Execute( this ) ;
					if( uret==PACKET_EXE_ERROR )
					{//�����쳣���󣬶Ͽ����������
						if( pPacket ) 
							g_pPacketFactoryManager->RemovePacket( pPacket ) ;
						return FALSE ;
					}
					else if( uret==PACKET_EXE_BREAK )
					{//��ǰ��Ϣ�Ľ���ִ�н�ֹͣ
					 //ֱ���¸�ѭ��ʱ�ż����Ի����е����ݽ�����Ϣ��ʽ
					 //����ִ�С�
					 //����Ҫ���ͻ��˵�ִ�д�һ������ת�Ƶ�����һ������ʱ��
					 //��Ҫ�ڷ���ת����Ϣ��ִ���ڱ��߳���ֹͣ��
						if( pPacket ) 
							g_pPacketFactoryManager->RemovePacket( pPacket ) ;
						break ;
					}
					else if( uret==PACKET_EXE_CONTINUE )
					{//��������ʣ�µ���Ϣ
					}
					else if( uret==PACKET_EXE_NOTREMOVE )
					{//��������ʣ�µ���Ϣ�����Ҳ����յ�ǰ��Ϣ
						bNeedRemove = FALSE ;
					}
					else if( uret == PACKET_EXE_NOTREMOVE_ERROR )
					{
						return FALSE ;
					}
				}
				_MY_CATCH
				{
				}

				if( pPacket && bNeedRemove ) 
					g_pPacketFactoryManager->RemovePacket( pPacket ) ;
			}
			_MY_CATCH
			{
				return FALSE;
			}
		}
	}
	_MY_CATCH
	{
	}



	return TRUE ;

}

BOOL Player::ProcessOutput( )
{

	if( IsDisconnect( ) )
		return TRUE ;

	_MY_TRY
	{
		uint size = m_pSocketOutputStream->Length() ;
		if( size==0 )
		{
			return TRUE ;
		}

		uint ret = m_pSocketOutputStream->Flush( ) ;
		if( (INT)ret <= SOCKET_ERROR )
		{
			return FALSE ;
		}
	} 
	_MY_CATCH
	{
		return FALSE ;
	}

	return TRUE ;

}


BOOL Player::SendPacket( Packet* pPacket )
{

	if( IsDisconnect( ) )
		return TRUE ;

	if( m_pSocketOutputStream!=NULL )
	{
		pPacket->SetPacketIndex( m_PacketIndex++ ) ;

		PacketID_t packetID = pPacket->GetPacketID() ;
		UINT w;
		if ( packetID < PACKET_LC_MAXCOUNT)
		{
			w = m_pSocketOutputStream->Write( PACK_COMPART , PACK_COMPART_SIZE ) ;
		}

		UINT t_uTail_Begin = m_pSocketOutputStream->GetTail();//��ѯ��ǰ��βλ�á���¼д��ǰλ��

		w = m_pSocketOutputStream->Write( (CHAR*)&packetID , sizeof(PacketID_t) ) ;

		WORD packetTick = 0;//g_pTimeManager->RunTick();
		w = m_pSocketOutputStream->Write( (CHAR*)&packetTick , sizeof(WORD) ) ;

		UINT packetUINT ;
		UINT packetSize = pPacket->GetPacketSize( ) ;
		UINT packetIndex = pPacket->GetPacketIndex( ) ;

		SET_PACKET_INDEX(packetUINT, packetIndex) ;
		SET_PACKET_LEN(packetUINT, packetSize) ;

		w = m_pSocketOutputStream->Write( (CHAR*)&packetUINT, sizeof(UINT) ) ;

		BOOL ret = pPacket->Write( *m_pSocketOutputStream ) ;

		UINT t_uTail_End = m_pSocketOutputStream->GetTail();//��ѯ��ǰ��βλ�á���¼д����λ��

		//��Ϣ���ܴ���--Begin
		{
			UINT t_uSize = t_uTail_End - t_uTail_Begin;
			UINT t_uHead = m_pSocketOutputStream->GetHead();
			UINT t_uTail = m_pSocketOutputStream->GetTail();
			UINT t_uBufferLen = m_pSocketOutputStream->GetBuffLen();
			CHAR* t_szBuffer = m_pSocketOutputStream->GetBuff();
			if ( t_uHead < t_uTail ) 
			{
				this->Encrypt_SC(&(t_szBuffer[t_uTail_Begin]), t_uSize, 0);
			} 
			else
			{
				UINT rightLen = t_uBufferLen - t_uHead ;
				if( t_uSize <= rightLen ) 
				{
					this->Encrypt_SC(&(t_szBuffer[t_uTail_Begin]), t_uSize, 0);
				} 
				else 
				{
					this->Encrypt_SC(&(t_szBuffer[t_uTail_Begin]), rightLen, 0);
					this->Encrypt_SC(t_szBuffer, t_uSize-rightLen, rightLen);
				}
			}
		}
		//��Ϣ���ܴ���--End
	}

	return TRUE ;

}

VOID Player::Encrypt_SC(CHAR* header, UINT uLen, UINT uBeginPlace)
{
	//todo
}

VOID Player::DecryptHead_CS(CHAR* header)
{
	//todo
}

VOID Player::Decrypt_CS(CHAR* header, UINT uLen, UINT uBeginPlace)
{
	//todo
}

BOOL Player::IsLoginPlayer()
{
	return TRUE;
	//todo
}

BOOL Player::IsServerPlayer()
{
	return TRUE;
	//todo
}
