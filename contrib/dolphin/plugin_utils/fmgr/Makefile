#-------------------------------------------------------------------------
#
# Makefile--
#    Makefile for utils/fmgr
#
# IDENTIFICATION
#    src/common/backend/utils/fmgr/Makefile
#
#-------------------------------------------------------------------------

subdir = contrib/dolphin/plugin_utils/fmgr
top_builddir = ../../../..
base_dir = contrib/dolphin
include $(top_builddir)/src/Makefile.global
include $(top_builddir)/$(base_dir)/configure.mk

ifneq "$(MAKECMDGOALS)" "clean"
  ifneq "$(MAKECMDGOALS)" "distclean"
    ifneq "$(shell which g++ |grep hutaf_llt |wc -l)" "1"
      -include $(DEPEND)
    endif
  endif
endif
OBJS = fmgr.o

override CPPFLAGS += -DDLSUFFIX=\"$(DLSUFFIX)\" -I${top_builddir}/contrib/log_fdw/
override CPPFLAGS += -DDLSUFFIX=\"$(DLSUFFIX)\" -I${top_builddir}/contrib/gc_fdw/

include $(top_srcdir)/src/gausskernel/common.mk
