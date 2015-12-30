#ifndef _CDE_NET_CONNECTION_H_
#define _CDE_NET_CONNECTION_H_


#include "netaddress.h"
#include "netconnectionimpl.h"
/**
* this is the bridge of the connecton
*/
namespace CDENET
{
	class INetConnection
	{
	public:
		INetConnection();
		virtual ~INetConnection();

		/**
		* to close connection
		*/
		int close();

		/**
		* close with time
		*/
		virtual int close(const int interval);

		/**
		* return 0-- success -1--error
		*/
		int sendData(const void * buf, size_t size, void* act);

		/**
		*
		*/
		void address(SInetAddress& local, SInetAddress& remote);

		/**
		*/
		int getMsgInSendCount();

		/**
		* flower is the back events
		*/
		virtual bool onConnected() = 0;
		/**
		* the close event
		*/
		virtual void onClosed(bool bySend = false) = 0;
		/**
		* on error
		*/
		virtual void onError(int error) = 0;
		/**
		* the send event
		*/
		virtual void onSend(int packetCount, bool resultV, const void* act) = 0;
		/**
		* the revc event
		*/
		virtual bool onRecv(void* buf, size_t size) = 0;

	public:
		INetConnectionImpl* getConnectionImpl();
		void setConnectionImpl(INetConnectionImpl* connectionImpl);

		bool isClient() const;
		void setClient(bool isClient);


	private:
		INetConnectionImpl* _connectionImpl;
		bool _isClient;
	};
}

#endif
