//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		exception.cpp
//
//	@doc:
//		Initialization of DXL-specific exception messages
//---------------------------------------------------------------------------

#include "naucrates/exception.h"

#include "spqos/error/CMessage.h"
#include "spqos/error/CMessageRepository.h"

using namespace spqos;
using namespace spqdxl;


//---------------------------------------------------------------------------
//	@function:
//		EresExceptionInit
//
//	@doc:
//		Message initialization for DXL exceptions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
spqdxl::EresExceptionInit(CMemoryPool *mp)
{
	//---------------------------------------------------------------------------
	// Basic DXL messages in English
	//---------------------------------------------------------------------------
	CMessage rgmsg[ExmiDXLSentinel] = {
		// DXL-parsing related messages
		CMessage(CException(spqdxl::ExmaDXL, spqdxl::ExmiDXLUnexpectedTag),
				 CException::ExsevError, SPQOS_WSZ_WSZLEN("Unexpected tag: %ls"),
				 1,	 // elemname
				 SPQOS_WSZ_WSZLEN("Unexpected tag encountered during parsing;")),

		CMessage(
			CException(spqdxl::ExmaDXL, spqdxl::ExmiDXLMissingAttribute),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN(
				"Missing attribute value for attribute %ls in element %ls"),
			2,	// # params: attrname, elemname
			SPQOS_WSZ_WSZLEN("Missing attribute value")),

		CMessage(
			CException(spqdxl::ExmaDXL, spqdxl::ExmiDXLInvalidAttributeValue),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN("Invalid value for attribute %ls in element %ls"),
			2,	// # params: attrname, elemname
			SPQOS_WSZ_WSZLEN("Invalid attribute value")),

		CMessage(CException(spqdxl::ExmaDXL, spqdxl::ExmiDXLUnrecognizedOperator),
				 CException::ExsevError,
				 SPQOS_WSZ_WSZLEN("DXL: Unrecognized operator: %ls"),
				 1,	 // elemname
				 SPQOS_WSZ_WSZLEN("Unrecognized operator name")),

		CMessage(CException(spqdxl::ExmaDXL, spqdxl::ExmiDXLUnrecognizedType),
				 CException::ExsevError,
				 SPQOS_WSZ_WSZLEN("Unrecognized type name"),
				 0,	 // TODO:  - Sep 30, 2010; add params: typename
				 SPQOS_WSZ_WSZLEN("Unrecognized type name")),

		CMessage(
			CException(spqdxl::ExmaDXL, spqdxl::ExmiDXLUnrecognizedCompOperator),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN("Unrecognized comparison operator"),
			1,	// comp op
			SPQOS_WSZ_WSZLEN("Unrecognized comparison operator")),

		CMessage(
			CException(spqdxl::ExmaDXL, spqdxl::ExmiDXLValidationError),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN(
				"XML validation exception: XML document does not conform to the DXL schema. Details: %s"),
			1,	// exception details
			SPQOS_WSZ_WSZLEN("XML validation exception")),

		CMessage(CException(spqdxl::ExmaDXL, spqdxl::ExmiDXLXercesParseError),
				 CException::ExsevError,
				 SPQOS_WSZ_WSZLEN("Xerces parse exception"),
				 0,	 //
				 SPQOS_WSZ_WSZLEN("Xerces parse exception")),

		CMessage(
			CException(spqdxl::ExmaDXL, spqdxl::ExmiDXLIncorrectNumberOfChildren),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN("Incorrect Number of children"),
			0,	//
			SPQOS_WSZ_WSZLEN("Incorrect Number of children")),

		CMessage(
			CException(spqdxl::ExmaDXL, spqdxl::ExmiPlStmt2DXLConversion),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN("SPQDB Expression type: %ls not supported in DXL"),
			1,	//
			SPQOS_WSZ_WSZLEN("SPQDB Expression type not supported in DXL")),

		CMessage(
			CException(spqdxl::ExmaDXL,
					   spqdxl::ExmiDXL2PlStmtMissingPlanForSubPlanTranslation),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN(
				"DXL-to-PlStmt: Missing Plan During SubPlan Translation"),
			0,	//
			SPQOS_WSZ_WSZLEN(
				"DXL-to-PlStmt: Missing Plan During SubPlan Translation")),

		CMessage(
			CException(spqdxl::ExmaDXL, spqdxl::ExmiDXL2PlStmtConversion),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN("DXL-to-PlStmt Translation: %ls not supported"),
			1,	//
			SPQOS_WSZ_WSZLEN("DXL-to-PlStmt Translation not supported")),

		CMessage(
			CException(spqdxl::ExmaDXL, spqdxl::ExmiDXL2PlStmtExternalScanError),
			CException::ExsevError, SPQOS_WSZ_WSZLEN("External scan error: %ls"),
			1,	//
			SPQOS_WSZ_WSZLEN("External scan error")),

		CMessage(
			CException(spqdxl::ExmaDXL, spqdxl::ExmiQuery2DXLAttributeNotFound),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN(
				"Query-to-DXL Translation: Attribute number %d not found in project list"),
			1,	//
			SPQOS_WSZ_WSZLEN(
				"Query-to-DXL Translation: Attribute number not found in project list")),

		CMessage(
			CException(spqdxl::ExmaDXL, spqdxl::ExmiQuery2DXLUnsupportedFeature),
			CException::ExsevNotice,
			SPQOS_WSZ_WSZLEN("Feature not supported: %ls"),
			1,	//
			SPQOS_WSZ_WSZLEN("Feature not supported")),

		CMessage(CException(spqdxl::ExmaDXL, spqdxl::ExmiQuery2DXLMissingValue),
				 CException::ExsevError,
				 SPQOS_WSZ_WSZLEN("Query-to-DXL Translation: Missing %ls value"),
				 1,	 //
				 SPQOS_WSZ_WSZLEN("Query-to-DXL Translation: Missing value")),

		CMessage(
			CException(spqdxl::ExmaDXL, spqdxl::ExmiQuery2DXLNotNullViolation),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN(
				"null value in column \"%ls\" violates not-null constraint"),
			1,	//
			SPQOS_WSZ_WSZLEN(
				"null value in column violates not-null constraint")),

		CMessage(
			CException(spqdxl::ExmaDXL, spqdxl::ExmiQuery2DXLDuplicateRTE),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN(
				"DXL-to-Query: Duplicate range table entry at query level %d at position %d"),
			2,	// query level and var no
			SPQOS_WSZ_WSZLEN("DXL-to-Query: Duplicate range table entry")),

		// MD related messages
		CMessage(
			CException(spqdxl::ExmaMD, spqdxl::ExmiMDCacheEntryDuplicate),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN(
				"Entry for metadata cache object %ls already exists"),
			1,	// mdid
			SPQOS_WSZ_WSZLEN("Entry for metadata cache object already exists")),

		CMessage(CException(spqdxl::ExmaMD, spqdxl::ExmiMDCacheEntryNotFound),
				 CException::ExsevError,
				 SPQOS_WSZ_WSZLEN("Lookup of object %ls in cache failed"),
				 1,	 // mdid
				 SPQOS_WSZ_WSZLEN("Lookup of object in cache failed")),

		CMessage(CException(spqdxl::ExmaMD, spqdxl::ExmiMDObjUnsupported),
				 CException::ExsevNotice,
				 SPQOS_WSZ_WSZLEN("Feature not supported: %ls"),
				 1,	 // md obj
				 SPQOS_WSZ_WSZLEN("Feature not supported")),

		CMessage(CException(spqdxl::ExmaComm, spqdxl::ExmiCommPropagateError),
				 CException::ExsevError, SPQOS_WSZ_WSZLEN("%S"),
				 1,	 // message
				 SPQOS_WSZ_WSZLEN("Propagate remote exception")),

		CMessage(
			CException(spqdxl::ExmaComm, spqdxl::ExmiCommPropagateError),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN("Received unexpected message type from OPT: %d"),
			1,	// type
			SPQOS_WSZ_WSZLEN("Received unexpected message type from OPT")),

		CMessage(CException(spqdxl::ExmaSPQDB, spqdxl::ExmiSPQDBError),
				 CException::ExsevError, SPQOS_WSZ_WSZLEN("PG exception raised"),
				 1,	 // type
				 SPQOS_WSZ_WSZLEN("PG exception converted to SPQOS exception")),

		CMessage(
			CException(spqdxl::ExmaDXL, spqdxl::ExmiQuery2DXLError),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN(
				"Query-to-DXL Translation: %ls entry found due to incorrect normalization of query"),
			1,	// entry name
			SPQOS_WSZ_WSZLEN(
				"Query-to-DXL Translation: Entry not found due to incorrect normalization of query")),

		CMessage(
			CException(spqdxl::ExmaDXL, spqdxl::ExmiExpr2DXLUnsupportedFeature),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN("Feature not supported: %ls"),
			1,	// feature name
			SPQOS_WSZ_WSZLEN("Feature not supported: %ls")),

		CMessage(
			CException(spqdxl::ExmaConstExprEval,
					   spqdxl::ExmiConstExprEvalNonConst),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN(
				"Illegal column reference \"%ls\" in constant expression"),
			1,	// column name
			SPQOS_WSZ_WSZLEN(
				"Illegal column reference \"%ls\" in constant expression")),

		CMessage(
			CException(spqdxl::ExmaDXL, spqdxl::ExmiExpr2DXLAttributeNotFound),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN(
				"Expr-to-DXL Translation: Attribute number %d not found in project list"),
			1,	// attno
			SPQOS_WSZ_WSZLEN(
				"Expr-to-DXL Translation: Attribute number not found in project list")),

		CMessage(
			CException(spqdxl::ExmaDXL, spqdxl::ExmiDXL2PlStmtAttributeNotFound),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN(
				"DXL-to-PlStmt Translation: Attribute number %d not found in project list"),
			1,	// attno
			SPQOS_WSZ_WSZLEN(
				"DXL-to-PlStmt Translation: Attribute number not found in project list")),

		CMessage(
			CException(spqdxl::ExmaDXL, spqdxl::ExmiDXL2ExprAttributeNotFound),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN(
				"DXL-to-Expr Translation: Attribute number %d not found in project list"),
			1,	// attno
			SPQOS_WSZ_WSZLEN(
				"DXL-to-Expr Translation: Attribute number not found in project list")),

		CMessage(
			CException(spqdxl::ExmaDXL, spqdxl::ExmiOptimizerError),
			CException::ExsevError, SPQOS_WSZ_WSZLEN("%s"),
			1,	// attno
			SPQOS_WSZ_WSZLEN(
				"PQO unable to generate a plan, please see the above message for details.")),

		CMessage(
			CException(spqdxl::ExmaDXL, spqdxl::ExmiNoAvailableMemory),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN("No available memory to allocate string buffer."),
			0,
			SPQOS_WSZ_WSZLEN("No available memory to allocate string buffer.")),

		CMessage(
			CException(spqdxl::ExmaDXL, spqdxl::ExmiInvalidComparisonTypeCode),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN(
				"Invalid comparison type code. Valid values are Eq, NEq, LT, LEq, GT, GEq."),
			0,
			SPQOS_WSZ_WSZLEN(
				"Invalid comparison type code. Valid values are Eq, NEq, LT, LEq, GT, GEq."))

	};

	SPQOS_RESULT eres = SPQOS_FAILED;

	SPQOS_TRY
	{
		// copy exception array into heap
		CMessage *rspqmsg[ExmiDXLSentinel];
		for (ULONG i = 0; i < SPQOS_ARRAY_SIZE(rspqmsg); i++)
		{
			rspqmsg[i] = SPQOS_NEW(mp) CMessage(rgmsg[i]);
		}

		CMessageRepository *pmr = CMessageRepository::GetMessageRepository();

		for (ULONG i = 0; i < SPQOS_ARRAY_SIZE(rgmsg); i++)
		{
			pmr->AddMessage(ElocEnUS_Utf8, rspqmsg[i]);
		}

		eres = SPQOS_OK;
	}
	SPQOS_CATCH_EX(ex)
	{
	}
	SPQOS_CATCH_END;

	return eres;
}


// EOF
