#include "netacceptor.h"
#include "acetrbnetacceptorimpl.h"

using namespace CDENET;

INetAcceptor::INetAcceptor()
{
	_acceptorImpl = new CACETrbNetAcceptorImpl();
}

INetAcceptor::~INetAcceptor()
{
}

bool INetAcceptor::binding(const SInetAddress& address)
{
	return _acceptorImpl->binding(address);
}

bool INetAcceptor::close()
{
	return _acceptorImpl->close();
}
