# Spq autoconf input script.
#
# Converted into an actual configure script by autogen.sh. This
# conversion only has to be done when configure.in changes. To avoid
# everyone needing autoconf installed, the resulting files are checked
# into the SCM.

AC_INIT([Spq], [2.0.0])
AC_COPYRIGHT([Copyright (c) Huawei Technologies Co.,Ltd.])

# we'll need sed and awk for some of the version commands
AC_PROG_SED
AC_PROG_AWK

# SPQ_NAME definition
AC_DEFINE_UNQUOTED(SPQ_NAME, "$PACKAGE_NAME", [spq full name as a string])

# SPQ_EDITION definition
AC_DEFINE_UNQUOTED(SPQ_EDITION, "community", [Spq edition as a string])

# SPQ_EXTENSION_NAME in pg_extension
AC_DEFINE_UNQUOTED(SPQ_EXTENSION_NAME, "spq", [Spq extension name in pg_extension])

# SPQ_MAJORVERSION definition
[SPQ_MAJORVERSION=`expr "$PACKAGE_VERSION" : '\([0-9][0-9]*\.[0-9][0-9]*\)'`]
AC_DEFINE_UNQUOTED(SPQ_MAJORVERSION, "$SPQ_MAJORVERSION", [Spq major version as a string])

# SPQ_VERSION definition
PGAC_ARG_REQ(with, extra-version, [STRING], [append STRING to version],
             [SPQ_VERSION="$PACKAGE_VERSION$withval"],
             [SPQ_VERSION="$PACKAGE_VERSION"])
AC_DEFINE_UNQUOTED(SPQ_VERSION, "$SPQ_VERSION", [Spq version as a string])

# SPQ_VERSION_NUM definition
# awk -F is a regex on some platforms, and not on others, so make "." a tab
[SPQ_VERSION_NUM="`echo "$PACKAGE_VERSION" | sed 's/[A-Za-z].*$//' |
tr '.' '	' |
$AWK '{printf "%d%02d%02d", $1, $2, (NF >= 3) ? $3 : 0}'`"]
AC_DEFINE_UNQUOTED(SPQ_VERSION_NUM, $SPQ_VERSION_NUM, [Spq version as a number])

# SPQ_EXTENSIONVERSION definition
[SPQ_EXTENSIONVERSION="`grep '^default_version' $srcdir/src/backend/distributed/spq.control | cut -d\' -f2`"]
AC_DEFINE_UNQUOTED([SPQ_EXTENSIONVERSION], "$SPQ_EXTENSIONVERSION", [Extension version expected by this spq build])

# Re-check for flex. That allows to compile spq against a postgres
# which was built without flex available (possible because generated
# files are included)
AC_PATH_PROG([FLEX], [flex])

# Locate pg_config binary
AC_ARG_VAR([PG_CONFIG], [Location to find pg_config for target PostgreSQL instalation (default PATH)])
AC_ARG_VAR([PATH], [PATH for target PostgreSQL install pg_config])

if test -z "$PG_CONFIG"; then
  AC_PATH_PROG(PG_CONFIG, pg_config)
fi

if test -z "$PG_CONFIG"; then
   AC_MSG_ERROR([Could not find pg_config. Set PG_CONFIG or PATH.])
fi

AC_MSG_CHECKING([for .git directory])
if test -d "${srcdir}/.git"; then
  AC_MSG_RESULT([found at ${srcdir}/.git])
  AS_MKDIR_P([${srcdir}/.git/hooks])

  AC_CONFIG_COMMANDS([install-git-hook],
    [cp "${srcdir}/.ci/git-pre-commit.hook" "${srcdir}/.git/hooks/pre-commit"
     chmod +x ${srcdir}/.git/hooks/pre-commit
     echo "Git pre-commit hook installed."],
    [srcdir="${srcdir}"])
else
  AC_MSG_RESULT([not found git. Skipping git hook installation])
fi

# check we're building against a supported version of PostgreSQL
spqac_pg_config_version=$($PG_CONFIG --version 2>/dev/null)
version_num=$(echo "$spqac_pg_config_version"|
              $SED -e 's/^PostgreSQL \([[0-9]]*\)\(\.[[0-9]]*\)\{0,1\}\(.*\)$/\1\2/')

# if PostgreSQL version starts with two digits, the major version is those digits
version_num=$(echo "$version_num"| $SED -e 's/^\([[0-9]]\{2\}\)\(.*\)$/\1/')

if test -z "$version_num"; then
  AC_MSG_ERROR([Could not detect PostgreSQL version from pg_config.])
fi

PGAC_ARG_BOOL(with, pg-version-check, yes,
              [do not check postgres version during configure])
AC_SUBST(with_pg_version_check)

if test "$with_pg_version_check" = no; then
    AC_MSG_NOTICE([building against PostgreSQL $version_num (skipped compatibility check)])
elif test "$version_num" != '15' -a  "$version_num" != '16' -a  "$version_num" != '17'; then
   AC_MSG_WARN([Spq is not compatible with the detected PostgreSQL version ${version_num}.])
else
   AC_MSG_NOTICE([building against PostgreSQL $version_num])
fi;

# Check whether we're building inside the source tree, if not, prepare
# the build directory.
if test "$srcdir" -ef '.' ; then
  vpath_build=no
else
  vpath_build=yes
  _AS_ECHO_N([preparing build tree... ])
  spqac_abs_top_srcdir=`cd "$srcdir" && pwd`
  $SHELL "$spqac_abs_top_srcdir/prep_buildtree" "$spqac_abs_top_srcdir" "." \
      || AC_MSG_ERROR(failed)
  AC_MSG_RESULT(done)
fi
AC_SUBST(vpath_build)

# Allow to overwrite the C compiler, default to the one postgres was
# compiled with. We don't want autoconf's default CFLAGS though, so save
# those.
SAVE_CXXFLAGS="$CXXFLAGS"
AC_PROG_CXX([$($PG_CONFIG --cc)])
CXXFLAGS="$SAVE_CXXFLAGS"

host_guess=`${SHELL} $srcdir/config/config.guess`

# Create compiler version string
if test x"$G++" = x"yes" ; then
  cc_string=`${CXX} --version | sed q`
  case $cc_string in [[A-Za-z]]*) ;; *) cc_string="GCC $cc_string";; esac
elif test x"$SUN_STUDIO_CXX" = x"yes" ; then
  cc_string=`${CXX} -V 2>&1 | sed q`
else
  cc_string=$CXX
fi

AC_CHECK_SIZEOF([void *])
AC_DEFINE_UNQUOTED(SPQ_VERSION_STR,
                   ["$PACKAGE_NAME $SPQ_VERSION on $host_guess, compiled by $cc_string, `expr $ac_cv_sizeof_void_p \* 8`-bit"],
                   [A string containing the version number, platform, and C compiler])

# Locate source and build directory of the postgres we're building
# against. Can't rely on either still being present, but e.g. optional
# test infrastructure can rely on it.
POSTGRES_SRCDIR=$(grep ^abs_top_srcdir $(dirname $($PG_CONFIG --pgxs))/../Makefile.global|cut -d ' ' -f3-)
POSTGRES_BUILDDIR=$(grep ^abs_top_builddir $(dirname $($PG_CONFIG --pgxs))/../Makefile.global|cut -d ' ' -f3-)


# check for a number of CFLAGS that make development easier

# SPQAC_PROG_CXX_FLAGS_OPT
# -----------------------
# Given a string, check if the compiler supports the string as a
# command-line option. If it does, add the string to CXXFLAGS.
AC_DEFUN([SPQAC_PROG_CXX_FLAGS_OPT],
[define([Ac_cachevar], [AS_TR_SH([spqac_cv_prog_cxx_flags_$1])])dnl
AC_CACHE_CHECK([whether $CXX supports $1], [Ac_cachevar],
[spqac_save_CXXFLAGS=$CXXFLAGS
flag=$1
case $flag in -Wno*)
	 flag=-W$(echo $flag | cut -c 6-)
esac
CXXFLAGS="$spqac_save_CXXFLAGS $flag"
ac_save_c_werror_flag=$ac_c_werror_flag
ac_c_werror_flag=yes
_AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
                   [Ac_cachevar=yes],
                   [Ac_cachevar=no])
ac_c_werror_flag=$ac_save_c_werror_flag
CXXFLAGS="$spqac_save_CXXFLAGS"])
if test x"$Ac_cachevar" = x"yes"; then
  SPQ_CXXFLAGS="$SPQ_CXXFLAGS $1"
fi
undefine([Ac_cachevar])dnl
])# SPQAC_PROG_CXX_CFLAGS_OPT

SPQAC_PROG_CXX_FLAGS_OPT([-std=c++14])
SPQAC_PROG_CXX_FLAGS_OPT([-Wall])
SPQAC_PROG_CXX_FLAGS_OPT([-Wextra])
# disarm options included in the above, which are too noisy for now
SPQAC_PROG_CXX_FLAGS_OPT([-Wno-unused-parameter])
SPQAC_PROG_CXX_FLAGS_OPT([-Wno-sign-compare])
SPQAC_PROG_CXX_FLAGS_OPT([-Wno-missing-field-initializers])
SPQAC_PROG_CXX_FLAGS_OPT([-Wno-clobbered])
#SPQAC_PROG_CXX_FLAGS_OPT([-Wno-gnu-variable-sized-type-not-at-end])

#SPQAC_PROG_CXX_FLAGS_OPT([-Wno-declaration-after-statement])

# And add a few extra warnings
SPQAC_PROG_CXX_FLAGS_OPT([-Wendif-labels])
SPQAC_PROG_CXX_FLAGS_OPT([-Wmissing-format-attribute])
SPQAC_PROG_CXX_FLAGS_OPT([-Wmissing-declarations])

#SPQAC_PROG_CXX_FLAGS_OPT([-Wmissing-prototypes])

SPQAC_PROG_CXX_FLAGS_OPT([-Wshadow])
SPQAC_PROG_CXX_FLAGS_OPT([-Werror=vla])  # visual studio does not support these
#SPQAC_PROG_CXX_FLAGS_OPT([-Werror=implicit-int])
#SPQAC_PROG_CXX_FLAGS_OPT([-Werror=implicit-function-declaration])
SPQAC_PROG_CXX_FLAGS_OPT([-Werror=return-type])
# Security flags
# Flags taken from: https://liquid.microsoft.com/Web/Object/Read/ms.security/Requirements/Microsoft.Security.SystemsADM.10203#guide
# We do not enforce the following flag because it is only available on GCC>=8
SPQAC_PROG_CXX_FLAGS_OPT([-fstack-clash-protection])
SPQAC_PROG_CXX_FLAGS_OPT([-Wno-conversion])
SPQAC_PROG_CXX_FLAGS_OPT([-fPIC])

PGAC_ARG_BOOL(with, loose-compile-options, no,
              [compile spq wih loose compile options.])
AC_SUBST(with_loose_compile_options)

# @TODO:important!!!!!!!!!! remove the following suppressed warnings in release version
if test "$with_loose_compile_options" = yes; then
    SPQAC_PROG_CXX_FLAGS_OPT([-Wno-cast-function-type])
    SPQAC_PROG_CXX_FLAGS_OPT([-Wno-ignored-qualifiers])
    SPQAC_PROG_CXX_FLAGS_OPT([-Wno-shadow])
    SPQAC_PROG_CXX_FLAGS_OPT([-Wno-type-limits])
    SPQAC_PROG_CXX_FLAGS_OPT([-Wno-unused-function])
    SPQAC_PROG_CXX_FLAGS_OPT([-Wno-unused-variable])
    SPQAC_PROG_CXX_FLAGS_OPT([-Wno-missing-declarations])
fi
#end of compiling suppressed warnings

#
# --enable-coverage enables generation of code coverage metrics with gcov
#
AC_ARG_ENABLE([coverage], AS_HELP_STRING([--enable-coverage], [build with coverage testing instrumentation]))
if test "$enable_coverage" = yes; then
    SPQ_CXXFLAGS="$SPQ_CXXFLAGS -O0 -g --coverage"
    SPQ_CPPFLAGS="$SPQ_CPPFLAGS -DNDEBUG"
    SPQ_LDFLAGS="$SPQ_LDFLAGS --coverage"
fi

#
# libcurl
#
PGAC_ARG_BOOL(with, libcurl, yes,
              [do not use libcurl for anonymous statistics collection],
              [AC_DEFINE([HAVE_LIBCURL], 1, [Define to 1 to build with libcurl support. (--with-libcurl)])])

if test "$with_libcurl" = yes; then
  AC_CHECK_LIB(curl, curl_global_init, [],
              [AC_MSG_ERROR([libcurl not found
If you have libcurl already installed, see config.log for details on the
failure. It is possible the compiler isn't looking in the proper directory.
Use --without-libcurl to disable anonymous statistics collection.])])
  AC_CHECK_HEADER(curl/curl.h, [], [AC_MSG_ERROR([libcurl header not found
If you have libcurl already installed, see config.log for details on the
failure.  It is possible the compiler isn't looking in the proper directory.
Use --without-libcurl to disable libcurl support.])])
fi

#
# LZ4
#
PGAC_ARG_BOOL(with, lz4, yes,
              [do not use lz4],
              [AC_DEFINE([HAVE_SPQ_LIBLZ4], 1, [Define to 1 to build with lz4 support. (--with-lz4)])])
AC_SUBST(with_lz4)

if test "$with_lz4" = yes; then
  AC_CHECK_LIB(lz4, LZ4_compress_default, [],
              [AC_MSG_ERROR([lz4 library not found
If you have lz4 installed, see config.log for details on the
failure.  It is possible the compiler isn't looking in the proper directory.
Use --without-lz4 to disable lz4 support.])])
  AC_CHECK_HEADER(lz4.h, [], [AC_MSG_ERROR([lz4 header not found
If you have lz4 already installed, see config.log for details on the
failure.  It is possible the compiler isn't looking in the proper directory.
Use --without-lz4 to disable lz4 support.])])
fi

#
# ZSTD
#
PGAC_ARG_BOOL(with, zstd, yes,
              [do not use zstd])
AC_SUBST(with_zstd)

if test "$with_zstd" = yes; then
  AC_CHECK_LIB(zstd, ZSTD_decompress, [],
              [AC_MSG_ERROR([zstd library not found
If you have zstd installed, see config.log for details on the
failure.  It is possible the compiler isn't looking in the proper directory.
Use --without-zstd to disable zstd support.])])
  AC_CHECK_HEADER(zstd.h, [], [AC_MSG_ERROR([zstd header not found
If you have zstd already installed, see config.log for details on the
failure.  It is possible the compiler isn't looking in the proper directory.
Use --without-zstd to disable zstd support.])])
fi


PGAC_ARG_BOOL(with, security-flags, no,
              [use security flags])
AC_SUBST(with_security_flags)

if test "$with_security_flags" = yes; then
# Flags taken from: https://liquid.microsoft.com/Web/Object/Read/ms.security/Requirements/Microsoft.Security.SystemsADM.10203#guide

# We always want to have some compiler flags for security concerns.
SECURITY_CXXFLAGS="-fstack-protector-strong -D_FORTIFY_SOURCE=2 -O2 -z noexecstack -fpic -shared -Wl,-z,relro -Wl,-z,now -Wformat -Wformat-security -Werror=format-security"
SPQ_CXXFLAGS="$SPQ_CXXFLAGS $SECURITY_CXXFLAGS"
AC_MSG_NOTICE([Blindly added security flags for linker: $SECURITY_CXXFLAGS, spq flags: $SPQ_CXXFLAGS])

# We always want to have some clang flags for security concerns.
# This doesn't include "-Wl,-z,relro -Wl,-z,now" on purpuse, because bitcode is not linked.
# This doesn't include -fsanitize=cfi because it breaks builds on many distros including
# Debian/Buster, Debian/Stretch, Ubuntu/Bionic, Ubuntu/Xenial and EL7.
SECURITY_BITCODE_CXXFLAGS="-fsanitize=safe-stack -fstack-protector-strong -flto -fPIC -Wformat -Wformat-security -Werror=format-security"
SPQ_BITCODE_CXXFLAGS="$SPQ_BITCODE_CXXFLAGS $SECURITY_BITCODE_CXXFLAGS"
AC_MSG_NOTICE([Blindly added security flags for llvm: $SECURITY_BITCODE_CXXFLAGS])

AC_MSG_WARN([If you run into issues during linking or bitcode compilation, you can use --without-security-flags.])
fi

SPQ_OG_CLAGS=$($PG_CONFIG --cflags 2>/dev/null)
AC_MSG_NOTICE([Spq for opengauss will be build with options:$CXXFLAGS $SPQ_CXXFLAGS])
AC_MSG_NOTICE([Spq will be built with extra opengauss's builtin options: $SPQ_OG_CLAGS])

# Check if git is installed, when installed the gitref of the checkout will be baked in the application
AC_PATH_PROG(GIT_BIN, git)
AC_CHECK_FILE(.git,[HAS_DOTGIT=yes], [HAS_DOTGIT=])

AC_SUBST(SPQ_CXXFLAGS, "$SPQ_CXXFLAGS")
AC_SUBST(SPQ_BITCODE_CXXFLAGS, "$SPQ_BITCODE_CXXFLAGS")
AC_SUBST(SPQ_CPPFLAGS, "$SPQ_CPPFLAGS")
AC_SUBST(SPQ_LDFLAGS, "$LIBS $SPQ_LDFLAGS")
AC_SUBST(POSTGRES_SRCDIR, "$POSTGRES_SRCDIR")
AC_SUBST(POSTGRES_BUILDDIR, "$POSTGRES_BUILDDIR")
AC_SUBST(HAS_DOTGIT, "$HAS_DOTGIT")

AC_CONFIG_FILES([Makefile.global])
AC_CONFIG_HEADERS([src/include/spq_config.h] [src/include/spq_version.h])
AH_TOP([
/*
 * spq_config.h.in is generated by autoconf/autoheader and
 * converted into spq_config.h by configure.  Include when code needs to
 * depend on determinations made by configure.
 *
 * Do not manually edit!
 */
])
AC_OUTPUT
