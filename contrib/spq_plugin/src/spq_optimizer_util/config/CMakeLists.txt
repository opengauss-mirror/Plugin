AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} TGT_spq_config_SRC)

SET(TGT_spq_config_INC ${PROJECT_SRC_DIR}/include ../../../include
                       ../../spq_optimizer/libspqdbcost/include
                       ../../spq_optimizer/libspqopt/include
                       ../../spq_optimizer/libspqos/include
                       ../../spq_optimizer/libnaucrates/include)

set(spq_config_DEF_OPTIONS ${MACRO_OPTIONS})
set(spq_config_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_SECURE_OPTIONS} ${CHECK_OPTIONS})
set(spq_config_LINK_OPTIONS ${LIB_LINK_OPTIONS})

add_static_objtarget(spqplugin_spq_config TGT_spq_config_SRC TGT_spq_config_INC "${spq_config_DEF_OPTIONS}" "${spq_config_COMPILE_OPTIONS}" "${spq_config_LINK_OPTIONS}")
