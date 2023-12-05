set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_RULE_MESSAGES OFF)
set(CMAKE_SKIP_RPATH TRUE)

execute_process(
    COMMAND cp -r ${PROJECT_SRC_DIR}/common/backend/utils/mb/nlssort ${CMAKE_CURRENT_SOURCE_DIR}/mb
)

set(CMAKE_MODULE_PATH 
    ${CMAKE_CURRENT_SOURCE_DIR}/adt
    ${CMAKE_CURRENT_SOURCE_DIR}/fmgr
    ${CMAKE_CURRENT_SOURCE_DIR}/mb
)

add_subdirectory(adt)
add_subdirectory(fmgr)
add_subdirectory(mb)

execute_process(
    COMMAND perl ${CMAKE_CURRENT_SOURCE_DIR}/Gen_fmgrtab.pl ${CMAKE_CURRENT_SOURCE_DIR}/../include/builtin_funcs.ini
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

execute_process(
    COMMAND ln -fs ${CMAKE_CURRENT_SOURCE_DIR}/fmgroids.h ${CMAKE_CURRENT_SOURCE_DIR}/../include/plugin_utils/fmgroids.h
)
