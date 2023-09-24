//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		exception.cpp
//
//	@doc:
//		Initialization of SPQOPT-specific exception messages
//---------------------------------------------------------------------------

#include "spqopt/exception.h"

#include "spqos/error/CMessage.h"
#include "spqos/error/CMessageRepository.h"

using namespace spqos;

//---------------------------------------------------------------------------
//	@function:
//		EresExceptionInit
//
//	@doc:
//		Message initialization for SPQOPT exceptions
//
//---------------------------------------------------------------------------
SPQOS_RESULT
spqopt::EresExceptionInit(CMemoryPool *mp)
{
	//---------------------------------------------------------------------------
	// Basic DXL messages in English
	//---------------------------------------------------------------------------
	CMessage rgmsg[ExmiSentinel] = {
		CMessage(CException(spqopt::ExmaSPQOPT, spqopt::ExmiNoPlanFound),
				 CException::ExsevError,
				 SPQOS_WSZ_WSZLEN(
					 "No plan has been computed for required properties"),
				 0, SPQOS_WSZ_WSZLEN("No plan found")),

		CMessage(
			CException(spqopt::ExmaSPQOPT, spqopt::ExmiInvalidPlanAlternative),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN(
				"Plan identifier %lld out of range, max plans: %lld"),
			2,	// plan id, max plans
			SPQOS_WSZ_WSZLEN("Plan enumeration")),

		CMessage(CException(spqopt::ExmaSPQOPT, spqopt::ExmiUnsupportedOp),
				 CException::ExsevNotice,
				 SPQOS_WSZ_WSZLEN("Operator %ls not supported"),
				 1,	 // operator type
				 SPQOS_WSZ_WSZLEN("Unsupported operator")),

		CMessage(CException(spqopt::ExmaSPQOPT, spqopt::ExmiUnexpectedOp),
				 CException::ExsevError,
				 SPQOS_WSZ_WSZLEN("Unexpected Operator %ls"),
				 1,	 // operator type
				 SPQOS_WSZ_WSZLEN("Unexpected operator")),

		CMessage(CException(spqopt::ExmaSPQOPT, spqopt::ExmiUnsupportedPred),
				 CException::ExsevNotice,
				 SPQOS_WSZ_WSZLEN("Predicate %s not supported"),
				 1,	 // predicate type
				 SPQOS_WSZ_WSZLEN("Unsupported predicate")),

		CMessage(
			CException(spqopt::ExmaSPQOPT,
					   spqopt::ExmiUnsupportedCompositePartKey),
			CException::ExsevNotice,
			SPQOS_WSZ_WSZLEN(
				"Feature not supported by the Pivotal Query Optimizer: composite partitioning keys"),
			0,
			SPQOS_WSZ_WSZLEN(
				"Feature not supported by the Pivotal Query Optimizer: composite partitioning keys")),

		CMessage(
			CException(spqopt::ExmaSPQOPT,
					   spqopt::ExmiUnsupportedNonDeterministicUpdate),
			CException::ExsevNotice,
			SPQOS_WSZ_WSZLEN(
				"Feature not supported by the Pivotal Query Optimizer: non-deterministic DML statements"),
			0,
			SPQOS_WSZ_WSZLEN(
				"Feature not supported by the Pivotal Query Optimizer: non-deterministic DML statements")),

		CMessage(CException(spqopt::ExmaSPQOPT,
							spqopt::ExmiUnsatisfiedRequiredProperties),
				 CException::ExsevError,
				 SPQOS_WSZ_WSZLEN("Plan does not satisfy required properties"),
				 0,
				 SPQOS_WSZ_WSZLEN("Plan does not satisfy required properties")),

		CMessage(
			CException(spqopt::ExmaSPQOPT, spqopt::ExmiEvalUnsupportedScalarExpr),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN(
				"Expecting a scalar expression like (const cmp const), ignoring casts"),
			0, SPQOS_WSZ_WSZLEN("Not a constant scalar expression")),

		CMessage(
			CException(spqopt::ExmaSPQOPT,
					   spqopt::ExmiCTEProducerConsumerMisAligned),
			CException::ExsevError,
			SPQOS_WSZ_WSZLEN(
				"CTE Producer-Consumer execution locality mismatch for CTE id %lld"),
			1,
			SPQOS_WSZ_WSZLEN(
				"CTE Producer-Consumer execution locality mismatch")),

		CMessage(CException(spqopt::ExmaSPQOPT, spqopt::ExmiNoStats),
				 CException::ExsevError,
				 SPQOS_WSZ_WSZLEN("Missing group stats in %ls"), 1,
				 SPQOS_WSZ_WSZLEN("Missing group stats")),
	};

	SPQOS_RESULT eres = SPQOS_FAILED;

	SPQOS_TRY
	{
		// copy exception array into heap
		CMessage *rspqmsg[spqopt::ExmiSentinel];
		CMessageRepository *pmr = CMessageRepository::GetMessageRepository();

		for (ULONG i = 0; i < SPQOS_ARRAY_SIZE(rspqmsg); i++)
		{
			rspqmsg[i] = SPQOS_NEW(mp) CMessage(rgmsg[i]);
			pmr->AddMessage(ElocEnUS_Utf8, rspqmsg[i]);
		}

		eres = SPQOS_OK;
	}
	SPQOS_CATCH_EX(ex)
	{
		return SPQOS_FAILED;
	}

	SPQOS_CATCH_END;

	return eres;
}


// EOF
