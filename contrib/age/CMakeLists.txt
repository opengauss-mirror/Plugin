set(BACKENDDIR "src/backend")
set(CATDIR "src/backend/catalog")
set(COMMDIR "src/backend/commands")
set(EXDIR "src/backend/executor")
set(NODIR "src/backend/nodes")
set(OPDIR "src/backend/optimizer")
set(PADIR "src/backend/parser")
set(UTILDIR "src/backend/utils")
set(ADDIR "src/backend/utils/adt")
set(CADIR "src/backend/utils/cache")
set(LODIR "src/backend/utils/load")

set(TOOLSDIR_AGE "${CMAKE_CURRENT_SOURCE_DIR}/src/tools")
set(AGE_GENERATED_BACKEND_DIR "${CMAKE_CURRENT_BINARY_DIR}/${PADIR}")
set(AGE_GENERATED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/src/include/parser")

find_program(AGE_FLEX_EXECUTABLE NAMES flex REQUIRED)
find_program(AGE_BISON_EXECUTABLE NAMES bison REQUIRED)
find_program(AGE_PERL_EXECUTABLE NAMES perl REQUIRED)

foreach(tool_variable AGE_FLEX_EXECUTABLE AGE_BISON_EXECUTABLE AGE_PERL_EXECUTABLE)
    execute_process(
        COMMAND ${${tool_variable}} --version
        RESULT_VARIABLE tool_version_result
        OUTPUT_VARIABLE tool_version_output
        ERROR_VARIABLE tool_version_error
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(NOT tool_version_result EQUAL 0)
        message(FATAL_ERROR "Unable to query ${tool_variable}: ${tool_version_error}")
    endif()
    message(STATUS "AGE generator ${tool_variable}: ${tool_version_output}")
endforeach()

set(AGE_SCANNER_SOURCE "${AGE_GENERATED_BACKEND_DIR}/ag_scanner.cpp")
set(AGE_GRAMMAR_SOURCE "${AGE_GENERATED_BACKEND_DIR}/cypher_gram.cpp")
set(AGE_GRAMMAR_HEADER "${AGE_GENERATED_INCLUDE_DIR}/cypher_gram_def.h")
set(AGE_KEYWORD_HEADER "${AGE_GENERATED_INCLUDE_DIR}/kwlist_d.h")
set(AGE_CYPHER_KEYWORD_HEADER "${AGE_GENERATED_INCLUDE_DIR}/cypher_kwlist_d.h")

add_custom_command(
    OUTPUT ${AGE_SCANNER_SOURCE}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${AGE_GENERATED_BACKEND_DIR}
    COMMAND ${AGE_FLEX_EXECUTABLE} -CF -b -p -p -o ${AGE_SCANNER_SOURCE}
            ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/ag_scanner.l
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/ag_scanner.l
    COMMENT "Generating AGE scanner with Flex"
    VERBATIM
)

add_custom_command(
    OUTPUT ${AGE_GRAMMAR_SOURCE} ${AGE_GRAMMAR_HEADER}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${AGE_GENERATED_BACKEND_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${AGE_GENERATED_INCLUDE_DIR}
    COMMAND ${AGE_BISON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_gram.y
            --defines=${AGE_GRAMMAR_HEADER} -o ${AGE_GRAMMAR_SOURCE}
    COMMAND ${AGE_PERL_EXECUTABLE} -pi -e "s/YY_NULL nullptr/YY_NULL 0/g"
            ${AGE_GRAMMAR_SOURCE}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_gram.y
    COMMENT "Generating AGE grammar with Bison"
    VERBATIM
)

add_custom_command(
    OUTPUT ${AGE_KEYWORD_HEADER}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${AGE_GENERATED_INCLUDE_DIR}
    COMMAND ${AGE_PERL_EXECUTABLE} -I ${TOOLSDIR_AGE}
            ${TOOLSDIR_AGE}/gen_keywordlist.pl --extern -o ${AGE_GENERATED_INCLUDE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/src/include/parser/kwlist.h
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/include/parser/kwlist.h
            ${TOOLSDIR_AGE}/gen_keywordlist.pl ${TOOLSDIR_AGE}/PerfectHash.pm
    COMMENT "Generating AGE SQL keyword table"
    VERBATIM
)

add_custom_command(
    OUTPUT ${AGE_CYPHER_KEYWORD_HEADER}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${AGE_GENERATED_INCLUDE_DIR}
    COMMAND ${AGE_PERL_EXECUTABLE} -I ${TOOLSDIR_AGE}
            ${TOOLSDIR_AGE}/gen_keywordlist.pl --extern --varname CypherKeyword
            --output ${AGE_GENERATED_INCLUDE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/src/include/parser/cypher_kwlist.h
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/include/parser/cypher_kwlist.h
            ${TOOLSDIR_AGE}/gen_keywordlist.pl ${TOOLSDIR_AGE}/PerfectHash.pm
    COMMENT "Generating AGE Cypher keyword table"
    VERBATIM
)

add_custom_target(age_generated_sources
    DEPENDS ${AGE_SCANNER_SOURCE} ${AGE_GRAMMAR_SOURCE} ${AGE_GRAMMAR_HEADER}
            ${AGE_KEYWORD_HEADER} ${AGE_CYPHER_KEYWORD_HEADER}
)

set(TGT_age_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/${BACKENDDIR}/age.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CATDIR}/ag_catalog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CATDIR}/ag_graph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CATDIR}/ag_label.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CATDIR}/ag_namespace.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/${COMMDIR}/graph_commands.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${COMMDIR}/label_commands.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/${EXDIR}/cypher_create.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${EXDIR}/cypher_delete.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${EXDIR}/cypher_merge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${EXDIR}/cypher_set.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${EXDIR}/cypher_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${EXDIR}/execGraphVle.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/${NODIR}/ag_extensible.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${NODIR}/ag_extensible_read.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${NODIR}/ag_nodes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${NODIR}/cypher_copyfuncs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${NODIR}/cypher_outfuncs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${NODIR}/cypher_readfuncs.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/${OPDIR}/cypher_createplan.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${OPDIR}/cypher_pathnode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${OPDIR}/cypher_paths.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/ag_parse_expr.cpp
    ${AGE_SCANNER_SOURCE}
    ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_analyze.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_clause.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_expr.cpp
    ${AGE_GRAMMAR_SOURCE}
    ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_item.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_keywords.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_parse_agg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_parse_node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_parser.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/${UTILDIR}/ag_func.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${UTILDIR}/ag_guc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${UTILDIR}/graph_generation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${UTILDIR}/name_validation.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/${ADDIR}/age_global_graph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${ADDIR}/age_graphid_ds.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${ADDIR}/age_session_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${ADDIR}/age_vle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${ADDIR}/ag_extend_func.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${ADDIR}/ag_float8_supp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${ADDIR}/agtype.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${ADDIR}/agtype_ext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${ADDIR}/agtype_gin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${ADDIR}/agtype_ops.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${ADDIR}/agtype_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${ADDIR}/agtype_raw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${ADDIR}/agtype_util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${ADDIR}/cypher_funcs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${ADDIR}/graphid.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/${CADIR}/ag_cache.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${CADIR}/agehash.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/${LODIR}/ag_csv_reader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${LODIR}/age_load.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${LODIR}/ag_load_edges.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${LODIR}/ag_load_labels.cpp
)

set(TGT_age_INC 
    ${PROJECT_OPENGS_DIR}/contrib/age
    ${PROJECT_OPENGS_DIR}/contrib/age/src/include
	${PROJECT_OPENGS_DIR}/contrib/age/src/include/parser
    ${PROJECT_SRC_DIR}/include
)

set(age_DEF_OPTIONS ${MACRO_OPTIONS})
set(age_COMPILE_OPTIONS ${OPTIMIZE_OPTIONS} ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${LIB_SECURE_OPTIONS} ${CHECK_OPTIONS})
set(age_LINK_OPTIONS ${LIB_LINK_OPTIONS})
list(APPEND age_LINK_OPTIONS
    "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/age.exports.map")
add_shared_libtarget(age TGT_age_SRC TGT_age_INC "${age_DEF_OPTIONS}" "${age_COMPILE_OPTIONS}" "${age_LINK_OPTIONS}")
set_property(TARGET age APPEND PROPERTY LINK_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/age.exports.map")
add_dependencies(age age_generated_sources)
include_directories(
    ${PROJECT_OPENGS_DIR}/contrib/age
    ${PROJECT_OPENGS_DIR}/contrib/age/src/include
    ${CMAKE_CURRENT_BINARY_DIR}/src/include
    ${AGE_GENERATED_INCLUDE_DIR}
    ${PROJECT_SRC_DIR}/include
)
set_target_properties(age PROPERTIES PREFIX "")

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/age.control
    DESTINATION share/postgresql/extension/
)
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/age--1.0.0.sql
    ${CMAKE_CURRENT_SOURCE_DIR}/age--1.0.1.sql
    ${CMAKE_CURRENT_SOURCE_DIR}/age--1.0.0--1.0.1.sql
    ${CMAKE_CURRENT_SOURCE_DIR}/age--1.0.1--1.0.0.sql
    DESTINATION share/postgresql/extension/
)
install(TARGETS age DESTINATION lib/postgresql)