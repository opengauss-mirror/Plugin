#
# Makefile for utils/adt
#
# src/common/backend/utils/adt/Makefile
#

subdir = contrib/b_sql_plugin/plugin_utils/adt
top_builddir = ../../../../
include $(top_builddir)/src/Makefile.global
base_dir = contrib/b_sql_plugin
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
OBJS = acl.o arrayfuncs.o array_selfuncs.o array_typanalyze.o \
	array_userfuncs.o arrayutils.o bool.o \
	cash.o char.o date.o datetime.o datum.o domains.o \
	enum.o float.o format_type.o \
	int.o int8.o int16.o \
	json.o jsonb.o jsonb_gin.o jsonb_op.o jsonb_util.o jsonfuncs.o like.o lockfuncs.o \
	misc.o nabstime.o name.o numeric.o numutils.o \
	a_compat.o orderedsetaggs.o pseudotypes.o rangetypes.o rangetypes_gist.o \
	rowtypes.o regexp.o regproc.o ruleutils.o selfuncs.o \
	timestamp.o varbit.o varchar.o varlena.o version.o \
	network.o mac.o inet_cidr_ntop.o inet_net_pton.o \
	ri_triggers.o pg_lzcompress.o pg_locale.o formatting.o \
	ascii.o quote.o pgstatfuncs.o encode.o dbsize.o genfile.o trigfuncs.o \
	uuid.o windowfuncs.o xml.o extended_statistics.o clientlogic_bytea.o clientlogicsettings.o \
	median_aggs.o expr_distinct.o nlssort.o first_last_agg.o

like.o: like.cpp like_match.cpp

include $(top_srcdir)/src/gausskernel/common.mk
