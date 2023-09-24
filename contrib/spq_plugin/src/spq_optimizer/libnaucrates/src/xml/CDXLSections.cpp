//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CDXLSections.cpp
//
//	@doc:
//		DXL section header and footer definitions
//
//---------------------------------------------------------------------------


#include "naucrates/dxl/xml/CDXLSections.h"

using namespace spqdxl;

// static member definitions

const WCHAR *CDXLSections::m_wszDocumentHeader = SPQOS_WSZ_LIT(
	"<?xml version=\"1.0\" encoding=\"UTF-8\"?><dxl:DXLMessage xmlns:dxl=\"http://greenplum.com/dxl/2010/12/\">");

const WCHAR *CDXLSections::m_wszDocumentFooter =
	SPQOS_WSZ_LIT("</dxl:DXLMessage>");

const WCHAR *CDXLSections::m_wszThreadHeaderTemplate =
	SPQOS_WSZ_LIT("<dxl:Thread Id=\"%d\">");

const WCHAR *CDXLSections::m_wszThreadFooter = SPQOS_WSZ_LIT("</dxl:Thread>");

const WCHAR *CDXLSections::m_wszOptimizerConfigHeader =
	SPQOS_WSZ_LIT("<dxl:OptimizerConfig>");

const WCHAR *CDXLSections::m_wszOptimizerConfigFooter =
	SPQOS_WSZ_LIT("</dxl:OptimizerConfig>");

const WCHAR *CDXLSections::m_wszMetadataHeaderPrefix =
	SPQOS_WSZ_LIT("<dxl:Metadata SystemIds=\"");

const WCHAR *CDXLSections::m_wszMetadataHeaderSuffix = SPQOS_WSZ_LIT("\">");

const WCHAR *CDXLSections::m_wszMetadataFooter =
	SPQOS_WSZ_LIT("</dxl:Metadata>");

const WCHAR *CDXLSections::m_wszTraceFlagsSectionPrefix =
	SPQOS_WSZ_LIT("<dxl:TraceFlags Value=\"");

const WCHAR *CDXLSections::m_wszTraceFlagsSectionSuffix = SPQOS_WSZ_LIT("\"/>");

const WCHAR *CDXLSections::m_wszStackTraceHeader =
	SPQOS_WSZ_LIT("<dxl:Stacktrace>");

const WCHAR *CDXLSections::m_wszStackTraceFooter =
	SPQOS_WSZ_LIT("</dxl:Stacktrace>");

// EOF
