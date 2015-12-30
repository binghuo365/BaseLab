#include "netconnector.h"
#include "acetrbnetconnectorimpl.h"

using namespace CDENET;

INetConnector::INetConnector()
{
	_connectorImpl = new CACETrbNetConnectorImpl();
	_connectorImpl->setConnector(this);
}

INetConnector::~INetConnector()
{
}

int
INetConnector::open()
{
	INetConnectorImpl*connectorImpl = getConnectionImpl();
	return connectorImpl->open();
}

void
INetConnector::close()
{
	INetConnectorImpl* connectorImpl = getConnectionImpl();
	if (connectorImpl)
	{
		connectorImpl->setConnector(NULL);
	}
	_connectorImpl = NULL;
}

int INetConnector::connect(const SInetAddress& addr, void* act)
{
	INetConnectorImpl* connectorImpl = getConnectionImpl();
	return connectorImpl->connect(addr, act);
}

INetConnectorImpl* INetConnector::getConnectionImpl()
{
	return _connectorImpl;
}

void INetConnector::setConnectionImpl(INetConnectorImpl* connectionImpl)
{
	_connectorImpl = connectionImpl;
}



