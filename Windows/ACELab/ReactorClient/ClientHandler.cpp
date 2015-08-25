#include "ClientHandler.h"

using namespace BaseLabWin::ACELab;

int ClientHandler::open(void *p)
{
	if (super::open(p) == -1)
	{
		return -1;
	}
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("[%T](%P|%t) Connection start\n")));
	this->iterations_ = 0;
	ACE_Time_Value iter_delay(2);   // Two seconds
	return this->reactor()->schedule_timer(this, 0, ACE_Time_Value::zero, iter_delay);
}

int ClientHandler::handle_input(ACE_HANDLE){
	const int INPUT_SIZE = 4096;
	char buffer[INPUT_SIZE];
	memset(buffer, 0, INPUT_SIZE);
	int recv_cnt = this->peer().recv(buffer, sizeof(buffer));

	if (recv_cnt <= 0) {
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("[%T](%P|%t) Connection close\n")));
		this->reactor()->end_reactor_event_loop();
		return -1;
	}

	ACE_DEBUG((LM_DEBUG, ACE_TEXT("[%T](%P|%t) Receive msg from server: %s"), buffer));
	return 0;
}

int ClientHandler::handle_timeout(const ACE_Time_Value &, const void *)
{
	if (++this->iterations_ > ITERATIONS)
	{
		this->peer().close_writer();
		return 0;
	}

	const int INPUT_SIZE = 4096;
	char buffer[INPUT_SIZE];
	memset(buffer, 0, INPUT_SIZE);
	int nbytes = ACE_OS::sprintf(buffer, "Iteration %d\n", this->iterations_);
	this->peer().send(buffer, nbytes);
	return 0;
}