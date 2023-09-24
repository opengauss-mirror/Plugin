//---------------------------------------------------------------------------
// Greenplum Database
// Copyright (c) 2020 VMware, Inc. or its affiliates
//---------------------------------------------------------------------------
#ifndef COstreamStdString_H
#define COstreamStdString_H

#include <sstream>
#include <string>

#include "spqos/io/COstreamBasic.h"

namespace spqos
{
class COstreamStdString : public spqos::COstreamBasic
{
public:
	COstreamStdString();
	std::wstring Str() const;

private:
	std::wostringstream m_oss;
};
}  // namespace spqos

#endif
