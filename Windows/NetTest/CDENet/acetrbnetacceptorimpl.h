#ifndef _CDF_ACE_TRB_NET_ACCEPTOR_IMPL_H_
#define _CDF_ACE_TRB_NET_ACCEPTOR_IMPL_H_

#include "netacceptorimpl.h"
#include "netacceptor.h"
#include "acetrbnetconnectionimpl.h"

namespace CDENET
{
	typedef TRB_Asynch_Acceptor<CACETrbNetConnectionImpl> CTRBAsynchAcceptor;

	class CACETrbNetAcceptorImpl
		:public INetAcceptorImpl,
		public CTRBAsynchAcceptor
	{
	public:
		CACETrbNetAcceptorImpl();
		virtual ~CACETrbNetAcceptorImpl();
		virtual CACETrbNetConnectionImpl *make_handler(void);

		virtual bool binding(const CDENET::SInetAddress& address);
		virtual bool close();

		virtual int validate_connection(
			const TRB_Asynch_Accept::Result& result,
			const ACE_INET_Addr& remote,
			const ACE_INET_Addr& local);
	private:
		INetAcceptor* getAcceptor();
	};
}

#endif
