#ifndef _CDF_NET_ACCEPTOR_H_
#define _CDF_NET_ACCEPTOR_H_

#include "netaddress.h"
#include "netacceptorimpl.h"
#include "netconnection.h"

namespace CDENET
{
	class INetAcceptor
	{
	public:
		INetAcceptor();
		virtual ~INetAcceptor();
		virtual bool binding(const CDENET::SInetAddress& address);
		virtual bool close();
		virtual bool onMakeHandler(
			const CDENET::INetConnectionImpl* connectionImpl,
			CDENET::INetConnection* connection
			) = 0;
		virtual bool onCheckAddress(
			const CDENET::SInetAddress& local,
			const CDENET::SInetAddress& remote
			) = 0;
		virtual bool syncServerInfo() = 0;
	private:
		CDENET::INetAcceptorImpl* _acceptorImpl;
	};
}

#endif