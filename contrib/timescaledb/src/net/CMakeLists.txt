set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/conn.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/conn_plain.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/http.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/http_response.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/http_request.cpp
)

if (USE_OPENSSL)
  list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/conn_ssl.cpp)
endif (USE_OPENSSL)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
