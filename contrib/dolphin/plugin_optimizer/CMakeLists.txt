set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_RULE_MESSAGES OFF)
set(CMAKE_SKIP_RPATH TRUE)

set(CMAKE_MODULE_PATH 
    ${CMAKE_CURRENT_SOURCE_DIR}/commands
    ${CMAKE_CURRENT_SOURCE_DIR}/plan
    ${CMAKE_CURRENT_SOURCE_DIR}/prep
    ${CMAKE_CURRENT_SOURCE_DIR}/util
)

add_subdirectory(commands)
add_subdirectory(plan)
add_subdirectory(prep)
add_subdirectory(util)
