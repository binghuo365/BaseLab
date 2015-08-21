#include "Test_Acceptor.h"
#include "ace/Guard_T.h"

Test_Acceptor::Test_Acceptor()
: cancel_flag_ (0)
, accept_cnt_  (0)
, service_handler_(0)
{
    //AL: nothing is wrong here , just my habbit to initialize
    //cancelled_ = 0;
    //cancel_flag_ = 0;
    //accept_cnt_ = 0;
    //service_handler_ = 0;
}

Test_Acceptor::~Test_Acceptor()
{
}

int Test_Acceptor::cancel()
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

    cancel_flag_ = 1;


//AL: commented 
//AL: see ACE_Asynch_Acceptor<HANDLER>::cancel (void)
//    will do reissue_accept(0);

//AL: that is all what you needed
//Use Terabit  Asynch_Acceptor !! (not ACE original version)
//  it will call reissue_accept(0) and will do
//  cancel_and_close for you
//AL: see ACE_Asynch_Acceptor<HANDLER>::cancel (void)

    ACE_Asynch_Acceptor<Test_Service_Handler>::cancel();

//AL: commented
//    ACE_HANDLE h = this->get_handle();
//    if (h != ACE_INVALID_HANDLE)
//    {
//#if defined(ACE_WIN32)
//        ACE_OS::closesocket(h);
//        handle(ACE_INVALID_HANDLE);
//#else
//        ACE_Asynch_Acceptor<Test_Service_Handler>::cancel();
//#endif
//    }

//AL:
//    if (accept_cnt_ == 0)
//        cancelled_ = 1;

    return 0;
}

bool Test_Acceptor::is_cancelled(
    ) const
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);
   return (cancel_flag_ != 0 && accept_cnt_ == 0);

 //AL:
 //    mtx_.acquire();
 //   int cancelled = cancelled_.value();
 //   mtx_.release();
 //   return 0 != cancelled;
}

void Test_Acceptor::prepare_for_connection(
    Test_Service_Handler *service_handler
    )
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);
    service_handler_ = service_handler;
}

int Test_Acceptor::validate_connection(
        const ACE_Asynch_Accept::Result& result,
        const ACE_INET_Addr & /*remote*/,
        const ACE_INET_Addr& /*local*/
        )
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

    //AL:
    //if (0 != service_handler_)
    //    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) Test_Acceptor::validate_connection, success = %d\n"), result.success()));

    if (result.success())
    {
        //AL:
        ACE_ASSERT(0 != service_handler_);
        return 0;
    }
    else
    {
        service_handler_ = 0;
        return -1;
    }
}

Test_Service_Handler* Test_Acceptor::make_handler()
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

    ACE_ASSERT(0 != service_handler_);
    Test_Service_Handler *service_handler = service_handler_;
    service_handler_ = 0;
    return service_handler;
}

int Test_Acceptor::accept(
    size_t bytes_to_read, 
    const void *act
    )
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

//AL: I remommend replace the logic
    //accept_cnt_++;
    //if  (cancel_flag_.value())
    //    reissue_accept(0);
    //return ACE_Asynch_Acceptor<Test_Service_Handler>::accept(bytes_to_read, act);

    if  (cancel_flag_!= 0)  // AL: no need in atomic value())
      return -1;

    accept_cnt_++;
    int rc = ACE_Asynch_Acceptor<Test_Service_Handler>::accept(bytes_to_read, act);
    if (rc != 0)
      accept_cnt_--;

    return rc;
}

void Test_Acceptor::handle_accept(
    const ACE_Asynch_Accept::Result &result
    )
{   
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

//AL: setting cancel_flag_ also calls reissue_accept()
//    by ACE_Asynch_Acceptor<Test_Service_Handler>::cancel();
//
//    if (cancel_flag_.value())
//        reissue_accept(0);

    ACE_Asynch_Acceptor<Test_Service_Handler>::handle_accept(result);

    //AL:
    //if (--accept_cnt_ == 0)
    //    cancelled_ = 1;

    --accept_cnt_;
}

//ACE_INET_Addr Test_Acceptor::get_listen_addr() const
//{
//    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);
//    sockaddr_in addr;
//    int addr_size = sizeof (addr);
//
//    if (ACE_OS::getsockname(handle(), 
//        ACE_reinterpret_cast (sockaddr*, &addr), &addr_size) < 0)
//    {
//        return ACE_INET_Addr((u_short)0);
//    }
//    else
//    {
//        return ACE_INET_Addr(&addr, addr_size);
//    }
//}
