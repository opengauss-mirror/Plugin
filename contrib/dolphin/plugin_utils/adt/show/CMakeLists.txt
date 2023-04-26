AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} dolphin_adt_show_SRC)
set(dolphin_adt_show_INC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${PROJECT_SRC_DIR}/include
    ${PROJECT_SRC_DIR}/lib/gstrace
    ${LZ4_INCLUDE_PATH}
    ${ZLIB_INCLUDE_PATH}
)

set(dolphin_adt_show_DEF_OPTIONS ${MACRO_OPTIONS})
set(dolphin_adt_show_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_DOLPHIN_OPTIONS} ${CHECK_OPTIONS})
set(dolphin_adt_show_LINK_OPTIONS ${LIB_LINK_OPTIONS})
add_static_objtarget(dolphin_adt_show dolphin_adt_show_SRC dolphin_adt_show_INC "${dolphin_adt_show_DEF_OPTIONS}" "${dolphin_adt_show_COMPILE_OPTIONS}" "${dolphin_adt_show_LINK_OPTIONS}")
