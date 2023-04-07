set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/reorder_api.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/drop_chunks_api.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/compress_chunks_api.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/job.cpp
)
target_sources(${TSL_LIBRARY_NAME} PRIVATE ${SOURCES})
