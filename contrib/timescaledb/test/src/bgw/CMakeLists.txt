set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/log.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/timer_mock.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/scheduler_mock.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/params.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_job_refresh.cpp)

target_sources(${TESTS_LIB_NAME} PRIVATE ${SOURCES})
