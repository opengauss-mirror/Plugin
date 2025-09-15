/* ---------------------------------------------------------------------------------------
 *
 * spq_backup.cpp
 * receive a base backup using streaming replication protocol
 *
 * Portions Copyright (c) 2025 Huawei Technologies Co.,Ltd.
 *
 * IDENTIFICATION
 * src/spq_backup/spq_backup.cpp
 *
 * ---------------------------------------------------------------------------------------
 */

#include "postgres.h"
#include "knl/knl_variable.h"
#include "libpq/libpq-fe.h"
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>

#include "tool_common.h"
#include "getopt_long.h"
#include "receivelog.h"
#include "streamutil.h"
#include "gs_tar_const.h"
#include "bin/elog.h"
#include "lib/string.h"
#include "PageCompression.h"
#include <semaphore.h>
#include <unordered_map>
#include <set>
#include <vector>

namespace Spq {
/*
 * We have to use postgres.h not postgres_fe.h here, because there's so much
 * backend-only stuff in the XLOG include files we need.  But we need a
 * frontend-ish environment otherwise. Hence this ugly hack.
 */
#define FRONTEND 1
#define PROG_NAME_BACKUP "spq_backup"

typedef struct TablespaceListCell {
    struct TablespaceListCell* next;
    char old_dir[MAXPGPATH];
    char new_dir[MAXPGPATH];
} TablespaceListCell;

typedef struct TablespaceList {
    TablespaceListCell* head;
    TablespaceListCell* tail;
} TablespaceList;

typedef struct {
    PGconn* bgconn;
    XLogRecPtr startptr;
    XLogRecPtr endptr;
    char xlogdir[MAXPGPATH];
    char* sysidentifier;
    int timeline;
} logstreamer_param;

/* Global options */
char* basedir = NULL;
static TablespaceList tablespacee_dirs = {NULL, NULL};
char format = 'p'; /* p(lain)/t(ar) */
char* label = "spq_backup backup";
/* We hope always print progress here. To maitain compatibility, we don't remove this
 * parameter. */
bool showprogress = false;
int compresslevel = 0;
bool includewal = true;
bool streamwal = true;
bool fastcheckpoint = false;
logstreamer_param* g_childParam = NULL;

extern int standby_message_timeout; /* 10 sec = default */

/* Progress counters */
static uint64 totalsize;
static uint64 totaldone;
static int g_tablespacenum;
static volatile bool g_progressFlag = false;
static pthread_cond_t g_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

static int standby_message_timeout_local = 10; /* 10 sec = default */

/* Pipe to communicate with background wal receiver process */
#ifndef WIN32
static int bgpipe[2] = {-1, -1};
#endif

/* Handle to child process */
static pid_t bgchild = -1;

/* End position for xlog streaming, empty string if unknown yet */
static XLogRecPtr xlogendptr;

/* Function headers */
static void usage(void);
static void verify_dir_is_empty_or_create(char* dirname);
static void* ProgressReport(void* arg);

static void ReceiveTarFile(PGconn* conn, PGresult* res, int rownum);
static void ReceiveAndUnpackTarFile(PGconn* conn, PGresult* res, int rownum);
static void BaseBackup(void);
static void backup_dw_file(const char* target_dir);

static bool reached_end_position(XLogRecPtr segendpos, uint32 timeline,
                                 bool segment_finished, XLogRecPtr paramEndPtr);
static void free_basebackup();

static int GsBaseBackup(int argc, char** argv);

static const char* get_tablespace_mapping(const char* dir);

#define TABLESPACE_LIST_RELEASE()               \
    if (tblspaceDirectory != NULL) {            \
        int k;                                  \
        for (k = 0; k < tblspaceCount; k++) {   \
            if (tblspaceDirectory[k] != NULL) { \
                free(tblspaceDirectory[k]);     \
                tblspaceDirectory[k] = NULL;    \
            }                                   \
        }                                       \
        free(tblspaceDirectory);                \
        tblspaceDirectory = NULL;               \
    }

class BackupWorker;

class BackupCoordinator {
public:
    BackupCoordinator(int nodeCount) : workerMap(nodeCount)
    {
        sem_init(&dataBackupSem, 0, 0);
        sem_init(&xlogBackupSem, 0, 0);
        activeDataBackupNodes = nodeCount;
        activeXlogBackupNodes = nodeCount;
    }
    sem_t dataBackupSem;
    sem_t xlogBackupSem;
    std::atomic<int> activeDataBackupNodes{0};
    std::atomic<int> activeXlogBackupNodes{0};
    std::unordered_map<int, BackupWorker*> workerMap;
};

struct NodeInfo {
    std::string dbid;
    std::string dbhost;
    std::string dbport;
};

static int LogStreamerMain(logstreamer_param* param);

class BackupWorker {
public:
    BackupWorker(int id, std::string host, std::string port,
                 BackupCoordinator* backupCoordinator);
    void backup();
    void dataFileBackupFinished();
    void StartLogThread(const char* startpos, uint32 timeline, char* sysidentifier);
    void StartDataThread();
    void setRestorePoint(char* endPtrStr);
    static void* DataThread(void* arg);
    static void* LogStreamerThread(void* arg);
    PGconn* GetConnection();
    NodeInfo nodeInfo;

private:
    BackupCoordinator* coordinator;
    std::string nodeBaseDir;
    pthread_t logThread;
    pthread_t dataThread;
    logstreamer_param child_params_;
    std::atomic<XLogRecPtr> endPtr;
    PGconn* conn;
    void xlogBackup();
    void onDataBackupFinished();
    void onXlogBackupFinished();
    char** tblspaceDirectory = NULL;
    int tblspaceCount = 0;
    int tblspaceIndex = 0;

    void* RunLogStreamer();
    void tablespace_list_create();
    void save_tablespace_dir(const char* dir);
    void removeCreatedTblspace();
    void ReceiveTarFile(PGconn* conn, PGresult* res, int rownum);
    void ReceiveAndUnpackTarFile(PGconn* conn, PGresult* res, int rownum);
};

BackupWorker::BackupWorker(int id, std::string host, std::string port,
                           BackupCoordinator* backupCoordinator)
    : coordinator(backupCoordinator)
{
    nodeInfo = {std::to_string(id), host, port};

    std::ostringstream oss;
    oss << basedir << "/" << nodeInfo.dbid << "_" << nodeInfo.dbhost << "_"
        << nodeInfo.dbport << "_" << (id == 0 ? "CN" : "DN");

    nodeBaseDir = oss.str();
}

void BackupWorker::StartLogThread(const char* startpos, uint32 timeline,
                                  char* sysidentifier)
{
    uint32 hi, lo;
    uint32 pathlen = 0;
    /* Convert the starting position */
    if (sscanf_s(startpos, "%X/%X", &hi, &lo) != 2) {
        pg_log(stderr, _("%s: could not parse transaction log location \"%s\"\n"),
               progname, startpos);
        exit(1);
    }

    child_params_.timeline = timeline;
    child_params_.sysidentifier = xstrdup(sysidentifier);
    child_params_.startptr = ((uint64)hi) << 32 | lo;
    child_params_.startptr -= child_params_.startptr % XLOG_SEG_SIZE;

    pathlen = strlen("pg_xlog") + 1 + nodeBaseDir.length() + 1;
    int errorno = snprintf_s(child_params_.xlogdir, sizeof(child_params_.xlogdir),
                             pathlen, "%s/pg_xlog", nodeBaseDir.c_str());
    securec_check_ss_c(errorno, "", "");

    int rc = pthread_create(&logThread, NULL, &BackupWorker::LogStreamerThread, this);

    if (rc != 0) {
        exit(1);
    }

    pthread_detach(logThread);
}

void BackupWorker::StartDataThread()
{
    if (mkdir(nodeBaseDir.c_str(), S_IRWXU) != 0) {
        pg_log(stderr, ("%s: could not create directory \"%s\": %s\n"), progname,
               nodeBaseDir.c_str(), strerror(errno));
        exit(1);
    }
    int rc = pthread_create(&dataThread, NULL, &BackupWorker::DataThread, this);
    if (rc != 0) {
        exit(1);
    }

    pthread_detach(dataThread);
}

void GenerateRecoveryConf(std::string path, char* endPtrStr)
{
    std::string filename = path + "/recovery.conf";
    FILE* file = fopen(filename.c_str(), "wb");
    std::string content = "recovery_target_lsn = '";
    content += endPtrStr;
    content += "'\n";

    if (fwrite(content.c_str(), content.length(), 1, file) != 1) {
        pg_log(stderr, _("%s: could not write to recovery.conf \"%s\": %s\n"), progname,
               filename, strerror(errno));
        exit(1);
    }

    if (file != nullptr) {
        fclose(file);
        file = NULL;
    }
}

void BackupWorker::setRestorePoint(char* endPtrStr)
{
    int hi, lo;
    if (sscanf_s(endPtrStr, "%X/%X", &hi, &lo) != 2) {
        pg_log(stderr, _("node %s setRestorePoint to %s failed\n"), nodeInfo.dbid.c_str(),
               endPtrStr == nullptr ? "" : endPtrStr);
        exit(1);
    }
    child_params_.endptr = ((uint64)hi) << 32 | lo;
    pg_log(stderr, _("node %s setRestorePoint to %s\n"), nodeInfo.dbid.c_str(),
           endPtrStr);
    /* generate recovery.conf */
    try {
        GenerateRecoveryConf(nodeBaseDir.c_str(), endPtrStr);
    } catch (const std::exception& e) {
        pg_log(stderr, _("node %s generate recovery.conf failed\n"),
               nodeInfo.dbid.c_str());
        exit(1);
    }
}

void* BackupWorker::DataThread(void* arg)
{
    BackupWorker* self = static_cast<BackupWorker*>(arg);
    sigset_t mask;
    sigfillset(&mask);
    pthread_sigmask(SIG_SETMASK, &mask, NULL);

    self->backup();
    return nullptr;
}

void* BackupWorker::LogStreamerThread(void* arg)
{
    BackupWorker* self = static_cast<BackupWorker*>(arg);

    sigset_t mask;
    sigfillset(&mask);
    pthread_sigmask(SIG_SETMASK, &mask, NULL);

    return self->RunLogStreamer();
}
void BackupWorker::onDataBackupFinished()
{
    if (--coordinator->activeDataBackupNodes == 0) {
        sem_post(&coordinator->dataBackupSem);
    }
}

void BackupWorker::onXlogBackupFinished()
{
    if (--coordinator->activeXlogBackupNodes == 0) {
        sem_post(&coordinator->xlogBackupSem);
    }
}

void* BackupWorker::RunLogStreamer()
{
    child_params_.bgconn = GetConnection();
    int ret = LogStreamerMain(&child_params_);
    if (ret) {
        abort();
    }
    PQfinish(child_params_.bgconn);
    child_params_.bgconn = nullptr;
    onXlogBackupFinished();
    return reinterpret_cast<void*>(static_cast<intptr_t>(ret));
}

static PGresult* FetchOneRestorePoint(PGconn* connection)
{
    if (PQsendQuery(connection, "select * from pg_create_restore_point('spq_backup')") ==
        0) {
        pg_log(stderr, _("%s: could not send query create restore point: %s\n"), progname,
               PQerrorMessage(connection));
        exit(1);
    }
    PGresult* res = PQgetResult(connection);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        pg_log(stderr, _("%s: could not create restore point: %s\n"), progname,
               PQerrorMessage(connection));
        exit(1);
    }
    return res;
}

static PGresult* GetDatabases()
{
    if (PQsendQuery(conn, "SELECT datname FROM pg_database WHERE datname NOT IN "
                          "('template0', 'template1') ORDER BY datname") == 0) {
        pg_log(stderr, _("%s: could not send query databases command \"%s\": %s\n"),
               progname, PQerrorMessage(conn));
        exit(1);
    }
    PGresult* res = PQgetResult(conn);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        pg_log(stderr, _("could not get databases: %s\n"), PQerrorMessage(conn));
        exit(1);
    }
    return res;
}

static PGresult* GetSpqNodeCount(PGconn* connection = conn)
{
    if (PQsendQuery(connection, "SELECT * from pg_dist_node where isactive = 't'") == 0) {
        pg_log(stderr, _("%s: could not send select pg_dist_node command: %s\n"),
               progname, PQerrorMessage(connection));
        exit(1);
    }
    PGresult* res = PQgetResult(connection);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        pg_log(stderr, _("could not get node infomation: %s\n"),
               PQerrorMessage(connection));
        exit(1);
    }
    return res;
}

static PGresult* GetSpqNodes(PGconn* connection = conn)
{

    if (PQsendQuery(connection,
                    "SELECT * from pg_class where relname = 'pg_dist_node'") == 0) {
        pg_log(stderr, _("%s: could not send query pg_class command: %s\n"), progname,
               PQerrorMessage(connection));
        exit(1);
    }
    PGresult* res = PQgetResult(connection);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        pg_log(stderr, _("could not get SpqNodes: %s\n"), PQerrorMessage(connection));
        exit(1);
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        PQreset(connection);
        return nullptr;
    }

    PQclear(res);
    PQreset(connection);

    return GetSpqNodeCount(connection);
}

void BackupWorker::backup()
{
    PGresult* res = NULL;
    char* sysidentifier = NULL;
    uint32 timeline = 0;
    char current_path[MAXPGPATH] = {0};
    char nodetablespacepath[MAXPGPATH] = {0};
    char escaped_label[MAXPGPATH] = {0};
    int i = 0;
    char xlogstart[64];
    errno_t rc = EOK;
    char* get_value = NULL;

    conn = GetConnection();
    if (NULL == conn) {
        exit(1);
    }

    /*
     * Run IDENTIFY_SYSTEM so we can get the timeline
     */
    res = PQexec(conn, "IDENTIFY_SYSTEM");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        pg_log(stderr, _("%s: could not send replication command \"%s\": %s\n"), progname,
               "IDENTIFY_SYSTEM", PQerrorMessage(conn));
        exit(1);
    }
    if (PQntuples(res) != 1 || PQnfields(res) < 3) {
        pg_log(stderr,
               _("%s: could not identify system: got %d rows and %d fields, expected %d "
                 "rows and %d or more fields\n"),
               progname, PQntuples(res), PQnfields(res), 1, 3);
        exit(1);
    }
    sysidentifier = strdup(PQgetvalue(res, 0, 0));
    if (NULL == sysidentifier) {
        pg_log(stderr, _("%s: strdup for sysidentifier failed! \n"), progname);
        exit(1);
    }
    timeline = atoi(PQgetvalue(res, 0, 1));
    PQclear(res);

    /*
     * Start the actual backup
     */
    PQescapeStringConn(conn, escaped_label, label, sizeof(escaped_label), &i);
    rc = snprintf_s(current_path, sizeof(current_path), sizeof(current_path) - 1,
                    "BASE_BACKUP LABEL '%s' %s %s %s %s %s", escaped_label,
                    showprogress ? "PROGRESS" : "", includewal && !streamwal ? "WAL" : "",
                    fastcheckpoint ? "FAST" : "", includewal ? "NOWAIT" : "",
                    format == 't' ? "TABLESPACE_MAP" : "");
    securec_check_ss_c(rc, "", "");

    if (PQsendQuery(conn, current_path) == 0) {
        pg_log(stderr, _("%s: could not send replication command \"%s\": %s\n"), progname,
               "BASE_BACKUP", PQerrorMessage(conn));
        PQfreemem(sysidentifier);
        sysidentifier = NULL;
        exit(1);
    }
    /*
     *get the xlog location
     */
    res = PQgetResult(conn);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        pg_log(stderr, _("could not get xlog location: %s\n"), PQerrorMessage(conn));
        PQfreemem(sysidentifier);
        sysidentifier = NULL;
        exit(1);
    }
    if (PQntuples(res) != 1) {
        pg_log(stderr, _("no xlog location returned from server\n"));
        PQfreemem(sysidentifier);
        sysidentifier = NULL;
        exit(1);
    }
    get_value = PQgetvalue(res, 0, 0);
    /* if linkpath is NULL ? */
    if (NULL == get_value) {
        pg_log(stderr, _("get xlog location failed\n"));
        PQfreemem(sysidentifier);
        sysidentifier = NULL;
        exit(1);
    }

    PQclear(res);

    /*
     * Get the starting xlog position
     */
    res = PQgetResult(conn);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        pg_log(stderr, _("%s: could not initiate base backup: %s\n"), progname,
               PQerrorMessage(conn));
        PQfreemem(sysidentifier);
        sysidentifier = NULL;
        exit(1);
    }
    if (PQntuples(res) != 1) {
        pg_log(stderr, _("%s: no start point returned from server\n"), progname);
        PQfreemem(sysidentifier);
        sysidentifier = NULL;
        exit(1);
    }
    rc = strncpy_s(xlogstart, sizeof(xlogstart), PQgetvalue(res, 0, 0),
                   sizeof(xlogstart) - 1);
    securec_check_c(rc, "", "");
    if (includewal) {
        pg_log(stderr, "node %s transaction log start point: %s\n", nodeInfo.dbid.c_str(),
               xlogstart);
    }
    PQclear(res);

    /*
     * Get the header
     */
    res = PQgetResult(conn);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        pg_log(stderr, _("%s: could not get backup header: %s"), progname,
               PQerrorMessage(conn));
        PQfreemem(sysidentifier);
        sysidentifier = NULL;
        exit(1);
    }
    if (PQntuples(res) < 1) {
        pg_log(stderr, _("%s: no data returned from server\n"), progname);
        PQfreemem(sysidentifier);
        sysidentifier = NULL;
        exit(1);
    }

    /*
     * Sum up the total size, for progress reporting
     */
    totalsize = totaldone = 0;
    long int pgValue = 0;
    tblspaceCount = PQntuples(res);

    pg_log(PG_PROGRESS, _("node %s begin build tablespace list\n"),
           nodeInfo.dbid.c_str());

    /* make the tablespace directory list */
    tablespace_list_create();

    for (i = 0; i < PQntuples(res); i++) {
        pgValue = atol(PQgetvalue(res, i, 2));
        if (showprogress && ((uint64)pgValue <= UINT_MAX - totalsize)) {
            totalsize += pgValue;
        }
        /*
         * Verify tablespace directories are empty. Don't bother with the
         * first once since it can be relocated, and it will be checked before
         * we do anything anyway.
         */
        if (format == 'p' && !PQgetisnull(res, i, 1)) {
            char* tablespacepath = PQgetvalue(res, i, 1);
            char* relative = PQgetvalue(res, i, 3);
            char prefix[MAXPGPATH] = {'\0'};
            if (*relative == '1') {
                rc = snprintf_s(prefix, MAXPGPATH, nodeBaseDir.length() + 1, "%s/",
                                nodeBaseDir.c_str());
                securec_check_ss_c(rc, "\0", "\0");
            } else {
                pg_log(stderr, _("%s: Absolute tablespace path not supported.\n"),
                       progname);
                exit(1);
            }
            rc = snprintf_s(nodetablespacepath, MAXPGPATH, sizeof(nodetablespacepath) - 1,
                            "%s%s", prefix, tablespacepath);
            securec_check_ss_c(rc, "\0", "\0");

            const char* mappingSpacePath = get_tablespace_mapping(nodetablespacepath);
            verify_dir_is_empty_or_create((char*)mappingSpacePath);
            /* Save the tablespace directory here so we can remove it when errors happen.
             */
            save_tablespace_dir(mappingSpacePath);
        }
    }

    pg_log(PG_PROGRESS, _("node %s finish build tablespace list\n"),
           nodeInfo.dbid.c_str());

    /*
     * When writing to stdout, require a single tablespace
     */
    if (format == 't' && strcmp(basedir, "-") == 0 && PQntuples(res) > 1) {
        pg_log(stderr,
               _("%s: can only write single tablespace to stdout, database has %d\n"),
               progname, PQntuples(res));
        PQfreemem(sysidentifier);
        sysidentifier = NULL;
        exit(1);
    }

    pg_log(PG_PROGRESS, _("node %s begin get xlog by xlogstream\n"),
           nodeInfo.dbid.c_str());

    /*
     * If we're streaming WAL, start the streaming session before we start
     * receiving the actual data chunks.
     */
    if (streamwal) {
        StartLogThread(xlogstart, timeline, sysidentifier);
    }

    /* free sysidentifier after use */
    PQfreemem(sysidentifier);
    sysidentifier = NULL;

    pg_log(stderr, "node %s start receiving chunks\n", nodeInfo.dbid.c_str());

    /*
     * Start receiving chunks
     */
    for (i = 0; i < PQntuples(res); i++) {
        g_tablespacenum = i + 1;
        if (format == 't')
            ReceiveTarFile(conn, res, i);
        else
            ReceiveAndUnpackTarFile(conn, res, i);
    } /* Loop over all tablespaces */

    PQclear(res);
    g_progressFlag = true;
    pthread_mutex_lock(&g_mutex);
    pthread_cond_signal(&g_cond);
    pthread_mutex_unlock(&g_mutex);
    pg_log(stderr, "node %s finish receiving chunks\n", nodeInfo.dbid.c_str());

    /*
     * Get the stop position but we don't use it since the end pos will be set by the
     * returning value of spq_create_restore_point
     */
    res = PQgetResult(conn);
    onDataBackupFinished();
    PQclear(res);

    res = PQgetResult(conn);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        pg_log(stderr, _("%s: final receive failed: %s\n"), progname,
               PQerrorMessage(conn));
        exit(1);
    }

    /*
     * End of copy data. Final result is already checked inside the loop.
     */
    PQclear(res);

    TABLESPACE_LIST_RELEASE();

    PQfinish(conn);
    conn = NULL;

    if (format == 'p') {
        /* delete dw file if exists, recreate it and write a page of zero */
        backup_dw_file(nodeBaseDir.c_str());
    }

    pg_log(stderr, "%s: node %s base backup of data files completed\n", progname,
           nodeInfo.dbid.c_str());
}

static void BlockDistTrans(PGconn* connection)
{
    char* cmds[] = {"BEGIN;", "LOCK TABLE pg_dist_node IN EXCLUSIVE MODE NOWAIT",
                    "LOCK TABLE pg_dist_partition IN EXCLUSIVE MODE NOWAIT",
                    "LOCK TABLE pg_dist_transaction IN EXCLUSIVE MODE NOWAIT"};

    int cmdCount = sizeof(cmds) / sizeof(cmds[0]);
    for (int i = 0; i < cmdCount; i++) {
        if (PQsendQuery(connection, cmds[i]) == 0) {
            pg_log(stderr, _("%s: could not send query pg_class command \"%s\": %s\n"),
                   progname, cmds[i], PQerrorMessage(connection));
            exit(1);
        }
        PGresult* res = PQgetResult(connection);
        if (PQntuples(res) != 0) {
            pg_log(stderr, _("%s: BlockDistTrans could get result failed: %s\n"),
                   progname, PQerrorMessage(connection));
            exit(1);
        }
        PQclear(res);
        PQreset(connection);
    }
}

static void ResumeDistTrans(PGconn* connection)
{
    if (PQsendQuery(connection, "ROLLBACK") == 0) {
        pg_log(stderr, _("%s: ResumeDistTrans failed: %s, close connection anyway\n"),
               progname, PQerrorMessage(connection));
    }
}

static void BaseBackupMain()
{
    /* Connect to cn and get nodes info. */
    conn = GetConnection();
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection failed: %s\n", PQerrorMessage(conn));
        exit(1);
    }

    PGresult* dbres = GetDatabases();
    int dbcount = PQntuples(dbres);
    if (dbcount <= 0) {
        pg_log(stderr, _("No distributed databases, exit.\n"));
        exit(0);
    }

    /* Add coordinator node, the host maybe set to 127.0.0.1, we use host and
    port set in command line here. */
    std::set<std::pair<std::string, std::string>> nodeSet;
    nodeSet.insert({dbhost, dbport});
    std::set<std::string> dbSet;
    std::vector<PGconn*> dbConnVec;
    for (int i = 0; i < dbcount; i++) {
        char* dbname = PQgetvalue(dbres, i, 0);
        PGconn* connDB = GetConnection(dbname);
        dbConnVec.push_back(connDB);
        dbSet.insert(dbname);
        PGresult* dbNodeRes = GetSpqNodes(connDB);
        if (dbNodeRes == nullptr) {
            continue;
        }
        int dbNodeCount = PQntuples(dbNodeRes);
        if (dbNodeCount <= 0) {
            continue;
        }

        for (int i = 0; i < dbNodeCount; i++) {
            /* Group id of coordinator is 0, skip it since we added it before.*/
            if (atoi(PQgetvalue(dbNodeRes, i, 1)) == 0) {
                continue;
            }
            nodeSet.insert({PQgetvalue(dbNodeRes, i, 2), PQgetvalue(dbNodeRes, i, 3)});
        }
        PQclear(dbNodeRes);
        PQreset(connDB);
    }
    PQclear(dbres);

    pg_log(stderr, _("get all node to be backuped.\n"));

    /* Initialize the coordinator and sem in it. */
    int nodeCount = nodeSet.size();
    Spq::BackupCoordinator coordinator(nodeCount);
    int nodeId = 0;

    /* Start data file backup of all nodes. */
    for (auto it = nodeSet.begin(); it != nodeSet.end(); ++it, ++nodeId) {
        auto worker = new BackupWorker(nodeId, it->first, it->second, &coordinator);
        coordinator.workerMap[nodeId] = worker;
        worker->StartDataThread();
    }

    /* Wait for all the data files backuped. */
    sem_wait(&coordinator.dataBackupSem);

    pg_log(stderr, _("backup datafiles done.\n"));
    /* Block distributed transactions and get restore point. */
    dbcount = dbConnVec.size();
    for (auto it = dbConnVec.begin(); it != dbConnVec.end(); it++) {
        BlockDistTrans(*it);
    }

    for (int i = 0; i < nodeCount; i++) {
        auto worker = coordinator.workerMap[i];
        if (worker == nullptr) {
            pg_log(
                stderr,
                _("get restore Point failed since corresponding worker not exists.\n"));
            exit(1);
        }
        PGconn* restorePointConn = GetConnection(
            "postgres", worker->nodeInfo.dbhost.c_str(), worker->nodeInfo.dbport.c_str());
        PGresult* restorePointRes = FetchOneRestorePoint(restorePointConn);
        if (PQntuples(restorePointRes) != 1) {
            pg_log(stderr, _("get restore Point failed.\n"));
            exit(1);
        }

        worker->setRestorePoint(PQgetvalue(restorePointRes, 0, 0));
        PQclear(restorePointRes);
        PQfinish(restorePointConn);
    }
    pg_log(stderr, _("set xlog end ptr done.\n"));

    /* Resume distributed transactions. */
    for (auto it = dbConnVec.begin(); it != dbConnVec.end(); it++) {
        ResumeDistTrans(*it);
        PQfinish(*it);
    }
    dbConnVec.clear();
    dbSet.clear();
    nodeSet.clear();

    PQfinish(conn);
    ClearAndFreePasswd();

    sem_wait(&coordinator.xlogBackupSem);
}

/*
 * Replace the actual password with *'s.
 */
void replace_password(int argc, char** argv, const char* optionName)
{
    int count = 0;
    char* pchPass = NULL;
    char* pchTemp = NULL;

    // Check if password option is specified in command line
    for (count = 0; count < argc; count++) {
        // Password can be specified by optionName
        if (strncmp(optionName, argv[count], strlen(optionName)) == 0) {
            pchTemp = strchr(argv[count], '=');
            if (pchTemp != NULL) {
                pchPass = pchTemp + 1;
            } else if ((NULL != strstr(argv[count], optionName)) &&
                       (strlen(argv[count]) > strlen(optionName))) {
                pchPass = argv[count] + strlen(optionName);
            } else {
                pchPass = argv[(int)(count + 1)];
            }

            // Replace first char of password with * and rest clear it
            if (strlen(pchPass) > 0) {
                *pchPass = '*';
                pchPass = pchPass + 1;
                while ('\0' != *pchPass) {
                    *pchPass = '\0';
                    pchPass++;
                }
            }

            break;
        }
    }
}

static void TablespaceValueCheck(TablespaceListCell* cell, const char* arg)
{
    if (!*cell->old_dir || !*cell->new_dir) {
        fprintf(stderr,
                _("%s: invalid tablespace mapping format \"%s\", must be "
                  "\"OLDDIR=NEWDIR\"\n"),
                progname, arg);
        exit(1);
    }

    /*
     * This check isn't absolutely necessary.  But all tablespaces are created
     * with absolute directories, so specifying a non-absolute path here would
     * just never match, possibly confusing users.  It's also good to be
     * consistent with the new_dir check.
     */
    if (!is_absolute_path(cell->old_dir)) {
        fprintf(
            stderr,
            _("%s: old directory is not an absolute path in tablespace mapping: %s\n"),
            progname, cell->old_dir);
        exit(1);
    }

    if (!is_absolute_path(cell->new_dir)) {
        fprintf(
            stderr,
            _("%s: new directory is not an absolute path in tablespace mapping: %s\n"),
            progname, cell->new_dir);
        exit(1);
    }

    /* tablespace directory does not allow specifying under database directory */
    char* tableSpaceChar = strstr(cell->new_dir, basedir);
    if (tableSpaceChar != nullptr) {
        pg_log(PG_WARNING, _(" tablespace directory does not allow specifying under "
                             "database directory"));
        exit(1);
    }
}

static void isPortNumber(const char* portarg)
{
    int len = strlen(portarg);
    for (int i = 0; i < len; i++) {
        if (!(portarg[i] >= '0' && portarg[i] <= '9')) {
            fprintf(stderr, _("invalid port number \"%s\"\n"), portarg);
            exit(1);
        }
    }
}

/*
 * Split argument into old_dir and new_dir and append to tablespace mapping
 * list.
 */
static void tablespace_list_append(const char* arg)
{
    TablespaceListCell* cell = (TablespaceListCell*)xmalloc0(sizeof(TablespaceListCell));
    char* dst = NULL;
    char* dst_ptr = NULL;
    const char* arg_ptr = NULL;

    dst_ptr = dst = cell->old_dir;
    for (arg_ptr = arg; *arg_ptr; arg_ptr++) {
        if (dst_ptr - dst >= MAXPGPATH) {
            fprintf(stderr, _("%s: directory name too long\n"), progname);
            exit(1);
        }

        if ((*arg_ptr == '\\') && (*(arg_ptr + 1) == '=')) {
            ; /* skip backslash escaping = */
        } else if (*arg_ptr == '=' && (arg_ptr == arg || *(arg_ptr - 1) != '\\')) {
            if (*cell->new_dir) {
                fprintf(stderr, _("%s: multiple \"=\" signs in tablespace mapping\n"),
                        progname);
                exit(1);
            } else {
                dst = dst_ptr = cell->new_dir;
            }
        } else {
            *dst_ptr++ = *arg_ptr;
        }
    }
    TablespaceValueCheck(cell, arg);
    /*
     * Comparisons done with these values should involve similarly
     * canonicalized path values.  This is particularly sensitive on Windows
     * where path values may not necessarily use Unix slashes.
     */
    canonicalize_path(cell->old_dir);
    canonicalize_path(cell->new_dir);

    if (tablespacee_dirs.tail != NULL) {
        tablespacee_dirs.tail->next = cell;
    } else {
        tablespacee_dirs.head = cell;
    }
    tablespacee_dirs.tail = cell;
}

static void usage(void)
{
    printf(_("%s takes a base backup of a running openGauss server.\n\n"), progname);
    printf(_("Usage:\n"));
    printf(_("  %s [OPTION]...\n"), progname);
    printf(_("\nOptions controlling the output:\n"));
    printf(_("  -D, --pgdata=DIRECTORY           receive base backup into directory\n"));
    printf(_("\nGeneral options:\n"));
    printf(_("  -c, --checkpoint=fast|spread\n"
             "                                   set fast or spread checkpointing\n"));
    printf(_("  -l, --label=LABEL                set backup label\n"));
    printf(
        _("  -V, --version                    output version information, then exit\n"));
    printf(_("  -?, --help                       show this help, then exit\n"));
    printf(_("\nConnection options:\n"));
    printf(_(
        "  -h, --host=HOSTNAME              database server host or socket directory\n"));
    printf(_("  -p, --port=PORT                  database server port number\n"));
    printf(_("  -t, --rw-timeout=RW_TIMEOUT\n"
             "                                   read-write timeout during idle "
             "connection.(in seconds)\n"));
    printf(_("  -U, --username=NAME              connect as specified database user\n"));
    printf(_("  -w, --no-password                never prompt for password\n"));
    printf(_(
        "  -W, --password=password          the password of specified database user\n"));

    printf(_("\nReport bugs to community@opengauss.org> or join opengauss community "
             "<https://opengauss.org>.\n"));
}

void BackupWorker::tablespace_list_create()
{
    if (tblspaceCount <= 0) {
        return;
    }
    tblspaceDirectory = (char**)malloc(tblspaceCount * sizeof(char*));
    if (tblspaceDirectory == NULL) {
        pg_log(PG_WARNING, _(" Out of memory occured during creating tablespace list"));
        exit(1);
    }
    errno_t rcm = memset_s(tblspaceDirectory, tblspaceCount * sizeof(char*), 0,
                           tblspaceCount * sizeof(char*));
    securec_check_c(rcm, "", "");
}

void BackupWorker::save_tablespace_dir(const char* dir)
{
    if (tblspaceDirectory != NULL) {
        tblspaceDirectory[tblspaceIndex] = xstrdup(dir);
        tblspaceIndex++;
    }
}

/*
 * Called in the background process every time data is received.
 * On Unix, we check to see if there is any data on our pipe
 * (which would mean we have a stop position), and if it is, check if
 * it is time to stop.
 * On Windows, we are in a single process, so we can just check if it's
 * time to stop.
 */
static bool reached_end_position(XLogRecPtr segendpos, uint32 timeline,
                                 bool segment_finished, XLogRecPtr endPos)
{
    if (endPos == 0) {
        return false;
    }

    if (segendpos >= endPos) {
        return true;
    }

    return false;
}

static int LogStreamerMain(logstreamer_param* param)
{
    int ret = 0;

    if (!ReceiveXlogStream(param->bgconn, param->startptr, param->timeline,
                           (const char*)param->sysidentifier, (const char*)param->xlogdir,
                           reached_end_position, &param->endptr,
                           standby_message_timeout_local, true)) {

        /*
         * Any errors will already have been reported in the function process,
         * but we need to tell the parent that we didn't shutdown in a nice
         * way.
         */
        ret = 1;
    }
    return ret;
}

/*
 * Verify that the given directory exists and is empty. If it does not
 * exist, it is created. If it exists but is not empty, an error will
 * be give and the process ended.
 */
static void verify_dir_is_empty_or_create(char* dirname)
{
    switch (pg_check_dir(dirname)) {
        case 0:

            /*
             * Does not exist, so create
             */
            if (pg_mkdir_p(dirname, S_IRWXU) == -1) {
                fprintf(stderr, _("%s: could not create directory \"%s\": %s\n"),
                        progname, dirname, strerror(errno));
                exit(1);
            }
            return;
        case 1:

            /*
             * Exists, empty
             */
            return;
        case 2:

            /*
             * Exists, not empty
             */
            fprintf(stderr, _("%s: directory \"%s\" exists but is not empty\n"), progname,
                    dirname);
            exit(1);
        case -1:

            /*
             * Access problem
             */
            fprintf(stderr, _("%s: could not access directory \"%s\": %s\n"), progname,
                    dirname, strerror(errno));
            exit(1);
        default:
            break;
    }
}

/*
 * Receive a tar format file from the connection to the server, and write
 * the data from this file directly into a tar file. If compression is
 * enabled, the data will be compressed while written to the file.
 *
 * The file will be named base.tar[.gz] if it's for the main data directory
 * or <tablespaceoid>.tar[.gz] if it's for another tablespace.
 *
 * No attempt to inspect or validate the contents of the file is done.
 */
void BackupWorker::ReceiveTarFile(PGconn* conn, PGresult* res, int rownum)
{
#define MAX_REALPATH_LEN 4096
    char filename[MAXPGPATH];
    char* copybuf = NULL;
    FILE* tarfile = NULL;
    errno_t errorno = EOK;
    bool basetablespace = (bool)PQgetisnull(res, rownum, 0);

#ifdef HAVE_LIBZ
    gzFile ztarfile = NULL;
    int duplicatedfd = -1;
#endif

    if (basetablespace) {
        /*
         * Base tablespaces
         */
        if (strcmp(basedir, "-") == 0) {
#ifdef HAVE_LIBZ
            if (compresslevel != 0) {
                duplicatedfd = dup(fileno(stdout));
                if (duplicatedfd == -1) {
                    pg_log(stderr,
                           _("%s: could not allocate dup fd by fileno(stdout): %s\n"),
                           progname, strerror(errno));
                    exit(1);
                }

                ztarfile = gzdopen(duplicatedfd, "wb");
                if (gzsetparams(ztarfile, compresslevel, Z_DEFAULT_STRATEGY) != Z_OK) {
                    pg_log(stderr, _("%s: could not set compression level %d: %s\n"),
                           progname, compresslevel, get_gz_error(ztarfile));
                    close(duplicatedfd);
                    duplicatedfd = -1;
                    exit(1);
                }
            } else
#endif
                tarfile = stdout;

            errorno = strcpy_s(filename, MAXPGPATH, "-");
            securec_check_c(errorno, "\0", "\0");
        } else {
#ifdef HAVE_LIBZ
            if (compresslevel != 0) {
                errorno = snprintf_s(filename, sizeof(filename), sizeof(filename) - 1,
                                     "%s/base.tar.gz", nodeBaseDir.c_str());
                securec_check_ss_c(errorno, "", "");
                ztarfile = openGzFile(filename, compresslevel);
                if (ztarfile == NULL) {
                    pg_log(stderr, _("%s: could not set compression level %d: %s\n"),
                           progname, compresslevel, get_gz_error(ztarfile));
                    exit(1);
                }
            } else
#endif
            {
                errorno = snprintf_s(filename, sizeof(filename), sizeof(filename) - 1,
                                     "%s/base.tar", nodeBaseDir.c_str());
                // basdir has been realpath before
                securec_check_ss_c(errorno, "", "");
                tarfile = fopen(filename, "wb");
            }
        }
    } else {
        /*
         * Specific tablespace
         */
#ifdef HAVE_LIBZ
        if (compresslevel != 0) {
            errorno = snprintf_s(filename, sizeof(filename), sizeof(filename) - 1,
                                 "%s/%s.tar.gz", nodeBaseDir.c_str(),
                                 PQgetvalue(res, rownum, 0));
            securec_check_ss_c(errorno, "", "");
            ztarfile = openGzFile(filename, compresslevel);
        } else
#endif
        {
            errorno =
                snprintf_s(filename, sizeof(filename), sizeof(filename) - 1, "%s/%s.tar",
                           nodeBaseDir.c_str(), PQgetvalue(res, rownum, 0));
            securec_check_ss_c(errorno, "", "");
            tarfile = fopen(filename, "wb");
        }
    }

#ifdef HAVE_LIBZ
    if (compresslevel != 0) {
        if (ztarfile == NULL) {
            /* Compression is in use */
            pg_log(stderr, _("%s: could not create compressed file \"%s\": %s\n"),
                   progname, filename, get_gz_error(ztarfile));
            if (duplicatedfd != -1) {
                close(duplicatedfd);
                duplicatedfd = -1;
            }
            exit(1);
        }
    } else
#endif
    {
        /* Either no zlib support, or zlib support but compresslevel = 0 */
        if (NULL == tarfile) {
            pg_log(stderr, _("%s: could not create file \"%s\": %s\n"), progname,
                   filename, strerror(errno));
            exit(1);
        }
    }

    /*
     * Get the COPY data stream
     */
    res = PQgetResult(conn);
    if (PQresultStatus(res) != PGRES_COPY_OUT) {
        pg_log(stderr, _("%s: could not get COPY data stream: %s\n"), progname,
               PQerrorMessage(conn));
        if (tarfile != NULL) {
            fclose(tarfile);
            tarfile = NULL;
        }
        exit(1);
    }
    PQclear(res);

    while (true) {
        if (copybuf != NULL) {
            PQfreemem(copybuf);
            copybuf = NULL;
        }

        int r = PQgetCopyData(conn, &copybuf, 0);
        if (r == -1) {
#ifdef HAVE_LIBZ
            if (ztarfile != NULL) {
                if (gzclose(ztarfile) != 0) {
                    pg_log(stderr, _("%s: could not close compressed file \"%s\": %s\n"),
                           progname, filename, get_gz_error(ztarfile));
                    if (duplicatedfd != -1) {
                        close(duplicatedfd);
                        duplicatedfd = -1;
                    }
                    exit(1);
                }
            } else
#endif
            {
                if (strcmp(basedir, "-") != 0) {
                    if (fclose(tarfile) != 0) {
                        pg_log(stderr, _("%s: could not close file \"%s\": %s\n"),
                               progname, filename, strerror(errno));
                        tarfile = NULL;
                        exit(1);
                    }
                    tarfile = NULL;
                }
            }

            break;
        } else if (r == -2) {
            pg_log(stderr, _("%s: could not read COPY data: %s\n"), progname,
                   PQerrorMessage(conn));
            exit(1);
        }

#ifdef HAVE_LIBZ
        if (ztarfile != NULL) {
            if (!writeGzFile(ztarfile, copybuf, r)) {
                pg_log(stderr, _("%s: could not write to compressed file \"%s\": %s\n"),
                       progname, filename, get_gz_error(ztarfile));
                if (duplicatedfd != -1) {
                    close(duplicatedfd);
                    duplicatedfd = -1;
                }
                exit(1);
            }
        } else
#endif
        {
            if (fwrite(copybuf, r, 1, tarfile) != 1) {
                pg_log(stderr, _("%s: could not write to file \"%s\": %s\n"), progname,
                       filename, strerror(errno));
                exit(1);
            }
        }
        totaldone += r;
    } /* while (1) */

#ifdef HAVE_LIBZ
    if (duplicatedfd != -1) {
        close(duplicatedfd);
        duplicatedfd = -1;
    }
#endif
    if (copybuf != NULL) {
        PQfreemem(copybuf);
        copybuf = NULL;
    }

    if (tarfile != NULL) {
        fclose(tarfile);
        tarfile = NULL;
    }
}

/*
 * Retrieve tablespace path, either relocated or original depending on whether
 * -T was passed or not.
 */
static const char* get_tablespace_mapping(const char* dir)
{
    TablespaceListCell* cell = NULL;
    char canon_dir[MAXPGPATH];
    errno_t errorno = EOK;

    /* Canonicalize path for comparison consistency */
    errorno = strcpy_s(canon_dir, MAXPGPATH, dir);
    securec_check_c(errorno, "\0", "\0");
    canonicalize_path(canon_dir);

    for (cell = tablespacee_dirs.head; cell; cell = cell->next) {
        if (strcmp(canon_dir, cell->old_dir) == 0) {
            return cell->new_dir;
        }
    }

    return dir;
}

static PGresult* backup_get_result(PGconn* conn)
{
    PGresult* res = PQgetResult(conn);
    if (PQresultStatus(res) != PGRES_COPY_OUT) {
        pg_log(stderr, _("%s: could not get COPY data stream: %s\n"), progname,
               PQerrorMessage(conn));
        exit(1);
    }
    return res;
}

static bool check_input_path_relative_path(const char* input_path_value)
{
    if (strstr(input_path_value, "..") != NULL) {
        return true;
    }
    return false;
}

static bool IsXlogDir(const char* filename)
{
    size_t xlogDirLen = strlen("/pg_xlog");
    size_t filenameLen = strlen(filename);
    /* pg_xlog may be created in StartLogStreamer, which will be called when 'streamwal'
     * is true */
    if (filenameLen >= xlogDirLen && streamwal &&
        strcmp(filename + filenameLen - xlogDirLen, "/pg_xlog") == 0) {
        return true;
    }
    return false;
}

/*
 * Receive a tar format stream from the connection to the server, and unpack
 * the contents of it into a directory. Only files, directories and
 * symlinks are supported, no other kinds of special files.
 *
 * If the data is for the main data directory, it will be restored in the
 * specified directory. If it's for another tablespace, it will be restored
 * in the original directory, since relocation of tablespaces is not
 * supported.
 */
void BackupWorker::ReceiveAndUnpackTarFile(PGconn* conn, PGresult* res, int rownum)
{
    char current_path[MAXPGPATH] = {0};
    char filename[MAXPGPATH] = {0};
    char absolut_path[MAXPGPATH] = {0};
    uint64 current_len_left = 0;
    uint64 current_padding = 0;
    const char* mapped_tblspc_path = NULL;
    bool basetablespace = (bool)PQgetisnull(res, rownum, 0);
    char* copybuf = NULL;
    FILE* file = NULL;
    const char* get_value = NULL;

    errno_t errorno = EOK;

    if (basetablespace) {
        errorno =
            strncpy_s(current_path, MAXPGPATH, nodeBaseDir.c_str(), nodeBaseDir.length());
        securec_check_c(errorno, "", "");
        current_path[MAXPGPATH - 1] = '\0';
    } else {
        char* buffer = PQgetvalue(res, rownum, 1);
        if (buffer == NULL) {
            pg_log(PG_WARNING, _("PQgetvalue get value failed\n"));
            exit(1);
        }
        get_value = get_tablespace_mapping(buffer);
        char* relative = PQgetvalue(res, rownum, 3);
        if (*relative == '1') {
            errorno = snprintf_s(current_path, MAXPGPATH, MAXPGPATH - 1, "%s/%s",
                                 nodeBaseDir.c_str(), get_value);
            securec_check_ss_c(errorno, "\0", "\0");
        } else {
            errorno = strncpy_s(current_path, MAXPGPATH, get_value, strlen(get_value));
            securec_check_c(errorno, "\0", "\0");
        }
        current_path[MAXPGPATH - 1] = '\0';
    }

    /*
     * Get the COPY data
     */
    res = backup_get_result(conn);
    PQclear(res);

    while (1) {
        int r;

        if (copybuf != NULL) {
            PQfreemem(copybuf);
            copybuf = NULL;
        }

        r = PQgetCopyData(conn, &copybuf, 0);
        if (r == -1) {
            /*
             * End of chunk
             */
            if (file != NULL) {
                /* punch hole before closing file */
                PunchHoleForCompressedFile(file, filename);
                fclose(file);
                file = NULL;
            }

            break;
        } else if (r == TAR_READ_ERROR) {
            pg_log(stderr, _("%s: could not read COPY data: %s\n"), progname,
                   PQerrorMessage(conn));
            exit(1);
        }

        if (file == NULL) {
            /* new file */
            int filemode;

            /*
             * No current file, so this must be the header for a new file
             */
            if (r != TAR_BLOCK_SIZE) {
                pg_log(stderr, _("%s: invalid tar block header size: %d\n"), progname, r);
                exit(1);
            }
            totaldone += TAR_BLOCK_SIZE;

            if (sscanf_s(copybuf + TAR_LEN_LEFT, "%20lo", &current_len_left) != 1) {
                pg_log(stderr, _("%s: could not parse file size\n"), progname);
                exit(1);
            }

            /* Set permissions on the file */
            if (sscanf_s(&copybuf[1024], "%07o ", (unsigned int*)&filemode) != 1) {
                pg_log(stderr, _("%s: could not parse file mode\n"), progname);
                exit(1);
            }

            /*
             * All files are padded up to 512 bytes
             */
            if (current_len_left > INT_MAX - 511) {
                pg_log(stderr,
                       _("%s: the file '%s' is too big or file size is invalid\n"),
                       progname, copybuf);
                exit(1);
            }
            current_padding = PADDING_LEFT(current_len_left);

            /*
             * First part of header is zero terminated filename
             */
            if (check_input_path_relative_path(copybuf) ||
                check_input_path_relative_path(current_path)) {
                pg_log(stderr,
                       _("%s: the copybuf/current_path file path including .. is "
                         "unallowed: %s\n"),
                       progname, strerror(errno));
                exit(1);
            }
            errorno = snprintf_s(filename, sizeof(filename), sizeof(filename) - 1,
                                 "%s/%s", current_path, copybuf);
            securec_check_ss_c(errorno, "", "");

            if (filename[strlen(filename) - 1] == '/') {
                /*
                 * Ends in a slash means directory or symlink to directory
                 */
                if (strstr(filename, "../") != NULL) {
                    pg_log(stderr, _("%s: path is illegal \n"), filename);
                    exit(1);
                }
                if (copybuf[TAR_FILE_TYPE] == TAR_TYPE_DICTORY) {
                    /*
                     * Directory
                     */
                    filename[strlen(filename) - 1] = '\0'; /* Remove trailing slash */
                    if (mkdir(filename, S_IRWXU) != 0) {
                        /*
                         * When streaming WAL, pg_xlog will have been created
                         * by the wal receiver process. So just ignore creation
                         * failures on related directories.
                         */
                        if (!((pg_str_endswith(filename, "/pg_xlog") ||
                               pg_str_endswith(filename, "/archive_status")) &&
                              errno == EEXIST)) {
                            pg_log(stderr,
                                   _("%s: could not create directory \"%s\": %s\n"),
                                   progname, filename, strerror(errno));
                            exit(1);
                        }
                    }
#ifndef WIN32
                    if (chmod(filename, (mode_t)filemode))
                        pg_log(
                            stderr,
                            _("%s: could not set permissions on directory \"%s\": %s\n"),
                            progname, filename, strerror(errno));
#endif
                } else if (copybuf[TAR_FILE_TYPE] == '2') {
                    /*
                     * Symbolic link
                     */
                    filename[strlen(filename) - 1] = '\0'; /* Remove trailing slash */
                    mapped_tblspc_path =
                        get_tablespace_mapping(&copybuf[TAR_FILE_TYPE + 1]);
                    if (symlink(mapped_tblspc_path, filename) != 0) {
                        if (IsXlogDir(filename)) {
                            pg_log(
                                stderr,
                                _("WARNING: could not create symbolic link for pg_xlog,"
                                  " will backup data to \"%s\" directly\n"),
                                filename);
                        } else {
                            pg_log(stderr,
                                   _("%s: could not create symbolic link from \"%s\" to "
                                     "\"%s\": %s\n"),
                                   progname, filename, mapped_tblspc_path,
                                   strerror(errno));
                            exit(1);
                        }
                    }
                } else if (copybuf[1080] == '3') {
                    /*
                     * Symbolic link for relative tablespace. please refer to function
                     * _tarWriteHeader
                     */
                    filename[strlen(filename) - 1] = '\0'; /* Remove trailing slash */

                    errorno = snprintf_s(
                        absolut_path, sizeof(absolut_path), sizeof(absolut_path) - 1,
                        "%s/%s", nodeBaseDir.c_str(), &copybuf[TAR_FILE_TYPE + 1]);
                    securec_check_ss_c(errorno, "\0", "\0");

                    if (symlink(absolut_path, filename) != 0) {
                        if (!IsXlogDir(filename)) {
                            pg_log(PG_WARNING,
                                   _("could not create symbolic link from \"%s\" to "
                                     "\"%s\": %s\n"),
                                   filename, &copybuf[1081], strerror(errno));
                            exit(1);
                        }
                    }
                } else {
                    pg_log(PG_WARNING, _("unrecognized link indicator \"%c\"\n"),
                           copybuf[1080]);
                    exit(1);
                }
                continue; /* directory or link handled */
            }

            canonicalize_path(filename);
            /*
             * regular file
             */
            file = fopen(filename,
                         IsCompressedFile(filename, strlen(filename)) ? "wb+" : "wb");
            if (NULL == file) {
                pg_log(stderr, _("%s: could not create file \"%s\": %s\n"), progname,
                       filename, strerror(errno));
                exit(1);
            }

#ifndef WIN32
            if (chmod(filename, (mode_t)filemode))
                pg_log(stderr, _("%s: could not set permissions on file \"%s\": %s\n"),
                       progname, filename, strerror(errno));
#endif

            if (current_len_left == 0) {
                /*
                 * Done with this file, next one will be a new tar header
                 */
                fclose(file);
                file = NULL;
                continue;
            }
        } else {
            /*
             * Continuing blocks in existing file
             */
            if (current_len_left == 0 && (uint64)r == current_padding) {
                /*
                 * Received the padding block for this file, ignore it and
                 * close the file, then move on to the next tar header.
                 */
                fclose(file);
                file = NULL;
                totaldone += (uint64)r;
                continue;
            }

            if (fwrite(copybuf, r, 1, file) != 1) {
                pg_log(stderr, _("%s: could not write to file \"%s\": %s\n"), progname,
                       filename, strerror(errno));
                fclose(file);
                file = NULL;
                exit(1);
            }
            totaldone += r;

            current_len_left -= r;
            if (current_len_left == 0 && current_padding == 0) {
                /*
                 * Received the last block, and there is no padding to be
                 * expected. Close the file and move on to the next tar
                 * header.
                 */

                /* punch hole before closing file */
                PunchHoleForCompressedFile(file, filename);
                fclose(file);
                file = NULL;
                continue;
            }
        } /* continuing data in existing file */
    }     /* loop over all data blocks */

    if (file != NULL) {
        pg_log(stderr, _("%s: COPY stream ended before last file was finished\n"),
               progname);
        fclose(file);
        file = NULL;
        exit(1);
    }

    if (copybuf != NULL) {
        PQfreemem(copybuf);
        copybuf = NULL;
    }
}

static void remove_dw_file(const char* dw_file_name, const char* target_dir,
                           char* real_file_path)
{
    char dw_file_path[MAXPGPATH];

    int rc = snprintf_s(dw_file_path, MAXPGPATH, MAXPGPATH - 1, "%s/%s", target_dir,
                        dw_file_name);
    securec_check_ss_c(rc, "\0", "\0");

    if (realpath(dw_file_path, real_file_path) == NULL) {
        if (real_file_path[0] == '\0') {
            pg_log(stderr, _("could not get canonical path for file %s: %s\n"),
                   dw_file_path, gs_strerror(errno));
            exit(1);
        }
    }

    (void)unlink(real_file_path);
}

/* *
 * * delete existing double write file if existed, recreate it and write one page of zero
 * * @param target_dir data base root dir
 *     */
static void backup_dw_file(const char* target_dir)
{
    int rc;
    int fd = -1;
    char real_file_path[PATH_MAX + 1] = {0};
    char* buf = NULL;
    char* unaligned_buf = NULL;

    /* Delete the dw file, if it exists. */
    remove_dw_file(T_OLD_DW_FILE_NAME, target_dir, real_file_path);

    rc = memset_s(real_file_path, (PATH_MAX + 1), 0, (PATH_MAX + 1));
    securec_check_c(rc, "\0", "\0");

    /* Delete the dw build file, if it exists. */
    remove_dw_file(T_DW_BUILD_FILE_NAME, target_dir, real_file_path);

    /* Create the dw build file. */
    if ((fd = open(real_file_path, (DW_FILE_FLAG | O_CREAT), DW_FILE_PERM)) < 0) {
        pg_log(stderr, _("could not open file %s: %s\n"), real_file_path,
               gs_strerror(errno));
        exit(1);
    }

    unaligned_buf = (char*)malloc(BLCKSZ + BLCKSZ);
    if (unaligned_buf == NULL) {
        pg_log(stderr, _("out of memory"));
        close(fd);
        exit(1);
    }

    buf = (char*)TYPEALIGN(BLCKSZ, unaligned_buf);
    rc = memset_s(buf, BLCKSZ, 0, BLCKSZ);
    securec_check_c(rc, "\0", "\0");

    if (write(fd, buf, BLCKSZ) != BLCKSZ) {
        pg_log(stderr, _("could not write data to file %s: %s\n"), real_file_path,
               gs_strerror(errno));
        free(unaligned_buf);
        close(fd);
        exit(1);
    }

    free(unaligned_buf);
    close(fd);
}

static int GsBaseBackup(int argc, char** argv)
{
    static struct option long_options[] = {{"help", no_argument, NULL, '?'},
                                           {"version", no_argument, NULL, 'V'},
                                           {"pgdata", required_argument, NULL, 'D'},
                                           {"checkpoint", required_argument, NULL, 'c'},
                                           {"label", required_argument, NULL, 'l'},
                                           {"host", required_argument, NULL, 'h'},
                                           {"port", required_argument, NULL, 'p'},
                                           {"username", required_argument, NULL, 'U'},
                                           {"no-password", no_argument, NULL, 'w'},
                                           {"password", required_argument, NULL, 'W'},
                                           {"rw-timeout", required_argument, NULL, 't'},
                                           {NULL, 0, NULL, 0}};
    int c = 0, option_index = 0;
    progname = "spq_backup";
    set_pglocale_pgservice(argv[0], PG_TEXTDOMAIN("spq_backup"));

    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-?") == 0) {
            usage();
            exit(0);
        } else if (strcmp(argv[1], "-V") == 0 || strcmp(argv[1], "--version") == 0) {
            puts("spq_backup " DEF_GS_VERSION);
            exit(0);
        }
    }

    char optstring[] = "D:l:c:h:p:U:t:wW:";
    /* check if a required_argument option has a void argument */
    int i;
    for (i = 0; i < argc; i++) {
        char* optstr = argv[i];
        if (strlen(optstr) == 1 && optstr[0] == '-') {
            /* ignore the case of redirecting output like "gs_probackup ... -D ->
             * xxx.tar.gz" */
            if (i > 0 && strcmp(argv[i - 1], "-D") == 0) {
                continue;
            }
            fprintf(stderr, _("%s: The option '-' is not a valid option.\n"), progname);
            exit(1);
        }

        char* oli = strchr(optstring, optstr[1]);
        int is_shortopt_with_space;
        if (oli != NULL && strlen(optstr) >= 1 && strlen(oli) >= 2) {
            is_shortopt_with_space = optstr[0] == '-' && oli != NULL && oli[1] == ':' &&
                                     oli[2] != ':' && optstr[2] == '\0';
        } else {
            is_shortopt_with_space = 0;
        }
        if (is_shortopt_with_space) {
            if (i == argc - 1) {
                fprintf(stderr, _("%s: The option '-%c' need a parameter.\n"), progname,
                        optstr[1]);
                exit(1);
            }

            char* next_optstr = argv[i + 1];
            if (strcmp(optstr, "-D") == 0 && strcmp(next_optstr, "-") == 0) {
                continue;
            }
            char* next_oli = strchr(optstring, next_optstr[1]);
            if (next_optstr[0] == '-' && next_oli != NULL) {
                fprintf(stderr, _("%s: The option '-%c' need a parameter.\n"), progname,
                        optstr[1]);
                exit(1);
            }
        }
    }

    while ((c = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1) {
        switch (c) {
            case 'D': {
                GS_FREE(basedir);
                // check_env_value_c(optarg);
                char realDir[PATH_MAX] = {0};
                bool argIsMinus = (strcmp(optarg, "-") == 0);
                if (!argIsMinus && realpath(optarg, realDir) == nullptr) {
                    pg_log(stderr, _("%s: realpath dir \"%s\" failed: %m\n"), progname,
                           optarg);
                    exit(1);
                }
                if (argIsMinus) {
                    basedir = xstrdup(optarg);
                } else {
                    basedir = xstrdup(realDir);
                }
                break;
            }
            case 'T':
                if (strlen(optarg) >= MAXPGPATH - 1) {
                    fprintf(stderr, _("%s: invalid output length\n"), progname);
                    exit(1);
                }
                tablespace_list_append(optarg);
                break;
            case 'X':
                // check_env_value_c(optarg);
                if (strcmp(optarg, "f") == 0 || strcmp(optarg, "fetch") == 0)
                    streamwal = false;
                else if (strcmp(optarg, "s") == 0 || strcmp(optarg, "stream") == 0)
                    streamwal = true;
                else {
                    fprintf(stderr,
                            _("%s: invalid xlog-method option \"%s\", must be \"fetch\" "
                              "or \"stream\"\n"),
                            progname, optarg);
                    exit(1);
                }
                break;
            case 'l':
                if (label != NULL && strcmp(label, "spq_backup backup") != 0) {
                    GS_FREE(label);
                }
                // check_env_value_c(optarg);
                label = xstrdup(optarg);
                break;
            case 'c':
                if (pg_strcasecmp(optarg, "fast") == 0)
                    fastcheckpoint = true;
                else if (pg_strcasecmp(optarg, "spread") == 0)
                    fastcheckpoint = false;
                else {
                    fprintf(stderr,
                            _("%s: invalid checkpoint argument \"%s\", must be \"fast\" "
                              "or \"spread\"\n"),
                            progname, optarg);
                    exit(1);
                }
                break;
            case 'h':
                GS_FREE(dbhost);
                // check_env_value_c(optarg);
                dbhost = xstrdup(optarg);
                break;
            case 'p':
                GS_FREE(dbport);
                // check_env_value_c(optarg);
                isPortNumber(optarg);
                // dbport = inc_dbport(optarg);
                dbport = xstrdup(optarg);
                break;
            case 'U':
                GS_FREE(dbuser);
                // check_env_name_c(optarg);
                dbuser = xstrdup(optarg);
                break;
            case 'w':
                dbgetpassword = -1;
                break;
            case 'W':
                dbpassword = strdup(optarg);
                if (NULL == dbpassword) {
                    fprintf(stderr, _("%s: out of memory\n"), progname);
                    exit(1);
                }
                replace_password(argc, argv, "-W");
                break;
            case 't':
                if (atoi(optarg) < 0 || atoi(optarg) > PG_INT32_MAX) {
                    fprintf(stderr, _("%s: invalid read-write timeout \"%s\"\n"),
                            progname, optarg);
                    exit(1);
                }
                rwtimeout = atoi(optarg);
                break;
            default:

                /*
                 * getopt_long already emitted a complaint
                 */
                fprintf(stderr, _("Try \"%s --help\" for more information.\n"), progname);
                exit(1);
                break;
        }
    }

    /* If port is not specified by using -p, obtain the port through environment variables
     */
    if (dbport == NULL) {
        char* value = NULL;
        value = gs_getenv_r("PGPORT");
        if (value != NULL) {
            // check_env_value_c(value);
            // dbport = inc_dbport(value);
            dbport = xstrdup(value);
        } else {
            fprintf(
                stderr,
                _("%s:The specified port is missing, it can be specified by -p parameter "
                  "or import environment variables PGPORT.\n"),
                progname);
            fprintf(stderr, _("Try \"%s --help\" for more information.\n"), progname);
            exit(1);
        }
    }

    /*
     * Any non-option arguments?
     */
    if (optind < argc) {
        fprintf(stderr, _("%s: too many command-line arguments (first is \"%s\")\n"),
                progname, argv[optind]);
        fprintf(stderr, _("Try \"%s --help\" for more information.\n"), progname);
        exit(1);
    }

    /*
     * Required arguments
     */
    if (basedir == NULL) {
        fprintf(stderr, _("%s: no target directory specified\n"), progname);
        fprintf(stderr, _("Try \"%s --help\" for more information.\n"), progname);
        exit(1);
    }

    /*
     * Mutually exclusive arguments
     */
    if (format == 'p' && compresslevel != 0) {
        fprintf(stderr, _("%s: only tar mode backups can be compressed\n"), progname);
        fprintf(stderr, _("Try \"%s --help\" for more information.\n"), progname);
        exit(1);
    }

    if (format != 'p' && streamwal) {
        fprintf(stderr, _("%s: wal streaming can only be used in plain mode\n"),
                progname);
        fprintf(stderr, _("Try \"%s --help\" for more information.\n"), progname);
        exit(1);
    }

#ifndef HAVE_LIBZ
    if (compresslevel != 0) {
        fprintf(stderr, _("%s: this build does not support compression\n"), progname);
        exit(1);
    }
#endif

    /*
     * Verify that the target directory exists, or create it. For plaintext
     * backups, always require the directory. For tar backups, require it
     * unless we are writing to stdout.
     */
    if (format == 'p' || strcmp(basedir, "-") != 0)
        verify_dir_is_empty_or_create(basedir);

    BaseBackupMain();

    free_basebackup();

    pg_log(stderr, _("%s: base backup successfully\n"), progname);

    return 0;
}

static void free_basebackup()
{
    GS_FREE(basedir);
    if (label != NULL && strcmp(label, "spq_backup backup") != 0) {
        GS_FREE(label);
    }
    GS_FREE(dbhost);
    GS_FREE(dbport);
    GS_FREE(dbuser);
}

void CalculateArgCount(int* argcount);
extern const int PASSWDLEN = 100;
void CheckConnectionHost(PGconn* tmpconn);
/*
 * Connect to the server. Returns a valid PGconn pointer if connected,
 * or NULL on non-permanent error. On permanent error, the function will
 * call exit(1) directly. You should call ClearAndFreePasswd() outside
 * if password it is not needed anymore.
 */
PGconn* BackupWorker::GetConnection()
{
    PGconn* tmpconn = NULL;
    int argcount = 6; /* dbname, fallback_app_name, connect_time, rw_timeout, pwd */
    const char** keywords;
    const char** values;
    const char* tmpparam = NULL;
    errno_t rc = EOK;
    char rwtimeoutStr[12] = {0};

    CalculateArgCount(&argcount);

    keywords = (const char**)xmalloc0((argcount + 1) * sizeof(*keywords));
    values = (const char**)xmalloc0((argcount + 1) * sizeof(*values));

    keywords[0] = "dbname";
    values[0] = dbname == nullptr ? "replication" : dbname;
    keywords[1] = "replication";
    values[1] = dbname == nullptr ? "true" : "database";
    keywords[2] = "fallback_application_name";
    values[2] = "gs_basebackup";
    keywords[3] = "connect_timeout"; /* param connect_time   */
    values[3] = "120";               /* default connect_time */

    rc = snprintf_s(rwtimeoutStr, sizeof(rwtimeoutStr), sizeof(rwtimeoutStr) - 1, "%d",
                    rwtimeout ? rwtimeout : 120); /* default rw_timeout 120 */
    securec_check_ss_c(rc, "", "");

    keywords[4] = "rw_timeout"; /* param rw_timeout     */
    values[4] = rwtimeoutStr;   /* rw_timeout value     */
    int i = 5;
    if (!nodeInfo.dbhost.empty()) {
        keywords[i] = "host";
        values[i] = nodeInfo.dbhost.c_str();
        i++;
    }
    if (dbuser != nullptr) {
        keywords[i] = "user";
        values[i] = dbuser;
        i++;
    }
    if (!nodeInfo.dbport.empty()) {
        keywords[i] = "port";
        values[i] = nodeInfo.dbport.c_str();  // inc_dbport(dbport);
        i++;
    }

    while (true) {
        if (dbpassword != NULL) {
            /*
             * We've saved a pwd when a previous connection succeeded,
             * meaning this is the call for a second session to the same
             * database, so just forcibly reuse that pwd.
             */
            keywords[argcount - 1] = "password";
            values[argcount - 1] = dbpassword;
            dbgetpassword = -1; /* Don't try again if this fails */
        } else if (dbgetpassword == 1) {
            dbpassword = simple_prompt(_("Password: "), PASSWDLEN, false);
            keywords[argcount - 1] = "password";
            values[argcount - 1] = dbpassword;
            dbgetpassword = -1;
        }

        tmpconn = PQconnectdbParams(keywords, values, true);
        /*
         * If there is too little memory even to allocate the PGconn object
         * and PQconnectdbParams returns NULL, we call exit(1) directly.
         */
        CheckConnectionHost(tmpconn);
        if (PQstatus(tmpconn) == CONNECTION_BAD && PQconnectionNeedsPassword(tmpconn) &&
            dbgetpassword != -1) {
            dbgetpassword = 1; /* ask for password next time */
            ClearAndFreePasswd();
            PQfinish(tmpconn);
            tmpconn = NULL;
            continue;
        }

        if (PQstatus(tmpconn) != CONNECTION_OK) {
            fprintf(stderr, _("%s: could not connect to server: %s\n"), progname,
                    PQerrorMessage(tmpconn));
            PQfinish(tmpconn);
            tmpconn = NULL;
            ClearAndFreePasswd();
            free(values);
            values = NULL;
            free(keywords);
            keywords = NULL;
            return NULL;
        }

        free(values);
        values = NULL;
        free(keywords);
        keywords = NULL;

        /*
         * Ensure we have the same value of integer timestamps as the server
         * we are connecting to.
         */
        tmpparam = PQparameterStatus(tmpconn, "integer_datetimes");
        if (tmpparam == NULL) {
            fprintf(stderr,
                    _("%s: could not determine server setting for integer_datetimes\n"),
                    progname);
            PQfinish(tmpconn);
            tmpconn = NULL;
            exit(1);
        }

#ifdef HAVE_INT64_TIMESTAMP
        if (strcmp(tmpparam, "on") != 0)
#else
        if (strcmp(tmpparam, "off") != 0)
#endif
        {
            fprintf(stderr,
                    _("%s: integer_datetimes compile flag does not match server\n"),
                    progname);
            PQfinish(tmpconn);
            tmpconn = NULL;
            exit(1);
        }
        return tmpconn;
    }
}

void BackupWorker::removeCreatedTblspace()
{
    if (tblspaceDirectory != NULL) {
        int k;
        for (k = 0; k < tblspaceCount; k++) {
            if (tblspaceDirectory[k] != NULL) {
                rmtree(tblspaceDirectory[k], true);
            }
        }
    }
}

}  // namespace Spq

int main(int argc, char** argv)
{
    return Spq::GsBaseBackup(argc, argv);
}
