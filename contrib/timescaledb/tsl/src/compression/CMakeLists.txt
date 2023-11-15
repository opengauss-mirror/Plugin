set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/array.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/compression.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/create.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/compress_utils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/datum_serialize.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/deltadelta.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/dictionary.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gorilla.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/segment_meta.cpp
)
target_sources(${TSL_LIBRARY_NAME} PRIVATE ${SOURCES})
set(PROJECT_TRUNK_DIR ${CMAKE_SOURCE_DIR}/../..)
set(PROJECT_OPENGS_DIR ${PROJECT_TRUNK_DIR} CACHE INTERNAL "")
set(PROJECT_SRC_DIR ${PROJECT_OPENGS_DIR}/src CACHE INTERNAL "")
set(Third_party_library $ENV{BINARYLIBS}/kernel/dependency/libobs/comm/include)
add_definitions(-DPGXC)
add_definitions(-DUSE_SPQ)
include_directories(${PROJECT_SRC_DIR}/include ${PROJECT_INCLUDE_DIR} ${Third_party_library})