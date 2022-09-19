#
# Makefile for utils/adt/show
#
# src/common/backend/utils/adt/show/Makefile
#

subdir = contrib/dolphin/plugin_utils/adt/show
top_builddir = ../../../../../
include $(top_builddir)/src/Makefile.global
base_dir = contrib/dolphin
include $(top_builddir)/$(base_dir)/configure.mk
# seems to be required for some date/time stuff 1999/07/22 bjm
ifeq "$(findstring alpha,$(host_cpu))" "alpha"
ifeq "$(GCC)" "yes"
override CFLAGS+= -mieee -I$(top_builddir)/$(subdir)/../include -fvisibility=hidden -fPIC
endif
endif
OBJS = show_grants.o show_common.o show_function.o show_trigger.o

include $(top_srcdir)/src/gausskernel/common.mk
