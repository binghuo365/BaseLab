#include "acetrbnetconnectionimpl.h"
#include "netconnection.h"

using namespace CDENET;

CACETrbNetConnectionImpl::CACETrbNetConnectionImpl()
:_reader(NULL), _writer(NULL)
{
}

CACETrbNetConnectionImpl::~CACETrbNetConnectionImpl()
{
	delete _reader;
	delete _writer;
}

//to close handle only
void CACETrbNetConnectionImpl::close()
{
	bool closed = false;
	{
		if (this->handle() != ACE_INVALID_HANDLE)
		{
			if (_reader != NULL)
			{
				_reader->cancel();
			}
			if (_writer != NULL)
			{
				_writer->cancel();
			}
			ACE_OS::closesocket(this->handle());
			handle(ACE_INVALID_HANDLE);
			closed = true;
		}
	}
}

void CACETrbNetConnectionImpl::close(const int interval)
{
}

int CACETrbNetConnectionImpl::sendData(const void * buf, size_t size, void* act)
{
	{
		if (this->handle() == ACE_INVALID_HANDLE)
		{
			return -1;
		}
	}
	ACE_Message_Block * mb = NULL;
	ACE_NEW_RETURN(mb, ACE_Message_Block(size), -1);
	mb->reset();
	mb->copy((const char*)buf, size);
	mb->wr_ptr(mb->base() + mb->size());

	//ACE_Message_Block *mb = NULL ;
	//ACE_NEW_RETURN( mb , 
	//	ACE_Message_Block( (char*)buf , size ) ,
	//	-1
	//	);
	//if( mb == NULL )
	//{
	//	assert( false );
	//	return -1;
	//}
	//mb->length( size );

	{

		if (this->handle() == ACE_INVALID_HANDLE)
		{
			mb->release();
			return -1;
		}

		if (_writer == NULL)
		{
			mb->release();
			return -1;
		}
		if (0 != this->_writer->write_n(*mb, mb->length(), act))
		{
			mb->release();
			goto WRITE_STREAM_ERROR;
		}
		return 0;
	}

WRITE_STREAM_ERROR:
	INetConnection* conn = getNetConnction();
	if (conn)
	{
		conn->onClosed(true);
	}
	return -1;
}

void CACETrbNetConnectionImpl::address(SInetAddress& local, SInetAddress& remote)
{
	local.ip = _localAddress.get_ip_address();
	local.port = _localAddress.get_port_number();

	remote.ip = _remoteAddress.get_ip_address();
	remote.port = _remoteAddress.get_port_number();
}

void CACETrbNetConnectionImpl::open(
	ACE_HANDLE h,
	ACE_Message_Block& result
	)
{
	this->handle(h);

	_reader = new TRB_Asynch_Read_Stream();
	_writer = new TRB_Asynch_Write_Stream();

	CACETrbNetConnectionImpl* ptr(this);
	INetConnection* conn = getNetConnction();

	if (!conn)
	{
		close();
		return;
	}

	if (this->_reader->open(*this, handle(), 0, NULL) != 0)
	{
		conn->onError(-1);
		close();
		return;
	}
	if (_nodelay)
	{
		int flag = 1;
		setsockopt((SOCKET)h, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));

	}

	if (this->_writer->open(*this, handle(), 0, NULL) != 0)
	{
		conn->onError(-1);
		return;
	}

	{
		bool ret = false;
		ret = conn->onConnected();

		if (!ret)
		{
			close();
			_connectionPtr = NULL;
			//result.set_flags( faile );
			return;
		}

		ACE_Message_Block *mb;
		ACE_NEW(mb, ACE_Message_Block(_recvBuffSize));

		if (this->_reader->read(*mb, mb->space()) != 0)
		{
			mb->release();
			conn->onClosed();
			return;
		}
	}
}

void CACETrbNetConnectionImpl::addresses(
	const ACE_INET_Addr &remoteAddress,
	const ACE_INET_Addr &localAddress
	)
{
	_remoteAddress = remoteAddress;
	_localAddress = localAddress;
}

void CACETrbNetConnectionImpl::handle_read_stream(
	const TRB_Asynch_Read_Stream::Result &result
	)
{
	ACE_Message_Block &mb = result.message_block();
	INetConnection* conn = getNetConnction();
	//result.act();
	if (!conn)
	{
		mb.release();
		close();
		return;
	}
	CACETrbNetConnectionImpl* ptr(this);
	if (result.success() && result.bytes_transferred() != 0)
	{
		if (!conn->onRecv(mb.rd_ptr(), (size_t)result.bytes_transferred()))
		{
			goto READ_STREAM_ERROR;
		}
		mb.reset();
		{
			if (handle() == ACE_INVALID_HANDLE)
			{
				goto READ_STREAM_ERROR;
			}
			if (_reader == NULL)
			{
				goto READ_STREAM_ERROR;
			}

			if (0 != _reader->read(mb, mb.space()))
			{
				goto READ_STREAM_ERROR;
			}
			//thi count is about the begin read
		}
		return;
	}
READ_STREAM_ERROR:
	conn->onClosed();

	mb.release();
}

void CACETrbNetConnectionImpl::handle_write_stream(
	const TRB_Asynch_Write_Stream::Result &result
	)
{
	/**
	* the message count of the connection
	*/
	INetConnection* conn = getNetConnction();
	ACE_Message_Block &mb = result.message_block();
	mb.release();
	if (_logError &&
		(result.error() != 0 || result.bytes_transferred() == 0))
	{
		char remoteAddr[32];
		char localAddress[32];
		_remoteAddress.addr_to_string(remoteAddr, sizeof(remoteAddr)-1);      //remote address
		_localAddress.addr_to_string(localAddress, sizeof(localAddress)-1);   //local address
	}
	//if( conn )
	//{
	//	conn->onSend( count , 
	//		result.bytes_transferred() != 0 && result.success() , 
	//		result.act()
	//		);
	//}

	//assert( getRef() > 1 );
}

INetConnection* CACETrbNetConnectionImpl::getNetConnction()
{
	return _connectionPtr;
}
