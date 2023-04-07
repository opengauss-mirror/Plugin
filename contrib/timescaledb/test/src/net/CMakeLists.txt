set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/test_conn.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_http.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/conn_mock.cpp
)

target_sources(${TESTS_LIB_NAME} PRIVATE ${SOURCES})
target_include_directories(${TESTS_LIB_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src/net)
