#---------------------------------------------------------------------------------------
#     
# IDENTIFICATION
#        src/gausskernel/optimizer/commands/Makefile
#
# ---------------------------------------------------------------------------------------

subdir = contrib/b_sql_plugin/plugin_optimizer/commands
top_builddir = ../../../..
include $(top_builddir)/src/Makefile.global
override CPPFLAGS :=$(filter-out -fPIE, $(CPPFLAGS)) -fPIC -I. -I$(srcdir) $(CPPFLAGS) -I$(top_builddir)/$(subdir)/../../include -I../../ -fvisibility=hidden
ifneq "$(MAKECMDGOALS)" "clean"
  ifneq "$(MAKECMDGOALS)" "distclean"
    ifneq "$(shell which g++ |grep hutaf_llt |wc -l)" "1"
      -include $(DEPEND)
    endif
  endif
endif

SUBDIRS     = sequence

OBJS = aggregatecmds.o alter.o analyze.o async.o comment.o  \
	collationcmds.o constraint.o conversioncmds.o copy.o createas.o \
	dbcommands.o define.o discard.o dropcmds.o extension.o \
	foreigncmds.o functioncmds.o \
	indexcmds.o lockcmds.o operatorcmds.o opclasscmds.o \
	portalcmds.o prepare.o proclang.o packagecmds.o \
	schemacmds.o tablecmds.o tablespace.o trigger.o \
	typecmds.o user.o \
	variable.o view.o formatter.o datasourcecmds.o \
	directory.o shutdown.o

include $(top_srcdir)/src/gausskernel/common.mk
