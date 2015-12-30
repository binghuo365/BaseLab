#include "netacceptorimpl.h"
#include "netacceptor.h"

using namespace CDENET;

INetAcceptorImpl::INetAcceptorImpl()
{
};

INetAcceptorImpl::~INetAcceptorImpl()
{
}

void INetAcceptorImpl::setAcceptor(INetAcceptor* acceptor)
{
	_acceptor = acceptor;
}
