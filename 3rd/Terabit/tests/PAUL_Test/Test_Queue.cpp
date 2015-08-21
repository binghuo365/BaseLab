#include "Test_Queue.h"

int Test_Queue::svc()
{
    while (!flg_stop_)
    {
        ACE_Method_Request *request = 0;
        try
        {
            request = queue_.dequeue();

            //AL:
            ACE_ASSERT(request != 0);
            //if (request == 0)
            //{
            //    // Queue is exiting
            //    break;
            //}

            request->call();
        }       
        catch (...)
        {
            ACE_ASSERT(0);
        }
    }

    return 0;
}

int Test_Queue::perform(
    ACE_Method_Request *req
    )
{
    int ret = queue_.enqueue(req);
    if (-1 == ret)
    {
        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) Test_Queue::perform failed\n")));
    }
    return ret;
}


