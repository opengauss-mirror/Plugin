# contrib/tidrangescan/Makefile

MODULES = tidrangescan
EXTENSION = tidrangescan
REGRESS = tidrangescan
REGRESS_OPTS = --dlpath=$(top_builddir)/src/test/regress -c 0 -d 1 --single_node

DATA = tidrangescan--1.0.sql

override CPPFLAGS :=$(filter-out -fPIE, $(CPPFLAGS)) -fPIC

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/tidrangescan
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
