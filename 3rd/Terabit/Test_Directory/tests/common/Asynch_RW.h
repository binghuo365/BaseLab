/*********************************************************************
** Copyright (C) 2003 Terabit Pty Ltd.  All rights reserved.
**
** This file is part of the POSIX-Proactor module.
**
**  
**   
**
**
**
**
**
**********************************************************************/

// ============================================================================
//
// = LIBRARY
//    
//
// = FILENAME
//    ACE_Asynch_RW_Stream.h
//
// = DESCRIPTION
//
// = AUTHOR
//    Alexander Libman <libman@terabit.com.au>
//
// ============================================================================
#ifndef ASYNCH_RW_H
#define ASYNCH_RW_H

#include "ace/Asynch_IO.h"

template <class READER, class WRITER>
class ACE_Asynch_RW_T 
{
public:
  
  /// A do nothing constructor.
  ACE_Asynch_RW_T (void);

  /// Destructor
  virtual ~ACE_Asynch_RW_T (void);

  /**
   * Initializes the factory with information which will be used with
   * each asynchronous call. If (<handle> == ACE_INVALID_HANDLE),
   * <ACE_Handler::handle> will be called on the <handler> to get the
   * correct handle.
   */
  int open (ACE_Handler &handler,
            ACE_HANDLE handle = ACE_INVALID_HANDLE,
            const void *completion_key = 0,
            ACE_Proactor *proactor = 0,
            int pass_ownership=0);

  int cancel (void);
  int close  (void);
  int cancel_and_close (void);

  int should_finish (void) const { return flg_cancel_ != 0;}

  
  READER &  reader (void) { return rs_;}
  WRITER &  writer (void) { return ws_;}

 
protected:

  /// Implementation class that all methods will be forwarded to.
  ACE_HANDLE handle_;
  int flg_owner_; 
  int flg_cancel_;

  const void * completion_key_;
  READER rs_;
  WRITER ws_;

};

template <class READER, class WRITER>
class ACE_Asynch_RW_Stream_T :  public ACE_Asynch_RW_T<READER, WRITER>
{
public:
  typedef ACE_Asynch_Result::Buffer_Info Buffer_Info;
  
  /// A do nothing constructor.
  ACE_Asynch_RW_Stream_T (void) {;}

  /// Destructor
  virtual ~ACE_Asynch_RW_Stream_T (void) {;}

  int read (const Buffer_Info& buf_info,
            size_t num_bytes_to_read,
            const void *act = 0,
            int priority = 0,
            int signal_number = ACE_SIGRTMIN);

  int write (const Buffer_Info& buf_info,
            size_t num_bytes_to_write,
            const void *act = 0,
            int priority = 0,
            int signal_number = ACE_SIGRTMIN);
  

};

template <class READER, class WRITER>
class ACE_Asynch_RW_Dgram_T :  public ACE_Asynch_RW_T<READER, WRITER>
{
public:
  typedef ACE_Asynch_Result::Buffer_Info Buffer_Info;
  
  /// A do nothing constructor.
  ACE_Asynch_RW_Dgram_T (void) {;}

  /// Destructor
  virtual ~ACE_Asynch_RW_Dgram_T (void) {;}

  ssize_t recv  (const Buffer_Info&   buf_info,  
                 size_t &             number_of_bytes_recvd,  
                 int                  flags,  
                 int                  protocol_family = PF_INET,  
                 const void *         act = 0,
                 int                  priority = 0,  
                 int                  signal_number = ACE_SIGRTMIN);

  ssize_t send  (const Buffer_Info&   buf_info,  
                 size_t &            number_of_bytes_sent,  
                 int                 flags,  
                 const ACE_Addr &    remote_addr,  
                 const void *        act = 0,  
                 int                 priority = 0,  
                 int                 signal_number = ACE_SIGRTMIN);  
 

};

#if defined (ACE_TEMPLATES_REQUIRE_SOURCE)
#include "Asynch_RW.cpp"
#endif /* ACE_TEMPLATES_REQUIRE_SOURCE */

#if defined (ACE_TEMPLATES_REQUIRE_PRAGMA)
#pragma implementation ("Asynch_RW.cpp")
#endif /* ACE_TEMPLATES_REQUIRE_PRAGMA */
  
typedef ACE_Asynch_RW_Stream_T<ACE_Asynch_Read_Stream,ACE_Asynch_Write_Stream> 
        Asynch_RW_Stream;

typedef ACE_Asynch_RW_Dgram_T<ACE_Asynch_Read_Dgram,ACE_Asynch_Write_Dgram> 
        Asynch_RW_Dgram;

#endif //ASYNCH_RW_H
