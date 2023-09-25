#
# Makefile for optimizer
#
# src/spq_optimizer/libspqopt/src/Makefile
#
subdir = src/spq_optimizer/libspqopt/src
top_builddir = ../../../../../..
include $(top_builddir)/src/Makefile.global
include ../../spqorca.mk

OBJS = exception.o \
       init.o

SUBDIRS = base \
          engine \
          eval \
          mdcache \
          metadata \
          minidump \
          operators \
          optimizer \
          search \
          translate \
          xforms

include $(top_srcdir)/src/gausskernel/common.mk