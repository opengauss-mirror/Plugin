override CPPFLAGS := -I$(top_builddir)/contrib/spq_plugin/src/spq_optimizer/libspqos/include $(CPPFLAGS)
override CPPFLAGS := -I$(top_builddir)/contrib/spq_plugin/src/spq_optimizer/libspqopt/include $(CPPFLAGS)
override CPPFLAGS := -I$(top_builddir)/contrib/spq_plugin/src/spq_optimizer/libnaucrates/include $(CPPFLAGS)
override CPPFLAGS := -I$(top_builddir)/contrib/spq_plugin/src/spq_optimizer/libspqdbcost/include $(CPPFLAGS)
override CPPFLAGS := -DUSE_SPQ $(CPPFLAGS)
# Do not omit frame pointer. Even with RELEASE builds, it is used for
# backtracing.
override CPPFLAGS := -std=c++11 $(CPPFLAGS)
override CXXFLAGS := -std=c++11 -Wno-variadic-macros -fno-omit-frame-pointer $(CXXFLAGS)
include $(top_builddir)/contrib/spq_plugin/spqplugin.mk