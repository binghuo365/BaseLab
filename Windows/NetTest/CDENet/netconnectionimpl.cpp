#include "netconnectionimpl.h"
#include "netconnection.h"

using namespace CDENET;

INetConnectionImpl::INetConnectionImpl()
:_recvBuffSize(defaultRecvBuffSize), _nodelay(false),
_sendBufSize(defaultSendBuffSize), _logError(false)
{
}

INetConnectionImpl::~INetConnectionImpl()
{
}

/**
* to set the net connection
*/
void INetConnectionImpl::setNetConnection(INetConnection* netConnection)
{
	_connectionPtr = netConnection;
}

void INetConnectionImpl::setRecvBuffSize(int recvBuffSize)
{
	if (recvBuffSize > defaultRecvBuffSize)
	{
		_recvBuffSize = recvBuffSize;
	}
}

void INetConnectionImpl::setNodelay(bool noDelay)
{
	_nodelay = noDelay;
}

void INetConnectionImpl::setSendBuffSize(int sendBufSize)
{
	_sendBufSize = sendBufSize;
}

void INetConnectionImpl::setLogError(bool logError)
{
	_logError = logError;
}