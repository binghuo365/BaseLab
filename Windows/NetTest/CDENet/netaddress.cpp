#include "netaddress.h"
#include "ace/INET_Addr.h"
#include <stdio.h>

using namespace CDENET;

SInetAddress::SInetAddress()
:ip(0), port(0)
{
}

SInetAddress::SInetAddress(const SInetAddress& address)
: ip(address.ip), port(address.port)
{
}

SInetAddress::SInetAddress(const std::string& address)
{
	fromString(address);
}

std::string SInetAddress::toString() const
{
	char buf[64];
	sprintf_s(buf, "%u.%u.%u.%u:%u",
		ip >> 24, (ip >> 16) & 0xFF,
		(ip >> 8) & 0xFF, ip & 0xFF,
		port);
	return buf;
}

std::string SInetAddress::ipToString() const
{
	char buf[64];
	sprintf_s(buf, "%u.%u.%u.%u",
		ip >> 24, (ip >> 16) & 0xFF,
		(ip >> 8) & 0xFF, ip & 0xFF);
	return buf;
}

void SInetAddress::fromString(const std::string& str)
{
	ACE_INET_Addr addr(str.c_str());
	ip = addr.get_ip_address();
	port = addr.get_port_number();
}

bool SInetAddress::operator == (const CDENET::SInetAddress& address) const
{
	if (this == &address)
	{
		return true;
	}
	if (ip != address.ip)
	{
		return false;
	}
	if (port != address.port)
	{
		return false;
	}
	return true;
}

bool SInetAddress::operator < (const CDENET::SInetAddress& address) const
{
	if (this == &address)
	{
		return false;
	}
	if (ip < address.ip)
	{
		return true;
	}
	else if (address.ip < ip)
	{
		return false;
	}
	if (port < address.port)
	{
		return true;
	}
	else if (address.port < port)
	{
		return false;
	}
	return false;
}