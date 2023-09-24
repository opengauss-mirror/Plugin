ifeq (,$(findstring -DUSE_SPQ,$(CPPFLAGS)))
override CPPFLAGS := -DUSE_SPQ $(CPPFLAGS)
endif
override CPPFLAGS := -I$(abs_top_srcdir)/contrib/spq_plugin/include -lxerces-c $(CPPFLAGS)
override CPPFLAGS :=$(filter-out -fPIE, $(CPPFLAGS)) -fPIC
override LIBS := -lxerces-c $(LIBS)

ifneq "$(MAKECMDGOALS)" "clean"
  ifneq "$(MAKECMDGOALS)" "distclean"
    ifneq "$(shell which g++ |grep hutaf_llt |wc -l)" "1"
      -include $(DEPEND)
    endif
  endif
endif