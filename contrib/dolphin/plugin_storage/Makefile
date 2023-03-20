#
# Makefile for plugin_storage
#
# contrib/dolphin/plugin_storage/Makefile
#

subdir = contrib/dolphin/plugin_storage
top_builddir = ../../../
include $(top_builddir)/src/Makefile.global
base_dir = contrib/dolphin
include $(top_builddir)/$(base_dir)/configure.mk

ifneq "$(MAKECMDGOALS)" "clean"
  ifneq "$(MAKECMDGOALS)" "distclean"
    ifneq "$(shell which g++ |grep hutaf_llt |wc -l)" "1"
      -include $(DEPEND)
    endif
  endif
endif

OBJS = hashfunc.o tupdesc.o

include $(top_builddir)/src/gausskernel/common.mk
