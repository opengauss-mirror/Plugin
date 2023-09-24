#This is the main CMAKE for build bin.

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_RULE_MESSAGES OFF)
set(CMAKE_SKIP_RPATH TRUE)

set(CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/libspqdbcost
    ${CMAKE_CURRENT_SOURCE_DIR}/libspqopt
    ${CMAKE_CURRENT_SOURCE_DIR}/libspqos
    ${CMAKE_CURRENT_SOURCE_DIR}/libnaucrates
)

add_subdirectory(libspqdbcost)
add_subdirectory(libspqopt)
add_subdirectory(libspqos)
add_subdirectory(libnaucrates)

