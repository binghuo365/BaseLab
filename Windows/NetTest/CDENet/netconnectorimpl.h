#ifndef _CDF_NET_CONNECTOR_IMPL_H_
#define _CDF_NET_CONNECTOR_IMPL_H_

#include "netaddress.h"

namespace CDENET
{
	class INetConnector;
	class INetConnectorImpl;

	class INetConnectorImpl
	{
	public:
		INetConnectorImpl();
		virtual ~INetConnectorImpl();

		//to open the connector
		virtual int open() = 0;

		virtual int connect(const SInetAddress& addr, void *act = NULL) = 0;

		void setConnector(INetConnector* connector);

	protected:
		INetConnector* _connector;
	};
}
#endif
