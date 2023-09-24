//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CName.h
//
//	@doc:
//		Name abstraction for metadata names to keep optimizer
//		agnostic of encodings etc.
//---------------------------------------------------------------------------
#ifndef SPQOPT_CName_H
#define SPQOPT_CName_H

#include "spqos/base.h"
#include "spqos/common/DbgPrintMixin.h"
#include "spqos/string/CWStringConst.h"

#define SPQOPT_NAME_QUOTE_BEGIN "\""
#define SPQOPT_NAME_QUOTE_END "\""

#define SPQOPT_NAME_SEPARATOR SPQOS_WSZ_LIT(".")

namespace spqopt
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		CName
//
//	@doc:
//		Names consist of a null terminated wide character string;
//		No assumptions about format and encoding; only semantics
//		enforced is zero termination of string;
//
//---------------------------------------------------------------------------
class CName : public DbgPrintMixin<CName>
{
private:
	// actual name
	const CWStringConst *m_str_name;

	// keep track of copy status
	BOOL m_fDeepCopy;

	// deep copy function
	void DeepCopy(CMemoryPool *mp, const CWStringConst *str);

public:
	// ctors
	CName(CMemoryPool *, const CWStringBase *);
	CName(const CWStringConst *, BOOL fOwnsMemory = false);
	CName(const CName &);

	CName(CMemoryPool *mp, const CName &);
	CName(CMemoryPool *mp, const CName &, const CName &);

	// dtor
	~CName();

	// accessors
	const CWStringConst *
	Pstr() const
	{
		return m_str_name;
	}

	ULONG
	Length() const
	{
		return m_str_name->Length();
	}

	// comparison
	BOOL Equals(const CName &) const;

	// debug print
	IOstream &OsPrint(IOstream &) const;

};	// class CName
}  // namespace spqopt

#endif	// !SPQOPT_CName_H

// EOF
