//	Greenplum Database
//	Copyright (C) 2018 Pivotal Software, Inc.

#ifndef SPQDXL_CCostModelConfigSerializer_H
#define SPQDXL_CCostModelConfigSerializer_H

#include <spqopt/cost/ICostModel.h>

#include "spqos/common/CAutoP.h"
#include "spqos/memory/CMemoryPool.h"


namespace spqdxl
{
class CXMLSerializer;
class CCostModelConfigSerializer
{
private:
	const spqopt::ICostModel *m_cost_model;

public:
	CCostModelConfigSerializer(const spqopt::ICostModel *cost_model);

	void Serialize(CXMLSerializer &xml_serializer) const;
};
}  // namespace spqdxl
#endif
