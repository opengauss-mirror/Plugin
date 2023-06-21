/*-------------------------------------------------------------------------
 *
 * objectaddress.h
 *	  functions for working with object addresses
 *
 * Portions Copyright (c) 1996-2016, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/objectaddress.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef tsdb_OBJECTADDRESS_H
#define tsdb_OBJECTADDRESS_H

#include "nodes/pg_list.h"

#include "utils/acl.h"
#include "utils/relcache.h"



extern const ObjectAddress InvalidObjectAddress;

#define ObjectAddressSubSet(addr, class_id, object_id, object_sub_id) \
	do { \
		(addr).classId = (class_id); \
		(addr).objectId = (object_id); \
		(addr).objectSubId = (object_sub_id); \
	} while (0)

#define ObjectAddressSet(addr, class_id, object_id) \
	ObjectAddressSubSet(addr, class_id, object_id, 0)

extern Oid	get_object_oid_index(Oid class_id);
extern int	get_object_catcache_oid(Oid class_id);
extern AttrNumber get_object_attnum_name(Oid class_id);
extern AttrNumber get_object_attnum_namespace(Oid class_id);
extern bool get_object_namensp_unique(Oid class_id);

extern HeapTuple get_catalog_object_by_oid(Relation catalog,
						  Oid objectId);

extern char *getObjectTypeDescription(const ObjectAddress *object);
extern char *getObjectIdentity(const ObjectAddress *address);
extern char *getObjectIdentityParts(const ObjectAddress *address,
					   List **objname, List **objargs);
extern ArrayType *strlist_to_textarray(List *list);


extern char *format_procedure_qualified(Oid procedure_oid);

extern void format_procedure_parts(Oid operator_oid, List **objnames,
					   List **objargs);
extern char *get_namespace_name_or_temp(Oid nspid);

extern char *get_am_name(Oid amOid);

extern char *format_type_be_qualified(Oid type_oid);

extern void format_operator_parts(Oid operator_oid, List **objnames,
					  List **objargs);

extern char *format_operator_qualified(Oid operator_oid);

#endif   /* tsdb_OBJECTADDRESS_H */