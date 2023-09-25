//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2010 Greenplum, Inc.
//
//	@filename:
//		init.h
//
//	@doc:
//		Initialization of the spqdxl library.
//---------------------------------------------------------------------------*/
#ifndef SPQDXL_init_H
#define SPQDXL_init_H

#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus


// initialize DXL library support
void InitDXL();

// shutdown DXL library support
void ShutdownDXL();

// initialize Xerces parser utils
void spqdxl_init(void);

// terminate Xerces parser utils
void spqdxl_terminate(void);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// !SPQDXL_init_H


// EOF
