override CPPFLAGS := -I$(abs_top_srcdir)/contrib/spq_plugin/include $(CPPFLAGS)
override CPPFLAGS := -I$(abs_top_srcdir)/contrib/spq_plugin/src/spq_optimizer/libspqos/include $(CPPFLAGS)
override CPPFLAGS := -I$(abs_top_srcdir)/contrib/spq_plugin/src/spq_optimizer/libspqopt/include $(CPPFLAGS)
override CPPFLAGS := -I$(abs_top_srcdir)/contrib/spq_plugin/src/spq_optimizer/libnaucrates/include $(CPPFLAGS)
override CPPFLAGS := -I$(abs_top_srcdir)/contrib/spq_plugin/src/spq_optimizer/libspqdbcost/include $(CPPFLAGS)

override CPPFLAGS := -DUSE_SPQ $(CPPFLAGS)
override CPPFLAGS := -std=c++11 -fno-omit-frame-pointer $(CPPFLAGS)
override CXXFLAGS := -std=c++11 -fno-omit-frame-pointer $(CXXFLAGS)
include $(top_builddir)/contrib/spq_plugin/spqplugin.mk