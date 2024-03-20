# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

MODULE_big = age


OBJS = src/backend/age.o \
       src/backend/catalog/ag_catalog.o \
       src/backend/catalog/ag_graph.o \
       src/backend/catalog/ag_label.o \
       src/backend/catalog/ag_namespace.o \
       src/backend/commands/graph_commands.o \
       src/backend/commands/label_commands.o \
       src/backend/executor/cypher_create.o \
       src/backend/executor/cypher_merge.o \
       src/backend/executor/cypher_set.o \
       src/backend/executor/cypher_utils.o \
       src/backend/nodes/ag_nodes.o \
       src/backend/nodes/cypher_copyfuncs.o \
       src/backend/nodes/cypher_outfuncs.o \
       src/backend/nodes/cypher_readfuncs.o \
       src/backend/nodes/ag_extensible.o \
       src/backend/nodes/ag_extensible_read.o \
       src/backend/optimizer/cypher_createplan.o \
       src/backend/optimizer/cypher_pathnode.o \
       src/backend/optimizer/cypher_paths.o \
       src/backend/executor/cypher_delete.o\
       src/backend/parser/ag_scanner.o \
       src/backend/parser/cypher_analyze.o \
       src/backend/parser/cypher_clause.o \
       src/backend/parser/cypher_expr.o \
       src/backend/parser/cypher_gram.o \
       src/backend/parser/cypher_item.o \
       src/backend/parser/cypher_keywords.o \
       src/backend/parser/cypher_parse_agg.o \
       src/backend/parser/cypher_parse_node.o \
       src/backend/parser/cypher_parser.o \
       src/backend/utils/adt/age_graphid_ds.o \
       src/backend/parser/ag_parse_expr.o \
       src/backend/utils/adt/agtype.o \
       src/backend/utils/adt/agtype_ext.o \
       src/backend/utils/adt/agtype_ops.o \
       src/backend/utils/adt/agtype_parser.o \
       src/backend/utils/adt/agtype_util.o \
       src/backend/utils/adt/ag_extend_func.o \
       src/backend/utils/adt/age_global_graph.o \
       src/backend/utils/adt/cypher_funcs.o \
       src/backend/utils/adt/ag_float8_supp.o \
       src/backend/utils/adt/graphid.o \
       src/backend/utils/adt/age_vle.o \
       src/backend/utils/ag_func.o \
       src/backend/utils/cache/ag_cache.o \
       src/backend/utils/load/ag_load_labels.o \
       src/backend/utils/load/ag_load_edges.o \
       src/backend/utils/load/age_load.o \
       src/backend/utils/load/libcsv.o


EXTENSION = age

DATA = age--1.0.0.sql

override CPPFLAGS :=$(filter-out -fPIE,  $(CPPFLAGS)) -fPIC


#compile in og source
top_builddir = ../..
#include $(top_builddir)/src/Makefile.global


#limiao
FLEX = /bin/flex
FLEXFLAGS = -CF -b -p -p


# sorted in dependency order    expr
REGRESS = scan\
          graphid \
          expr \
          agtype \
          catalog \
          cypher \
          cypher_create \
          cypher_match \
          cypher_unwind \
          cypher_set \
          cypher_remove \
          cypher_delete \
          cypher_with \
          cypher_vle \
          cypher_union \
          cypher_merge \
          age_load \
          drop

srcdir=`pwd`

ag_regress_dir = $(srcdir)/regress

top_builddir = ../..
REGRESS_OPTS =  --load-extension=age --extra-install='contrib/age' --top-builddir="../.."  --inputdir=$(ag_regress_dir) --outputdir=$(ag_regress_dir) --encoding=UTF-8 --temp-install=$(ag_regress_dir)/instance --port=61958 --encoding=UTF-8 --dlpath=$(top_builddir)/src/test/regress -c 0 -d 1 --single_node

ag_regress_out = instance/ log/ results/ regression.*

EXTRA_CLEAN = src/backend/parser/cypher_gram.cpp src/include/parser/cypher_gram_def.h \
				src/backend/parser/ag_scanner.cpp \
                src/include/parser/cypher_kwlist_d.h src/include/parser/kwlist_d.h \
                regress/regression.diffs regress/regression.out regress/instance/ regress/log/ regress/results/ regress/.sql/




ag_include_dir = $(srcdir)/src/include
PG_CPPFLAGS = -I$(ag_include_dir) -I$(ag_include_dir)/parser

BISON = /usr/bin/bison
BISONFLAGS += --defines=src/include/parser/cypher_gram_def.h
src/include/parser/cypher_gram_def.h: src/backend/parser/cypher_gram.cpp;

src/backend/parser/cypher_gram.cpp: src/backend/parser/cypher_gram.y
ifdef BISON
	$(BISON) src/backend/parser/cypher_gram.y --defines=src/include/parser/cypher_gram_def.h -o src/backend/parser/cypher_gram.cpp
	sed -i 's/YY_NULL nullptr/YY_NULL 0/g' src/backend/parser/cypher_gram.cpp
else
	@$(missing) bison $< $@
	sed -i 's/YY_NULL nullptr/YY_NULL 0/g' src/backend/parser/cypher_gram.cpp
endif

src/backend/parser/cypher_parser.o: src/backend/parser/cypher_gram.cpp
src/backend/parser/cypher_keywords.o: src/backend/parser/cypher_gram.cpp kwlist_d.h src/include/parser/cypher_kwlist_d.h

src/backend/parser/ag_scanner.o: src/backend/parser/ag_scanner.cpp
src/backend/parser/ag_scanner.cpp: src/backend/parser/ag_scanner.l
ifdef FLEX
	$(FLEX) $(FLEXFLAGS) -o $@ $<
	#$(FLEX) -CF -b -p -p  -o src/backend/parser/ag_scanner.cpp src/backend/parser/ag_scanner.l
else
	@$(missing) flex $< $@
endif

TOOLSDIR_AGE = ./src/tools
PERL = /bin/perl
GEN_KEYWORDLIST_AGE = $(PERL) -I $(TOOLSDIR_AGE) $(TOOLSDIR_AGE)/gen_keywordlist.pl
GEN_KEYWORDLIST_DEPS_AGE = $(TOOLSDIR_AGE)/gen_keywordlist.pl $(TOOLSDIR_AGE)/PerfectHash.pm

kwlist_d.h: ./src/include/parser/kwlist.h $(GEN_KEYWORDLIST_DEPS_AGE)
	$(GEN_KEYWORDLIST_AGE) --extern -o ./src/include/parser ./src/include/parser/kwlist.h
src/include/parser/cypher_kwlist_d.h: ./src/include/parser/cypher_kwlist.h $(GEN_KEYWORDLIST_DEPS_AGE)
	$(GEN_KEYWORDLIST_AGE) --extern --varname CypherKeyword --output ./src/include/parser  $<

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/age
include $(top_builddir)/src/Makefile.global
include $(top_builddir)/contrib/contrib-global.mk
endif

