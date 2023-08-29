#-------------------------------------------------------------------------
#
# Makefile for the pl/pgsql procedural language
#
# src/common/pl/plpgsql/src/Makefile
#
#-------------------------------------------------------------------------

subdir = contrib/whale/plugin_pl/plpgsql/src
top_builddir = ../../../../..
base_dir = contrib/whale
include $(top_builddir)/src/Makefile.global
include $(top_builddir)/$(base_dir)/configure.mk

# Shared library parameters
#NAME= plpgsql

override CPPFLAGS := -I. -I$(srcdir) -I$(top_builddir)/src/common/pl/plpgsql/src $(CPPFLAGS)
SHLIB_LINK = $(filter -lintl, $(LIBS))

ifneq "$(MAKECMDGOALS)" "clean"
  ifneq "$(MAKECMDGOALS)" "distclean"
    ifneq "$(shell which g++ |grep hutaf_llt |wc -l)" "1"
      -include $(DEPEND)
    endif
  endif
endif
OBJS = pl_gram.o pl_scanner.o pl_comp.o pl_handler.o

FLEXFLAGS = -CF -b -p -p

include $(top_srcdir)/src/gausskernel/common.mk
# where to find gen_keywordlist.pl and subsidiary files
TOOLSDIR = $(top_srcdir)/src/tools
GEN_KEYWORDLIST = $(PERL) -I $(TOOLSDIR) $(TOOLSDIR)/gen_keywordlist.pl
GEN_KEYWORDLIST_DEPS = $(TOOLSDIR)/gen_keywordlist.pl $(TOOLSDIR)/PerfectHash.pm

pl_scanner.o: pl_scanner.cpp pl_gram.o pl_reserved_kwlist_d.h pl_unreserved_kwlist_d.h
pl_gram.o : pl_gram.hpp
pl_gram.hpp: pl_gram.cpp ;
pl_gram.cpp: gram.y $(top_builddir)/src/mtlocal.pl
ifdef BISON
	$(BISON) -d $(BISONFLAGS) -o $@ $<
	sed -i 's/YY_NULL nullptr/YY_NULL 0/g' pl_gram.cpp
else
	@$(missing) bison $< $@
	sed -i 's/YY_NULL nullptr/YY_NULL 0/g' pl_gram.cpp
endif
	$(PERL) $(top_builddir)/src/mtlocal.pl $(top_builddir)/$(subdir)/pl_gram.cpp
	$(PERL) $(top_builddir)/src/mtlocal.pl $(top_builddir)/$(subdir)/pl_gram.hpp


# generate keyword headers for the scanner
pl_reserved_kwlist_d.h: pl_reserved_kwlist.h $(GEN_KEYWORDLIST_DEPS)
	$(GEN_KEYWORDLIST) --varname ReservedPLKeywords $<

pl_unreserved_kwlist_d.h: pl_unreserved_kwlist.h $(GEN_KEYWORDLIST_DEPS)
	$(GEN_KEYWORDLIST) --varname UnreservedPLKeywords $<

clean distclean maintainer-clean:
	rm -f pl_gram.cpp pl_gram.hpp pl_reserved_kwlist_d.h pl_unreserved_kwlist_d.h

