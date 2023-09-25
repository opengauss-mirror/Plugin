AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} TGT_spq_libnaucrates_statistics_SRC)

SET(TGT_spq_libnaucrates_statistics_INC ../../../libspqdbcost/include
                                        ../../../libspqopt/include
                                        ../../../libspqos/include
                                        ../../../libnaucrates/include)

set(spq_libnaucrates_statistics_DEF_OPTIONS ${MACRO_OPTIONS})
set(spq_libnaucrates_statistics_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_SECURE_OPTIONS} ${CHECK_OPTIONS})
set(spq_libnaucrates_statistics_LINK_OPTIONS ${LIB_LINK_OPTIONS})

add_static_objtarget(spqplugin_spq_libnaucrates_statistics TGT_spq_libnaucrates_statistics_SRC TGT_spq_libnaucrates_statistics_INC "${spq_libnaucrates_statistics_DEF_OPTIONS}" "${spq_libnaucrates_statistics_COMPILE_OPTIONS}" "${spq_libnaucrates_statistics_LINK_OPTIONS}")
