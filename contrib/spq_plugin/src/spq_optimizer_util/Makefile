#-------------------------------------------------------------------------
#
# Makefile--
#    Makefile for gpopt
#
#-------------------------------------------------------------------------
subdir = src/spq_optimizer_util
top_builddir = ../../../..
include $(top_builddir)/src/Makefile.global
include ./spq_optimizer_util.mk

OBJS = spq_wrappers.o SPQOptimizer.o

SUBDIRS = config \
		  translate \
		  relcache \
		  utils

include $(top_srcdir)/src/gausskernel/common.mk