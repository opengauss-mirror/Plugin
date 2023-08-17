#
#Copyright (c) 2020 China Unicom.
#
#openGauss is licensed under Mulan PSL v2.
#You can use this software according to the terms and conditions of the Mulan PSL v2.
#You may obtain a copy of Mulan PSL v2 at:
#
#          http://license.coscl.org.cn/MulanPSL2
#
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.
#-------------------------------------------------------------------------
#
# Makefile
#    Makefile for protocol
#
#-------------------------------------------------------------------------

subdir = contrib/whale/plugin_orafce
top_builddir = ../../../
base_dir = contrib/whale
include $(top_builddir)/src/Makefile.global
include $(top_builddir)/$(base_dir)/configure.mk
override CPPFLAGS:=-std=c++14 $(CPPFLAGS)
ifneq "$(MAKECMDGOALS)" "clean"
  ifneq "$(MAKECMDGOALS)" "distclean"
    ifneq "$(shell which g++ |grep hutaf_llt |wc -l)" "1"
      -include $(DEPEND)
    endif
  endif
endif

OBJS = alert.o assert.o pipe.o putline.o shmmc.o utility.o random.o datefce.o convert.o others.o \
aggregate.o charlen.o charpad.o plvstr.o replace_empty_string.o varchar2.o file.o plvdate.o plvsubst.o \
plunit.o 

override CPPFLAGS += -D__STDC_FORMAT_MACROS

include $(top_srcdir)/src/gausskernel/common.mk
