#This is the main CMAKE for build all assessment.
# assessment.so
set(TGT_assessment_SRC assessment.cpp)
set(assessment_DEF_OPTIONS ${MACRO_OPTIONS})
set(assessment_LINK_OPTIONS ${LIB_LINK_OPTIONS})
set(assessment_COMPILE_OPTIONS ${LIB_SECURE_OPTIONS} -fPIC)
add_shared_libtarget(assessment TGT_assessment_SRC "" "${assessment_DEF_OPTIONS}" "${assessment_COMPILE_OPTIONS}" "${assessment_LINK_OPTIONS}")
set_target_properties(assessment PROPERTIES PREFIX "")

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/assessment.control
        DESTINATION share/postgresql/extension/
        )
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/assessment--1.0.sql
        DESTINATION share/postgresql/extension/
        )
install(TARGETS assessment DESTINATION lib/postgresql)


set(TGT_assessment_INC
        ${PROJECT_SRC_DIR}/include
        ${PROJECT_SRC_DIR}/common/interfaces/libpq
        ${PROJECT_SRC_DIR}/include/libpq
        # psqlscan.h
        ${PROJECT_SRC_DIR}/bin/psql
        )
# assessment_database
set(assessment_database_LINK_LIBS libpgcommon.a libpgport.a libsecurec.a -lpq -lgssapi_krb5_gauss -lgssrpc_gauss -lkrb5_gauss -lkrb5support_gauss -lk5crypto_gauss -lcom_err_gauss)
set(assessment_LINK_DIRS ${SECURE_LIB_PATH} ${CMAKE_BINARY_DIR}/lib ${KERBEROS_LIB_PATH})
set(assessment_DEF_OPTIONS ${MACRO_OPTIONS} -DFRONTEND)
set(assessment_COMPILE_OPTIONS ${OS_OPTIONS} ${PROTECT_OPTIONS} ${WARNING_OPTIONS} ${CHECK_OPTIONS} ${BIN_SECURE_OPTIONS} ${OPTIMIZE_OPTIONS})
set(TGT_assessment_database_SRC main.cpp OpenGaussConnection.cpp Scan.cpp Report.cpp)
add_bintarget(assessment_database TGT_assessment_database_SRC TGT_assessment_INC "${assessment_DEF_OPTIONS}" "${assessment_COMPILE_OPTIONS}" "${assessment_LINK_OPTIONS}" "${assessment_database_LINK_LIBS}")
add_dependencies(assessment_database gsql)
target_link_directories(assessment_database PUBLIC ${assessment_LINK_DIRS})
install(TARGETS assessment_database RUNTIME DESTINATION bin)
