#ifndef TEST_SERVICE_HANDLER_H
#define TEST_SERVICE_HANDLER_H

#include "TProactor/Asynch_IO.h"
#include "ace/Manual_Event.h"
#include "ace/Recursive_Thread_Mutex.h"
#include <string>
#include <deque>
#include <set>

class Test_Service_Handler : public TRB_Service_Handler
{
  // Construction
 public:
  Test_Service_Handler(int receive_expected = 1, bool is_caller = false);
  virtual ~Test_Service_Handler();

  // Operations
 public:
  int recv();
  int send();
  void wait_for_all_received();
  void delete_if_reqd();

  bool is_client() const { return this->is_caller_; }

  // ACE_Service_Handler overrides
 public:
  virtual int cancel();
  virtual void open(ACE_HANDLE h, ACE_Message_Block &mb);
  virtual void handle_read_stream(const TRB_Asynch_Read_Stream::Result &result);
  virtual void handle_write_stream(const TRB_Asynch_Write_Stream::Result &result);
  virtual void incr_refs();
  virtual void decr_refs();

  // Attributes
 private:
  mutable ACE_Recursive_Thread_Mutex mtx_;

  ACE_Manual_Event event_pkt_recv_;
  TRB_Asynch_Read_Stream reader_;
  TRB_Asynch_Write_Stream writer_;
  bool is_caller_;
  long read_count_;
  long write_count_;
  bool cancel_called_;
  bool delete_called_;
  std::string current_read_pkt_;
  int receive_expected_;
  int received_;
  int refs_;

  // static collection of all service handlers
 public:
  static void wait_all_removed();
 private:
  static ACE_Recursive_Thread_Mutex service_handlers_mtx_;
  static std::set<Test_Service_Handler*> service_handlers_;
  static void add(Test_Service_Handler *service_handler);
  static void remove(Test_Service_Handler *service_handler);
};
#endif // TEST_SERVICE_HANDLER_H
