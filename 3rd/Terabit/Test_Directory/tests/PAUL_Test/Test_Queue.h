#ifndef TEST_QUEUE_H
#define TEST_QUEUE_H

#include "ace/Task_T.h"
#include "ace/Method_Request.h"
#include "ace/Activation_Queue.h"
#include "ace/Singleton.h"
#include "Test_Connector.h"

//AL
class Stop_Request : public ACE_Method_Request
{
public:
    Stop_Request () {}
    virtual ~Stop_Request() {}

    virtual int call()
    {
        return -1;
    }

};

// Starts the connect from the caller
class Connect_Request : public ACE_Method_Request
{
public:
    Connect_Request(const ACE_INET_Addr &remote_addr) :
      remote_addr_(remote_addr) {}
    virtual ~Connect_Request() {}

    virtual int call()
    {
        
        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) Test_Connector::connecting\n")));
        if (-1 == TEST_CONNECTOR->connect(remote_addr_))
        {
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) Test_Connector::connect failed\n")));
        }
        delete this; 
        return -1;
    }

private:
    ACE_INET_Addr remote_addr_;
};

// Callee queues this when Test_Service_Handler::open is called.
// This method will call recv() on the Test_Service_Handler.
class On_Open_Request : public ACE_Method_Request
{
public:
    On_Open_Request(Test_Service_Handler *service_handler) :
      service_handler_(service_handler)
        {
            service_handler_->incr_refs();
        }
    virtual ~On_Open_Request()
        {
            service_handler_->decr_refs();
        }    

    virtual int call()
    {
       int rc;

       if (service_handler_->is_client())
		 rc = service_handler_->recv();
       else
		 rc = service_handler_->recv();

       ACE_DEBUG ((LM_DEBUG,
            ACE_TEXT("(%t) Open_Request, %s_Handler(%@), recv rc=%d\n"), 
            service_handler_->is_client()? "Client" : "Server",
            service_handler_,
            rc));
            
       if (service_handler_->is_client())
       {
         rc = service_handler_->send();
         ACE_DEBUG ((LM_DEBUG,
            ACE_TEXT("(%t) Open_Request, %s_Handler(%@), send rc=%d\n"), 
            service_handler_->is_client()? "Client" : "Server",
            service_handler_,
            rc));
       }        
  
 
        delete this; 
        return -1;
    }

private:
    Test_Service_Handler *service_handler_;
};

// When we delete a service handler, we queue this event to avoid deadlock between
// Test_Service_Handler::mtx_ and Test_Service_Handler::service_handlers_mtx_
class Delete_Service_Handler_Request : public ACE_Method_Request
{
public:
    Delete_Service_Handler_Request(Test_Service_Handler *service_handler) :
      service_handler_(service_handler) {}
    virtual ~Delete_Service_Handler_Request() {}    

    virtual int call()
    {
        
        ACE_DEBUG ((LM_DEBUG, 
            ACE_TEXT("(%t) Delete_Request, %s_Handler(%@), deleting\n"),
            service_handler_->is_client()? "Client" : "Server",
            service_handler_));

        delete service_handler_;
        delete this; 
        return -1;
    }

private:
    Test_Service_Handler *service_handler_;
};

// This event is queued to to perform another read and send another pkt from one
// service handler to the other.  This is queued in handle_read_stream.
class Send_Request : public ACE_Method_Request
{
public:
    Send_Request(Test_Service_Handler *service_handler) :
      service_handler_(service_handler)
        {
            service_handler_->incr_refs();
        }
    virtual ~Send_Request()
        {
            service_handler_->decr_refs();
        }

    virtual int call()
    {
        int rc;   
        rc = service_handler_->recv();
        ACE_DEBUG ((LM_DEBUG,
            ACE_TEXT("(%t) Send_Request, %s_Handler(%@), recv rc=%d\n"), 
            service_handler_->is_client()? "Client" : "Server",
            service_handler_,
            rc));
            
       service_handler_->send();
       ACE_DEBUG ((LM_DEBUG,
            ACE_TEXT("(%t) Send_Request, %s_Handler(%@), send rc=%d\n"), 
            service_handler_->is_client()? "Client" : "Server",
            service_handler_,
            rc));

        delete this; 
        return -1;
    }

private:
    Test_Service_Handler *service_handler_;
};

// Activation queue handles ACE_Method_Requests
class Test_Queue : public ACE_Task_Base
{
public:
    Test_Queue() : flg_stop_(false) {}
    virtual ~Test_Queue() {}

    // Pass req == 0 to cause queue to exit
    virtual int perform(
        ACE_Method_Request *req
        );

    virtual int svc();

    int stop()
      {
         flg_stop_ = true;
         this->perform(&stopper_);
         return 0;
      }
    

private:
    ACE_Activation_Queue queue_;
    Stop_Request stopper_;
    bool flg_stop_;
};

typedef ACE_Singleton<Test_Queue, ACE_Recursive_Thread_Mutex> Test_Queue_Singleton;
#define TEST_QUEUE Test_Queue_Singleton::instance()

#endif // TEST_QUEUE_H

