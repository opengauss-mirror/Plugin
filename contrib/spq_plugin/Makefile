# contrib/spq_plugin/Makefile
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include ./spqplugin.mk

MODULE_big = spqplugin
EXTENSION = spqplugin
REGRESS = spqplugin
DATA = spqplugin--1.0.sql

# recurse make all cpp file to .o file and save objects into ./src/objfiles.txt
# load all objects from ./src/objfiles.txt, and put spqplugin.o to the first place
# for opengauss plugin PG_MODULE_MAGIC should place at first of .so, which contained by spqplugin.o
src_objs = ./src/objfiles.txt
obj_files = $(addprefix ./, $(shell cat ./src/objfiles.txt))

OBJS = $(filter spqplugin.o, $(obj_files)) $(filter-out spqplugin.o, $(obj_files))

subdir = contrib/spq_plugin
include $(top_srcdir)/contrib/contrib-global.mk

compile_src:
	@echo "Compile spq plugin source files..."
	$(MAKE) -j16 -C src || { echo "Error compile spq plugin source files."; exit 1; }
all: compile_src