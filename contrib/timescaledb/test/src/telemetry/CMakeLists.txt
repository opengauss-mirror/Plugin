set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/metadata.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_privacy.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_telemetry.cpp
)

target_sources(${TESTS_LIB_NAME} PRIVATE ${SOURCES})
