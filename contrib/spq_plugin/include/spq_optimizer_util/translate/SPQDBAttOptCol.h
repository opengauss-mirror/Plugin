//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Greenplum
//
//	@filename:
//		SPQDBAttOptCol.h
//
//	@doc:
//		Class to represent pair of SPQDB var info to optimizer col info
//
//	@test:
//
//
//---------------------------------------------------------------------------

#ifndef SPQDXL_SPQDBAttOptCol_H
#define SPQDXL_SPQDBAttOptCol_H

#include "spqos/common/CRefCount.h"

#include "spq_optimizer_util/translate/SPQDBAttInfo.h"
#include "spq_optimizer_util/translate/COptColInfo.h"

namespace spqdxl
{
using namespace spqos;

//---------------------------------------------------------------------------
//	@class:
//		SPQDBAttOptCol
//
//	@doc:
//		Class to represent pair of SPQDB var info to optimizer col info
//
//---------------------------------------------------------------------------
class SPQDBAttOptCol : public CRefCount
{
private:
	// spqdb att info
	SPQDBAttInfo *m_spqdb_att_info;

	// optimizer col info
	COptColInfo *m_opt_col_info;

	// copy c'tor
	SPQDBAttOptCol(const SPQDBAttOptCol &);

public:
	// ctor
	SPQDBAttOptCol(SPQDBAttInfo *spqdb_att_info, COptColInfo *opt_col_info)
		: m_spqdb_att_info(spqdb_att_info), m_opt_col_info(opt_col_info)
	{
		SPQOS_ASSERT(NULL != m_spqdb_att_info);
		SPQOS_ASSERT(NULL != m_opt_col_info);
	}

	// d'tor
	virtual ~SPQDBAttOptCol()
	{
		m_spqdb_att_info->Release();
		m_opt_col_info->Release();
	}

	// accessor
	const SPQDBAttInfo *
	GetSPQDBAttInfo() const
	{
		return m_spqdb_att_info;
	}

	// accessor
	const COptColInfo *
	GetOptColInfo() const
	{
		return m_opt_col_info;
	}
};

}  // namespace spqdxl

#endif	// !SPQDXL_SPQDBAttOptCol_H

// EOF
