#
# Makefile for optimizer
#
# src/spq_optimizer/libnaucrates/src/Makefile
#
subdir = src/spq_optimizer/libnaucrates/src
top_builddir = ../../../../../..
include $(top_builddir)/src/Makefile.global
include ../../spqorca.mk

OBJS = CCostModelConfigSerializer.o \
       CDXLUtils.o \
       CIdGenerator.o \
       exception.o \
       init.o \
       traceflags.o

SUBDIRS = base \
          md \
          operators \
          parser \
          statistics \
          xml

include $(top_srcdir)/src/gausskernel/common.mk