set(SOURCES
  loader.cpp
  bgw_message_queue.cpp
  bgw_counter.cpp
  bgw_launcher.cpp
  bgw_interface.cpp
  lwlocks.cpp
  tsdb_loader.cpp
  ${PROJECT_SOURCE_DIR}/src/tsdb_dsm.cpp
  ${PROJECT_SOURCE_DIR}/src/tsdb_shm.cpp
  ${PROJECT_SOURCE_DIR}/src/tsdb_event_trigger.cpp
  ${PROJECT_SOURCE_DIR}/src/tsdb_get_obj.cpp
  ${PROJECT_SOURCE_DIR}/src/tsdb_extension.cpp
  ${PROJECT_SOURCE_DIR}/src/tsdb_head.cpp
  ${PROJECT_SOURCE_DIR}/src/tsdb_static.cpp
  ${PROJECT_SOURCE_DIR}/src/tsdb_static2.cpp
  ${PROJECT_SOURCE_DIR}/src/tsdb.cpp
  
)

set(TEST_SOURCES
  ${PROJECT_SOURCE_DIR}/test/src/symbol_conflict.cpp
)

add_library(${PROJECT_NAME}-loader MODULE ${SOURCES})

if (CMAKE_BUILD_TYPE MATCHES Debug)
  # Include code for tests in Debug build
  target_sources(${PROJECT_NAME}-loader PRIVATE ${TEST_SOURCES})
  # This define generates extension-specific code for symbol conflict testing
  target_compile_definitions(${PROJECT_NAME}-loader PUBLIC MODULE_NAME=loader)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

set_target_properties(${PROJECT_NAME}-loader PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    PREFIX "")

install(
  TARGETS ${PROJECT_NAME}-loader
  DESTINATION ${PG_PKGLIBDIR})
