#ifndef _CDF_NET_CONNECTION_IMPL_H_
#define _CDF_NET_CONNECTION_IMPL_H_

#include "netaddress.h"

namespace CDENET
{
	class INetConnection;

	class INetConnectionImpl;

	class INetConnectionImpl
	{
	public:
		static const int defaultRecvBuffSize = 4 * 1024;
		static const int defaultSendBuffSize = 64 * 1024;
		INetConnectionImpl();

		virtual ~INetConnectionImpl();

		/**
		* to close connection
		*/
		virtual void close() = 0;

		/**
		* set send and close
		*/
		virtual void close(const int& interval) = 0;

		/**
		* to send and close
		*/
		virtual int sendData(const void * buf, size_t size, void * act) = 0;

		/**
		* to return address
		*/
		virtual void address(SInetAddress& local, SInetAddress& remote) = 0;

		/**
		* 获取正要发送的数据包
		*/
		int getMsgInSendCount(){ return 0; };

		/**
		* to set the net connection
		*/
		void setNetConnection(INetConnection* netConnection);

		/**
		* set recv buffer size
		*/
		void setRecvBuffSize(int recvBuffSize);

		/**
		* set recv buffer size
		*/
		void setSendBuffSize(int sendBufSize);

		/**
		* no delay off net
		*/
		void setNodelay(bool noDelay);

		/**
		* log error
		*/
		void setLogError(bool logError);

	protected:
		INetConnection* _connectionPtr;
		int _recvBuffSize;
		bool _nodelay;                       //nodelay
		int _sendBufSize;                    //send buf size
		int _logError;                       //log error
	};
}
#endif
