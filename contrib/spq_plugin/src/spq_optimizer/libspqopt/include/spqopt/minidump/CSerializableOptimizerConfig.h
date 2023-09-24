//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2018 Pivotal, Inc.
//
//	@filename:
//		CSerializableOptimizerConfig.h
//
//	@doc:
//		Serializable optimizer configuration object used to use for minidumping
//---------------------------------------------------------------------------
#ifndef SPQOS_CSerializableOptimizerConfig_H
#define SPQOS_CSerializableOptimizerConfig_H

#include "spqos/base.h"
#include "spqos/error/CSerializable.h"
#include "spqos/string/CWStringDynamic.h"

#include "naucrates/dxl/operators/CDXLNode.h"

using namespace spqos;
using namespace spqdxl;

namespace spqopt
{
// fwd decl
class COptimizerConfig;

//---------------------------------------------------------------------------
//	@class:
//		CSerializableOptimizerConfig
//
//	@doc:
//		Serializable optimizer configuration object
//
//---------------------------------------------------------------------------
class CSerializableOptimizerConfig : public CSerializable
{
private:
	CMemoryPool *m_mp;

	// optimizer configurations
	const COptimizerConfig *m_optimizer_config;

	// private copy ctor
	CSerializableOptimizerConfig(const CSerializableOptimizerConfig &);

public:
	// ctor
	CSerializableOptimizerConfig(CMemoryPool *mp,
								 const COptimizerConfig *optimizer_config);

	// dtor
	virtual ~CSerializableOptimizerConfig();

	// serialize object to passed stream
	virtual void Serialize(COstream &oos);

};	// class CSerializableOptimizerConfig
}  // namespace spqopt

#endif	// !SPQOS_CSerializableOptimizerConfig_H

// EOF
