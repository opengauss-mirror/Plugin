# Add all *.c to sources in upperlevel directory
set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/chunk_append.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/exec.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/explain.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/planner.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/transform.cpp
)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
