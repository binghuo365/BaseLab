#ifndef _CDF_ACE_TRB_NET_CONNECTION_IMPL_H_
#define _CDF_ACE_TRB_NET_CONNECTION_IMPL_H_

#include "TProactor/Asynch_Acceptor.h"
#include "TProactor/Asynch_Connector.h"
#include "TProactor/Asynch_Connector2.h"
#include "TProactor/Proactor.h"
#include "TProactor/Asynch_IO.h"

#include "netconnectionimpl.h"

namespace CDENET
{
	class CACETrbNetConnectionImpl;

	//typedef TRB_Service_Handler_T<ACE_INET_Addr> CTRBServiceHandlerT;

	class CACETrbNetConnectionImpl
		:public INetConnectionImpl,
		public TRB_Service_Handler
	{
	public:
		typedef ACE_INET_Addr ADDRESS;
		CACETrbNetConnectionImpl();
		virtual ~CACETrbNetConnectionImpl();

		/**
		* to close connection
		*/
		virtual void close();

		/**
		* set send and close
		*/
		virtual void close(const int interval);

		/**
		* to send and close
		*/
		virtual int sendData(const void * buf, size_t size, void* act);


		/**
		* to return address
		*/
		virtual void address(
			SInetAddress& local,
			SInetAddress& remote
			);

	public:
		//open handle
		virtual void open(
			ACE_HANDLE h,
			ACE_Message_Block& result
			);

		//addresses
		virtual void addresses(
			const ACE_INET_Addr &remoteAddress,
			const ACE_INET_Addr &localAddress
			);

		//read stream
		virtual void handle_read_stream(
			const TRB_Asynch_Read_Stream::Result &result
			);

		//write stream
		virtual void handle_write_stream(
			const TRB_Asynch_Write_Stream::Result &result
			);

	protected:
		//asynch read
		TRB_Asynch_Read_Stream *_reader;
		//asynch write
		TRB_Asynch_Write_Stream *_writer;

		//CLightLock _lock;
	protected:
		ACE_INET_Addr _remoteAddress;		//remote address
		ACE_INET_Addr _localAddress;		//local address

		INetConnection* getNetConnction();
	};
}

#endif
