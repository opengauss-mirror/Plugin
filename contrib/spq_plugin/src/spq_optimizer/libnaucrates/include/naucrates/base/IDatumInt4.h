//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		IDatumInt4.h
//
//	@doc:
//		Base abstract class for int4 representation
//---------------------------------------------------------------------------
#ifndef SPQNAUCRATES_IDatumInt4_H
#define SPQNAUCRATES_IDatumInt4_H

#include "spqos/base.h"

#include "naucrates/base/IDatum.h"

namespace spqnaucrates
{
//---------------------------------------------------------------------------
//	@class:
//		IDatumInt4
//
//	@doc:
//		Base abstract class for int4 representation
//
//---------------------------------------------------------------------------
class IDatumInt4 : public IDatum
{
private:
	// private copy ctor
	IDatumInt4(const IDatumInt4 &);

public:
	// ctor
	IDatumInt4(){};

	// dtor
	virtual ~IDatumInt4(){};

	// accessor for datum type
	virtual IMDType::ETypeInfo
	GetDatumType()
	{
		return IMDType::EtiInt4;
	}

	// accessor of integer value
	virtual INT Value() const = 0;

	// can datum be mapped to a double
	BOOL
	IsDatumMappableToDouble() const
	{
		return true;
	}

	// map to double for stats computation
	CDouble
	GetDoubleMapping() const
	{
		return CDouble(Value());
	}

	// can datum be mapped to LINT
	BOOL
	IsDatumMappableToLINT() const
	{
		return true;
	}

	// map to LINT for statistics computation
	LINT
	GetLINTMapping() const
	{
		return LINT(Value());
	}

	// byte array representation of datum
	virtual const BYTE *
	GetByteArrayValue() const
	{
		SPQOS_ASSERT(!"Invalid invocation of MakeCopyOfValue");
		return NULL;
	}

	// does the datum need to be padded before statistical derivation
	virtual BOOL
	NeedsPadding() const
	{
		return false;
	}

	// return the padded datum
	virtual IDatum *
	MakePaddedDatum(CMemoryPool *,	// mp,
					ULONG			// col_len
	) const
	{
		SPQOS_ASSERT(!"Invalid invocation of MakePaddedDatum");
		return NULL;
	}

	// does datum support like predicate
	virtual BOOL
	SupportsLikePredicate() const
	{
		return false;
	}

	// return the default scale factor of like predicate
	virtual CDouble
	GetLikePredicateScaleFactor() const
	{
		SPQOS_ASSERT(!"Invalid invocation of DLikeSelectivity");
		return false;
	}

};	// class IDatumInt4

}  // namespace spqnaucrates


#endif	// !SPQNAUCRATES_IDatumInt4_H

// EOF
