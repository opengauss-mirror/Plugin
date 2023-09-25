//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CColRefComputed.h
//
//	@doc:
//		Column reference implementation for computed columns
//---------------------------------------------------------------------------
#ifndef SPQOS_CColRefComputed_H
#define SPQOS_CColRefComputed_H

#include "spqos/base.h"
#include "spqos/common/CDynamicPtrArray.h"
#include "spqos/common/CList.h"

#include "spqopt/base/CColRef.h"
#include "spqopt/metadata/CName.h"
#include "naucrates/md/IMDType.h"

namespace spqopt
{
using namespace spqos;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CColRefComputed
//
//	@doc:
//
//---------------------------------------------------------------------------
class CColRefComputed : public CColRef
{
private:
	// private copy ctor
	CColRefComputed(const CColRefComputed &);

public:
	// ctor
	CColRefComputed(const IMDType *pmdtype, INT type_modifier, ULONG id,
					const CName *pname);

	// dtor
	virtual ~CColRefComputed();

	virtual CColRef::Ecolreftype
	Ecrt() const
	{
		return CColRef::EcrtComputed;
	}

	// is column a system column?
	BOOL
	IsSystemCol() const
	{
		// we cannot introduce system columns as computed column
		return false;
	}

	// is column a distribution column?
	BOOL
	IsDistCol() const
	{
		// we cannot introduce distribution columns as computed column
		return false;
	};


};	// class CColRefComputed

}  // namespace spqopt

#endif	// !SPQOS_CColRefComputed_H

// EOF
