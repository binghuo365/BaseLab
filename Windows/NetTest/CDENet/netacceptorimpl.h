#ifndef _CDF_NET_ACCEPTOR_IMPL_H_
#define _CDF_NET_ACCEPTOR_IMPL_H_

#include "netaddress.h"

namespace CDENET
{
	class INetAcceptor;

	class INetAcceptorImpl;

	class INetAcceptorImpl
	{
	public:
		INetAcceptorImpl();
		virtual ~INetAcceptorImpl();
		virtual bool binding(const CDENET::SInetAddress& address) = 0;
		virtual bool close() = 0;
		void setAcceptor(INetAcceptor* acceptor);

	protected:
		INetAcceptor* _acceptor;
	};
}

#endif
