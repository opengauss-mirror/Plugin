AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} TGT_dolphin_proto_SRC)

set(TGT_dolphin_proto_INC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${PROJECT_SRC_DIR}/gausskernel/cbb/communication
    ${PROJECT_SRC_DIR}/include/libcomm
    ${PROJECT_SRC_DIR}/include
    ${PROJECT_SRC_DIR}/lib/gstrace
    ${LIBCGROUP_INCLUDE_PATH}
    ${LIBCGROUP_INCLUDE_PATH}/../include
    ${LIBORC_INCLUDE_PATH}
    ${LIBPARQUET_INCLUDE_PATH}
    ${ZLIB_INCLUDE_PATH}
    ${EVENT_INCLUDE_PATH}
    ${LZ4_INCLUDE_PATH}
    ${LIBOPENSSL_INCLUDE_PATH}
)

set(dolphin_proto_DEF_OPTIONS ${MACRO_OPTIONS})
set(dolphin_proto_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_DOLPHIN_OPTIONS} ${CHECK_OPTIONS})
set(dolphin_proto_LINK_OPTIONS ${LIB_LINK_OPTIONS})
add_static_objtarget(dolphin_proto TGT_dolphin_proto_SRC TGT_dolphin_proto_INC "${dolphin_proto_DEF_OPTIONS}" "${dolphin_proto_COMPILE_OPTIONS}" "${dolphin_proto_LINK_OPTIONS}")