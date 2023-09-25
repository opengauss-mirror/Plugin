//	Greenplum Database
//	Copyright (C) 2018 Pivotal Software, Inc.

#include "naucrates/dxl/CCostModelConfigSerializer.h"

#include "spqos/common/CAutoRef.h"

#include "spqdbcost/CCostModelParamsSPQDB.h"
#include "naucrates/dxl/xml/CXMLSerializer.h"
#include "naucrates/dxl/xml/dxltokens.h"

using namespace spqdxl;
using spqos::CAutoRef;

void
CCostModelConfigSerializer::Serialize(CXMLSerializer &xml_serializer) const
{
	xml_serializer.OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenCostModelConfig));
	xml_serializer.AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenCostModelType),
		m_cost_model->Ecmt());
	xml_serializer.AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenSegmentsForCosting),
		m_cost_model->UlHosts());

	xml_serializer.OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenCostParams));

	xml_serializer.OpenElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenCostParam));

	xml_serializer.AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenName),
		m_cost_model->GetCostModelParams()->SzNameLookup(
			CCostModelParamsSPQDB::EcpNLJFactor));
	xml_serializer.AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenValue),
		m_cost_model->GetCostModelParams()
			->PcpLookup(CCostModelParamsSPQDB::EcpNLJFactor)
			->Get());
	xml_serializer.AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenCostParamLowerBound),
		m_cost_model->GetCostModelParams()
			->PcpLookup(CCostModelParamsSPQDB::EcpNLJFactor)
			->GetLowerBoundVal());
	xml_serializer.AddAttribute(
		CDXLTokens::GetDXLTokenStr(EdxltokenCostParamUpperBound),
		m_cost_model->GetCostModelParams()
			->PcpLookup(CCostModelParamsSPQDB::EcpNLJFactor)
			->GetUpperBoundVal());
	xml_serializer.CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenCostParam));

	xml_serializer.CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenCostParams));

	xml_serializer.CloseElement(
		CDXLTokens::GetDXLTokenStr(EdxltokenNamespacePrefix),
		CDXLTokens::GetDXLTokenStr(EdxltokenCostModelConfig));
}

CCostModelConfigSerializer::CCostModelConfigSerializer(
	const spqopt::ICostModel *cost_model)
	: m_cost_model(cost_model)
{
}
