#---------------------------------------------------------------------------------------
#
#  IDENTIFICATION
#        src/gausskernel/optimizer/Makefile
#
# ---------------------------------------------------------------------------------------

subdir = contrib/b_sql_plugin/plugin_optimizer
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global
override CPPFLAGS :=$(filter-out -fPIE, $(CPPFLAGS)) -fPIC -I. -I$(srcdir) $(CPPFLAGS) -I$(top_builddir)/$(subdir)/../include -fvisibility=hidden

SUBDIRS     = commands

include $(top_srcdir)/src/gausskernel/common.mk
