# -*- makefile -*-
#----------------------------------------------------------------------------
#       GNU ACE Workspace
#
# $Id: GNUACEWorkspaceCreator.pm 80826 2008-03-04 14:51:23Z wotte $
#
# This file was generated by MPC.  Any changes made directly to
# this file will be lost the next time it is generated.
#
# MPC Command:
# /export/anduriltmp/bczar/DOC_ROOT/stage-4800/ACE_wrappers/bin/mwc.pl -type gnuace -exclude "TAO/TAO_*.mwc,TAO/CIAO/CIAO_*.mwc" -recurse -hierarchy -relative ACE_ROOT=/export/anduriltmp/bczar/DOC_ROOT/stage-4800/ACE_wrappers -relative TAO_ROOT=/export/anduriltmp/bczar/DOC_ROOT/stage-4800/ACE_wrappers/TAO -relative CIAO_ROOT=/export/anduriltmp/bczar/DOC_ROOT/stage-4800/ACE_wrappers/TAO/CIAO
#
#----------------------------------------------------------------------------

MAKEFILE = GNUmakefile

ifeq ($(findstring k,$(MAKEFLAGS)),k)
  KEEP_GOING = -
endif

include $(ACE_ROOT)/include/makeinclude/macros.GNU

all: Send_Msg_Receiver Send_Msg_Sender Member

REMAINING_TARGETS := $(subst all, , $(TARGETS_NESTED:.nested=)) $(CUSTOM_TARGETS)

$(REMAINING_TARGETS):
	@cd RMCast/Send_Msg && $(MAKE) -f GNUmakefile.Send_Msg_Receiver $(@)
	@cd RMCast/Send_Msg && $(MAKE) -f GNUmakefile.Send_Msg_Sender $(@)
	@cd TMCast/Member && $(MAKE) -f GNUmakefile.Member $(@)

.PHONY: Send_Msg_Receiver
Send_Msg_Receiver:
	@cd RMCast/Send_Msg && $(MAKE) -f GNUmakefile.Send_Msg_Receiver all

.PHONY: Send_Msg_Sender
Send_Msg_Sender:
	@cd RMCast/Send_Msg && $(MAKE) -f GNUmakefile.Send_Msg_Sender all

.PHONY: Member
Member:
	@cd TMCast/Member && $(MAKE) -f GNUmakefile.Member all

project_name_list:
	@echo Send_Msg_Receiver
	@echo Send_Msg_Sender
	@echo Member
