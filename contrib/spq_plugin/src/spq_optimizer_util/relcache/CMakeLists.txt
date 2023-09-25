AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} TGT_spq_relcache_SRC)

SET(TGT_spq_relcache_INC ${PROJECT_SRC_DIR}/include ../../../include
                         ../../spq_optimizer/libspqdbcost/include
                         ../../spq_optimizer/libspqopt/include
                         ../../spq_optimizer/libspqos/include
                         ../../spq_optimizer/libnaucrates/include)

set(spq_relcache_DEF_OPTIONS ${MACRO_OPTIONS})
set(spq_relcache_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_SECURE_OPTIONS} ${CHECK_OPTIONS})
set(spq_relcache_LINK_OPTIONS ${LIB_LINK_OPTIONS})

add_static_objtarget(spqplugin_spq_relcache TGT_spq_relcache_SRC TGT_spq_relcache_INC "${spq_relcache_DEF_OPTIONS}" "${spq_relcache_COMPILE_OPTIONS}" "${spq_relcache_LINK_OPTIONS}")
