set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/job.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/job_stat.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/launcher_interface.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/scheduler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/timer.cpp
)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
