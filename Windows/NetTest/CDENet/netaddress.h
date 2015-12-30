#ifndef _CDF_NET_ADDRESS_H_
#define _CDF_NET_ADDRESS_H_

#include <string>

namespace CDENET
{
	struct SInetAddress
	{
	public:
		SInetAddress();
		SInetAddress(const SInetAddress& address);
		SInetAddress(const std::string& address);
		unsigned int ip;
		unsigned short port;
		std::string toString() const;
		std::string ipToString() const;
		void fromString(const std::string& str);

		bool operator == (const CDENET::SInetAddress& address) const;
		bool operator < (const CDENET::SInetAddress& address) const;
	};
}

#endif