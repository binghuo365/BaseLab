#include "netconnectorimpl.h"
#include "netconnector.h"

using namespace CDENET;

INetConnectorImpl::INetConnectorImpl()
{
}

INetConnectorImpl::~INetConnectorImpl()
{
}

void INetConnectorImpl::setConnector(INetConnector* connector)
{
	_connector = connector;
}
