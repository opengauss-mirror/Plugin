//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		IDatumGeneric.h
//
//	@doc:
//		Base abstract class for generic datum representation
//---------------------------------------------------------------------------
#ifndef SPQNAUCRATES_IDatumGeneric_H
#define SPQNAUCRATES_IDatumGeneric_H

#include "spqos/base.h"
#include "spqos/common/CDouble.h"

#include "naucrates/base/IDatum.h"

namespace spqnaucrates
{
//---------------------------------------------------------------------------
//	@class:
//		IDatumGeneric
//
//	@doc:
//		Base abstract class for generic datum representation
//
//---------------------------------------------------------------------------
class IDatumGeneric : public IDatum
{
private:
	// private copy ctor
	IDatumGeneric(const IDatumGeneric &);

public:
	// ctor
	IDatumGeneric(){};

	// dtor
	virtual ~IDatumGeneric(){};

	// accessor for datum type
	virtual IMDType::ETypeInfo
	GetDatumType()
	{
		return IMDType::EtiGeneric;
	}

};	// class IDatumGeneric

}  // namespace spqnaucrates


#endif	// !SPQNAUCRATES_IDatumGeneric_H

// EOF
