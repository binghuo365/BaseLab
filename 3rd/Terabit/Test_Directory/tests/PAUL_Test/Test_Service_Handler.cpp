#include "Test_Service_Handler.h"
#include "Test_Queue.h"
#include "ace/OS.h"
#include "ace/Guard_T.h"
#include "ace/Message_Block.h"
#include "ace/Proactor.h"

static const char *pkt_header = "The header is 52 bytes                              ";                          
static const char *pkt_payload = "The payload is 140 bytes                           "
                                 "                                                   "
                                 "                                      ";

ACE_Recursive_Thread_Mutex Test_Service_Handler::service_handlers_mtx_;
std::set<Test_Service_Handler*> Test_Service_Handler::service_handlers_;

void Test_Service_Handler::wait_all_removed()
{
    service_handlers_mtx_.acquire();
    while (service_handlers_.size() > 0)
    {
        service_handlers_mtx_.release();
        ACE_OS::sleep(ACE_Time_Value(1, 0));
        service_handlers_mtx_.acquire();
    }

    //AL: was missing  mutex release
    service_handlers_mtx_.release();
}
    
void Test_Service_Handler::add(Test_Service_Handler *service_handler)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(service_handlers_mtx_);
    std::set<Test_Service_Handler*>::iterator pos = service_handlers_.find(service_handler);
    ACE_ASSERT(pos == service_handlers_.end());
    service_handlers_.insert(service_handler);
}

void Test_Service_Handler::remove(Test_Service_Handler *service_handler)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(service_handlers_mtx_);
    std::set<Test_Service_Handler*>::iterator pos = service_handlers_.find(service_handler);
    ACE_ASSERT(pos != service_handlers_.end());
    service_handlers_.erase(pos);
}

Test_Service_Handler::Test_Service_Handler(int receive_expected, bool is_caller) :
  is_caller_(is_caller),        
  read_count_(0),
  write_count_(0),
  cancel_called_(false),
  delete_called_(false),
  receive_expected_(receive_expected),
  received_(0),
  refs_(0)
{
  Test_Service_Handler::add(this);
  event_pkt_recv_.reset();

  ACE_DEBUG ((LM_DEBUG, 
      ACE_TEXT("(%t) %s_Handler(%@)::Test_Service_Handler\n"),
      is_caller_ ? "Client" : "Server" ,
      this));
}

Test_Service_Handler::~Test_Service_Handler()
{
  // Acquire/release mtx_ in case Delete_Service_Handler_Request was queued
  // and handled before the mtx_ was released in method that queued the request.
  mtx_.acquire();


  // AL: moved down
  //mtx_.release();

  ACE_DEBUG ((LM_DEBUG, 
      ACE_TEXT("(%t) %s_Handler(%@)::~Test_Service_Handler\n"), 
      is_caller_ ? "Client" : "Server" ,
      this));

  ACE_ASSERT(0 == read_count_  && 
             0 == write_count_ &&
             0 == refs_  && 
             cancel_called_ && 
             delete_called_ );

  //AL: release moved here
  mtx_.release();

  Test_Service_Handler::remove(this);
}

void Test_Service_Handler::incr_refs()
{
  ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);
  refs_++;
}

void Test_Service_Handler::decr_refs()
{
  ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);
  refs_--;
  
  delete_if_reqd();
}

int Test_Service_Handler::recv()
{
  ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

  // First read is the header
  ACE_ASSERT(0 == read_count_);

  ACE_DEBUG ((LM_DEBUG, 
      ACE_TEXT("(%t) %s_Handler(%@)::recv() fd=%d\n"), 
      is_caller_ ? "Client" : "Server" ,
      this,
      this->handle()));

  //AL:
  if (!cancel_called_)
  {
      ACE_Message_Block *mb = 0;

      size_t len = ACE_OS::strlen(pkt_header);
 
      ACE_NEW_RETURN(mb, ACE_Message_Block(len), -1);


      int ret = reader_.read(*mb, len);

      if (ret == 0)
      {
        read_count_++;
        return 0;
      }

      mb->release ();

  }

  ACE_DEBUG ((LM_DEBUG, 
      ACE_TEXT("(%t) %s_Handler(%@)::recv failed\n"),
      is_caller_ ? "Client" : "Server" ,
      this));

  //AL: delete_if_reqd();
  this->cancel();

  return -1;
}

int Test_Service_Handler::send()
{
  ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

  ACE_DEBUG ((LM_DEBUG, 
      ACE_TEXT("(%t) %s_Handler(%@)::send() fd=%d\n"),
      is_caller_ ? "Client" : "Server" ,
      this,
      this->handle()));

  //AL:
  if (!cancel_called_)
  {
     //AL: not thread safe
     //static std::string pkt = pkt_header;
     //pkt += pkt_payload;

     size_t len_header  = ACE_OS::strlen(pkt_header);
     size_t len_payload = ACE_OS::strlen(pkt_payload);

     ACE_Message_Block *mb = 0;

     ACE_NEW_RETURN(mb, ACE_Message_Block(len_header+len_payload), -1);

     mb->copy(pkt_header, len_header);
     mb->copy(pkt_payload, len_payload);
    
     int ret = writer_.write(*mb, mb->length());
     if (ret == 0)
     {
        write_count_++;
        return 0;
     }

     mb->release();

  }

  ACE_DEBUG ((LM_DEBUG,
      ACE_TEXT("(%t) %s_Handler(%@)::send failed\n"),
      is_caller_ ? "Client" : "Server" ,
      this));

  //AL: delete_if_reqd();
  this->cancel();

  return -1;
}

void Test_Service_Handler::wait_for_all_received()
{
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);
        
        if (received_ >= receive_expected_)
            event_pkt_recv_.signal();
    }
    
    event_pkt_recv_.wait();
}

void Test_Service_Handler::delete_if_reqd()
{
  ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);
    
  if (0 != read_count_ || 0 != write_count_ || refs_ != 0 || !cancel_called_ )
  {
     return;
  }

  //AL:
  ACE_ASSERT(delete_called_ == false);

  delete_called_ = true;

  Delete_Service_Handler_Request *req;
  ACE_NEW_NORETURN(req, Delete_Service_Handler_Request(this));
  TEST_QUEUE->perform(req);
}

int Test_Service_Handler::cancel()
{
  ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

  cancel_called_ = true;

  if (handle() != ACE_INVALID_HANDLE)
    {
      reader_.cancel();
      writer_.cancel();
      
      reader_.close();
      writer_.close();
    
      ACE_OS::closesocket(handle());
      handle(ACE_INVALID_HANDLE);
    }
  
  delete_if_reqd();

  return 0;
}

void Test_Service_Handler::open(ACE_HANDLE h, ACE_Message_Block &mb)
{
  ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

  ACE_UNUSED_ARG(mb);

  ACE_DEBUG ((LM_DEBUG, 
      ACE_TEXT("(%t) %s_Handler(%@)::open fd=%d\n"),
      is_caller_ ? "Client" : "Server" ,
      this,
      h));

  ACE_ASSERT(this->handle() == h);

  int ret = reader_.open(*this, h, 0, proactor());
  ACE_ASSERT(0 == ret);

  ret = writer_.open(*this, h, 0, proactor());
  ACE_ASSERT(0 == ret);

  // In this test we perform recv in a separate thread
  On_Open_Request *req;
  ACE_NEW_NORETURN(req, On_Open_Request(this));
  TEST_QUEUE->perform(req);
}

void Test_Service_Handler::handle_read_stream(const ACE_Asynch_Read_Stream::Result &result)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

    if (!result.success() || result.bytes_transferred() == 0)
    {
      // Error or remote disconnect

      ACE_DEBUG ((LM_DEBUG, 
          ACE_TEXT("(%t) %s_Handler(%@)::handle_read_stream - disconnected\n"),
          is_caller_ ? "Client" : "Server" ,
          this));

      read_count_--;

      result.message_block().release();

      //AL: delete_if_reqd();
      this->cancel();
      return;
    }


    if (result.bytes_transferred() < result.bytes_to_read())
    {
      // More to read...

      ACE_DEBUG ((LM_DEBUG, 
          ACE_TEXT("(%t) %s_Handler(%@)::handle_read_stream, more to read\n"),
          is_caller_ ? "Client" : "Server" ,
          this));

      int ret = reader_.read(result.message_block(), 
                             result.bytes_to_read() - result.bytes_transferred());
      if (ret < 0)
	    {
          ACE_DEBUG ((LM_DEBUG, 
              ACE_TEXT("(%t) %s_Handler(%@)::handle_read_stream, failed to the remainder\n"),
              is_caller_ ? "Client" : "Server" ,
              this));

          read_count_--;

          result.message_block().release();
          
          //AL: delete_if_reqd();
          this->cancel();
	    }
      return;
    }

    if (current_read_pkt_.length() == 0)
    {
      // The header has been read

      current_read_pkt_ = std::string(result.message_block().base(), 
                                      result.message_block().length());

      // Read the payload

      result.message_block().size(ACE_OS::strlen(pkt_payload));
      result.message_block().reset();

      int ret = reader_.read(result.message_block(), ACE_OS::strlen(pkt_payload));

      if (ret < 0)
    	{
          read_count_--;

          result.message_block().release();

          this->cancel();
	    }
      return;
    }

    // The payload has been read

    ++received_;

    ACE_DEBUG ((LM_DEBUG, 
          ACE_TEXT("(%t) %s_Handler(%@)::handle_read_stream, read data item: %d\n"),
          is_caller_ ? "Client" : "Server" ,
          this,
          received_));

    current_read_pkt_ += std::string(result.message_block().base(), 
                                     result.message_block().size());
    
    current_read_pkt_.clear();

    read_count_--;

    if (received_ >= receive_expected_)
    {
          ACE_DEBUG ((LM_DEBUG, 
              ACE_TEXT("(%t) %s_Handler(%@)::handle_read_stream, received all data\n"), 
              is_caller_ ? "Client" : "Server" ,
              this));
          event_pkt_recv_.signal();
    }
    else
    {
          Send_Request *req;
          ACE_NEW_NORETURN(req, Send_Request(this));
          TEST_QUEUE->perform(req);
    }

}

void Test_Service_Handler::handle_write_stream(const ACE_Asynch_Write_Stream::Result &result)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

    if (!result.success() || 0 == result.bytes_transferred())
    {
      // Error
      ACE_DEBUG ((LM_DEBUG, 
          ACE_TEXT("(%t) %s_Handler(%@)::handle_write_stream, disconnected\n"),
          is_caller_ ? "Client" : "Server" ,
          this));

      write_count_--;

      result.message_block().release();

      //AL: delete_if_reqd();
      this->cancel();
      return;
    }

    if (result.bytes_transferred() < result.bytes_to_write())
    {
      // More to write...
      ACE_DEBUG ((LM_DEBUG, 
          ACE_TEXT("(%t) %s_Handler(%@)::handle_write_stream, more to write\n"),
          is_caller_ ? "Client" : "Server" ,
          this));

      int ret = writer_.write(result.message_block(), 
                              result.bytes_to_write() - result.bytes_transferred());

      if (ret < 0)
	    {
          write_count_--;
	  
          result.message_block().release();
      
          this->cancel();
	    }
      return;
    }

    // Wrote it all
      ACE_DEBUG ((LM_DEBUG, 
          ACE_TEXT("(%t) %s_Handler(%@)::handle_write_stream, write done ok\n"),
          is_caller_ ? "Client" : "Server" ,
          this));

    write_count_--;

    result.message_block().release();

    if (this->cancel_called_)
    {
        this->delete_if_reqd();
    }
}

