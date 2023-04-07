set(SOURCES)

if (${PG_VERSION_MAJOR} LESS "12")
  set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/tableam.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tupconvert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tuptable.cpp
    )
endif ()

if (${PG_VERSION_MAJOR} LESS "11")
  list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/fkeylist.cpp
    )
endif ()

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
