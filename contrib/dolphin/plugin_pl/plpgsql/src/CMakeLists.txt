#This is the main CMAKE for build all components.
# libdolphin_plpgsql.a
execute_process(
    COMMAND bison -d -o pl_gram.cpp gram.y
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE PL_GRAM
)

execute_process(
    COMMAND perl ${PROJECT_SRC_DIR}/mtlocal.pl ${CMAKE_CURRENT_SOURCE_DIR}/pl_gram.cpp
    COMMAND perl ${PROJECT_SRC_DIR}/mtlocal.pl ${CMAKE_CURRENT_SOURCE_DIR}/pl_gram.hpp
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE PL_GRAM
)

execute_process(
    COMMAND perl ${PROJECT_SRC_DIR}/common/pl/plpgsql/src/generate-plerrcodes.pl ${PROJECT_SRC_DIR}/common/backend/utils/errcodes.txt
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/plerrcodes.h 
)

execute_process(
    COMMAND perl -I ${PROJECT_SRC_DIR}/tools/ ${PROJECT_SRC_DIR}/tools/gen_keywordlist.pl --varname ReservedPLKeywords ${CMAKE_CURRENT_SOURCE_DIR}/pl_reserved_kwlist.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

execute_process(
    COMMAND perl -I ${PROJECT_SRC_DIR}/tools/ ${PROJECT_SRC_DIR}/tools/gen_keywordlist.pl --varname UnreservedPLKeywords ${CMAKE_CURRENT_SOURCE_DIR}/pl_unreserved_kwlist.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set(TGT_dolphin_plpgsql_SRC 
    ${CMAKE_CURRENT_SOURCE_DIR}/pl_gram.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pl_comp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pl_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pl_scanner.cpp
)

set(dolphin_plpgsql_INC 
    ${PROJECT_SRC_DIR}/common/backend
)

set(dolphin_plpgsql_DEF_OPTIONS ${MACRO_OPTIONS})
set(dolphin_plpgsql_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_DOLPHIN_OPTIONS} ${CHECK_OPTIONS})
set(dolphin_plpgsql_LINK_OPTIONS ${LIB_LINK_OPTIONS})
add_static_objtarget(dolphin_plpgsql TGT_dolphin_plpgsql_SRC dolphin_plpgsql_INC "${dolphin_plpgsql_DEF_OPTIONS}" "${dolphin_plpgsql_COMPILE_OPTIONS}" "${dolphin_plpgsql_LINK_OPTIONS}")
