# ---------------------------------------------------------------------------------------
# 
# Makefile
#     Makefile for src/spq_backup
# 
# IDENTIFICATION
#        src/spq_backup/Makefile
# 
# ---------------------------------------------------------------------------------------

PGFILEDESC = "spq_backup - takes a streaming base backup of a spq nodes"
PGAPPICON=win32

subdir = src/spq_backup
spq_top_builddir = ../..
include $(spq_top_builddir)/Makefile.global

override CPPFLAGS := -I$(libpq_srcdir) -I$(ZLIB_INCLUDE_PATH) $(CPPFLAGS) -DHAVE_LIBZ -fPIC -fPIE -DFRONTEND -I$(OG_CODE_BASE)/src/bin/pg_rewind \
	-I$(OG_CODE_BASE)/src/bin/pg_ctl -I$(LZ4_INCLUDE_PATH) -I$(ZSTD_INCLUDE_PATH) -I${OG_CODE_BASE}/src/lib/page_compression -I${OG_CODE_BASE}/src/include \
	-I${OG_CODE_BASE}/mppdb_temp_install/include -I${OG_CODE_BASE}/mppdb_temp_install/include/postgresql/internal/

override LDFLAGS := $(LDFLAGS) -Wl,-z,relro,-z,now -L$(LZ4_LIB_PATH) -L$(ZSTD_LIB_PATH) -L${OG_CODE_BASE}/src/lib/page_compression \
	-L${OG_CODE_BASE}/mppdb_temp_install/lib -std=c++11

LIBS += -lsecurec -lpq -lpgport -ldl -lz -llz4 -lzstd -lpagecompression
ifeq ($(enable_lite_mode), no)
    LIBS += -lgssapi_krb5_gauss -lgssrpc_gauss -lkrb5_gauss -lkrb5support_gauss -lk5crypto_gauss -lcom_err_gauss
endif

ifneq "$(MAKECMDGOALS)" "clean"
  ifneq "$(MAKECMDGOALS)" "distclean"
    ifneq "$(shell which g++ |grep hutaf_llt |wc -l)" "1"
      -include $(DEPEND)
    endif
  endif
endif


ifneq ($(wildcard $(OG_CODE_BASE)/tmp_build/lib/libbuildquery.a),)
	DEPEND_OBJS=$(OG_CODE_BASE)/tmp_build/lib/libbuildquery.a \
		$(OG_CODE_BASE)/tmp_build/lib/libpgcommon.a \
		$(OG_CODE_BASE)/tmp_build/lib/libhotpatchclient.a
else
	DEPEND_OBJS=$(OG_CODE_BASE)/src/lib/build_query/libbuildquery.a \
		$(OG_CODE_BASE)/src/lib/pgcommon/libpgcommon.a \
		$(OG_CODE_BASE)/src/lib/hotpatch/client/libhotpatchclient.a
endif

OBJS=receivelog.o streamutil.o $(WIN32RES) \
     xlogreader.o  $(WIN32RES) $(DEPEND_OBJS) \
	 $(OG_CODE_BASE)/src/common/backend/lib/string.o \
	 $(OG_CODE_BASE)/src/gausskernel/storage/gs_uwal/gs_uwal_adaptor.o


all: spq_backup

$(OG_CODE_BASE)/src/lib/elog/elog.a:
	$(MAKE) -C $(OG_CODE_BASE)/src/lib/elog elog.a

spq_backup: spq_backup.o $(OBJS)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) spq_backup.o $(OBJS) $(LIBS) $(libpq_pgport) $(LDFLAGS) $(LDFLAGS_EX) -o $@$(X)

xlogreader.cpp: % : $(OG_CODE_BASE)/src/gausskernel/storage/access/transam/%
	rm -f $@ && $(LN_S) $< .

spq_backup.o: $(spq_top_builddir)/../src/spq_backup/spq_backup.cpp
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

receivelog.o: $(spq_top_builddir)/../src/spq_backup/receivelog.cpp
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
streamutil.o: $(spq_top_builddir)/../src/spq_backup/streamutil.cpp
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
xlogreader.o: $(spq_top_builddir)/../src/spq_backup/xlogreader.cpp
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


install: all installdirs
	$(info $(INSTALL_PROGRAM) spq_backup$(X) '$(DESTDIR)$(bindir)/spq_backup$(X)')
	install -c -m 755 spq_backup$(X) '$(DESTDIR)$(bindir)/spq_backup$(X)'

installdirs:
	$(MKDIR_P) '$(DESTDIR)$(bindir)'

uninstall:
	rm -f '$(DESTDIR)$(bindir)/spq_backup$(X)'

clean distclean maintainer-clean:
	rm -f spq_backup$(X) $(OBJS) \
		spq_backup.o *.depend

# Be sure that the necessary archives are compiled
$(OG_CODE_BASE)/src/lib/build_query/libbuildquery.a:
	$(MAKE) -C $(OG_CODE_BASE)/src/lib/build_query libbuildquery.a

$(OG_CODE_BASE)/src/lib/pgcommon/libpgcommon.a:
	$(MAKE) -C $(OG_CODE_BASE)/src/lib/pgcommon libpgcommon.a

$(OG_CODE_BASE)/src/lib/hotpatch/client/libhotpatchclient.a:
	$(MAKE) -C $(OG_CODE_BASE)/src/lib/hotpatch/client libhotpatchclient.a
