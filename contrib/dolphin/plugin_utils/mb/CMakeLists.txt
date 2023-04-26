AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} dolphin_mb_SRC)
set(dolphin_mb_INC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${PROJECT_TRUNK_DIR}/distribute/include
    ${PROJECT_SRC_DIR}/include
    ${PROJECT_SRC_DIR}/lib/gstrace
    ${LZ4_INCLUDE_PATH}
    ${ZLIB_INCLUDE_PATH}
)

set(dolphin_mb_DEF_OPTIONS ${MACRO_OPTIONS})
set(dolphin_mb_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_DOLPHIN_OPTIONS} ${CHECK_OPTIONS})
set(dolphin_mb_LINK_OPTIONS ${LIB_LINK_OPTIONS})
add_static_objtarget(dolphin_mb dolphin_mb_SRC dolphin_mb_INC "${dolphin_mb_DEF_OPTIONS}" "${dolphin_mb_COMPILE_OPTIONS}" "${dolphin_mb_LINK_OPTIONS}")
