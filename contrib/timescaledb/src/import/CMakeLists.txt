set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/planner.cpp
)

if ((${PG_VERSION_MAJOR} EQUAL "12") OR
    (${PG_VERSION_MAJOR} GREATER "12"))
  set(SOURCES
      ${SOURCES}
      ${CMAKE_CURRENT_SOURCE_DIR}/allpaths.cpp
    )
endif ()

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
