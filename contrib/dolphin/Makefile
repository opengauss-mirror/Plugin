# contrib/dolphin/Makefile
parser = plugin_parser
utils = plugin_utils
executor = plugin_executor
adt = $(utils)/adt
mb = $(utils)/mb
vector = plugin_vector
subdir = contrib/dolphin
top_builddir = ../..
base_dir = contrib/dolphin
MODULE_big = dolphin
p=25432

OBJS = $(parser)/analyze.o $(parser)/gram.o $(parser)/hint_gram.o $(parser)/keywords.o $(parser)/kwlookup.o $(parser)/parser.o \
$(parser)/parse_agg.o $(parser)/parse_clause.o $(parser)/parse_coerce.o $(parser)/parse_collate.o $(parser)/parse_cte.o \
$(parser)/parse_expr.o $(parser)/parse_func.o $(parser)/parse_hint.o $(parser)/parse_merge.o $(parser)/parse_node.o $(parser)/parse_oper.o $(parser)/parse_param.o \
$(parser)/parse_relation.o $(parser)/parse_target.o $(parser)/parse_type.o $(parser)/parse_utilcmd.o $(parser)/scansup.o $(parser)/parse_compatibility.o \
$(parser)/parse_describe.o $(parser)/parse_show.o

OBJS += $(adt)/acl.o $(adt)/arrayfuncs.o $(adt)/array_selfuncs.o $(adt)/array_typanalyze.o\
$(adt)/array_userfuncs.o $(adt)/arrayutils.o $(adt)/bool.o\
$(adt)/cash.o $(adt)/char.o $(adt)/date.o $(adt)/datetime.o $(adt)/datum.o $(adt)/domains.o\
$(adt)/enum.o $(adt)/float.o $(adt)/format_type.o\
$(adt)/int.o $(adt)/int8.o $(adt)/int16.o\
$(adt)/json.o $(adt)/jsonfuncs.o $(adt)/like.o $(adt)/lockfuncs.o\
$(adt)/misc.o $(adt)/nabstime.o $(adt)/name.o $(adt)/numeric.o $(adt)/numutils.o\
$(adt)/a_compat.o $(adt)/orderedsetaggs.o $(adt)/pseudotypes.o $(adt)/rangetypes.o $(adt)/rangetypes_gist.o\
$(adt)/rowtypes.o $(adt)/regexp.o $(adt)/regproc.o $(adt)/ruleutils.o $(adt)/selfuncs.o\
$(adt)/timestamp.o $(adt)/varbit.o $(adt)/varchar.o $(adt)/varlena.o $(adt)/version.o \
$(adt)/network.o $(adt)/mac.o $(adt)/inet_cidr_ntop.o $(adt)/inet_net_pton.o\
$(adt)/ri_triggers.o $(adt)/pg_locale.o $(adt)/formatting.o\
$(adt)/ascii.o $(adt)/quote.o $(adt)/pgstatfuncs.o $(adt)/encode.o $(adt)/dbsize.o $(adt)/trigfuncs.o\
$(adt)/uuid.o $(adt)/windowfuncs.o $(adt)/extended_statistics.o $(adt)/clientlogic_bytea.o $(adt)/clientlogicsettings.o\
$(adt)/median_aggs.o $(adt)/expr_distinct.o $(adt)/nlssort.o $(adt)/first_last_agg.o $(adt)/my_locale.o $(adt)/year.o

OBJS += $(vector)/vecfuncache.o

OBJS += $(mb)/mbutils.o

OBJS += $(executor)/execQual.o

all $(OBJS): parser utils executor vector;

parser:
	make -C $(parser)

utils:
	make -C $(utils)

vector:
	make -C $(vector)

executor:
	make -C $(executor)

extra_clean:
	make clean -C $(parser)
	make clean -C $(utils)
	make clean -C $(vector)
	make clean -C $(executor)
clean: extra_clean

dolphin--1.0.sql: sql_script/*
	cat $^ > $@
DATA_built = dolphin--1.0.sql openGauss_expr_dolphin.ir

OBJS += plugin_postgres.o 
plugin_postgres.o: plugin_postgres.cpp

OBJS += pg_builtin_proc.o
pg_builtin_proc.o: pg_builtin_proc.cpp utils

OBJS += plugin_utility.o
plugin_utility.o: plugin_utility.cpp

OBJS += tablecmds.o
tablecmds.o: tablecmds.cpp

EXTENSION = dolphin
#a dummy testcase to let 'make check' can run, currently I have no idea how to fix this...
REGRESS:=dummy

REGRESS_OPTS = --dlpath=$(top_builddir)/src/test/regress -c 0 -d 1 --single_node -p ${p} --schedule=./parallel_schedule_dolphin --regconf=regress.conf -r 1 -n --keep_last_data=false --temp-config=./make_check_postgresql.conf

include $(top_builddir)/$(base_dir)/configure.mk
ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/dolphin
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif

openGauss_expr_dolphin.ir:
	cp llvmir/openGauss_expr_dolphin_$(host_cpu).ir $@
