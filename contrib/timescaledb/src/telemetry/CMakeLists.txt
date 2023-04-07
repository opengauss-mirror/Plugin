# Add all *.c to sources in upperlevel directory
set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/uuid.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/telemetry_metadata.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/telemetry.cpp
)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
