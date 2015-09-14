#include "ServerHandler.h"

#define MAX_MSG_SIZE 1024

void ServerHandler::open(ACE_HANDLE new_handle, ACE_Message_Block &)
{
	//this->handle(new_handle);
	//if (this->write_.open(*this) != 0 || this->reader_.open(*this) != 0)
	//{
	//	delete this;
	//	return;
	//}

	ACE_SOCK_STREAM stream(this->handle());
	ACE_INET_Addr remote_address;
	if (stream.get_remote_addr(remote_address) == 0 && remote_address.addr_to_string(peer_name, MAXHOSTNAMELEN) == 0) {
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("[%T](%P|%t) Connection from %s\n"), peer_name));
	}

}

void ServerHandler::handle_read_stream(const TRB_Asynch_Read_Stream::Result &result)
{
	ACE_Message_Block &mb = result.message_block();

	if (!result.success() || result.bytes_transferred() == 0)
	{
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("[%T](%P|%t) Disconnect from %s\n"), peer_name));
		mb.release();
		delete this;
	}
	else
	{
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("[%T](%P|%t) Receive msg from client: %s\n"), mb.rd_ptr()));

		if (this->write_.write(mb, mb.length()) != 0)
		{
			ACE_ERROR((LM_ERROR, ACE_TEXT("[%T](%P|%t) Asynch Write Failed.")));
			mb.release();
		}

		ACE_Message_Block* new_mb;
		ACE_NEW_NORETURN(new_mb, ACE_Message_Block(MAX_MSG_SIZE));
		this->reader_.read(*new_mb, new_mb->space());
	}
}

void ServerHandler::handle_write_stream(const TRB_Asynch_Write_Stream::Result &result)
{
	result.message_block().release();
}