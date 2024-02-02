/*-------------------------------------------------------------------------
 *
 * extensible.h
 *	  Definitions for extensible nodes and custom scans
 *
 *
 * Portions Copyright (c) 1996-2018, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/nodes/ag_extensible.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef AG_READFUNCS_H
#define AG_READFUNCS_H


#include "nodes/nodes.h"


/*
 * prototypes for functions in readfuncs.c
 */
extern Node* parseNodeString_AG(void);

#endif							/* EXTENSIBLE_H */
