//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 Greenplum, Inc.
//
//	@filename:
//		init.h
//
//	@doc:
//		Initialization of the spqopt library.
//---------------------------------------------------------------------------
#ifndef SPQOPT_init_H
#define SPQOPT_init_H

#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

// initialize spqopt library
void spqopt_init();


// terminate spqopt library
void spqopt_terminate(void);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// !SPQOPT_init_H


// EOF
