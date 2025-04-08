#
# Makefile for utils/adt
#
# src/common/backend/utils/adt/Makefile
#

subdir = contrib/dolphin/plugin_utils/adt
top_builddir = ../../../../
include $(top_builddir)/src/Makefile.global
base_dir = contrib/dolphin
include $(top_builddir)/$(base_dir)/configure.mk
# seems to be required for some date/time stuff 1999/07/22 bjm
ifeq "$(findstring alpha,$(host_cpu))" "alpha"
ifeq "$(GCC)" "yes"
override CFLAGS+= -mieee -I$(top_builddir)/$(subdir)/../include -fvisibility=hidden -fPIC
endif
endif
SUBDIRS = show

ifneq "$(MAKECMDGOALS)" "clean"
  ifneq "$(MAKECMDGOALS)" "distclean"
    ifneq "$(shell which g++ |grep hutaf_llt |wc -l)" "1"
      -include $(DEPEND)
    endif
  endif
endif
OBJS = bool.o cash.o date.o datetime.o \
	enum.o float.o format_type.o \
	int.o int8.o int16.o \
	json.o jsonfuncs.o like.o lockfuncs.o \
	misc.o name.o numeric.o numutils.o a_compat.o \
	rowtypes.o regproc.o ruleutils.o selfuncs.o \
	timestamp.o varbit.o varchar.o varlena.o version.o network.o \
	ri_triggers.o partitionfuncs.o pgstatfuncs.o encode.o \
	uuid.o unsigned_int.o my_locale.o year.o\
	vacuumfuncs.o set.o compress.o weightstring.o

like.o: like.cpp
include $(top_srcdir)/src/gausskernel/common.mk
