#
# Makefile for utils
#
# src/common/backend/utils/Makefile
#

subdir = contrib/b_sql_plugin/plugin_utils
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global
base_dir = contrib/b_sql_plugin
include $(top_builddir)/$(base_dir)/configure.mk

ifneq "$(MAKECMDGOALS)" "clean"
  ifneq "$(MAKECMDGOALS)" "distclean"
    ifneq "$(shell which g++ |grep hutaf_llt |wc -l)" "1"
      -include $(DEPEND)
    endif
  endif
endif
OBJS        = fmgrtab.o
SUBDIRS     = adt mb

# location of Catalog.pm
catalogdir  = $(top_builddir)/src/common/backend/catalog

include $(top_builddir)/src/gausskernel/common.mk

all: fmgroids.h

$(SUBDIRS:%=%-recursive): fmgroids.h

# see explanation in ../parser/Makefile
fmgroids.h: fmgrtab.cpp
  fmgrtab.cpp: $(top_builddir)/src/common/backend/utils/Gen_fmgrtab.pl  $(top_builddir)/$(subdir)/../include/builtin_funcs.ini
	  $(PERL)  $< $(top_builddir)/$(subdir)/../include/builtin_funcs.ini
  pg_builtin_proc.h: $(top_builddir)/src/common/backend/utils/Gen_fmgrtab.pl  $(top_builddir)/$(subdir)/../include/builtin_funcs.ini
	  $(PERL)  $< $(top_builddir)/$(subdir)/../include/builtin_funcs.ini

maintainer-clean: clean
	rm -f fmgroids.h fmgrtab.cpp pg_builtin_proc.h
