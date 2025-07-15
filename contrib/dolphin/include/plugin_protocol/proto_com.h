/*
 * Copyright (c) 2022 China Unicom Co.,Ltd.
 *
 * openGauss is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 * -------------------------------------------------------------------------
 *
 * proto_com.h
 *
 * IDENTIFICATION
 *    dolphin/include/plugin_protocol/proto_com.h
 * -------------------------------------------------------------------------
 */
#ifndef _PROTO_COM 
#define _PROTO_COM

#include "c.h"

typedef struct dolphin_proto_ctx {
    char *server_name;
    NameData database_name;
    char sock_path[MAXPGPATH];
    char mysql_ca[NAMEDATALEN];
    char mysql_server_cert[NAMEDATALEN];
    char mysql_server_key[NAMEDATALEN];
    int32 connect_id;
    int32 statement_id;
} dolphin_proto_ctx; 

extern dolphin_proto_ctx g_proto_ctx;

enum dolphin_server_capability {
  CLIENT_LONG_PASSWORD = 1,
  CLIENT_FOUND_ROWS = (1UL << 1),
  CLIENT_LONG_FLAG = (1UL << 2),
  CLIENT_CONNECT_WITH_DB = (1UL << 3),
  CLIENT_NO_SCHEMA = (1UL << 4),
  CLIENT_COMPRESS = (1UL << 5),
  CLIENT_ODBC = (1UL << 6),
  CLIENT_LOCAL_FILES = (1UL << 7),
  CLIENT_IGNORE_SPACE = (1UL << 8),
  CLIENT_PROTOCOL_41 = (1UL << 9),
  CLIENT_INTERACTIVE = (1UL << 10),
  CLIENT_SSL = (1UL << 11),
  CLIENT_IGNORE_SIGPIPE = (1UL << 12),
  CLIENT_TRANSACTIONS = (1UL << 13),
  CLIENT_RESERVED = (1UL << 14),
  CLIENT_SECURE_CONNECTION = (1UL << 15),
  CLIENT_MULTI_STATEMENTS = (1UL << 16),
  CLIENT_MULTI_RESULTS = (1UL << 17),
  CLIENT_PS_MULTI_RESULTS = (1UL << 18),
  CLIENT_PLUGIN_AUTH = (1UL << 19),
  CLIENT_CONNECT_ATTRS = (1UL << 20),
  CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA = (1UL << 21),
  CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS = (1UL << 22),
  CLIENT_SESSION_TRACK = (1UL << 23),
  CLIENT_DEPRECATE_EOF = (1UL << 24),
  CLIENT_OPTIONAL_RESULTSET_METADATA = (1UL << 25),
  CLIENT_ZSTD_COMPRESSION_ALGORITHM = (1UL << 26),
  CLIENT_QUERY_ATTRIBUTES = (1UL << 27),
  CLIENT_MULTI_FACTOR_AUTHENTICATION = (1UL << 28),
  CLIENT_CAPABILITY_EXTENSION = (1UL << 29),
  CLIENT_SSL_VERIFY_SERVER_CERT = (1UL << 30),
  CLIENT_REMEMBER_OPTIONS = (1UL << 31)
}; 

/*
 Support capablity in server side execpt: 
 CLIENT_NO_SCHEMA, CLIENT_IGNORE_SPACE, CLIENT_DEPRECATE_EOF, CLIENT_LOCAL_FILES, CLIENT_PROGRESS, 
 CLIENT_CONNECT_ATTRS, CLIENT_COMPRESS, CLIENT_SSL_VERIFY_SERVER_CERT, CLIENT_ZSTD_COMPRESSION_ALGORITHM 
*/
#define DOPHIN_DEFAULT_FLAGS                                                                                         \
    (CLIENT_LONG_PASSWORD | CLIENT_FOUND_ROWS | CLIENT_LONG_FLAG | CLIENT_CONNECT_WITH_DB |                          \
    CLIENT_NO_SCHEMA | CLIENT_COMPRESS | CLIENT_ODBC | CLIENT_LOCAL_FILES   |                                        \
    CLIENT_IGNORE_SPACE | CLIENT_PROTOCOL_41 | CLIENT_INTERACTIVE |                                                  \
    CLIENT_IGNORE_SIGPIPE | CLIENT_TRANSACTIONS | CLIENT_RESERVED | CLIENT_SECURE_CONNECTION |                       \
    CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS | CLIENT_PS_MULTI_RESULTS | CLIENT_PLUGIN_AUTH |                  \
    CLIENT_CONNECT_ATTRS | CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA | CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS |             \
    CLIENT_SESSION_TRACK |CLIENT_DEPRECATE_EOF | CLIENT_OPTIONAL_RESULTSET_METADATA| CLIENT_QUERY_ATTRIBUTES|        \
    CLIENT_MULTI_FACTOR_AUTHENTICATION | CLIENT_SSL_VERIFY_SERVER_CERT | CLIENT_REMEMBER_OPTIONS)

#define AUTH_PLUGIN_DATA_LEN 20

/* server status defintion */
#define SERVER_STATUS_IN_TRANS 1
#define SERVER_STATUS_AUTOCOMMIT 2 
#define SERVER_MORE_RESULTS_EXISTS 8
#define SERVER_QUERY_NO_GOOD_INDEX_USED 16
#define SERVER_QUERY_NO_INDEX_USED 32
#define SERVER_STATUS_CURSOR_EXISTS 64
#define SERVER_STATUS_LAST_ROW_SENT 128
#define SERVER_STATUS_DB_DROPPED 256
#define SERVER_STATUS_NO_BACKSLASH_ESCAPES 512
#define SERVER_STATUS_METADATA_CHANGED 1024
#define SERVER_QUERY_WAS_SLOW 2048
#define SERVER_PS_OUT_PARAMS 4096
#define SERVER_STATUS_IN_TRANS_READONLY 8192
#define SERVER_SESSION_STATE_CHANGED (1UL << 14)

/* Command phase definition*/
#define COM_SLEEP                   0x00 
#define COM_QUIT                    0x01
#define COM_INIT_DB                 0x02
#define COM_QUERY                   0x03
#define COM_FIELD_LIST              0x04
#define COM_CREATE_DB               0x05
#define COM_DROP_DB                 0x06       
#define COM_REFRESH                 0x07
#define COM_DEPRECATED_1            0x08
#define COM_STATISTICS              0x09
#define COM_PROCESS_INFO            0x0a
#define COM_CONNECT                 0x0b
#define COM_PROCESS_KILL            0x0c
#define COM_DEBUG                   0x0d
#define COM_PING                    0x0e
#define COM_TIME                    0x0f
#define COM_DELAYED_INSERT          0x10
#define COM_CHANGE_USER             0x11
#define COM_BINLOG_DUMP             0x12
#define COM_TABLE_DUMP              0x13
#define COM_CONNECT_OUT             0x14
#define COM_REGISTER_SLAVE          0x15
#define COM_STMT_PREPARE            0x16
#define COM_STMT_EXECUTE            0x17
#define COM_STMT_SEND_LONG_DATA     0x18
#define COM_STMT_CLOSE              0x19
#define COM_STMT_RESET              0x1a
#define COM_SET_OPTION              0x1b
#define COM_STMT_FETCH              0x1c
#define COM_DAEMON                  0x1d
#define COM_BINLOG_DUMP_GTID        0x1e
#define COM_RESET_CONNECTION        0x1f

enum dolphin_attr_type {
  DOLPHIN_TYPE_DECIMAL,
  DOLPHIN_TYPE_TINY,
  DOLPHIN_TYPE_SHORT,
  DOLPHIN_TYPE_LONG,
  DOLPHIN_TYPE_FLOAT,
  DOLPHIN_TYPE_DOUBLE,
  DOLPHIN_TYPE_NULL,
  DOLPHIN_TYPE_TIMESTAMP,
  DOLPHIN_TYPE_LONGLONG,
  DOLPHIN_TYPE_INT24,
  DOLPHIN_TYPE_DATE,
  DOLPHIN_TYPE_TIME,
  DOLPHIN_TYPE_DATETIME,
  DOLPHIN_TYPE_YEAR,
  DOLPHIN_TYPE_NEWDATE,
  DOLPHIN_TYPE_VARCHAR,
  DOLPHIN_TYPE_BIT,
  DOLPHIN_TYPE_TIMESTAMP2,
  DOLPHIN_TYPE_DATETIME2,
  DOLPHIN_TYPE_TIME2,
  DOLPHIN_TYPE_TYPED_ARRAY,
  DOLPHIN_TYPE_INVALID = 243,
  DOLPHIN_TYPE_BOOL = 244,
  DOLPHIN_TYPE_JSON = 245,
  DOLPHIN_TYPE_NEWDECIMAL = 246,
  DOLPHIN_TYPE_ENUM = 247,
  DOLPHIN_TYPE_SET = 248,
  DOLPHIN_TYPE_TINY_BLOB = 249,
  DOLPHIN_TYPE_MEDIUM_BLOB = 250,
  DOLPHIN_TYPE_LONG_BLOB = 251,
  DOLPHIN_TYPE_BLOB = 252,
  DOLPHIN_TYPE_VAR_STRING = 253,
  DOLPHIN_TYPE_STRING = 254,
  DOLPHIN_TYPE_GEOMETRY = 255
};

/* Dolphin server data type definition */
#define DOLPHIN_TYPE_DECIMAL      0
#define DOLPHIN_TYPE_TINY         1
#define DOLPHIN_TYPE_SHORT        2 
#define DOLPHIN_TYPE_LONG         3
#define DOLPHIN_TYPE_FLOAT        4
#define DOLPHIN_TYPE_DOUBLE       5
#define DOLPHIN_TYPE_NULL         6
#define DOLPHIN_TYPE_TIMESTAMP    7
#define DOLPHIN_TYPE_LONGLONG     8
#define DOLPHIN_TYPE_INT24        9
#define DOLPHIN_TYPE_DATE         10
#define DOLPHIN_TYPE_TIME         11
#define DOLPHIN_TYPE_DATETIME     12
#define DOLPHIN_TYPE_YEAR         13
#define DOLPHIN_TYPE_NEWDATE      14
#define DOLPHIN_TYPE_VARCHAR      15
#define DOLPHIN_TYPE_BIT          16
#define DOLPHIN_TYPE_TIMESTAMP2   17
#define DOLPHIN_TYPE_DATETIME2    18
#define DOLPHIN_TYPE_TIME2        19
#define DOLPHIN_TYPE_TYPED_ARRAY  20
#define DOLPHIN_TYPE_INVALID      243
#define DOLPHIN_TYPE_BOOL         244
#define DOLPHIN_TYPE_JSON         245
#define DOLPHIN_TYPE_NEWDECIMAL   246
#define DOLPHIN_TYPE_ENUM         247
#define DOLPHIN_TYPE_SET          248
#define DOLPHIN_TYPE_TINY_BLOB    249
#define DOLPHIN_TYPE_MEDIUM_BLOB  250
#define DOLPHIN_TYPE_LONG_BLOB    251
#define DOLPHIN_TYPE_BLOB         252
#define DOLPHIN_TYPE_VAR_STRING   253
#define DOLPHIN_TYPE_STRING       254
#define DOLPHIN_TYPE_GEOMETRY     255

/* flag number */
#define BLOB_FLAG 16
#define UNSIGNED_FLAG 32
#define BINARY_FLAG 128

/* charset number */
#define COLLATE_BINARY 63

typedef struct dolphin_column_definition {
  const char *name;
  char *org_name;
  char *table;
  char *org_table;
  char *db;
  char *catalog;
  char *def;
  ulong length;
  ulong max_length;
  uint name_length;
  uint org_name_length;
  uint table_length;
  uint org_table_length;
  uint db_length;
  uint catalog_length;
  uint def_length;
  uint flags;
  uint decimals;
  uint charsetnr;
  uint type; 
  void *extension;
  char *default_value;
} dolphin_column_definition;

#define MAX_TYPE_NAME_LEN 64
extern struct HTAB* b_typoid2DolphinMarcoHash;
typedef struct TypeItem {
    char* og_typname;             // type name of openGauss
    uint dolphin_type_id;         // the type Marco in M* database
    Oid og_type_oid;              // type oid
    uint flags;                   // type flag, such as unsigned flag or others
    uint charset_flag;            // charset
} TypeItem;

typedef struct b_typoidHashKey {
    Oid db_oid;
    Oid type_oid;
} b_typoidHashKey;

typedef struct HashEntryTypoid2TypeItem {
    b_typoidHashKey key;
    char status;
    const TypeItem* item;
} HashEntryTypoid2TypeItem;

typedef struct InputStmtParam {
    uint32* itypes;
    uint32 count;
} InputStmtParam;

typedef struct HashEntryStmtParamType {
    int32 stmt_id;
    InputStmtParam* value;
} HashEntryStmtParamType;

typedef struct BlobParams {
    const char** data;
    uint32 count;
    uint32 cursor;
} BlobParams;

typedef struct HashEntryBlob {
    uint32 stmt_id;
    BlobParams* value;
} HashEntryBlob;

extern void InitTypoid2DolphinMacroHtab();
extern const TypeItem* GetItemByTypeOid(Oid oid);


extern const InputStmtParam* GetCachedInputStmtParamTypes(int32 stmt_id);
extern void SaveCachedInputStmtParamTypes(int32 stmt_id, InputStmtParam* value);

extern const char* GetCachedParamBlob(uint32 stmt_id);
extern void SaveCachedParamBlob(uint32 stmt_id, char *data);

#endif /* proto_com.h */