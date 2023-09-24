# contrib/spq_plugin/src/spq/Makefile
subdir = src/spq
top_builddir = ../../../..
include $(top_builddir)/src/Makefile.global
include $(top_builddir)/contrib/spq_plugin/spqplugin.mk

override CPPFLAGS := -I$(abs_top_srcdir)/contrib/spq_plugin/include \
                     $(CPPFLAGS)

OBJS = spq_cat.o \
       spq_hash.o \
       spq_mutate.o \
       spq_plan.o \
       spq_util.o \
       uri.o

include $(top_srcdir)/src/gausskernel/common.mk