AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} TGT_spq_libspqos_error_SRC)

SET(TGT_spq_libspqos_error_INC ../../../libspqdbcost/include
                              ../../../libspqopt/include
                              ../../../libspqos/include
                              ../../../libnaucrates/include)

set(spq_libspqos_error_DEF_OPTIONS ${MACRO_OPTIONS})
set(spq_libspqos_error_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_SECURE_OPTIONS} ${CHECK_OPTIONS})
set(spq_libspqos_error_LINK_OPTIONS ${LIB_LINK_OPTIONS})

add_static_objtarget(spqplugin_spq_libspqos_error TGT_spq_libspqos_error_SRC TGT_spq_libspqos_error_INC "${spq_libspqos_error_DEF_OPTIONS}" "${spq_libspqos_error_COMPILE_OPTIONS}" "${spq_libspqos_error_LINK_OPTIONS}")
