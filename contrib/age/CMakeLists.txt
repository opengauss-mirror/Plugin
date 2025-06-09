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

execute_process(
    COMMAND flex -CF -b -p -p -o ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/ag_scanner.cpp ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/ag_scanner.l
)

execute_process(
    COMMAND bison ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_gram.y --defines=${CMAKE_CURRENT_SOURCE_DIR}/src/include/parser/cypher_gram_def.h -o ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_gram.cpp
)

execute_process(
    COMMAND sed -i 's/YY_NULL nullptr/YY_NULL 0/g' ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_gram.cpp
)

set(TOOLSDIR_AGE "${CMAKE_CURRENT_SOURCE_DIR}/src/tools")

execute_process(
    COMMAND perl -I ${CMAKE_CURRENT_SOURCE_DIR}/src/tools ${CMAKE_CURRENT_SOURCE_DIR}/src/tools/gen_keywordlist.pl --extern -o ${CMAKE_CURRENT_SOURCE_DIR}/src/include/parser ${CMAKE_CURRENT_SOURCE_DIR}/src/include/parser/kwlist.h
)

execute_process(
    COMMAND perl -I ${CMAKE_CURRENT_SOURCE_DIR}/src/tools ${CMAKE_CURRENT_SOURCE_DIR}/src/tools/gen_keywordlist.pl --extern --varname CypherKeyword --output ${CMAKE_CURRENT_SOURCE_DIR}/src/include/parser ${CMAKE_CURRENT_SOURCE_DIR}/src/include/parser/cypher_kwlist.h
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
    ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/ag_scanner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_analyze.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_clause.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_expr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_gram.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_item.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_keywords.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_parse_agg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_parse_node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${PADIR}/cypher_parser.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/${UTILDIR}/ag_func.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/${ADDIR}/age_global_graph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${ADDIR}/age_graphid_ds.cpp
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

    ${CMAKE_CURRENT_SOURCE_DIR}/${LODIR}/age_load.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${LODIR}/ag_load_edges.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${LODIR}/ag_load_labels.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${LODIR}/libcsv.cpp
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
add_shared_libtarget(age TGT_age_SRC TGT_age_INC "${age_DEF_OPTIONS}" "${age_COMPILE_OPTIONS}" "${age_LINK_OPTIONS}")
include_directories(
    ${PROJECT_OPENGS_DIR}/contrib/age
    ${PROJECT_OPENGS_DIR}/contrib/age/src/include
    ${PROJECT_SRC_DIR}/include
)
set_target_properties(age PROPERTIES PREFIX "")

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/age.control
    DESTINATION share/postgresql/extension/
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/age--1.0.0.sql
    DESTINATION share/postgresql/extension/
)
install(TARGETS age DESTINATION lib/postgresql)