top_builddir = ../../..
base_dir = contrib/dolphin
REGRESS_PREP = tablespace-setup

all: dolphin

dolphin:
	make -C ../

#a dummy testcase to let 'make check' can run, currently I have no idea how to fix this...
REGRESS:=dummy

.PHONY: tablespace-setup
tablespace-setup:
	rm -rf ./testtablespace
	mkdir ./testtablespace

# now we use same regconf and postgres conf with common dolphin check
REGRESS_OPTS = --dlpath=$(top_builddir)/src/test/regress -c 0 -d 1 --single_node -p ${p} --schedule=./parallel_schedule_server_fastcheck_dolphin --regconf=../regress.conf -r 1 -n --keep_last_data=false --temp-config=../make_check_postgresql.conf --dbcmpt=B --dbname=regression

include $(top_builddir)/$(base_dir)/configure.mk
ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/dolphin
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
