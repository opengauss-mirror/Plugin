#This is the main CMAKE for build all components.
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_RULE_MESSAGES OFF)
set(CMAKE_SKIP_RPATH TRUE)

set(CMAKE_MODULE_PATH 
    ${CMAKE_CURRENT_SOURCE_DIR}/show
)
add_subdirectory(show)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} dolphin_utils_adt_SRC)
list(REMOVE_ITEM dolphin_utils_adt_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/like_match.cpp
)

set(dolphin_utils_adt_INC 
    ${PROJECT_TRUNK_DIR}/distribute/include
    ${PROJECT_SRC_DIR}/include
    ${PROJECT_SRC_DIR}/lib/gstrace
    ${LZ4_INCLUDE_PATH}
    ${ZLIB_INCLUDE_PATH}
    ${LIBOPENSSL_INCLUDE_PATH}
)

set(dolphin_adt_DEF_OPTIONS ${MACRO_OPTIONS})
set(dolphin_adt_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_DOLPHIN_OPTIONS} ${CHECK_OPTIONS})
set(dolphin_adt_LINK_OPTIONS ${LIB_LINK_OPTIONS})
add_static_objtarget(dolphin_utils_adt dolphin_utils_adt_SRC dolphin_utils_adt_INC "${dolphin_adt_DEF_OPTIONS}" "${dolphin_adt_COMPILE_OPTIONS}" "${dolphin_adt_LINK_OPTIONS}")
