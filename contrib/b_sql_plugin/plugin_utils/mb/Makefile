#-------------------------------------------------------------------------
#
# Makefile--
#    Makefile for utils/mb
#
# IDENTIFICATION
#    src/common/backend/utils/mb/Makefile
#
#-------------------------------------------------------------------------

subdir = contrib/b_sql_plugin/plugin_utils/mb
top_builddir = ../../../..
include $(top_builddir)/src/Makefile.global

override CPPFLAGS :=$(filter-out -fPIE, $(CPPFLAGS)) -fPIC -I. -I$(srcdir) $(CPPFLAGS) -I$(top_builddir)/$(subdir)/../../include -fvisibility=hidden

ifneq "$(MAKECMDGOALS)" "clean"
  ifneq "$(MAKECMDGOALS)" "distclean"
    ifneq "$(shell which g++ |grep hutaf_llt |wc -l)" "1"
      -include $(DEPEND)
    endif
  endif
endif
OBJS = mbutils.o

include $(top_srcdir)/src/gausskernel/common.mk

clean distclean maintainer-clean:
	$(MAKE) -C conversion_procs $@
