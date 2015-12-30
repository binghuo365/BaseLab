#include "acetrbnetacceptorimpl.h"
#include "netacceptor.h"
#include "acetrbnetconnectionimpl.h"
#include "netaddress.h"

using namespace CDENET;

CACETrbNetAcceptorImpl::CACETrbNetAcceptorImpl()
{
}

CACETrbNetAcceptorImpl::~CACETrbNetAcceptorImpl()
{
}

bool CACETrbNetAcceptorImpl::binding(const CDENET::SInetAddress& address)
{
	ACE_INET_Addr aceAddr;
	aceAddr.set(address.port, address.ip);
	return TRB_Asynch_Acceptor<CACETrbNetConnectionImpl>::open(
		aceAddr,
		0,
		1,
		ACE_DEFAULT_ASYNCH_BACKLOG,
		1,
		TRB_Proactor::instance(),
		1
		) != -1;
}

bool CACETrbNetAcceptorImpl::close()
{
	ACE_HANDLE handle_ = handle();
	if (ACE_INVALID_HANDLE != handle_)
	{
		handle(ACE_INVALID_HANDLE);
		return ACE_OS::closesocket(handle_) == 0;
	}
	return false;
}
int CACETrbNetAcceptorImpl::validate_connection(
	const TRB_Asynch_Accept::Result& result,
	const ACE_INET_Addr& remote,
	const ACE_INET_Addr& local)
{
	SInetAddress remote1;
	SInetAddress local1;

	local1.ip = local.get_ip_address();
	local1.port = local.get_port_number();

	remote1.ip = remote.get_ip_address();
	remote1.port = remote.get_port_number();
	INetAcceptor* acceptor = getAcceptor();
	if (!acceptor)
	{
		return -1;
	}
	//to check and block ip
	if (!acceptor->onCheckAddress(local1, remote1))
	{
		return -1;
	}
	return 0;
}
CACETrbNetConnectionImpl* CACETrbNetAcceptorImpl::make_handler()
{
	INetAcceptor* acceptor = getAcceptor();
	if (!acceptor)
	{
		return NULL;
	}
	CACETrbNetConnectionImpl* connImpl = new CACETrbNetConnectionImpl();
	INetConnection* conn = NULL;
	if (!acceptor->onMakeHandler(connImpl, conn))
	{
		return NULL;
	}
	conn->setClient(false);
	connImpl->setNetConnection(conn);
	return connImpl;
}

INetAcceptor* CACETrbNetAcceptorImpl::getAcceptor()
{
	return _acceptor;
}