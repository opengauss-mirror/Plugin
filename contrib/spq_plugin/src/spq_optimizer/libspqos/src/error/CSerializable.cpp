//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CSerializable.cpp
//
//	@doc:
//		Interface for serializable objects
//---------------------------------------------------------------------------

#include "spqos/error/CSerializable.h"

#include "spqos/base.h"
#include "spqos/error/CErrorContext.h"
#include "spqos/task/CTask.h"

using namespace spqos;


//---------------------------------------------------------------------------
//	@function:
//		CSerializable::CSerializable
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CSerializable::CSerializable()
{
	CTask *task = CTask::Self();

	SPQOS_ASSERT(NULL != task);

	task->ConvertErrCtxt()->Register(this);
}


//---------------------------------------------------------------------------
//	@function:
//		CSerializable::~CSerializable
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CSerializable::~CSerializable()
{
	CTask *task = CTask::Self();

	SPQOS_ASSERT(NULL != task);

	task->ConvertErrCtxt()->Unregister(this);
}


// EOF
