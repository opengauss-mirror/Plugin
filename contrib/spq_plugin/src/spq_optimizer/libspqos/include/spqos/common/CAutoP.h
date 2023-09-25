//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		CAutoP.h
//
//	@doc:
//		Basic auto pointer implementation; do not anticipate ownership based
//		on assignment to other auto pointers etc. Require explicit return/assignment
//		to re-init the object;
//
//		This is primarily used for auto destruction.
//		Not using Boost's auto pointer logic to discourage blurring ownership
//		of objects.
//---------------------------------------------------------------------------
#ifndef SPQOS_CAutoP_H
#define SPQOS_CAutoP_H

#include "spqos/base.h"
#include "spqos/common/CStackObject.h"

namespace spqos
{
//---------------------------------------------------------------------------
//	@class:
//		CAutoP
//
//	@doc:
//		Wrapps pointer of type T; overloads *, ->, = does not provide
//		copy ctor;
//
//---------------------------------------------------------------------------
template <class T>
class CAutoP : public CStackObject
{
protected:
	// actual element to point to
	T *m_object;

	// hidden copy ctor
	CAutoP<T>(const CAutoP &);

public:
	// ctor
	explicit CAutoP<T>() : m_object(NULL)
	{
	}

	explicit CAutoP<T>(T *object) : m_object(object)
	{
	}

	// dtor
	virtual ~CAutoP();

	// simple assignment
	CAutoP<T> const &
	operator=(T *object)
	{
		m_object = object;
		return *this;
	}

	// deref operator
	T &
	operator*()
	{
		SPQOS_ASSERT(NULL != m_object);
		return *m_object;
	}

	// returns only base pointer, compiler does appropriate deref'ing
	T *
	operator->()
	{
		return m_object;
	}

	// return basic pointer
	T *
	Value()
	{
		return m_object;
	}

	// unhook pointer from auto object
	T *
	Reset()
	{
		T *object = m_object;
		m_object = NULL;
		return object;
	}

};	// class CAutoP

//---------------------------------------------------------------------------
//	@function:
//		CAutoP::~CAutoP
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
template <class T>
CAutoP<T>::~CAutoP()
{
	SPQOS_DELETE(m_object);
}
}  // namespace spqos


#endif	// !SPQOS_CAutoP_H

// EOF
