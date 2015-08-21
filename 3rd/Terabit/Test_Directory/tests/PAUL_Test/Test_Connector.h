#ifndef TEST_CONNECTOR_H
#define TEST_CONNECTOR_H

#include "ace/Asynch_Connector.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Singleton.h"
#include "Test_Service_Handler.h"

// ----------------------------------------------------------------------------
// Test_Connector
// ----------------------------------------------------------------------------

class Test_Connector : public ACE_Asynch_Connector<Test_Service_Handler>
{
// Construction
public:
    Test_Connector(
        );        

    virtual ~Test_Connector(
        );

// Attributes
protected:
    Test_Service_Handler                *service_handler_;
    mutable ACE_Recursive_Thread_Mutex  mtx_;
    bool                                cancelling_;
    long                                timer_id_;
    int                                 connect_ip_;
    
// Operations
public:
    virtual int connect(
        const ACE_INET_Addr &remote_sap,
        const ACE_INET_Addr &local_sap = ACE_INET_Addr ((u_short)0),
        int reuse_addr = 1,
        const void *act = 0
        );    

    // Assign Test_Service_Handler for outbound connection
    void prepare_for_connection(
        Test_Service_Handler *service_handler
        );

    // Returns true when the connector is cancelled and can be deleted
    bool is_cancelled(
        ) const;

// Overrides
public:
    virtual int cancel(
        );

    virtual int validate_connection(
        const ACE_Asynch_Connect::Result& result,
        const ACE_INET_Addr &remote,
        const ACE_INET_Addr& local
        );    

protected:
    virtual Test_Service_Handler *make_handler(
        );
};

typedef ACE_Singleton<Test_Connector, ACE_Recursive_Thread_Mutex> Test_Connector_Singleton;
#define TEST_CONNECTOR Test_Connector_Singleton::instance()

#endif // TEST_CONNECTOR_H

