AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} TGT_spq_libnaucrates_xml_SRC)

SET(TGT_spq_libnaucrates_xml_INC ../../../libspqdbcost/include
                                 ../../../libspqopt/include
                                 ../../../libspqos/include
                                 ../../../libnaucrates/include)

set(spq_libnaucrates_xml_DEF_OPTIONS ${MACRO_OPTIONS})
set(spq_libnaucrates_xml_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_SECURE_OPTIONS} ${CHECK_OPTIONS})
set(spq_libnaucrates_xml_LINK_OPTIONS ${LIB_LINK_OPTIONS})

add_static_objtarget(spqplugin_spq_libnaucrates_xml TGT_spq_libnaucrates_xml_SRC TGT_spq_libnaucrates_xml_INC "${spq_libnaucrates_xml_DEF_OPTIONS}" "${spq_libnaucrates_xml_COMPILE_OPTIONS}" "${spq_libnaucrates_xml_LINK_OPTIONS}")
