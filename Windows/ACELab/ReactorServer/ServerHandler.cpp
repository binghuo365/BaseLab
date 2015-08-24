#include "ServerHandler.h"
#include <iostream>
#include <string>

using namespace BaseLabWin::ACELab;

int ServerHandler::handle_input(ACE_HANDLE fd /*= ACE_INVALID_HANDLE*/)
{
	const int INPUT_SIZE = 4096;
	char buffer[INPUT_SIZE];
	memset(buffer, 0, INPUT_SIZE);
	int recv_cnt = this->peer().recv(buffer, sizeof(buffer));

	if (recv_cnt <= 0)
	{
		printf("Connection close\n");
		return -1;
	}
	std::string str(buffer);
	std::cout << str << std::endl;
	this->peer().send(buffer, recv_cnt);
	return 0;
}
