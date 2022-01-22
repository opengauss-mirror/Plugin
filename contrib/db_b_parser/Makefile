# contrib/db_b_parser/Makefile

subdir = contrib/db_b_parser
top_builddir = ../..

MODULE_big = db_b_parser
OBJS =db_b_gram.o db_b_parser.o db_b_hint_gram.o db_b_funcs.o pl/pl_b_gram.o pl/pl_b.o pl/pl_b_scanner.o

FLEXFLAGS = -CF -b -p -p
# Latest flex causes warnings in this file.
ifeq ($(GCC),yes)
db_b_gram.o: CXXFLAGS += -Wno-error
endif

# scan is compiled as part of gram
db_b_gram.o: db_b_scan.inc

# There is no correct way to write a rule that generates two files.
# Rules with two targets don't have that meaning, they are merely
# shorthand for two otherwise separate rules.  To be safe for parallel
# make, we must chain the dependencies like this.  The semicolon is
# important, otherwise make will choose the built-in rule for
# gram.y=>gram.cpp.

db_b_gram.hpp: db_b_gram.cpp ;

db_b_gram.cpp: db_b_gram.y
	$(BISON) -d $(BISONFLAGS) -o $@ $<
	sed -i 's/YY_NULL nullptr/YY_NULL 0/g' db_b_gram.cpp

db_b_scan.inc: db_b_scan.l
	$(FLEX) $(FLEXFLAGS) -o'$@' $<

# Force these dependencies to be known even without dependency info built:
db_b_gram.o db_b_parser.o: db_b_gram.hpp

db_b_hint_gram.o: db_b_hint_scan.inc
ifeq ($(GCC),yes)
db_b_hint_gram.o: CXXFLAGS += -Wno-error
endif

db_b_hint_gram.hpp: db_b_hint_gram.cpp ;

db_b_hint_gram.cpp: db_b_hint_gram.y
	$(BISON) -d $(BISONFLAGS) -o $@ $<
	sed -i 's/YY_NULL nullptr/YY_NULL 0/g' db_b_hint_gram.cpp

db_b_hint_scan.inc: db_b_hint_scan.l | db_b_scan.inc
	$(FLEX) $(FLEXFLAGS) -o'$@' $<

db_b_hint_gram.o: db_b_hint_gram.hpp

pl/pl_b_scanner.o: pl/pl_b_scanner.cpp pl/pl_b_gram.o
pl/pl_b_gram.o : pl/pl_b_gram.hpp
pl/pl_b_gram.hpp: pl/pl_b_gram.cpp ;
pl/pl_b_gram.cpp: pl/pl_b_gram.y $(top_builddir)/$(subdir)/pl/tool/mtlocal.pl
	$(BISON) -d $(BISONFLAGS) -o $@ $<
	$(PERL) $(top_builddir)/$(subdir)/pl/tool/mtlocal.pl $(top_builddir)/$(subdir)/pl/pl_b_gram.cpp
	$(PERL) $(top_builddir)/$(subdir)/pl/tool/mtlocal.pl $(top_builddir)/$(subdir)/pl/pl_b_gram.hpp

REGRESS = db_b_parser
REGRESS_OPTS = --dlpath=$(top_builddir)/src/test/regress -c 0 -d 1 --single_node
EXTENSION = db_b_parser
DATA = db_b_parser--1.0.sql

override CPPFLAGS :=$(filter-out -fPIE, $(CPPFLAGS)) -I$(top_builddir)/$(subdir) -fPIC 

include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
