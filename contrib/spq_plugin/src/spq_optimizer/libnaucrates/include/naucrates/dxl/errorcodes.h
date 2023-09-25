//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		errorcodes.h
//
//	@doc:
//		Enum of errorcodes
//---------------------------------------------------------------------------
#ifndef SPQOPT_errorcodes_H
#define SPQOPT_errorcodes_H

namespace spqdxl
{
enum EErrorCode
{
	EerrcNotNullViolation,
	EerrcCheckConstraintViolation,
	EerrcTest,
	EerrcSentinel
};
}

#endif	// ! SPQOPT_errorcodes_H

// EOF
