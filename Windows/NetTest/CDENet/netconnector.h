#ifndef _CDF_NET_CONNECTOR_H_
#define _CDF_NET_CONNECTOR_H_

#include "netconnection.h"
#include "netconnectorimpl.h"

namespace CDENET
{
	class INetConnector
	{
	public:
		INetConnector();
		virtual ~INetConnector();

		//to open the connector
		virtual int open();

		//to close the connector
		virtual void close();

		//to connect to the server
		virtual int connect(const CDENET::SInetAddress& addr, void* act = NULL);

		//to make handler
		virtual bool onMakeHandler(
			const CDENET::INetConnectionImpl* connectionImpl,
			CDENET::INetConnection* connection
			) = 0;

		virtual void onConnectError(int code, const void* act) = 0;

		virtual void onConnectSuccess(const void* act) = 0;


		INetConnectorImpl* getConnectionImpl();

		void setConnectionImpl(CDENET::INetConnectorImpl* connectionImpl);

	private:
		INetConnectorImpl* _connectorImpl;
	};
}

#endif
