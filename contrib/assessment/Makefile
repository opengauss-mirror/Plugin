# contrib/assessment/Makefile"

MODULE_big = assessment

OBJS =  assessment.o

EXTENSION = assessment
DATA = assessment--1.0.sql

REGRESS = assessment

top_builddir = ../..
subdir = contrib/assessment

override CPPFLAGS :=$(filter-out -fPIE,  $(CPPFLAGS)) -fPIC
TARGET_NAME=gs_assessment
EXTRA_CLEAN=$(TARGET_NAME)
# make clean if change EXEC_OBJS
EXEC_OBJS = main.cpp OpenGaussConnection.cpp Scan.cpp Report.cpp \
	$(top_builddir)/src/lib/pgcommon/libpgcommon.a
INCLUDE_PATH =-I. -I$(top_builddir)/src/bin/psql

include $(top_builddir)/src/Makefile.global
LD_LINK=-L$(libdir) -lpq -lsecurec -lpgport
ifeq ($(enable_memory_check), yes)
    LD_LINK += -ldl -lrt
endif

$(TARGET_NAME):
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -DFRONTEND $(LD_LINK) $(INCLUDE_PATH) $(EXEC_OBJS) -o $(TARGET_NAME)

$(TARGET_NAME)_install: $(TARGET_NAME)
	cp $(TARGET_NAME) $(bindir)/$(TARGET_NAME)

all: $(TARGET_NAME)

install: $(TARGET_NAME)_install

.PHONY: $(TARGET_NAME)_install

include $(top_srcdir)/contrib/contrib-global.mk