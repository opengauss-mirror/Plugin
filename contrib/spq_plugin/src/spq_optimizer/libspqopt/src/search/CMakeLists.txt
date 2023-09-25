AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} TGT_spq_libspqopt_search_SRC)

SET(TGT_spq_libspqopt_search_INC ../../../libspqdbcost/include
                                ../../../libspqopt/include
                                ../../../libspqos/include
                                ../../../libnaucrates/include)

set(spq_libspqopt_search_DEF_OPTIONS ${MACRO_OPTIONS})
set(spq_libspqopt_search_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_SECURE_OPTIONS} ${CHECK_OPTIONS})
set(spq_libspqopt_search_LINK_OPTIONS ${LIB_LINK_OPTIONS})

add_static_objtarget(spqplugin_spq_libspqopt_search TGT_spq_libspqopt_search_SRC TGT_spq_libspqopt_search_INC "${spq_libspqopt_search_DEF_OPTIONS}" "${spq_libspqopt_search_COMPILE_OPTIONS}" "${spq_libspqopt_search_LINK_OPTIONS}")
