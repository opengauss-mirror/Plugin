AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} TGT_executor_SRC)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_RULE_MESSAGES OFF)
set(CMAKE_SKIP_RPATH TRUE)

SET(TGT_executor_INC
        ${PROJECT_SRC_DIR}/include
        ../../include
        ${PROJECT_OPENGS_DIR}/contrib/ndpplugin/component/thread
)

set(spq_DEF_OPTIONS ${MACRO_OPTIONS})
set(spq_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_SECURE_OPTIONS} ${CHECK_OPTIONS})
set(spq_LINK_OPTIONS ${LIB_LINK_OPTIONS})

add_static_objtarget(spqplugin_executor TGT_executor_SRC TGT_executor_INC "${spq_DEF_OPTIONS}" "${spq_COMPILE_OPTIONS}" "${spq_LINK_OPTIONS}")
