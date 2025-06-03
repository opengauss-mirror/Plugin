# contrib/spq_plugin/src/executor/Makefile
subdir = src/executor
top_builddir = ../../../..
include $(top_builddir)/src/Makefile.global
include $(top_builddir)/contrib/spq_plugin/spqplugin.mk

override CPPFLAGS := -I$(abs_top_srcdir)/contrib/ndpplugin/component/thread \
                     -I$(abs_top_srcdir)/contrib/spq_plugin/include \
                     -I$(abs_top_srcdir)/src/include \
                     $(CPPFLAGS)

OBJS = spq_seqscan.o spq_indexscan.o spq_indexonlyscan.o spq_bitmapheapscan.o spq_cstorescan.o

include $(top_srcdir)/src/gausskernel/common.mk