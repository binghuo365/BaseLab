#ifndef _CDF_ACE_TRB_NET_CONNECTOR_H_
#define _CDF_ACE_TRB_NET_CONNECTOR_H_

#include "netconnectorimpl.h"
#include "acetrbnetconnectionimpl.h"

namespace CDENET
{
	typedef TRB_Asynch_Connector2<CACETrbNetConnectionImpl> CTRBAsynchConnector;

	class CACETrbNetConnectorImpl
		:public INetConnectorImpl,
		public CTRBAsynchConnector
	{
	public:
		CACETrbNetConnectorImpl();
		virtual ~CACETrbNetConnectorImpl();
		virtual CACETrbNetConnectionImpl *make_handler(void);

		virtual int validate_connection(
			const TRB_Asynch_Connect::Result& result,
			const ACE_INET_Addr& remote,
			const ACE_INET_Addr& local);

		//to open the connector
		virtual int open();


		//to connect to the server
		virtual int connect(const SInetAddress& addr, void *act = NULL);

		//to get the netconnector
		virtual INetConnector* getConnector();
	};
}
#endif