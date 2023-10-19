# contrib/spq_plugin/src/Makefile
subdir = src
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global
include $(top_builddir)/contrib/spq_plugin/spqplugin.mk

override CPPFLAGS := -I$(abs_top_srcdir)/contrib/spq_plugin/include \
                     -I$(abs_top_srcdir)/contrib/spq_plugin/src/spq_optimizer/libnaucrates/include \
                     $(CPPFLAGS)

OBJS = guc_spq.o spq_opt.o spqplugin.o

SUBDIRS = executor \
          spq \
          spq_optimizer \
          spq_optimizer_util

include $(top_srcdir)/src/gausskernel/common.mk
