/* -*- C++ -*- */
// Asynch_Acceptor.cpp,v 4.47 2002/10/05 00:25:52 shuston Exp

#ifndef TPROACTOR_ASYNCH_ACCEPTOR_C 
#define TPROACTOR_ASYNCH_ACCEPTOR_C 

#include "TProactor/Asynch_Acceptor.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

ACE_RCSID(ace, Asynch_Acceptor, "Asynch_Acceptor.cpp,v 4.47 2002/10/05 00:25:52 shuston Exp")

#if defined (ACE_WIN32) || defined (ACE_HAS_AIO_CALLS) || defined(ACE_HAS_AIO_EMULATION)

// This only works on platforms that support async i/o.


#include "ace/OS_Errno.h"
#include "ace/OS_Memory.h"
#include "ace/OS_NS_sys_socket.h"


#include "ace/Log_Msg.h"
#include "ace/Message_Block.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/Sock_Connect.h"
#include "ace/Trace.h"

ACE_BEGIN_VERSIONED_NAMESPACE_DECL

template <class HANDLER>
TRB_Asynch_Acceptor<HANDLER>::TRB_Asynch_Acceptor (void)
  : listen_handle_ (ACE_INVALID_HANDLE)
  , asynch_accept_ ()
  , pass_addresses_ (0)
  , validate_new_connection_ (0)
  , reissue_accept_ (1)
  , bytes_to_read_ (0)
{
}

template <class HANDLER>
TRB_Asynch_Acceptor<HANDLER>::~TRB_Asynch_Acceptor (void)
{
  //this->asynch_accept_.close ();
  // Close down the listen socket
  if (this->listen_handle_ != ACE_INVALID_HANDLE)
     ACE_OS::closesocket (this->listen_handle_);
}

template <class HANDLER> int
TRB_Asynch_Acceptor<HANDLER>::open (const ACE_INET_Addr &address,
                                    size_t bytes_to_read,
                                    int pass_addresses,
                                    int backlog,
                                    int reuse_addr,
                                    TRB_Proactor *proactor,
                                    int validate_new_connection,
                                    int reissue_accept,
                                    int number_of_initial_accepts)
{
  ACE_TRACE ("TRB_Asynch_Acceptor<>::open");

  this->proactor (proactor);
  this->pass_addresses_ = pass_addresses;
  this->bytes_to_read_ = bytes_to_read;
  this->validate_new_connection_ = validate_new_connection;
  this->reissue_accept_ = reissue_accept;

  // Create the listener socket
  this->listen_handle_ = ACE_OS::socket (address.get_type (), SOCK_STREAM, 0);
  if (this->listen_handle_ == ACE_INVALID_HANDLE)
  {
    ACE_Errno_Guard g (errno);
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_LIB_TEXT ("%N:%l %p\n"),
                       ACE_LIB_TEXT ("ACE_OS::socket")),
                      -1);
  }

  if (reuse_addr)
  {
      // Reuse the address
      int one = 1;
      if (ACE_OS::setsockopt (this->listen_handle_,
                              SOL_SOCKET,
                              SO_REUSEADDR,
                              (const char*) &one,
                              sizeof one) == -1)
    {
        ACE_Errno_Guard g (errno);
        ACE_ERROR_RETURN ((LM_ERROR,
                           ACE_LIB_TEXT ("%N:%l %p\n"),
                           ACE_LIB_TEXT ("ACE_OS::setsockopt")),
                          -1);
    }
  }

  // If port is not specified, bind to any port.
  static ACE_INET_Addr sa (ACE_sap_any_cast (const ACE_INET_Addr &));

  if (address == sa &&
      ACE::bind_port (this->listen_handle_) == -1)
  {
    ACE_Errno_Guard g (errno);
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_LIB_TEXT ("%N:%l %p\n"),
                       ACE_LIB_TEXT ("ACE::bind_port")),
                      -1);
  }

  // Bind to the specified port.
  if (ACE_OS::bind (this->listen_handle_,
                    ACE_reinterpret_cast (sockaddr *,
                                          address.get_addr ()),
                    address.get_size ()) == -1)
  {
    ACE_Errno_Guard g (errno);
    ACE_ERROR_RETURN ((LM_ERROR,
                       "%N:%l %p\n",
                       "ACE_OS::bind"),
                      -1);
  }

  // Start listening.
  if (ACE_OS::listen (this->listen_handle_, backlog) == -1)
  {
    ACE_Errno_Guard g (errno);
    ACE_ERROR_RETURN ((LM_ERROR,
                       "%N:%l %p\n",
                       "ACE_OS::listen"),
                      -1);
  }
  
  // Initialize the TRB_Asynch_Accept
  if (this->asynch_accept_.open (*this,
                                 this->listen_handle_,
                                 0,
                                 this->proactor ()) == -1)
  {
    ACE_Errno_Guard g (errno);
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_LIB_TEXT ("%N:%l %p\n"),
                       ACE_LIB_TEXT ("TRB_Asynch_Accept::open")),
                      -1);
  }


  // For the number of <intial_accepts>.
  if (number_of_initial_accepts == -1)
    number_of_initial_accepts = backlog;

  for (int i = 0; i < number_of_initial_accepts; i++)
  {
    // Initiate accepts.
    if (this->accept (bytes_to_read) == -1)
    {
      ACE_Errno_Guard g (errno);
      ACE_ERROR_RETURN ((LM_ERROR,
                         "%N:%l %p\n",
                         "TRB_Asynch_Acceptor::accept"),
                        -1);
    }
  }
  return 0;
}

template <class HANDLER> void
TRB_Asynch_Acceptor<HANDLER>::set_handle (ACE_HANDLE listen_handle)
{
  ACE_TRACE ("TRB_Asynch_Acceptor<>::set_handle");

  // Take ownership of the <listen_handle>
  this->listen_handle_ = listen_handle;

  // Reinitialize the TRB_Asynch_Accept
  if (this->asynch_accept_.open (*this,
                                 this->listen_handle_,
                                 0,
                                 this->proactor ()) == -1)
    ACE_ERROR ((LM_ERROR,
                ACE_LIB_TEXT ("%p\n"),
                ACE_LIB_TEXT ("TRB_Asynch_Accept::open")));
}

template <class HANDLER> ACE_HANDLE
TRB_Asynch_Acceptor<HANDLER>::get_handle (void) const
{
  return this->listen_handle_;
}

template <class HANDLER> int
TRB_Asynch_Acceptor<HANDLER>::accept (size_t bytes_to_read, const void *act)
{
  ACE_TRACE ("TRB_Asynch_Acceptor<>::accept");

  if (bytes_to_read == 0)
    return this->asynch_accept_.accept (act);
  
  ACE_Message_Block *message_block = 0;
  
  
  size_t space_needed = bytes_to_read + 2 *( 16 + this->address_size ());

  // Create a new message block big enough for the addresses and data
  ACE_NEW_RETURN (message_block,
                  ACE_Message_Block (space_needed),
                  -1);

  // Initiate asynchronous accepts
  if (this->asynch_accept_.accept (*message_block,
                                   bytes_to_read,
                                   act) == -1)
    {
      // Cleanup on error
      message_block->release ();
      return -1;
    }
  return 0;
}

template <class HANDLER> void
TRB_Asynch_Acceptor<HANDLER>::handle_accept (const TRB_Asynch_Accept::Result &result)
{
  ACE_TRACE ("TRB_Asynch_Acceptor<>::handle_accept");

  // Variable for error tracking
  int error = 0;

  // If the asynchronous accept fails.
  if (!result.success () ||
      result.accept_handle() == ACE_INVALID_HANDLE )
    {
      error = 1;
    }

  // Parse addresses.
  const ACE_INET_Addr & local_address = 
    static_cast<const ACE_INET_Addr &> (result.local_address ());
    
  const ACE_INET_Addr & remote_address =
    static_cast<const ACE_INET_Addr &> (result.remote_address ());
    
  // Validate remote address
  // Call validate_connection even if there was an error - it's the only
  // way the application can learn the accept disposition.
  if ( this->validate_new_connection_ &&
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
        }
    }

  ACE_Message_Block *mb = result.get_message_block_ptr ();
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
      new_handler->handle (result.accept_handle ());

      // Initiate the handler
      
        
      new_handler->open (result.accept_handle (),
                         mb ? *mb : dummy_msg_);
    }

  // On failure, no choice but to close the socket
  if (error &&
      result.accept_handle() != ACE_INVALID_HANDLE )
    ACE_OS::closesocket (result.accept_handle ());

  // Delete the dynamically allocated message_block
  if (mb != 0)
    mb->release ();

  // Start off another asynchronous accept to keep the backlog going
  if (this->should_reissue_accept ())
    this->accept (this->bytes_to_read_);
}

template <class HANDLER> int
TRB_Asynch_Acceptor<HANDLER>::validate_connection
  (const TRB_Asynch_Accept::Result&  result,
   const ACE_INET_Addr& /* remote */,
   const ACE_INET_Addr& /* local */)
{
  // Default implementation always validates the remote address.
  
  if (result.success())
    return 0;

  this->reissue_accept (0);

  return -1;
}

template <class HANDLER> int
TRB_Asynch_Acceptor<HANDLER>::validate_new_connection (const ACE_INET_Addr&)
{
  // Default implementation always validates the remote address.
  return 0;
}

template <class HANDLER> int
TRB_Asynch_Acceptor<HANDLER>::cancel (void)
{
  ACE_TRACE ("TRB_Asynch_Acceptor<>::cancel");

  // prevent reissue new accepts
  this->reissue_accept (0);

  // All I/O operations that are canceled will complete with the error
  // ERROR_OPERATION_ABORTED. All completion notifications for the I/O
  // operations will occur normally.

  int rc = this->asynch_accept_.cancel();

  // This only one way to stop all pending accepts
  // after close
  // 
  if (this->listen_handle_ != ACE_INVALID_HANDLE)
    {
      ACE_OS::closesocket (this->listen_handle_);
      this->listen_handle_ = ACE_INVALID_HANDLE;
    }

  return rc;
}


template <class HANDLER> ACE_HANDLE
TRB_Asynch_Acceptor<HANDLER>::handle (void) const
{
  return this->listen_handle_;
}

template <class HANDLER> void
TRB_Asynch_Acceptor<HANDLER>::handle (ACE_HANDLE h)
{
  TRB_Handler::handle (h);
}

template <class HANDLER> TRB_Asynch_Accept &
TRB_Asynch_Acceptor<HANDLER>::asynch_accept (void)
{
  return this->asynch_accept_;
}

template <class HANDLER> HANDLER *
TRB_Asynch_Acceptor<HANDLER>::make_handler (const void * /*act*/)
{
  return make_handler();
}

template <class HANDLER> HANDLER *
TRB_Asynch_Acceptor<HANDLER>::make_handler (void)
{
  // Default behavior
  HANDLER *handler = 0;
  ACE_NEW_RETURN (handler,
                  HANDLER,
                  0);
  return handler;
}

/* static */
template <class HANDLER> size_t
TRB_Asynch_Acceptor<HANDLER>::address_size (void)
{
  return sizeof (sockaddr) + sizeof (sockaddr_in);
}

template <class HANDLER> int
TRB_Asynch_Acceptor<HANDLER>::pass_addresses (void) const
{
  return this->pass_addresses_;
}

template <class HANDLER> void
TRB_Asynch_Acceptor<HANDLER>::pass_addresses (int new_value)
{
  this->pass_addresses_ = new_value;
}

template <class HANDLER> int
TRB_Asynch_Acceptor<HANDLER>::validate_new_connection (void) const
{
  return this->validate_new_connection_;
}

template <class HANDLER> void
TRB_Asynch_Acceptor<HANDLER>::validate_new_connection (int new_value)
{
  this->validate_new_connection_ = new_value;
}

template <class HANDLER> int
TRB_Asynch_Acceptor<HANDLER>::reissue_accept (void) const
{
  return this->reissue_accept_;
}

template <class HANDLER> void
TRB_Asynch_Acceptor<HANDLER>::reissue_accept (int new_value)
{
  this->reissue_accept_ = new_value;
}

template <class HANDLER> size_t
TRB_Asynch_Acceptor<HANDLER>::bytes_to_read (void) const
{
  return this->bytes_to_read_;
}

template <class HANDLER> void
TRB_Asynch_Acceptor<HANDLER>::bytes_to_read (size_t new_value)
{
  this->bytes_to_read_ = new_value;
}

template <class HANDLER> int
TRB_Asynch_Acceptor<HANDLER>::should_reissue_accept (void)
{
  return this->reissue_accept_;
}

ACE_END_VERSIONED_NAMESPACE_DECL

#endif /* ACE_WIN32 || ACE_HAS_AIO_CALLS */
#endif //TPROACTOR_ASYNCH_ACCEPTOR_C 
