//---------------------------------------------------------------------------
// Greenplum Database
// Copyright (c) 2020 VMware, Inc. or its affiliates
//---------------------------------------------------------------------------

#include "spqos/io/COstreamStdString.h"

namespace spqos
{
COstreamStdString::COstreamStdString() : COstreamBasic(&m_oss)
{
}

std::wstring
COstreamStdString::Str() const
{
	return m_oss.str();
}

}  // namespace spqos
