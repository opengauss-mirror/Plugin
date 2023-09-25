//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CSerializableStackTrace.h
//
//	@doc:
//		Serializable stack trace object
//---------------------------------------------------------------------------
#ifndef SPQOPT_CSerializableStackTrace_H
#define SPQOPT_CSerializableStackTrace_H

#include "spqos/base.h"
#include "spqos/error/CSerializable.h"
#include "spqos/string/CWStringDynamic.h"

#include "naucrates/dxl/operators/CDXLNode.h"

using namespace spqos;
using namespace spqdxl;


namespace spqopt
{
//---------------------------------------------------------------------------
//	@class:
//		CSerializableStackTrace
//
//	@doc:
//		Serializable stacktrace object
//
//---------------------------------------------------------------------------
class CSerializableStackTrace : public CSerializable
{
public:
	// ctor
	CSerializableStackTrace();

	// dtor
	virtual ~CSerializableStackTrace();

	// serialize object to passed stream
	virtual void Serialize(COstream &oos);

};	// class CSerializableStackTrace
}  // namespace spqopt

#endif	// !SPQOPT_CSerializableStackTrace_H

// EOF
