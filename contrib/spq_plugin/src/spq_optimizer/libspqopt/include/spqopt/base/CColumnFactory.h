//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008, 2009 Greenplum, Inc.
//
//	@filename:
//		CColumnFactory.h
//
//	@doc:
//		Column reference management; one instance per optimization
//---------------------------------------------------------------------------
#ifndef SPQOPT_CColumnFactory_H
#define SPQOPT_CColumnFactory_H

#include "spqos/base.h"
#include "spqos/common/CList.h"
#include "spqos/common/CSyncHashtable.h"

#include "spqopt/base/CColRefSet.h"
#include "spqopt/metadata/CColumnDescriptor.h"
#include "naucrates/md/IMDId.h"
#include "naucrates/md/IMDType.h"

namespace spqopt
{
class CExpression;

using namespace spqos;
using namespace spqmd;

//---------------------------------------------------------------------------
//	@class:
//		CColumnFactory
//
//	@doc:
//		Singleton factory class used to generate and manage CColRefs in ORCA.
//		The created CColRef objects are maintained in a hash table keyed by
//		Column ID.  CColumnFactory provides various overloaded PcrCreate()
//		methods to create CColRef and a LookupColRef() method to probe the hash
//		table.
//		NB: The class also owns the memory pool in which CColRefs are
//		allocated.
//
//---------------------------------------------------------------------------
class CColumnFactory
{
private:
	// MTS memory pool
	CMemoryPool *m_mp;

	// mapping between column id of computed column and a set of used column references
	ColRefToColRefSetMap *m_phmcrcrs;

	// id counter
	ULONG m_aul;

	// hash table
	CSyncHashtable<CColRef, ULONG> m_sht;

	// private copy ctor
	CColumnFactory(const CColumnFactory &);

	// implementation of factory methods
	CColRef *PcrCreate(const IMDType *pmdtype, INT type_modifier, ULONG id,
					   const CName &name);
	CColRef *PcrCreate(const CColumnDescriptor *pcoldesc, ULONG id,
					   const CName &name, ULONG ulOpSource,
					   BOOL mark_as_used = true, IMDId *mdid_table = NULL);

public:
	// ctor
	CColumnFactory();

	// dtor
	~CColumnFactory();

	// initialize the hash map between computed column and used columns
	void Initialize();

	// create a column reference given only its type and type modifier, used for computed columns
	CColRef *PcrCreate(const IMDType *pmdtype, INT type_modifier);

	// create column reference given its type, type modifier, and name
	CColRef *PcrCreate(const IMDType *pmdtype, INT type_modifier,
					   const CName &name);

	// create a column reference given its descriptor and name
	CColRef *PcrCreate(const CColumnDescriptor *pcoldescr, const CName &name,
					   ULONG ulOpSource, BOOL mark_as_used, IMDId *mdid_table);

	// create a column reference given its type, attno, nullability and name
	CColRef *PcrCreate(const IMDType *pmdtype, INT type_modifier,
					   IMDId *mdid_table, INT attno, BOOL is_nullable, ULONG id,
					   const CName &name, ULONG ulOpSource, BOOL isDistCol,
					   ULONG ulWidth = spqos::ulong_max);

	// create a column reference with the same type as passed column reference
	CColRef *
	PcrCreate(const CColRef *colref)
	{
		return PcrCreate(colref->RetrieveType(), colref->TypeModifier());
	}

	// add mapping between computed column to its used columns
	void AddComputedToUsedColsMap(CExpression *pexpr);

	// lookup the set of used column references (if any) based on id of computed column
	const CColRefSet *PcrsUsedInComputedCol(const CColRef *pcrComputedCol);

	// create a copy of the given colref
	CColRef *PcrCopy(const CColRef *colref);

	// lookup by id
	CColRef *LookupColRef(ULONG id);

	// destructor
	void Destroy(CColRef *);

};	// class CColumnFactory
}  // namespace spqopt

#endif	// !SPQOPT_CColumnFactory_H

// EOF
