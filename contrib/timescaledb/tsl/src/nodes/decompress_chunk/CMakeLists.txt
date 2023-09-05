# Add all *.c to sources in upperlevel directory
set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/decompress_chunk.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/exec.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/planner.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qual_pushdown.cpp
)
target_sources(${TSL_LIBRARY_NAME} PRIVATE ${SOURCES})
set(PROJECT_TRUNK_DIR ${CMAKE_SOURCE_DIR}/../..)
set(PROJECT_OPENGS_DIR ${PROJECT_TRUNK_DIR} CACHE INTERNAL "")
set(PROJECT_SRC_DIR ${PROJECT_OPENGS_DIR}/src CACHE INTERNAL "")
include_directories(${PROJECT_SRC_DIR}/include ${PROJECT_INCLUDE_DIR})