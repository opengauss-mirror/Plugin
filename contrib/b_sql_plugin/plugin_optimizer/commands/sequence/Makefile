#---------------------------------------------------------------------------------------
#     
# IDENTIFICATION
#        src/gausskernel/optimizer/commands/sequence/Makefile
#
# ---------------------------------------------------------------------------------------

subdir = contrib/b_sql_plugin/plugin_optimizer/commands/sequence
top_builddir = ../../../../..
base_dir = contrib/b_sql_plugin
include $(top_builddir)/src/Makefile.global
include $(top_builddir)/$(base_dir)/configure.mk
ifneq "$(MAKECMDGOALS)" "clean"
  ifneq "$(MAKECMDGOALS)" "distclean"
    ifneq "$(shell which g++ |grep hutaf_llt |wc -l)" "1"
      -include $(DEPEND)
    endif
  endif
endif

OBJS = sequence.o sequence_util.o

include $(top_srcdir)/src/gausskernel/common.mk
