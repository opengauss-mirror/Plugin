//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2009 Greenplum, Inc.
//
//	@filename:
//		CTableDescriptor.cpp
//
//	@doc:
//		Implementation of table abstraction
//---------------------------------------------------------------------------

#include "spqopt/metadata/CTableDescriptor.h"

#include "spqos/base.h"
#include "spqos/memory/CAutoMemoryPool.h"

#include "spqopt/base/CColumnFactory.h"
#include "spqopt/base/CDistributionSpecAny.h"
#include "spqopt/base/CDistributionSpecSingleton.h"
#include "spqopt/base/COptCtxt.h"
#include "spqopt/base/CUtils.h"
#include "naucrates/exception.h"
#include "naucrates/md/IMDIndex.h"

using namespace spqopt;

FORCE_GENERATE_DBGSTR(CTableDescriptor);

//---------------------------------------------------------------------------
//	@function:
//		CTableDescriptor::CTableDescriptor
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CTableDescriptor::CTableDescriptor(
	CMemoryPool *mp, IMDId *mdid, const CName &name,
	BOOL convert_hash_to_random, IMDRelation::Ereldistrpolicy rel_distr_policy,
	IMDRelation::Erelstoragetype erelstoragetype, ULONG ulExecuteAsUser)
	: m_mp(mp),
	  m_mdid(mdid),
	  m_name(mp, name),
	  m_pdrspqcoldesc(NULL),
	  m_rel_distr_policy(rel_distr_policy),
	  m_erelstoragetype(erelstoragetype),
	  m_pdrspqcoldescDist(NULL),
	  m_distr_opfamilies(NULL),
	  m_convert_hash_to_random(convert_hash_to_random),
	  m_pdrspqulPart(NULL),
	  m_pdrspqbsKeys(NULL),
	  m_num_of_partitions(0),
	  m_execute_as_user_id(ulExecuteAsUser),
	  m_fHasPartialIndexes(FDescriptorWithPartialIndexes())
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(mdid->IsValid());

	m_pdrspqcoldesc = SPQOS_NEW(m_mp) CColumnDescriptorArray(m_mp);
	m_pdrspqcoldescDist = SPQOS_NEW(m_mp) CColumnDescriptorArray(m_mp);
	m_pdrspqulPart = SPQOS_NEW(m_mp) ULongPtrArray(m_mp);
	m_pdrspqbsKeys = SPQOS_NEW(m_mp) CBitSetArray(m_mp);
	if (SPQOS_FTRACE(EopttraceConsiderOpfamiliesForDistribution))
	{
		m_distr_opfamilies = SPQOS_NEW(m_mp) IMdIdArray(m_mp);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CTableDescriptor::~CTableDescriptor
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CTableDescriptor::~CTableDescriptor()
{
	m_mdid->Release();

	m_pdrspqcoldesc->Release();
	m_pdrspqcoldescDist->Release();
	m_pdrspqulPart->Release();
	m_pdrspqbsKeys->Release();
	CRefCount::SafeRelease(m_distr_opfamilies);
}


//---------------------------------------------------------------------------
//	@function:
//		CTableDescriptor::ColumnCount
//
//	@doc:
//		number of columns
//
//---------------------------------------------------------------------------
ULONG
CTableDescriptor::ColumnCount() const
{
	// array allocated in ctor
	SPQOS_ASSERT(NULL != m_pdrspqcoldesc);

	return m_pdrspqcoldesc->Size();
}

//---------------------------------------------------------------------------
//	@function:
//		CTableDescriptor::UlPos
//
//	@doc:
//		Find the position of a column descriptor in an array of column descriptors.
//		If not found, return the size of the array
//
//---------------------------------------------------------------------------
ULONG
CTableDescriptor::UlPos(const CColumnDescriptor *pcoldesc,
						const CColumnDescriptorArray *pdrspqcoldesc) const
{
	SPQOS_ASSERT(NULL != pcoldesc);
	SPQOS_ASSERT(NULL != pdrspqcoldesc);

	ULONG arity = pdrspqcoldesc->Size();
	for (ULONG ul = 0; ul < arity; ul++)
	{
		if (pcoldesc == (*pdrspqcoldesc)[ul])
		{
			return ul;
		}
	}

	return arity;
}

//---------------------------------------------------------------------------
//	@function:
//		CTableDescriptor::GetAttributePosition
//
//	@doc:
//		Find the position of the attribute in the array of column descriptors
//
//---------------------------------------------------------------------------
ULONG
CTableDescriptor::GetAttributePosition(INT attno) const
{
	SPQOS_ASSERT(NULL != m_pdrspqcoldesc);
	ULONG ulPos = spqos::ulong_max;
	ULONG arity = m_pdrspqcoldesc->Size();

	for (ULONG ul = 0; ul < arity; ul++)
	{
		CColumnDescriptor *pcoldesc = (*m_pdrspqcoldesc)[ul];
		if (pcoldesc->AttrNum() == attno)
		{
			ulPos = ul;
		}
	}
	SPQOS_ASSERT(spqos::ulong_max != ulPos);

	return ulPos;
}

//---------------------------------------------------------------------------
//	@function:
//		CTableDescriptor::AddColumn
//
//	@doc:
//		Add column to table descriptor
//
//---------------------------------------------------------------------------
void
CTableDescriptor::AddColumn(CColumnDescriptor *pcoldesc)
{
	SPQOS_ASSERT(NULL != pcoldesc);

	m_pdrspqcoldesc->Append(pcoldesc);
}

//---------------------------------------------------------------------------
//	@function:
//		CTableDescriptor::AddDistributionColumn
//
//	@doc:
//		Add the column at the specified position to the array of column
//		descriptors defining a hash distribution
//
//---------------------------------------------------------------------------
void
CTableDescriptor::AddDistributionColumn(ULONG ulPos, IMDId *opfamily)
{
	CColumnDescriptor *pcoldesc = (*m_pdrspqcoldesc)[ulPos];
	pcoldesc->AddRef();
	m_pdrspqcoldescDist->Append(pcoldesc);
	pcoldesc->SetAsDistCol();

	if (SPQOS_FTRACE(EopttraceConsiderOpfamiliesForDistribution))
	{
		SPQOS_ASSERT(NULL != opfamily && opfamily->IsValid());
		opfamily->AddRef();
		m_distr_opfamilies->Append(opfamily);

		SPQOS_ASSERT(m_pdrspqcoldescDist->Size() == m_distr_opfamilies->Size());
	}
}

//---------------------------------------------------------------------------
//	@function:
//		CTableDescriptor::AddPartitionColumn
//
//	@doc:
//		Add the column at the specified position to the array of partition column
//		descriptors
//
//---------------------------------------------------------------------------
void
CTableDescriptor::AddPartitionColumn(ULONG ulPos)
{
	m_pdrspqulPart->Append(SPQOS_NEW(m_mp) ULONG(ulPos));
}

//---------------------------------------------------------------------------
//	@function:
//		CTableDescriptor::FAddKeySet
//
//	@doc:
//		Add a keyset, returns true if key set is successfully added
//
//---------------------------------------------------------------------------
BOOL
CTableDescriptor::FAddKeySet(CBitSet *pbs)
{
	SPQOS_ASSERT(NULL != pbs);
	SPQOS_ASSERT(pbs->Size() <= m_pdrspqcoldesc->Size());

	const ULONG size = m_pdrspqbsKeys->Size();
	BOOL fFound = false;
	for (ULONG ul = 0; !fFound && ul < size; ul++)
	{
		CBitSet *pbsCurrent = (*m_pdrspqbsKeys)[ul];
		fFound = pbsCurrent->Equals(pbs);
	}

	if (!fFound)
	{
		m_pdrspqbsKeys->Append(pbs);
	}

	return !fFound;
}

//---------------------------------------------------------------------------
//	@function:
//		CTableDescriptor::Pcoldesc
//
//	@doc:
//		Get n-th column descriptor
//
//---------------------------------------------------------------------------
const CColumnDescriptor *
CTableDescriptor::Pcoldesc(ULONG ulCol) const
{
	SPQOS_ASSERT(ulCol < ColumnCount());

	return (*m_pdrspqcoldesc)[ulCol];
}


//---------------------------------------------------------------------------
//	@function:
//		CTableDescriptor::OsPrint
//
//	@doc:
//		debug print
//
//---------------------------------------------------------------------------
IOstream &
CTableDescriptor::OsPrint(IOstream &os) const
{
	m_name.OsPrint(os);
	os << ": (";
	CUtils::OsPrintDrgPcoldesc(os, m_pdrspqcoldesc, m_pdrspqcoldesc->Size());
	os << ")";
	return os;
}


//---------------------------------------------------------------------------
//	@function:
//		CTableDescriptor::IndexCount
//
//	@doc:
//		 Returns number of b-tree indices
//
//
//---------------------------------------------------------------------------
ULONG
CTableDescriptor::IndexCount()
{
	SPQOS_ASSERT(NULL != m_mdid);

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDRelation *pmdrel = md_accessor->RetrieveRel(m_mdid);
	const ULONG ulIndices = pmdrel->IndexCount();

	return ulIndices;
}

//---------------------------------------------------------------------------
//	@function:
//		CTableDescriptor::PartitionCount
//
//	@doc:
//		 Returns number of leaf partitions
//
//
//---------------------------------------------------------------------------
ULONG
CTableDescriptor::PartitionCount() const
{
	SPQOS_ASSERT(NULL != m_mdid);

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDRelation *pmdrel = md_accessor->RetrieveRel(m_mdid);
	const ULONG ulPartitions = pmdrel->PartitionCount();

	return ulPartitions;
}

//---------------------------------------------------------------------------
//	@function:
//		CTableDescriptor::FDescriptorWithPartialIndexes
//
//	@doc:
//		Returns true if this given table descriptor has partial indexes
//
//---------------------------------------------------------------------------
BOOL
CTableDescriptor::FDescriptorWithPartialIndexes()
{
	const ULONG ulIndices = IndexCount();
	if (0 == ulIndices)
	{
		return false;
	}

	CMDAccessor *md_accessor = COptCtxt::PoctxtFromTLS()->Pmda();
	const IMDRelation *pmdrel = md_accessor->RetrieveRel(m_mdid);
	for (ULONG ul = 0; ul < ulIndices; ul++)
	{
		if (pmdrel->IsPartialIndex(pmdrel->IndexMDidAt(ul)))
		{
			return true;
		}
	}

	return false;
}

// EOF
