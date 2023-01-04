#---------------------------------------------------------------------------------------
#     
# IDENTIFICATION
#        src/gausskernel/optimizer/commands/Makefile
#
# ---------------------------------------------------------------------------------------

subdir = contrib/dolphin/plugin_optimizer/prep
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


OBJS = prepunion.o 

include $(top_srcdir)/src/gausskernel/common.mk

