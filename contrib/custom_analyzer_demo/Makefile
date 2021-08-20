# contrib/custom_analyzer_demo/Makefile

subdir = contrib/custom_analyzer_demo
top_builddir = ../..

MODULE_big = custom_analyzer_demo
OBJS =custom_analyzer_demo.o

EXTENSION = custom_analyzer_demo
DATA = custom_analyzer_demo--1.0.sql

override CPPFLAGS :=$(filter-out -fPIE, $(CPPFLAGS)) -fPIC

include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk