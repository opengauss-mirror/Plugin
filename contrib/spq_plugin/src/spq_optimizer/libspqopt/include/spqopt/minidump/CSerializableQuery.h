//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CSerializableQuery.h
//
//	@doc:
//		Serializable query object used to dump the query when an exception is raised;
//---------------------------------------------------------------------------
#ifndef SPQOS_CSerializableQuery_H
#define SPQOS_CSerializableQuery_H

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
//		CSerializableQuery
//
//	@doc:
//		Serializable query object
//
//---------------------------------------------------------------------------
class CSerializableQuery : public CSerializable
{
private:
	CMemoryPool *m_mp;

	// query DXL node;
	const CDXLNode *m_query_dxl_root;

	// query output
	const CDXLNodeArray *m_query_output;

	// CTE DXL nodes
	const CDXLNodeArray *m_cte_producers;


	// private copy ctor
	CSerializableQuery(const CSerializableQuery &);

public:
	// ctor
	CSerializableQuery(CMemoryPool *mp, const CDXLNode *query,
					   const CDXLNodeArray *query_output_dxlnode_array,
					   const CDXLNodeArray *cte_producers);

	// dtor
	virtual ~CSerializableQuery();

	// serialize object to passed stream
	virtual void Serialize(COstream &oos);

};	// class CSerializableQuery
}  // namespace spqopt

#endif	// !SPQOS_CSerializableQuery_H

// EOF
