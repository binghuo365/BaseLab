#include "netaddress.h"
#include "acetrbnetacceptorimpl.h"
#include "acetrbnetconnectorimpl.h"

int main(int argc, char* argv[])
{
	CDENET::CACETrbNetAcceptorImpl acceptor;
	CDENET::SInetAddress address;
	address.fromString("127.0.0.1:8080");
	acceptor.binding(address);

	//CDENET::CACETrbNetConnectorImpl connector;
	//CDENET::SInetAddress adresss;
	//adresss.fromString("127.0.0.1:8888");
	//connector.connect(adresss);

	system("pause");
	return 0;
}