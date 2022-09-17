#-------------------------------------------------------------------------
#
# Makefile for the pl/pgsql procedural language
#
# src/common/pl/plpgsql/src/Makefile
#
#-------------------------------------------------------------------------

subdir = contrib/dolphin/plugin_pl/plpgsql/src
top_builddir = ../../../../..
base_dir = contrib/dolphin
include $(top_builddir)/src/Makefile.global
include $(top_builddir)/$(base_dir)/configure.mk

# Shared library parameters
#NAME= plpgsql

override CPPFLAGS := -I. -I$(srcdir) $(CPPFLAGS)
SHLIB_LINK = $(filter -lintl, $(LIBS))

ifneq "$(MAKECMDGOALS)" "clean"
  ifneq "$(MAKECMDGOALS)" "distclean"
    ifneq "$(shell which g++ |grep hutaf_llt |wc -l)" "1"
      -include $(DEPEND)
    endif
  endif
endif
OBJS = pl_gram.o pl_scanner.o

FLEXFLAGS = -CF -b -p -p

include $(top_srcdir)/src/gausskernel/common.mk

pl_scanner.o: pl_scanner.cpp pl_gram.o
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


clean distclean maintainer-clean:
	rm -f pl_gram.cpp pl_gram.hpp

