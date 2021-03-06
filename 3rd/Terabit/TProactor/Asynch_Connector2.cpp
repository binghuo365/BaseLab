/* -*- C++ -*- */
// Asynch_Connector2.cpp,v 1.4 2002/06/16 00:32:32 shuston Exp

#ifndef TPROACTOR_ASYNCH_CONNECTOR2_C 
#define TPROACTOR_ASYNCH_CONNECTOR2_C 

#include "TProactor/Asynch_Connector2.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

ACE_RCSID(ace, Asynch_Connector2, "Asynch_Connector2.cpp,v 1.4 2002/06/16 00:32:32 shuston Exp")

#if defined (ACE_WIN32) || defined (ACE_HAS_AIO_CALLS) || defined(ACE_HAS_AIO_EMULATION)
// This only works on platforms that support async I/O.


#include "ace/OS_NS_sys_socket.h"
#include "ace/OS_Memory.h"
#include "ace/ACE.h"

#include "ace/Flag_Manip.h"
#include "ace/Log_Msg.h"
#include "ace/Message_Block.h"
#include "ace/INET_Addr.h"

ACE_BEGIN_VERSIONED_NAMESPACE_DECL

template <class HANDLER>
TRB_Asynch_Connector2<HANDLER>::TRB_Asynch_Connector2 (void)
: monitor_        ()
, flg_cancel_     (false)
, pending_count_  (0)
, asynch_connect_ ()
, pass_addresses_ (0)
, validate_new_connection_ (0)
{
}

template <class HANDLER>
TRB_Asynch_Connector2<HANDLER>::~TRB_Asynch_Connector2 (void)
{
  this->cancel ();
  this->wait ();
}


template <class HANDLER> int
TRB_Asynch_Connector2<HANDLER>::get_pending_count (void) const
{
  Guard_Monitor guard (monitor_);
  return this->pending_count_;
}

template <class HANDLER> int
TRB_Asynch_Connector2<HANDLER>::open (int pass_addresses,
                                     TRB_Proactor *proactor,
                                     int validate_new_connection)
{
  ACE_TRACE ("TRB_Asynch_Connector2<>::open");

  Guard_Monitor guard (monitor_);

  if (this->pending_count_ != 0)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_LIB_TEXT ("%N:%l %p\n"),
                       ACE_LIB_TEXT ("already open")),
                      -1);
    }

  this->flg_cancel_ = false;  // reset previous cancel

  this->proactor (proactor);
  this->pass_addresses_ = pass_addresses;
  this->validate_new_connection_ = validate_new_connection;

  // Initialize the TRB_Asynch_Connect
  if (this->asynch_connect_.open (*this,
                                  ACE_INVALID_HANDLE,
                                  0,
                                  this->proactor ()) == -1)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_LIB_TEXT ("%p\n"),
                       ACE_LIB_TEXT ("TRB_Asynch_Connect::open")),
                      -1);
    }
  return 0;
}


template <class HANDLER> int
TRB_Asynch_Connector2<HANDLER>::connect (const ACE_INET_Addr & remote_sap,
                                        const ACE_INET_Addr & local_sap,
                                        int reuse_addr,
                                        const void *act)
{
  Guard_Monitor guard (monitor_);

  if (this->flg_cancel_)
    {
      return -1;
    }


  // Initiate asynchronous connect
  if (this->asynch_connect_.connect (ACE_INVALID_HANDLE,
                                     remote_sap,
                                     local_sap,
                                     reuse_addr,
                                     act) == -1)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_LIB_TEXT ("%N:%l %p\n"),
                       ACE_LIB_TEXT ("TRB_Asynch_Connector2::connect")),
                      -1);
    }

  ++this->pending_count_;
  return 0;
}

template <class HANDLER> void
TRB_Asynch_Connector2<HANDLER>::handle_connect (const TRB_Asynch_Connect::Result &result)
{
  // Variable for error tracking
  int error = 0;

  // If the asynchronous connect fails.
  if (!result.success () ||
      result.connect_handle () == ACE_INVALID_HANDLE)
    {
      error = 1;
    }

  if (result.error () != 0)
    {
      error = 1;
    }

  // set blocking mode 
  if (!error &&
      ACE::clr_flags (result.connect_handle (), ACE_NONBLOCK) != 0)
    {
      error = 1;
      ACE_ERROR ((LM_ERROR,
                  ACE_LIB_TEXT ("%p\n"),
                  ACE_LIB_TEXT ("TRB_Asynch_Connector2::handle_connect : Set blocking mode")));
    }


  // Parse addresses.
  const ACE_INET_Addr & local_address = 
    static_cast<const ACE_INET_Addr &> (result.local_address ());
    
  const ACE_INET_Addr & remote_address =
    static_cast<const ACE_INET_Addr &> (result.remote_address ());

  // Call validate_connection even if there was an error - it's the only
  // way the application can learn the connect disposition.
  if (this->validate_new_connection_ &&
      this->validate_connection (result, remote_address, local_address) == -1)
    {
      error = 1;
    }

  HANDLER *new_handler = 0;
  if (!error)
    {
      // The Template method
      new_handler = this->make_handler (result.act ());
      if (new_handler == 0)
        {
          error = 1;
          ACE_ERROR ((LM_ERROR,
                      ACE_LIB_TEXT ("%p\n"),
                      ACE_LIB_TEXT ("TRB_Asynch_Connector2::handle_connect : Making of new handler failed")));
        }
    }

  // If no errors
  if (!error)
    {
      // Update the Proactor.
      new_handler->proactor (this->proactor ());

      // Pass the addresses
      if (this->pass_addresses_)
        new_handler->addresses (remote_address,
                                local_address);

      // Pass the ACT
      new_handler->act (result.act ());

      // Set up the handler's new handle value
      new_handler->handle (result.connect_handle ());

      ACE_Message_Block  mb;

      // Initiate the handler with empty message block;
      new_handler->open (result.connect_handle (), mb);
    }

  // On failure, no choice but to close the socket
  if (error &&
      result.connect_handle() != ACE_INVALID_HANDLE)
    {
      ACE_OS::closesocket (result.connect_handle ());
    }


  Guard_Monitor guard (monitor_);

  --this->pending_count_;
  if (this->pending_count_ == 0 && flg_cancel_)
    {
      guard.broadcast (); // notify all waiters
    }
}

template <class HANDLER> int
TRB_Asynch_Connector2<HANDLER>::validate_connection
  (const TRB_Asynch_Connect::Result &,
   const ACE_INET_Addr & /* remote_address */,
   const ACE_INET_Addr & /* local_address */)
{
  // Default implementation always validates the remote address.
  return 0;
}

template <class HANDLER> void
TRB_Asynch_Connector2<HANDLER>::wait (void)
{
  Guard_Monitor guard (monitor_);
  while (this->pending_count_ != 0)
    {
      guard.wait ();
    }
}

template <class HANDLER> int
TRB_Asynch_Connector2<HANDLER>::cancel (void)
{
  ACE_TRACE ("TRB_Asynch_Connector2<>::cancel");

  Guard_Monitor guard (monitor_);
  
  if (this->flg_cancel_)
     return 0;

  this->flg_cancel_ = true;

  //TODO: Windows cancel does not help
  // Compare: in Asynch_Acceptor we can close listen_handle
  // that makes real cancellation
  // POSIX: TProactor can handle cancel 
  return this->asynch_connect_.cancel ();
}

template <class HANDLER> HANDLER *
TRB_Asynch_Connector2<HANDLER>::make_handler (const void * /*act*/)
{
  return make_handler();
}

template <class HANDLER> HANDLER *
TRB_Asynch_Connector2<HANDLER>::make_handler (void)
{
  // Default behavior
  HANDLER *handler = 0;
  ACE_NEW_RETURN (handler, 
                  HANDLER,
                  0);
  return handler;
}

template <class HANDLER> int
TRB_Asynch_Connector2<HANDLER>::pass_addresses (void) const
{
  return this->pass_addresses_;
}

template <class HANDLER> void
TRB_Asynch_Connector2<HANDLER>::pass_addresses (int new_value)
{
  this->pass_addresses_ = new_value;
}

template <class HANDLER> int
TRB_Asynch_Connector2<HANDLER>::validate_new_connection (void) const
{
  return this->validate_new_connection_;
}

template <class HANDLER> void
TRB_Asynch_Connector2<HANDLER>::validate_new_connection (int new_value)
{
  this->validate_new_connection_ = new_value;
}

ACE_END_VERSIONED_NAMESPACE_DECL

#endif /* ACE_WIN32 || ACE_HAS_AIO_CALLS */
#endif //TPROACTOR_ASYNCH_CONNECTOR2_C 
