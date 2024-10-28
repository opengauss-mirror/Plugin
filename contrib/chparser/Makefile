EXTENSION = chparser
EXTVERSION = 1.0

MODULE_big = chparser
OBJS = chparser.o

SCWS_HOME := $(shell realpath $(shell dirname $(shell which scws))/..)

.PHONY: check-scws-install

all: check-scws-install

check-scws-install:
	@echo "$(SCWS_HOME)"
	@if command -v scws &> /dev/null; then \
		echo "SCWS is already installed, building chparser"; \
	else \
		echo "ERROR: failed to find SCWS"; \
		exit 1; \
	fi

PG_CPPFLAGS = -I$(SCWS_HOME)/include/scws
SHLIB_LINK = -lscws -L$(SCWS_HOME)/lib -Wl,-rpath -Wl,$(SCWS_HOME)/lib

DATA = sql/$(EXTENSION)--$(EXTVERSION).sql
DATA_TSEARCH = dict.utf8.xdb rules.utf8.ini

REGRESS = chparser

SCWS_HOME ?= /usr/local
PG_CPPFLAGS = -I$(SCWS_HOME)/include/scws 
SHLIB_LINK = -lscws -L$(SCWS_HOME)/lib -Wl,-rpath -Wl,$(SCWS_HOME)/lib

ifdef USE_PGXS
PG_CONFIG ?= pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/pg_freespacemap
top_builddir = ../..
include  $(top_builddir)/src/Makefile.global
include  $(top_srcdir)/contrib/contrib-global.mk
endif

override CXXFLAGS :=$(filter-out -fstack-protector,  $(CFLAGS)) -fstack-protector-all -Wl,-z,relro,-z,now -fPIC $(PG_CFLAGS)
override CPPFLAGS +=$(filter-out -fstack-protector,  $(CFLAGS)) -fstack-protector-all -Wl,-z,relro,-z,now -fPIC $(PG_CFLAGS)