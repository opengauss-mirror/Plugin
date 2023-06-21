/*-------------------------------------------------------------------------
 *
 * event_trigger.h
 *	  Declarations for command trigger handling.
 *
 * Portions Copyright (c) 1996-2016, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/commands/event_trigger.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef EVENT_TRIGGER_H
#define EVENT_TRIGGER_H

#include "catalog/dependency.h"
#include "catalog/objectaddress.h"
#include "nodes/parsenodes.h"
#include "tsdb.h"


typedef struct EventTriggerData
{
	NodeTag		type;
	const char *event;			/* event name */
	Node	   *parsetree;		/* parse tree */
	const char *tag;			/* command tag */
} EventTriggerData;

typedef struct AlterEventTrigStmt
{
	NodeTag		type;
	char	   *trigname;		/* TRIGGER's name */
	char		tgenabled;		/* trigger's firing configuration WRT
								 * session_replication_role */
} AlterEventTrigStmt; 


typedef struct CreateEventTrigStmt
{
	NodeTag		type;
	char	   *trigname;		/* TRIGGER's name */
	char	   *eventname;		/* event's identifier */
	List	   *whenclause;		/* list of DefElems indicating filtering */
	List	   *funcname;		/* qual. name of function to call */
} CreateEventTrigStmt; 

typedef struct InternalGrant
{
	bool		is_grant;
	GrantObjectType objtype;
	List	   *objects;
	bool		all_privs;
	AclMode		privileges;
	List	   *col_privs;
	List	   *grantees;
	bool		grant_option;
	DropBehavior behavior;
} InternalGrant;

typedef enum CollectedCommandType
{
	SCT_Simple,
	SCT_AlterTable,
	SCT_Grant,
	SCT_AlterOpFamily,
	SCT_AlterDefaultPrivileges,
	SCT_CreateOpClass,
	SCT_AlterTSConfig
} CollectedCommandType;

typedef struct CollectedCommand
{
	CollectedCommandType type;
	bool		in_extension;
	Node	   *parsetree;

	CollectedCommand * parent;
	union
	{
		/* most commands */
		struct
		{
			ObjectAddress address;
			ObjectAddress secondaryObject;
		}			simple;

		/* ALTER TABLE, and internal uses thereof */
		struct
		{
			Oid			objectId;
			Oid			classId;
			List	   *subcmds;
		}			alterTable;

		/* GRANT / REVOKE */
		struct
		{
			InternalGrant *istmt;
		}			grant;

		/* ALTER OPERATOR FAMILY */
		struct
		{
			ObjectAddress address;
			List	   *operators;
			List	   *procedures;
		}			opfam;

		/* CREATE OPERATOR CLASS */
		struct
		{
			ObjectAddress address;
			List	   *operators;
			List	   *procedures;
		}			createopc;

		/* ALTER TEXT SEARCH CONFIGURATION ADD/ALTER/DROP MAPPING */
		struct
		{
			ObjectAddress address;
			Oid		   *dictIds;
			int			ndicts;
		}			atscfg;

		/* ALTER DEFAULT PRIVILEGES */
		struct
		{
			GrantObjectType objtype;
		}			defprivs;
	}			d;
} CollectedCommand;

#define AT_REWRITE_ALTER_PERSISTENCE	0x01
#define AT_REWRITE_DEFAULT_VAL			0x02
#define AT_REWRITE_COLUMN_REWRITE		0x04
#define AT_REWRITE_ALTER_OID			0x08

/*
 * EventTriggerData is the node type that is passed as fmgr "context" info
 * when a function is called by the event trigger manager.
 */
#define CALLED_AS_EVENT_TRIGGER(fcinfo) \
	((fcinfo)->context != NULL && IsA((fcinfo)->context, EventTriggerData))

extern Oid	CreateEventTrigger(CreateEventTrigStmt *stmt);
extern void RemoveEventTriggerById(Oid ctrigOid);
extern Oid	get_event_trigger_oid(const char *trigname, bool missing_ok);

extern Oid	AlterEventTrigger(AlterEventTrigStmt *stmt);
extern ObjectAddress AlterEventTriggerOwner(const char *name, Oid newOwnerId);
extern void AlterEventTriggerOwner_oid(Oid, Oid newOwnerId);

extern bool EventTriggerSupportsObjectType(ObjectType obtype);
extern bool EventTriggerSupportsObjectClass(ObjectClass objclass);
extern bool EventTriggerSupportsGrantObjectType(GrantObjectType objtype);
extern void EventTriggerDDLCommandStart(Node *parsetree);
extern void EventTriggerDDLCommandEnd(Node *parsetree);
extern void EventTriggerSQLDrop(Node *parsetree);
extern void EventTriggerTableRewrite(Node *parsetree, Oid tableOid, int reason);

extern bool EventTriggerBeginCompleteQuery(void);
extern void EventTriggerEndCompleteQuery(void);
extern bool trackDroppedObjectsNeeded(void);
extern void EventTriggerSQLDropAddObject(const ObjectAddress *object,
							 bool original, bool normal);

extern void EventTriggerInhibitCommandCollection(void);
extern void EventTriggerUndoInhibitCommandCollection(void);

extern void EventTriggerCollectSimpleCommand(ObjectAddress address,
								 ObjectAddress secondaryObject,
								 Node *parsetree);

extern void EventTriggerAlterTableStart(Node *parsetree);
extern void EventTriggerAlterTableRelid(Oid objectId);
extern void EventTriggerCollectAlterTableSubcmd(Node *subcmd,
									ObjectAddress address);
extern void EventTriggerAlterTableEnd(void);

extern void EventTriggerCollectGrant(InternalGrant *istmt);
extern void EventTriggerCollectAlterOpFam(AlterOpFamilyStmt *stmt,
							  Oid opfamoid, List *operators,
							  List *procedures);
extern void EventTriggerCollectCreateOpClass(CreateOpClassStmt *stmt,
								 Oid opcoid, List *operators,
								 List *procedures);
extern void EventTriggerCollectAlterTSConfig(AlterTSConfigurationStmt *stmt,
								 Oid cfgId, Oid *dictIds, int ndicts);
extern void EventTriggerCollectAlterDefPrivs(AlterDefaultPrivilegesStmt *stmt);

#endif   /* EVENT_TRIGGER_H */