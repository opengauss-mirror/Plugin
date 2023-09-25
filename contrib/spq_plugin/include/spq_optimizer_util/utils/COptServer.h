//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		COptServer.h
//
//	@doc:
//		API for optimizer server
//
//	@test:
//
//
//---------------------------------------------------------------------------
#ifndef COptServer_H
#define COptServer_H

#include "spqos/base.h"
#include "spqos/common/CSyncHashtable.h"
#include "spqos/common/CSyncHashtableAccessByIter.h"
#include "spqos/common/CSyncHashtableAccessByKey.h"
#include "spqos/common/CSyncHashtableIter.h"
#include "spqos/net/CSocket.h"
#include "spqos/task/CTask.h"

#include "naucrates/md/CSystemId.h"

// forward declarations
namespace spqopt
{
class CExpression;
class CMDAccessor;
class CMiniDumperDXL;
class CQueryContext;
}  // namespace spqopt

namespace spqnaucrates
{
class CCommunicator;
}


namespace spqoptudfs
{
using namespace spqos;
using namespace spqopt;
using namespace spqnaucrates;

//---------------------------------------------------------------------------
//	@class:
//		COptServer
//
//	@doc:
//		Optimizer server; processes optimization requests from QDs;
//
//---------------------------------------------------------------------------
class COptServer
{
private:
	// connection descriptor
	struct SConnectionDescriptor
	{
		// ID
		ULONG_PTR m_id;

		// task
		CTask *m_task;

		// socket
		CSocket *m_socket;

		// link for hashtable
		SLink m_link;

		// invalid connection id
		static ULONG_PTR m_invalid_id;

		// ctor
		SConnectionDescriptor(CTask *task, CSocket *socket)
			: m_id((ULONG_PTR) task), m_task(task), m_socket(socket)
		{
		}
	};

	typedef CSyncHashtable<SConnectionDescriptor, ULONG_PTR> ConnectionHT;

	// path where socket is initialized
	const CHAR *m_socket_path;

	// memory pool for connections
	CMemoryPool *m_mp;

	// hashtable of connections
	ConnectionHT *m_connections_ht;

	// default id for the source system
	static const CSystemId m_default_id;

	// ctor
	explicit COptServer(const CHAR *path);

	// dtor
	~COptServer();

	// start serving requests
	void Loop();

	// initialize hashtable
	void InitHT();

	// register connection for status checking
	void TrackConnection(CTask *task, CSocket *socket);

	// release connection
	void ReleaseConnection(CTask *task);

	// connection check task
	static void *CheckConnections(void *ptr);

	// optimization task
	static void *Optimize(void *ptr);

	// receive optimization request and construct query context for it
	static CQueryContext *RecvQuery(CMemoryPool *mp,
									CCommunicator *communicator,
									CMDAccessor *md_accessor);

	// extract query plan, serialize it and send it to client
	static void SendPlan(CMemoryPool *mp, CCommunicator *communicator,
						 CMDAccessor *md_accessor, CQueryContext *query_ctxt,
						 CExpression *plan_expr);

	// dump collected artifacts to file
	static void FinalizeMinidump(CMiniDumperDXL *dump);

public:
	// invoke optimizer instance
	static void *Run(void *ptr);

};	// class COptServer
}  // namespace spqoptudfs

#endif	// !COptServer_H


// EOF
