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
#ifndef AG_EXTENSIBLE_H
#define AG_EXTENSIBLE_H


#include "nodes/execnodes.h"
#include "nodes/plannodes.h"
#include "nodes/relation.h"

/* maximum length of an extensible node identifier */
#define EXTNODENAME_MAX_LEN					64

extern void ag_extensiblecpp_register_ag_nodes();
extern void RegisterAGExtensibleNodeMethods(const ExtensibleNodeMethods *method);
extern const ExtensibleNodeMethods *GetExtensibleAGNodeMethods(const char *name,
                                                               bool missing_ok);


extern char* AgNodeToString(const void *obj);
extern void outNode_age(struct StringInfoData *str, const void *obj);
extern void outToken_age(struct StringInfoData *str, const char *s);
extern char* ag_pg_strtok(int *length, bool advance_reader = true);

extern void* stringToAGNode(char* str);
extern void* nodeRead_AG(char* token, int tok_len);

extern char *age_node_to_string(const void *object);

#endif							/* AG_EXTENSIBLE_H */
