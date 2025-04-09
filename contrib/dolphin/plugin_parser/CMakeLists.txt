#This is the main CMAKE for build all components.
execute_process(
    COMMAND bison -d -o gram.cpp gram.y
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE PARSER_GRAM
)
execute_process(
    COMMAND sed -i "s/YY_NULL nullptr/YY_NULL 0/g" gram.cpp 
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE PARSER_GRAM
)

execute_process(
    COMMAND bison -d -o hint_gram.cpp hint_gram.y
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE PARSER_HINT_GRAM
)
execute_process(
    COMMAND sed -i "s/YY_NULL nullptr/YY_NULL 0/g" hint_gram.cpp
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE PARSER_HINT_GRAM
)

execute_process(
    COMMAND perl -I ${PROJECT_SRC_DIR}/tools/ ${PROJECT_SRC_DIR}/tools/gen_keywordlist.pl --extern ${CMAKE_CURRENT_SOURCE_DIR}/../include/plugin_parser/kwlist.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set(parser_cmd_src
"${PROJECT_SRC_DIR}/../contrib/dolphin/plugin_parser|rm -fr lex.backup||flex -CF -b -p -p -o'scan.inc' scan.l|sed -i 's/YY_NULL/YY_ZERO/g' scan.inc"
"${PROJECT_SRC_DIR}/../contrib/dolphin/plugin_parser|rm -fr lex.backup||flex -CF -b -p -p -o'hint_scan.inc' hint_scan.l|sed -i 's/YY_NULL/YY_ZERO/g' hint_scan.inc"
)
add_cmd_gen_when_configure(flex_target parser_cmd_src)

execute_process(
    COMMAND ln -fs ${CMAKE_CURRENT_SOURCE_DIR}/gram.hpp  ${CMAKE_CURRENT_SOURCE_DIR}/../include/plugin_parser/gram.hpp
    COMMAND ln -fs ${CMAKE_CURRENT_SOURCE_DIR}/hint_gram.hpp  ${CMAKE_CURRENT_SOURCE_DIR}/../include/plugin_parser/hint_gram.hpp
)

execute_process(
    COMMAND ln -fs ${CMAKE_CURRENT_SOURCE_DIR}/kwlist_d.h  ${CMAKE_CURRENT_SOURCE_DIR}/../include/plugin_parser/kwlist_d.h
)

set(dolphin_parser_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/analyze.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/keywords.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/kwlookup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_agg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_checksum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_clause.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_coerce.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_collate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_cte.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_describe.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_expr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_flush.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_func.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_hint.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_merge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_oper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_show_create.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_show.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_startwith.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_relation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_target.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_type.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parse_utilcmd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gram.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hint_gram.cpp
)
set(dolphin_parser_INC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${PROJECT_SRC_DIR}/include
    ${PROJECT_SRC_DIR}/lib/gstrace
    ${LZ4_INCLUDE_PATH}
    ${ZLIB_INCLUDE_PATH}
)

if("${ENABLE_MULTIPLE_NODES}" STREQUAL "ON")
    list(APPEND dolphin_parser_INC ${PROJECT_TRUNK_DIR}/distribute/include)
endif()

set(dolphin_parser_DEF_OPTIONS ${MACRO_OPTIONS})
set(dolphin_parser_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_DOLPHIN_OPTIONS} ${CHECK_OPTIONS})
set(dolphin_parser_LINK_OPTIONS ${LIB_LINK_OPTIONS})
add_static_objtarget(dolphin_parser dolphin_parser_SRC dolphin_parser_INC "${dolphin_parser_DEF_OPTIONS}" "${dolphin_parser_COMPILE_OPTIONS}" "${dolphin_parser_LINK_OPTIONS}")
