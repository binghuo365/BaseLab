#include "netconnection.h"

using namespace CDENET;

INetConnection::INetConnection()
:_isClient(false)
{
}

INetConnection::~INetConnection()
{
}

int INetConnection::close()
{
	INetConnectionImpl* connectionImpl = getConnectionImpl();
	if (connectionImpl)
	{
		connectionImpl->close();
		return 0;
	}
	return -1;
}

int INetConnection::close(const int interval)
{
	INetConnectionImpl* connectionImpl = getConnectionImpl();
	if (connectionImpl)
	{
		connectionImpl->close(interval);
		return 0;
	}
	return -1;
}


int INetConnection::sendData(const void * buf, size_t size, void* act)
{
	INetConnectionImpl* connectionImpl = getConnectionImpl();
	if (connectionImpl)
	{
		int ret = connectionImpl->sendData(buf, size, act);
		return ret;
	}
	return -1;
}

void INetConnection::address(SInetAddress& local, SInetAddress& remote)
{
	INetConnectionImpl* connectionImpl = getConnectionImpl();
	if (connectionImpl)
	{
		connectionImpl->address(local, remote);
	}
}

int
INetConnection::getMsgInSendCount()
{
	INetConnectionImpl* connectionImpl = getConnectionImpl();
	if (connectionImpl)
	{
		return connectionImpl->getMsgInSendCount();
	}
	return 0;
}

INetConnectionImpl* INetConnection::getConnectionImpl()
{
	return _connectionImpl;
}

void INetConnection::setConnectionImpl(INetConnectionImpl* connectionImpl)
{
	_connectionImpl = connectionImpl;
}

bool INetConnection::isClient() const
{
	return _isClient;
}

void INetConnection::setClient(bool isClient)
{
	_isClient = isClient;
}
