// ============================================================================
/**
 *  @file PAUL_Test.cpp
 *
 *  PAUL_Test.cpp
 *
 *  This program duplicates a bug where asynch operations issued from a
 *  worker thread fails after 64 about operations.
 *
 *  @author Paul Daugherty <paul@nxicom.com>
 */
// ============================================================================

#include "test_config.h"

#include "Cfg.h"
#include "PSession.h"

#include "ace/INET_Addr.h"
//#include "ace/Countdown_Time.h"

#include "Test_Acceptor.h"
#include "Test_Connector.h"
#include "Test_Service_Handler.h"
#include "Test_Queue.h"

#include "Asynch_RW.h"

//AL: to work with older ACE
#ifndef ACE_DEFAULT_ASYNCH_BACKLOG
#define ACE_DEFAULT_ASYNCH_BACKLOG 5
#endif

static TestCfg cfg;

// *************************************************************
//   Configuration helpers
// *************************************************************
int
run (int argc, ACE_TCHAR *argv[])
{
    ACE_UNUSED_ARG(argc);
    ACE_UNUSED_ARG(argv);

    int ret = 0;

    ProactorTask proactor_task(cfg);


    ret = proactor_task.start();

    if (ret == 0)
      {
          proactor_task.enable_event_loop();

          ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) Task_Acceptor::open\n")));
          ret = TEST_ACCEPTOR->open(
            ACE_INET_Addr(cfg.listen_port()),
            0,
            1,
            ACE_DEFAULT_ASYNCH_BACKLOG,
            1, // reuse addr
            proactor_task.get_proactor(0),
          1);
      }

    if (ret == 0)
      {
         ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) Task_Connector::open\n")));
         ret = TEST_CONNECTOR->open(1, 
                                    proactor_task.get_proactor(0),
                                    1);
      }

    // ACE_Asynch_Connector::connect and ACE_Asynch_Read_Stream::read (callee)
    // are issued in another thread to mimic our actual usage.  An activation
    // queue is used to perform these actions.
    TEST_QUEUE->activate();

    // Allow connect_task enough time to wait on start_connection_
    ACE_OS::sleep(ACE_Time_Value(2, 0));

    int loop = 0;
    while (ret == 0 && loop < 20)
      {
          ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT("(%t) \n**********************\nLoop %d\n**********************\n"),
              loop++));

          // Caller sends a pkt in open, callee doesn't.
          Test_Service_Handler *handler_1 = 0;
          Test_Service_Handler *handler_2 = 0;

          int pkts_to_ping_pong = 8;

          ACE_NEW_NORETURN(handler_1, Test_Service_Handler(pkts_to_ping_pong, true));
          ACE_NEW_NORETURN(handler_2, Test_Service_Handler(pkts_to_ping_pong, false));
          
          if (ret == 0)
          {
              TEST_CONNECTOR->prepare_for_connection(handler_1);
              TEST_ACCEPTOR->prepare_for_connection(handler_2);

              // initiate the connection in the queue
              Connect_Request *req;

              ACE_NEW_NORETURN(req, 
                               Connect_Request(
                                       ACE_INET_Addr(cfg.connect_port(),
                                                     cfg.connect_host())));

              TEST_QUEUE->perform(req);
          }

          if (ret == 0)
          {
              ACE_DEBUG ((LM_DEBUG, 
                  ACE_TEXT("(%t) Server_Handler(%@)::wait_for_recv(2)\n"),
                  handler_2));
              handler_2->wait_for_all_received();

              //AL: client always receives last packet/reply after server
              //ACE_DEBUG ((LM_DEBUG, 
              //    ACE_TEXT("(%t) Test_Service_Handler(%@)::wait_for_recv(1)\n"),
              //    handler_1));
              //handler_1->wait_for_all_received();
          }
                  
          
          ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) Cleaning up\n")));

          // cleanup
          handler_1->cancel();
          handler_2->cancel();
 
          ACE_OS::sleep(ACE_Time_Value(0, 250000));
      }
    

    // Cleanup

    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) Test_Acceptor::cancel\n"), ret));
    TEST_ACCEPTOR->cancel();
    while (!TEST_ACCEPTOR->is_cancelled())
      {
        ACE_OS::sleep(ACE_Time_Value(0, 500000));
      }

    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) Test_Connector::cancel\n"), ret));
    TEST_CONNECTOR->cancel();
    while (!TEST_CONNECTOR->is_cancelled())
      {
          ACE_OS::sleep(ACE_Time_Value(0, 500000));
      }

    // TEST_QUEUE->perform(0) causes the queue to exit
    // AL: perform(0) causes access violation

    TEST_QUEUE->stop();
    TEST_QUEUE->wait();

    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) ProactorTask::stop\n"), ret));
    proactor_task.stop ();
     

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("(%t) Threads = %d BlkSize=%d\n"),
                cfg.threads(),
                cfg.blksize()));

    return 0; 
}

int
ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
    ACE_START_TEST (ACE_TEXT ("PAUL_Test"));


    if (cfg.parse_args (argc, argv) < 0)
        return -1;

    disable_signal (ACE_SIGRTMIN, ACE_SIGRTMAX);
    disable_signal (SIGPIPE, SIGPIPE);
    disable_signal (SIGIO, SIGIO);

    ACE_DEBUG ((LM_DEBUG,
             "s_blksize=%u r_blksize=%u win_size=%u\n",
              cfg.s_blksize(),
              cfg.r_blksize(),
              cfg.w_size()));


    run(argc, argv);

    ACE_END_TEST;

    return 0;
}

