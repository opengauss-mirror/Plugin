AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} whale_opt_plan_SRC)
set(whale_opt_plan_INC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${PROJECT_TRUNK_DIR}/distribute/include
    ${PROJECT_SRC_DIR}/include
    ${PROJECT_SRC_DIR}/lib/gstrace
    ${LZ4_INCLUDE_PATH}
    ${ZLIB_INCLUDE_PATH}
)

set(whale_opt_plan_DEF_OPTIONS ${MACRO_OPTIONS})
set(whale_opt_plan_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_WHALE_OPTIONS} ${CHECK_OPTIONS})
set(whale_opt_plan_LINK_OPTIONS ${LIB_LINK_OPTIONS})
add_static_objtarget(whale_opt_plan whale_opt_plan_SRC whale_opt_plan_INC "${whale_opt_plan_DEF_OPTIONS}" "${whale_opt_plan_COMPILE_OPTIONS}" "${whale_opt_plan_LINK_OPTIONS}")
