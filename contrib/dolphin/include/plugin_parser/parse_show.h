#ifndef PARSE_SHOW_H
#define PARSE_SHOW_H

#define SHOW_COLLATION_COL	  "Collation"
#define SHOW_COLLATION_COL_S	  "collation"
#define SHOW_COMMENT_COL	  "Comment"
#define SHOW_COMMENT_COL_S	  "comment"
#define SHOW_CONTEXT_COL          "Context"
#define SHOW_CONTEXT_COL_S        "context"
#define SHOW_DEFAULT_COL	  "Default"
#define SHOW_DEFAULT_COL_S	  "default"
#define SHOW_EXTRA_COL		  "Extra"
#define SHOW_EXTRA_COL_S	  "extra"
#define SHOW_FIELDS_COL           "Field"
#define SHOW_FIELDS_COL_S	  "field"
#define SHOW_KEY_COL		  "Key"
#define SHOW_KEY_COL_S		  "key"
#define SHOW_LIBRARY_COL	  "Library"
#define SHOW_LIBRARY_COL_S  	  "library"
#define SHOW_LICENSE_COL	  "License"
#define SHOW_LICENSE_COL_S  	  "license"
#define SHOW_NAME_COL		  "Name"
#define SHOW_NAME_COL_S		  "name"
#define SHOW_NULL_COL		  "Null"
#define SHOW_NULL_COL_S		  "null"
#define SHOW_PRIVILEGE_COL        "Privilege"
#define SHOW_PRIVILEGE_COL_S      "privilege"
#define SHOW_PRIVILEGES_COL	  "Privileges"
#define SHOW_PRIVILEGES_COL_S	  "privileges"
#define SHOW_STATUS_COL		  "Status"
#define SHOW_STATUS_COL_S   	  "Status"
#define SHOW_TBL_TYPE_COL	  "Table_type"
#define SHOW_TBL_TYPE_COL_S       "table_type"
#define SHOW_TYPE_COL		  "Type"
#define SHOW_TYPE_COL_S		  "type"

#define PG_CLASS_NAME 		  "pg_class"
#define PG_NAMESPACE_NAME   	  "pg_namespace"
#define PG_CATALOG_NAME		  "pg_catalog"
#define INFO_SCHEMA_NAME	  "information_schema"
#define DB4AI_NAME		  "db4ai"
#define PG_TOAST_PREFIX		  "^pg_toast"


#define PG_ATTRIBUTE_ALIAS	"a"
#define PG_CLASS_ALIAS 		"c"
#define PG_NAMESPACEA_ALIAS	"n"
#define PG_TYPE_ALIAS		"t"

#define PLPS_LOC_UNKNOWN (-1)

extern bool plps_check_schema_or_table_valid(char *schemaname, char *tablename, bool is_missingok);
extern Node* plpsAddCond(Node* left, Node* right, int location = PLPS_LOC_UNKNOWN);
extern Node* plpsMakeColumnRef(char* relName, char* colName, int location = PLPS_LOC_UNKNOWN);
extern Node* plpsMakeFunc(char* funcname, List* args, int location = PLPS_LOC_UNKNOWN);
extern Node* plpsMakeNormalColumn(char *relname, char *colname, char* aliasname, int location = PLPS_LOC_UNKNOWN);

#endif /* PARSE_TARGET_H */
