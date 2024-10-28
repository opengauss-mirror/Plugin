set(EXTENSION "chparser")
set(EXTVERSION "1.0")

find_program(SCWS_EXECUTABLE NAMES scws)
if (SCWS_EXECUTABLE)
message(STATUS "scws library or head file found")
get_filename_component(SCWS_BIN_DIR ${SCWS_EXECUTABLE} DIRECTORY)

get_filename_component(SCWS_HOME_B ${SCWS_BIN_DIR} DIRECTORY)
set(SCWS_HOME ${SCWS_HOME_B} CACHE PATH "Path to scws")
find_library(SCWS_LIB scws PATHS ${SCWS_HOME}/lib)
find_path(SCWS_INC scws.h PATHS  ${SCWS_HOME}/include/scws)

if(SCWS_LIB AND SCWS_INC)
message(STATUS "scws library ${SCWS_LIB}")
set(SCWS_LINK "-L${SCWS_HOME}/lib -lscws")

file(GLOB_RECURSE TGT_chparser_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

SET(TGT_chparser_INC
    ${PROJECT_SRC_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SCWS_INC}
)

set(chparser_DEF_OPTIONS ${MACRO_OPTIONS})
set(chparser_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_SECURE_OPTIONS} ${CHECK_OPTIONS})
set(chparser_LINK_OPTIONS ${LIB_LINK_OPTIONS})

add_shared_libtarget(chparser TGT_chparser_SRC TGT_chparser_INC "${chparser_DEF_OPTIONS}" "${chparser_COMPILE_OPTIONS}" "${chparser_LINK_OPTIONS}")

link_libraries(${SCWS_LIB})
link_directories("${SCWS_LIB}/lib")
target_link_libraries(chparser ${SCWS_LINK})
set_target_properties(chparser PROPERTIES PREFIX "")

add_custom_target(
    CopyFileChparser ALL
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/sql/${EXTENSION}--${EXTVERSION}.sql
)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES \${SQL_FILE_PATH})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${EXTENSION}.control
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/sql/${EXTENSION}--${EXTVERSION}.sql
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dict.utf8.xdb
    DESTINATION share/postgresql/tsearch_data/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dict_extra.txt
    DESTINATION share/postgresql/tsearch_data/
)

install(TARGETS chparser DESTINATION lib/postgresql)

else()
    message(WARNING "scws library or head file not found, skip building chparser")
    return()
endif()

else()
    message(WARNING "scws is not installed, skip building chparser")
    return()
endif()