# Makefile for the Citus extension

spq_subdir = src/backend/distributed
spq_top_builddir = ../../..
safestringlib_srcdir = $(spq_abs_top_srcdir)/vendor/safestringlib
safestringlib_builddir = $(spq_top_builddir)/vendor/safestringlib/build
safestringlib_a = $(safestringlib_builddir)/libsafestring_static.a
safestringlib_sources = $(wildcard $(safestringlib_srcdir)/safeclib/*)

MODULE_big = spq
EXTENSION = spq

template_sql_files = $(patsubst $(spq_abs_srcdir)/%,%,$(wildcard $(spq_abs_srcdir)/sql/*.sql))
template_downgrade_sql_files = $(patsubst $(spq_abs_srcdir)/sql/downgrades/%,%,$(wildcard $(spq_abs_srcdir)/sql/downgrades/*.sql))
generated_sql_files = $(patsubst %,$(spq_abs_srcdir)/build/%,$(template_sql_files))
generated_downgrade_sql_files += $(patsubst %,$(spq_abs_srcdir)/build/sql/%,$(template_downgrade_sql_files))
# All spq--*.sql files that are used to upgrade between versions
DATA_built = $(generated_sql_files)

# directories with source files
SUBDIRS = . commands connection ddl deparser executor metadata operations planner progress relay safeclib test transaction utils worker version context

# Symlinks are not copied over to the build directory if a separete build
# directory is used during configure (such as on CI)
ENSURE_SUBDIRS_EXIST := $(shell mkdir -p $(SUBDIRS))

# That patsubst rule searches all directories listed in SUBDIRS for .c
# files, and adds the corresponding .o files to OBJS
OBJS += \
	$(patsubst $(spq_abs_srcdir)/%.c,%.o,$(foreach dir,$(SUBDIRS), $(sort $(wildcard $(spq_abs_srcdir)/$(dir)/*.c))))
OBJS += \
	$(patsubst $(spq_abs_srcdir)/%.cpp,%.o,$(foreach dir,$(SUBDIRS), $(sort $(wildcard $(spq_abs_srcdir)/$(dir)/*.cpp))))

NO_PGXS = 1

SHLIB_LINK = $(libpq)

include $(spq_top_builddir)/Makefile.global

# make sure spq_version.o is recompiled whenever any change is made to the binary or any
# other artifact being installed to reflect the correct gitref for every build
SPQ_VERSION_INVALIDATE := $(filter-out utils/spq_version.o,$(OBJS))
SPQ_VERSION_INVALIDATE += $(generated_sql_files)
ifneq ($(wildcard $(spq_top_builddir)/.git/.*),)
        SPQ_VERSION_INVALIDATE += $(spq_top_builddir)/.git/index
endif
utils/spq_version.o: $(SPQ_VERSION_INVALIDATE)

SHLIB_LINK += $(filter -lssl -lcrypto -lssleay32 -leay32, $(LIBS))

override CPPFLAGS += -I$(libpq_srcdir) -I$(safestringlib_srcdir)/include

SQL_DEPDIR=.deps/sql
SQL_BUILDDIR=build/sql

$(generated_sql_files): $(spq_abs_srcdir)/build/%: %
	@mkdir -p $(spq_abs_srcdir)/$(SQL_DEPDIR) $(spq_abs_srcdir)/$(SQL_BUILDDIR)
	@# -MF is used to store dependency files(.Po) in another directory for separation
	@# -MT is used to change the target of the rule emitted by dependency generation.
	@# -P is used to inhibit generation of linemarkers in the output from the preprocessor.
	@# -undef is used to not predefine any system-specific or GCC-specific macros.
	@# `man cpp` for further information
	cd $(spq_abs_srcdir) && cpp -undef -w -P -MMD -MP -MF$(SQL_DEPDIR)/$(*F).Po -MT$@ $< > $@

$(generated_downgrade_sql_files): $(spq_abs_srcdir)/build/sql/%: sql/downgrades/%
	@mkdir -p $(spq_abs_srcdir)/$(SQL_DEPDIR) $(spq_abs_srcdir)/$(SQL_BUILDDIR)
	@# -MF is used to store dependency files(.Po) in another directory for separation
	@# -MT is used to change the target of the rule emitted by dependency generation.
	@# -P is used to inhibit generation of linemarkers in the output from the preprocessor.
	@# -undef is used to not predefine any system-specific or GCC-specific macros.
	@# `man cpp` for further information
	cd $(spq_abs_srcdir) && cpp -undef -w -P -MMD -MP -MF$(SQL_DEPDIR)/$(*F).Po -MT$@ $< > $@

SQL_Po_files := $(wildcard $(SQL_DEPDIR)/*.Po)
ifneq (,$(SQL_Po_files))
include $(SQL_Po_files)
endif


.PHONY: clean-full install install-downgrades install-all


cleanup-before-install:
	rm -f $(DESTDIR)$(datadir)/$(datamoduledir)/citus.control
	rm -f $(DESTDIR)$(datadir)/$(datamoduledir)/citus--*
	rm -f $(DESTDIR)$(datadir)/$(datamoduledir)/spq.control
	rm -f $(DESTDIR)$(datadir)/$(datamoduledir)/spq--*

install: cleanup-before-install

# install and install-downgrades should be run sequentially
install-all: install
	$(MAKE) install-downgrades

install-downgrades: $(generated_downgrade_sql_files)
	$(INSTALL_DATA) $(generated_downgrade_sql_files) '$(DESTDIR)$(datadir)/$(datamoduledir)/'

clean-full:
	$(MAKE) clean
	$(MAKE) -C clean-full
	rm -rf $(safestringlib_builddir)
