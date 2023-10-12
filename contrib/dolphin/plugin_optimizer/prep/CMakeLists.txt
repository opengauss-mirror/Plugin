AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} dolphin_opt_prep_SRC)
set(dolphin_opt_prep_INC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${PROJECT_SRC_DIR}/gausskernel/cbb/communication
    ${PROJECT_SRC_DIR}/include/libcomm
    ${PROJECT_SRC_DIR}/include
    ${PROJECT_SRC_DIR}/lib/gstrace
    ${LIBCGROUP_INCLUDE_PATH}
    ${LZ4_INCLUDE_PATH}
    ${ZLIB_INCLUDE_PATH}
)

set(dolphin_opt_prep_DEF_OPTIONS ${MACRO_OPTIONS})
set(dolphin_opt_prep_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_DOLPHIN_OPTIONS} ${CHECK_OPTIONS})
set(dolphin_opt_prep_LINK_OPTIONS ${LIB_LINK_OPTIONS})
add_static_objtarget(dolphin_opt_prep dolphin_opt_prep_SRC dolphin_opt_prep_INC "${dolphin_opt_prep_DEF_OPTIONS}" "${dolphin_opt_prep_COMPILE_OPTIONS}" "${dolphin_opt_prep_LINK_OPTIONS}")
