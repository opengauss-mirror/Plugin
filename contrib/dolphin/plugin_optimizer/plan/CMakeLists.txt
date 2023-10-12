AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} dolphin_opt_plan_SRC)
set(dolphin_opt_plan_INC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${PROJECT_TRUNK_DIR}/distribute/include
    ${PROJECT_SRC_DIR}/include
    ${PROJECT_SRC_DIR}/lib/gstrace
    ${LZ4_INCLUDE_PATH}
    ${ZLIB_INCLUDE_PATH}
)

set(dolphin_opt_plan_DEF_OPTIONS ${MACRO_OPTIONS})
set(dolphin_opt_plan_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_DOLPHIN_OPTIONS} ${CHECK_OPTIONS})
set(dolphin_opt_plan_LINK_OPTIONS ${LIB_LINK_OPTIONS})
add_static_objtarget(dolphin_opt_plan dolphin_opt_plan_SRC dolphin_opt_plan_INC "${dolphin_opt_plan_DEF_OPTIONS}" "${dolphin_opt_plan_COMPILE_OPTIONS}" "${dolphin_opt_plan_LINK_OPTIONS}")
