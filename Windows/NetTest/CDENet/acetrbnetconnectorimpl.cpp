#include "acetrbnetconnectorimpl.h"
#include "netconnector.h"
#include "netaddress.h"

using namespace CDENET;

CACETrbNetConnectorImpl::CACETrbNetConnectorImpl()
{
}

CACETrbNetConnectorImpl::~CACETrbNetConnectorImpl()
{
}

int CACETrbNetConnectorImpl::open()
{
	return CTRBAsynchConnector::open(
		1, TRB_Proactor::instance(), 1
		);
}

int CACETrbNetConnectorImpl::connect(const SInetAddress& addr, void* act)
{
	ACE_INET_Addr aceAddr(addr.port, addr.ip);
	return CTRBAsynchConnector::connect(aceAddr, ACE_INET_Addr((u_short)0), 1, act);
}

CACETrbNetConnectionImpl *CACETrbNetConnectorImpl::make_handler(void)
{
	INetConnector* connector = getConnector();
	if (!connector)
	{
		return NULL;
	}
	CACETrbNetConnectionImpl* connImpl = new CACETrbNetConnectionImpl();
	INetConnection* conn = NULL;
	if (!connector->onMakeHandler(connImpl, conn))
	{
		return NULL;
	}
	connImpl->setNetConnection(conn);
	return connImpl;
}

int CACETrbNetConnectorImpl::validate_connection(
	const TRB_Asynch_Connect::Result& result,
	const ACE_INET_Addr& remote,
	const ACE_INET_Addr& local)
{
	INetConnector* connector = getConnector();
	if (!connector)
	{
		return -1;
	}

	if (!result.success())
	{
		connector->onConnectError(result.error(), result.act());
	}
	return 0;
}

INetConnector* CACETrbNetConnectorImpl::getConnector()
{
	return INetConnectorImpl::_connector;
}
