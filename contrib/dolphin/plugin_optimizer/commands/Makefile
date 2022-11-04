#---------------------------------------------------------------------------------------
#     
# IDENTIFICATION
#        src/gausskernel/optimizer/commands/Makefile
#
# ---------------------------------------------------------------------------------------

subdir = contrib/dolphin/plugin_optimizer/commands
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


OBJS = functioncmds.o foreigncmds.o copy.o

include $(top_srcdir)/src/gausskernel/common.mk

