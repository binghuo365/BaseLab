#ifndef TEST_ACCEPTOR_H
#define TEST_ACCEPTOR_H

#include "Test_Service_Handler.h"
#include "TProactor/Asynch_Acceptor.h"
#include "ace/INET_Addr.h"
#include "ace/Atomic_Op.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Singleton.h"

class Test_Acceptor : public TRB_Asynch_Acceptor<Test_Service_Handler>
{
public:
    Test_Acceptor();
    virtual ~Test_Acceptor();

private:
    //AL: no need to have extra protection
    //ACE_Atomic_Op<ACE_Recursive_Thread_Mutex, int> accept_cnt_;
    //ACE_Atomic_Op<ACE_Recursive_Thread_Mutex, int> cancel_flag_;
    //ACE_Atomic_Op<ACE_Recursive_Thread_Mutex, int> cancelled_;

    int accept_cnt_;
    int cancel_flag_;


    Test_Service_Handler *service_handler_;
    bool ssl_;
    mutable ACE_Recursive_Thread_Mutex  mtx_;

public:
    // Call to cancel and poll is_cancelled 
    int cancel(
        );

    // Poll this after cancel to determine when safe to delete the acceptor
    bool is_cancelled(
        ) const;

    // Assign Test_Service_Handler for inbound connection
    void prepare_for_connection(
        Test_Service_Handler *service_handler
        );

    // Return the listening port.  open() uses INADDR_ANY and lets operating
    // system choose a port.  
    //AL: I had problem with this function!
    //ACE_INET_Addr get_listen_addr(
    //    ) const;

protected:
    virtual int validate_connection(
        const TRB_Asynch_Accept::Result& result,
        const ACE_INET_Addr &remote,
        const ACE_INET_Addr& local
        );

    virtual Test_Service_Handler *make_handler(
        );    

    virtual int accept(
        size_t bytes_to_read = 0, 
        const void *act = 0
        );

    virtual void handle_accept(
        const TRB_Asynch_Accept::Result &result
        );
};

typedef ACE_Singleton<Test_Acceptor, ACE_Recursive_Thread_Mutex> Test_Acceptor_Singleton;
#define TEST_ACCEPTOR Test_Acceptor_Singleton::instance()

#endif // TEST_ACCEPTOR_H
