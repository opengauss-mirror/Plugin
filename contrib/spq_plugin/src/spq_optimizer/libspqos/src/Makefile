#
# Makefile for optimizer
#
# src/spq_optimizer/libspqos/src/Makefile
#
subdir = src/spq_optimizer/libspqos/src
top_builddir = ../../../../../..
include $(top_builddir)/src/Makefile.global
include ../../spqorca.mk

OBJS = _api.o utils.o

SUBDIRS = common \
		  error \
		  io \
		  memory \
		  string \
		  task

include $(top_srcdir)/src/gausskernel/common.mk