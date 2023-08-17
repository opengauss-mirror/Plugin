#
# Makefile for plugin_vector
#
# contrib/whale/plugin_vector/Makefile
#

subdir = contrib/whale/plugin_vector
top_builddir = ../../../
include $(top_builddir)/src/Makefile.global
base_dir = contrib/whale
include $(top_builddir)/$(base_dir)/configure.mk
# seems to be required for some date/time stuff 1999/07/22 bjm
ifeq "$(findstring alpha,$(host_cpu))" "alpha"
ifeq "$(GCC)" "yes"
override CFLAGS+= -mieee -I$(top_builddir)/$(subdir)/../include -fvisibility=hidden -fPIC
endif
endif

ifneq "$(MAKECMDGOALS)" "clean"
  ifneq "$(MAKECMDGOALS)" "distclean"
    ifneq "$(shell which g++ |grep hutaf_llt |wc -l)" "1"
      -include $(DEPEND)
    endif
  endif
endif
OBJS = vecfuncache.o

include $(top_srcdir)/src/gausskernel/common.mk
