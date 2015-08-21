#include "Test_Connector.h"
#include "ace/Guard_T.h"

// ----------------------------------------------------------------------------
// Test_Connector
// ----------------------------------------------------------------------------

Test_Connector::Test_Connector(
    ) :
    cancelling_(false),
    timer_id_(-1),
    connect_ip_(0)
{
}

Test_Connector::~Test_Connector()
{
    mtx_.acquire();

    ACE_ASSERT(-1 == timer_id_);    
    ACE_ASSERT(0 == connect_ip_);

    mtx_.release();
}

int Test_Connector::cancel()
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

    if (cancelling_)
        return 0;
    
    cancelling_ = true;

    if (connect_ip_ > 0)
        TRB_Asynch_Connector<Test_Service_Handler>::cancel();

    return 0;
}

bool Test_Connector::is_cancelled(
    ) const
{
    //AL:
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);
    return (cancelling_ == true && 0 == connect_ip_);
}

void Test_Connector::prepare_for_connection(
    Test_Service_Handler *service_handler
    )
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);
    service_handler_ = service_handler;
}

int Test_Connector::connect(
    const ACE_INET_Addr &remote_sap,
    const ACE_INET_Addr &local_sap,
    int /*reuse_addr*/,
    const void * /*act*/
    )
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

    int ret = TRB_Asynch_Connector<Test_Service_Handler>::connect(
        remote_sap, local_sap, 0, 0);

    if (0 == ret)
        connect_ip_++;

    return ret;
}

int Test_Connector::validate_connection(
    const TRB_Asynch_Connect::Result& result,
    const ACE_INET_Addr&,
    const ACE_INET_Addr&
    )
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);


    //AL:
    //if (0 != service_handler_)
    //    ACE_DEBUG ((LM_DEBUG,
    //               ACE_TEXT("(%t) Test_Connector::validate_connection, success = %d\n"), 
    //               result.success()));

    if (result.success())
    {
        //AL: to terminate 
        ACE_ASSERT(0 != service_handler_);
        return 0;
    }
    else
    {
        service_handler_ = 0;
        connect_ip_--;
        return -1;
    }
}

Test_Service_Handler* Test_Connector::make_handler()
{ 
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

    ACE_ASSERT(0 != service_handler_);
    Test_Service_Handler *service_handler = service_handler_;
    service_handler_ = 0;

    connect_ip_--;

    return service_handler;
}

