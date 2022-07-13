; ModuleID = 'openGauss_expr_dolphin.cpp'
source_filename = "openGauss_expr_dolphin.cpp"
target datalayout = "e-m:e-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128"
target triple = "aarch64-unknown-linux-gnu"

%struct.FuncExprState = type { %struct.ExprState, %struct.List*, i8, %struct.FmgrInfo, %struct.Tuplestorestate*, %struct.TupleTableSlot*, %struct.tupleDesc*, i8, i8, i8, i8, %struct.FunctionCallInfoData, %class.ScalarVector* }
%struct.ExprState = type <{ i32, [4 x i8], %struct.Expr*, i64 (%struct.ExprState*, %struct.ExprContext*, i8*, i32*)*, %class.ScalarVector* (%struct.ExprState*, %struct.ExprContext*, i8*, %class.ScalarVector*, i32*)*, i8* (i8*)*, %class.ScalarVector, i32, [4 x i8] }>
%struct.Expr = type { i32, double }
%class.ScalarVector = type { i32, %struct.ScalarDesc, i8, i8*, %class.VarBuf*, i64*, { i64, i64 } }
%struct.ScalarDesc = type <{ i32, i32, i8, [3 x i8] }>
%class.VarBuf = type { %struct.varBuf*, %struct.varBuf*, %struct.MemoryContextData*, i32, i32 }
%struct.varBuf = type { i8*, i32, i32, %struct.varBuf* }
%struct.MemoryContextData = type { i32, i8, %struct.MemoryContextMethods*, %struct.MemoryContextData*, %struct.MemoryContextData*, %struct.MemoryContextData*, %struct.MemoryContextData*, i8*, %union.pthread_rwlock_t, i8, i8, i8, i32, i64, i64, %struct.ListCell }
%struct.MemoryContextMethods = type { i8* (%struct.MemoryContextData*, i64, i64, i8*, i32)*, void (%struct.MemoryContextData*, i8*)*, i8* (%struct.MemoryContextData*, i8*, i64, i64, i8*, i32)*, void (%struct.MemoryContextData*)*, void (%struct.MemoryContextData*)*, void (%struct.MemoryContextData*)*, i64 (%struct.MemoryContextData*, i8*)*, i1 (%struct.MemoryContextData*)*, void (%struct.MemoryContextData*, i32)*, void (%struct.MemoryContextData*)* }
%union.pthread_rwlock_t = type { %struct.__pthread_rwlock_arch_t }
%struct.__pthread_rwlock_arch_t = type { i32, i32, i32, i32, i32, i32, i32, i32, i64, i64, i32 }
%struct.ListCell = type { %union.anon, %struct.ListCell* }
%union.anon = type { i8* }
%struct.List = type { i32, i32, %struct.ListCell*, %struct.ListCell* }
%struct.FmgrInfo = type { i64 (%struct.FunctionCallInfoData*)*, i32, i16, i8, i8, i8, i8*, %struct.MemoryContextData*, %struct.Node*, i32, i32, [64 x i8], i8*, i8, i32, i8, %class.ScalarVector* (%struct.FunctionCallInfoData*)*, %class.ScalarVector* (%struct.FunctionCallInfoData*)**, %struct.GenericFunRuntime* }
%struct.Node = type { i32 }
%struct.GenericFunRuntime = type { i16, %struct.GenericFunRuntimeArg*, i64*, i8*, [1000 x i8], %struct.FunctionCallInfoData* }
%struct.GenericFunRuntimeArg = type { %class.ScalarVector**, i32, i64 (i64*)* }
%struct.Tuplestorestate = type opaque
%struct.TupleTableSlot = type <{ i32, i8, i8, i8, i8, i8*, i8*, i32, i8, [3 x i8], %struct.AttInMetadata*, i32, [4 x i8], %struct.MemoryContextData*, %struct.tupleDesc*, %struct.MemoryContextData*, i32, i32, i64*, i8*, i64*, %struct.MinimalTupleData*, %struct.HeapTupleData, i64, i64, i32, [4 x i8] }>
%struct.AttInMetadata = type opaque
%struct.MinimalTupleData = type { i32, [6 x i8], i16, i16, i8, [0 x i8] }
%struct.HeapTupleData = type { i32, i8, i8, i16, %struct.ItemPointerData, i32, i64, i64, i32, %struct.HeapTupleHeaderData* }
%struct.ItemPointerData = type { %struct.BlockIdData, i16 }
%struct.BlockIdData = type { i16, i16 }
%struct.HeapTupleHeaderData = type { %union.anon.0, %struct.ItemPointerData, i16, i16, i8, [0 x i8] }
%union.anon.0 = type { %struct.HeapTupleFields }
%struct.HeapTupleFields = type { i32, i32, %union.anon.1 }
%union.anon.1 = type { i32 }
%struct.tupleDesc = type { i32, i32, i8, %struct.FormData_pg_attribute**, %struct.tupleConstr*, %struct.tupInitDefVal*, i32, i32, i8, i32, i8 }
%struct.FormData_pg_attribute = type { i32, %struct.nameData, i32, i32, i16, i16, i32, i32, i32, i8, i8, i8, i8, i8, i8, i8, i8, i32, i32, i8 }
%struct.nameData = type { [64 x i8] }
%struct.tupleConstr = type { %struct.attrDefault*, %struct.constrCheck*, i16*, i16, i16, i16, i8, i8, i8* }
%struct.attrDefault = type { i16, i8*, i8 }
%struct.constrCheck = type { i8*, i8*, i8, i8 }
%struct.tupInitDefVal = type { i64*, i8, i16 }
%struct.FunctionCallInfoData = type { %struct.FmgrInfo*, %struct.Node*, %struct.Node*, i32, i8, i8, i16, i64*, i8*, i32*, [20 x i64], [20 x i8], [20 x i32], %class.ScalarVector*, %struct.RefcusorInfoData, %struct.UDFInfoType, %struct.StartWithFuncEvalInfo }
%struct.RefcusorInfoData = type { %struct.Cursor_Data*, %struct.Cursor_Data*, i32 }
%struct.Cursor_Data = type { i32, i32, i8, i8, i8, i8, i8 }
%struct.UDFInfoType = type <{ i64 (%struct.FunctionCallInfoData*, i32, i64)**, i64 (%struct.FunctionCallInfoData*, i32, i64)*, %struct.StringInfoData*, i8*, i32, i32, i64**, i8**, i64*, i8*, i8, [7 x i8] }>
%struct.StringInfoData = type { i8*, i32, i32, i32 }
%struct.StartWithFuncEvalInfo = type { %struct.Node*, %struct.Node* }
%class.HashJoinTbl = type { %class.hashBasedOperator.base, i32, i32*, i32*, i32*, %struct.List*, i32, i8, %class.ScalarVector*, i8, i8, i8, %class.VectorBatch*, %class.VectorBatch*, %class.VectorBatch*, %class.VectorBatch*, %class.VectorBatch*, %class.VectorBatch*, %class.VectorBatch*, %class.VectorBatch*, %struct.VecHashJoinState*, i32, i32, %struct.JoinStateLog, %class.hashOpSource*, [1000 x %struct.ReCheckCellLoc], [1000 x i8], i32, [1000 x i8], %class.hashFileSource*, %class.hashFileSource*, i8*, i32*, i8*, i8, i8*, %struct.hashCell**, i8, double, double, [2 x { i64, i64 }], [2 x { i64, i64 }], { i64, i64 }, [36 x { i64, i64 }], { i64, i64 }* }
%class.hashBasedOperator.base = type <{ i32 (...)**, %class.vechashtable*, %struct.MemoryContextData*, %struct.MemoryContextData*, [1000 x i64], [1000 x %struct.hashCell*], [1000 x i8], %struct.FmgrInfo*, %struct.FmgrInfo*, %struct.FmgrInfo*, %class.hashFileSource*, %class.hashFileSource*, i8, [3 x i8], i32, i64, i64, i64, i32, i32, i32*, i32*, %struct.ScalarDesc*, %struct.ScalarDesc*, i8, [3 x i8], i32, i32, [4 x i8], i64, i64, i64, i32, i8 }>
%class.vechashtable = type { i32, %struct.hashCell** }
%struct.hashCell = type { %union.anon.2, [0 x %struct.hashVal] }
%union.anon.2 = type { %struct.hashCell* }
%struct.hashVal = type { i64, i8 }
%class.VectorBatch = type { i32, i32, i8, i8*, %class.ScalarVector*, %struct.SysColContainer*, %struct.StringInfoData* }
%struct.SysColContainer = type <{ i32, [4 x i8], %class.ScalarVector*, [9 x i8], [7 x i8] }>
%struct.VecHashJoinState = type { %struct.HashJoinState, i32, i8*, %struct.FmgrInfo*, %class.ScalarVector* (%struct.ExprContext*)*, %class.ScalarVector* (%struct.ExprContext*)*, i8*, i8*, i8*, i8*, i32, %struct.BloomFilterRuntime, i8*, i8*, i8* }
%struct.HashJoinState = type { %struct.JoinState, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.HashJoinTableData*, i32, i32, i32, %struct.HashJoinTupleData*, %struct.HashJoinTupleData*, %struct.TupleTableSlot*, %struct.TupleTableSlot*, %struct.TupleTableSlot*, %struct.TupleTableSlot*, %struct.TupleTableSlot*, i32, i8, i8, i8, i8 }
%struct.JoinState = type { %struct.PlanState, i32, %struct.List*, %struct.List* }
%struct.PlanState = type { i32, %struct.Plan*, %struct.EState*, %struct.Instrumentation*, %struct.List*, %struct.List*, %struct.PlanState*, %struct.PlanState*, %struct.List*, %struct.List*, %struct.Bitmapset*, %struct.HbktScanSlot, %struct.TupleTableSlot*, %struct.ExprContext*, %struct.ProjectionInfo*, i8, i8, %struct.MemoryContextData*, i8, i8, i1 (%struct.ExprContext*, %class.VectorBatch*)*, %struct.List*, i8, i8, i8, i64 }
%struct.Plan = type { i32, i32, i32, i32, double, double, double, double, i32, i32, double, double, double, i64, i32, i8, i32, i8, %struct.List*, %struct.List*, %struct.Plan*, %struct.Plan*, i8, i32, i32, %struct.List*, %struct.List*, %struct.ExecNodes*, %struct.Bitmapset*, %struct.Bitmapset*, i8, i8, i8, [2 x i32], i32, i8, i8, %struct.List*, %struct.List*, i32**, i32, double, double, %struct.List* }
%struct.ExecNodes = type { i32, %struct.List*, %struct.List*, %struct.Distribution, i8, %struct.List*, i32, i32, i8, i32, %struct.ExecBoundary*, i32, %struct.List*, i32, i8, %struct.List*, %struct.List*, i32, %struct.List*, i32, %struct.List* }
%struct.Distribution = type { i32, %struct.Bitmapset* }
%struct.ExecBoundary = type { i32, i8, i32, %struct.SliceBoundary** }
%struct.SliceBoundary = type { i32, i32, i32, [4 x %struct.Const*] }
%struct.Const = type { %struct.Expr, i32, i32, i32, i32, i64, i8, i8, i32, i8, %struct.Cursor_Data }
%struct.EState = type { i32, i32, %struct.SnapshotData*, %struct.SnapshotData*, %struct.List*, %struct.PlannedStmt*, %struct.JunkFilter*, i32, %struct.ResultRelInfo*, i32, %struct.ResultRelInfo*, %struct.RelationData*, %struct.HTAB*, %struct.PlanState*, %struct.PlanState*, %struct.PlanState*, %struct.PlanState*, %struct.List*, %struct.TupleTableSlot*, %struct.TupleTableSlot*, %struct.TupleTableSlot*, %struct.ParamListInfoData*, %struct.ParamExecData*, %struct.MemoryContextData*, %struct.MemoryContextData*, %struct.List*, %struct.List*, %struct.List*, i64, i64, i64, i32, i32, i32, i8, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.ExprContext*, i8**, %struct.TupleTableSlot**, i8*, i8*, %struct.List*, i8, i8, i8, %struct.List*, i8, i32, %struct.BloomFilterControl, i8, i8, i8, %"struct.JitExec::JitContext"*, %struct.PruningResult* }
%struct.SnapshotData = type { i32, i64, i64, i64, i64*, i64*, i32, i32, i32, i32, i32, i8, i64, i32, i32, i64*, i8, i8, i32, i32, i32, i8*, i32 }
%struct.PlannedStmt = type { i32, i32, i64, i8, i8, i8, i8, i8, i8, %struct.Plan*, %struct.List*, %struct.List*, %struct.Node*, %struct.List*, %struct.Bitmapset*, %struct.List*, %struct.List*, %struct.List*, i32, i32, i32, i32, i32, %struct.NodeDefinition*, i32, i32, [2 x i32], [2 x i32], i8, i32, i32, i32, i32, double, i32, i32, i32, i32, i32, [20 x i16*], [20 x i32], i8*, %struct.List*, %struct.List*, i32, i32, i32, double, i8, i8, %struct.List*, %struct.List*, i32, %struct.NodeGroupQueryMem*, i8, i8, i8, i8, i64 }
%struct.NodeDefinition = type { i32, i32, %struct.nameData, %struct.nameData, i32, i32, i32, %struct.nameData, i32, i32, i32, i8, i8, i8, i8, i8, i32 }
%struct.NodeGroupQueryMem = type { i32, [64 x i8], [2 x i32] }
%struct.JunkFilter = type { i32, %struct.List*, %struct.tupleDesc*, i16*, %struct.TupleTableSlot*, i16, i16, i16, i16, i16, %struct.List* }
%struct.ResultRelInfo = type { i32, i32, %struct.RelationData*, i32, i8, %struct.RelationData**, %struct.IndexInfo**, %struct.TriggerDesc*, %struct.FmgrInfo*, %struct.List**, %struct.Instrumentation*, %struct.FdwRoutine*, i8*, %struct.List**, %struct.JunkFilter*, i16, i16, %struct.ProjectionInfo*, %struct.MergeState*, i32, %struct.ProjectionInfo*, %struct.ExprState**, i32 }
%struct.IndexInfo = type { i32, i32, i32, [32 x i16], %struct.List*, %struct.List*, %struct.List*, %struct.List*, i32*, i32*, i16*, i32*, i32*, i16*, i8, i8, i8, i8, i32, i16, %struct.UtilityDesc }
%struct.UtilityDesc = type { double, [2 x i32], i32, i32 }
%struct.TriggerDesc = type { %struct.Trigger*, i32, i8, i8, i8, i8, i8, i8, i8, i8, i8, i8, i8, i8, i8, i8, i8, i8, i8 }
%struct.Trigger = type { i32, i8*, i32, i16, i8, i8, i32, i32, i32, i8, i8, i16, i16, i16*, i8**, i8*, i32 }
%struct.FdwRoutine = type { i32, void (%struct.PlannerInfo*, %struct.RelOptInfo*, i32)*, void (%struct.PlannerInfo*, %struct.RelOptInfo*, i32)*, %struct.ForeignScan* (%struct.PlannerInfo*, %struct.RelOptInfo*, i32, %struct.ForeignPath*, %struct.List*, %struct.List*)*, void (%struct.ForeignScanState*, i32)*, %struct.TupleTableSlot* (%struct.ForeignScanState*)*, void (%struct.ForeignScanState*)*, void (%struct.ForeignScanState*)*, void (%struct.Query*, %struct.RangeTblEntry*, %struct.RelationData*)*, %struct.List* (%struct.PlannerInfo*, %struct.ModifyTable*, i32, i32)*, void (%struct.ModifyTableState*, %struct.ResultRelInfo*, %struct.List*, i32, i32)*, %struct.TupleTableSlot* (%struct.EState*, %struct.ResultRelInfo*, %struct.TupleTableSlot*, %struct.TupleTableSlot*)*, %struct.TupleTableSlot* (%struct.EState*, %struct.ResultRelInfo*, %struct.TupleTableSlot*, %struct.TupleTableSlot*)*, %struct.TupleTableSlot* (%struct.EState*, %struct.ResultRelInfo*, %struct.TupleTableSlot*, %struct.TupleTableSlot*)*, void (%struct.EState*, %struct.ResultRelInfo*)*, i32 (%struct.RelationData*)*, void (%struct.ForeignScanState*, %struct.ExplainState*)*, void (%struct.ModifyTableState*, %struct.ResultRelInfo*, %struct.List*, i32, %struct.ExplainState*)*, i1 (%struct.RelationData*, i32 (%struct.RelationData*, i32, %struct.HeapTupleData**, i32, double*, double*, i8*, i1)**, i32*, i8*, i1)*, i32 (%struct.RelationData*, i32, %struct.HeapTupleData**, i32, double*, double*, i8*, i1)*, %class.VectorBatch* (%struct.VecForeignScanState*)*, i32 ()*, void (%struct.Node*)*, void (%struct.Node*, i32, i32)*, void (%struct.ForeignScanState*, i8*, i32, i32)*, void (%struct.TruncateStmt*, %struct.RelationData*)*, void (%struct.VacuumStmt*, %struct.RelationData*)*, i64 (i32, i32)*, %struct.MotMemoryDetail* (i32*, i1)*, %struct.MotSessionMemoryDetail* (i32*)*, void ()* }
%struct.PlannerInfo = type { i32, %struct.Query*, %struct.PlannerGlobal*, i32, %struct.PlannerInfo*, %struct.RelOptInfo**, i32, %struct.List*, %struct.Bitmapset*, %struct.RangeTblEntry**, %struct.Bitmapset*, %struct.List*, %struct.HTAB*, %struct.List**, i32, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.MemoryContextData*, double, double, double, i8, i8, i8, i8, i8, i8, i32, i32, %struct.List*, i32, %struct.Plan*, %struct.Bitmapset*, %struct.List*, i32, i8, i32, i8, i32, %struct.List*, i8*, %struct.List*, %struct.List*, i16*, i8, i32, %struct.ItstDisKey, i8, i8, i8, i32, %struct.Bitmapset*, i8, %struct.List* }
%struct.Query = type { i32, i32, i32, i64, i8, %struct.Node*, i32, i8, i8, i8, i8, i8, i8, i8, i8, i8, i8, %struct.List*, %struct.List*, %struct.FromExpr*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.Node*, %struct.List*, %struct.List*, %struct.List*, %struct.Node*, %struct.Node*, %struct.List*, %struct.Node*, %struct.List*, %struct.HintState*, i8*, i8, i8, i8, i32, %struct.List*, %struct.ParamListInfoData*, i32, %struct.List*, %struct.List*, %struct.Query*, %struct.UpsertExpr*, i8, i8, i8, i8, i64, i8*, i8, i8, i32*, i32 }
%struct.FromExpr = type { i32, %struct.List*, %struct.Node* }
%struct.HintState = type { i32, i32, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, i8, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, i8 }
%struct.UpsertExpr = type { i32, i32, %struct.List*, %struct.List*, i32, %struct.Node* }
%struct.PlannerGlobal = type { i32, %struct.ParamListInfoData*, %struct.List*, %struct.List*, %struct.List*, %struct.Bitmapset*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, i32, i32, i8, i8, i32, i8, %struct.bloomfilter_context, i8, i32, i32, double, %struct.List*, %struct.PlannerContext*, i32 }
%struct.bloomfilter_context = type { i32, i8 }
%struct.PlannerContext = type { %struct.MemoryContextData*, %struct.MemoryContextData*, %struct.MemoryContextData*, i32 }
%struct.RangeTblEntry = type { i32, i32, i8*, %struct.List*, i32, i32, i8, i32, i8, i32, %struct.List*, i8, i8, %struct.TableSampleClause*, %struct.TimeCapsuleClause*, i8, i8, %struct.Query*, i8, i32, %struct.List*, %struct.Node*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, i8*, i32, i8, i8, %struct.List*, %struct.List*, %struct.List*, i8, %struct.List*, i8, i8, i8, %struct.Alias*, %struct.Alias*, %struct.Alias*, %struct.List*, i8, i8, i8, i32, i32, %struct.Bitmapset*, %struct.Bitmapset*, %struct.Bitmapset*, %struct.Bitmapset*, i32, i8, i8*, i8*, %struct.List*, i8, i8, i8, i8, i32, %struct.List*, i8, i8, %struct.Bitmapset*, i8 }
%struct.TableSampleClause = type { i32, i32, %struct.List*, %struct.Expr* }
%struct.TimeCapsuleClause = type { i32, i32, %struct.Node* }
%struct.Alias = type { i32, i8*, %struct.List* }
%struct.ItstDisKey = type { %struct.List*, %struct.List* }
%struct.RelOptInfo = type <{ i32, i32, %struct.Bitmapset*, i8, [3 x i8], i32, double, i32, i32, i16, [6 x i8], %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.Path*, %struct.Path*, %struct.List*, %struct.Path*, %struct.List*, i32, i32, i32, i16, i16, %struct.Bitmapset**, i32*, %struct.List*, %struct.Bitmapset*, %struct.List*, double, double, double, double, %struct.PruningResult*, i32, [4 x i8], %struct.PruningResult*, i32, [4 x i8], %struct.PruningResult*, i32, [4 x i8], %struct.BucketInfo*, %struct.Plan*, %struct.PlannerInfo*, %struct.List*, %struct.FdwRoutine*, i8*, %struct.List*, %struct.QualCost, i32, [4 x i8], %struct.List*, i8, [3 x i8], i32, i32, i8, [3 x i8], i32, [4 x i8], %struct.List*, %struct.ItstDisKey, %struct.List*, %struct.List*, i8, [7 x i8], %struct.List*, %struct.RelOptInfo*, i32, [4 x i8] }>
%struct.Path = type { i32, i32, %struct.RelOptInfo*, %struct.ParamPathInfo*, double, double, double, double, double, %struct.List*, %struct.List*, i8, i32, i32, i32, %struct.Distribution, i32, double, double }
%struct.ParamPathInfo = type { i32, %struct.Bitmapset*, double, %struct.List*, %struct.Bitmapset* }
%struct.BucketInfo = type { i32, %struct.List* }
%struct.QualCost = type { double, double }
%struct.ForeignScan = type <{ %struct.Scan, i32, [4 x i8], %struct.List*, %struct.List*, i8, i8, [6 x i8], %struct.ErrorCacheEntry*, %struct.List*, %struct.RelationMetaData*, %struct.ForeignOptions*, i64, %struct.BloomFilterSet**, i32, i8, i8, [2 x i8] }>
%struct.Scan = type { %struct.Plan, i32, i8, i32, %struct.PruningResult*, %struct.BucketInfo*, i32, i8, i8, %struct.TableSampleClause*, %struct.OpMemInfo, i8, i8, double }
%struct.OpMemInfo = type { double, double, double, double }
%struct.ErrorCacheEntry = type { i32, i32, %struct.RangeTblEntry*, i8*, %class.ImportErrorLogger** }
%class.ImportErrorLogger = type { %class.BaseErrorLogger }
%class.BaseErrorLogger = type { i32 (...)**, %struct.MemoryContextData*, %struct.tupleDesc*, %struct.StringInfoData*, i32, i64 }
%struct.RelationMetaData = type { i32, i32, i32, i32, i32, i32, i8*, i8, i8, i32, %struct.List* }
%struct.ForeignOptions = type { i32, i32, %struct.List* }
%struct.BloomFilterSet = type { i32, i64*, i64, i64, i64, i64, i64, i64, %struct.ValueBit*, i64, double, i8*, i64, double, i8*, i8, i8, i32, i32, i32, i32 }
%struct.ValueBit = type { [4 x i16] }
%struct.ForeignPath = type { %struct.Path, %struct.Path*, %struct.List* }
%struct.ForeignScanState = type { %struct.ScanState, %struct.FdwRoutine*, i8*, %struct.MemoryContextData*, %struct.ForeignOptions* }
%struct.ScanState = type { %struct.PlanState, %struct.RelationData*, %struct.TableScanDescData*, %struct.TupleTableSlot*, i8, %struct.RelationData*, i8, i32, i32, %struct.List*, %struct.List*, i32, %struct.List*, i8, i8, %struct.SeqScanAccessor*, i32, %struct.List*, i32, i32, %struct.RangeScanInRedis, i8, %struct.SampleScanParams, void (%struct.TableScanDescData*, %struct.TupleTableSlot*, i32)*, %struct.TupleTableSlot* (%struct.ScanState*)*, i8, %struct.ScanBatchState* }
%struct.TableScanDescData = type { %struct.RelationData*, %struct.SnapshotData*, i32, %struct.ScanKeyData*, i8, i32, i32, i32, %struct.BufferAccessStrategyData*, i8, i16, i8*, i8, i32, i32, i32, i32, [291 x i16], %struct.SeqScanAccessor*, %struct.RangeScanInRedis, %struct.TupleTableSlot*, i32, i32 }
%struct.ScanKeyData = type { i32, i16, i16, i32, i32, %struct.FmgrInfo, i64 }
%struct.BufferAccessStrategyData = type { i32, i32, i32, i32, i8, [0 x i32] }
%struct.SeqScanAccessor = type { i32, i32, i32, i32 }
%struct.RangeScanInRedis = type { i8, i8, i8 }
%struct.SampleScanParams = type { %struct.List*, %struct.ExprState*, i32, i8* }
%struct.ScanBatchState = type { %class.VectorBatch*, %class.VectorBatch*, i32, i32, i32*, i32, i8*, i8*, i8, %struct.ScanBatchResult }
%struct.ScanBatchResult = type { i32, %struct.TupleTableSlot** }
%struct.ModifyTable = type { %struct.Plan, i32, i8, %struct.List*, i32, %struct.List*, %struct.List*, %struct.List*, %struct.List*, i32, i8, %struct.List*, %struct.List*, %struct.List*, %struct.List*, i8, %struct.ErrorCacheEntry*, i32, %struct.List*, %struct.List*, i32, %struct.List*, %struct.List*, i32, %struct.Node*, %struct.OpMemInfo }
%struct.ModifyTableState = type { %struct.PlanState, i32, i8, i8, %struct.PlanState**, %struct.PlanState**, %struct.PlanState**, %struct.PlanState**, %struct.PlanState**, i32, i32, %struct.ResultRelInfo*, %struct.List**, %struct.EPQState, i8, %struct.RelationData*, %struct.RelationData*, %struct.ErrorCacheEntry*, %struct.TupleTableSlot*, %struct.TupleTableSlot*, %struct.TupleTableSlot*, %struct.TupleTableSlot*, i32, %struct.UpsertState*, %struct.timeval, %struct.ExprContext* }
%struct.EPQState = type { %struct.EState*, %struct.PlanState*, %struct.TupleTableSlot*, %struct.Plan*, %struct.List*, i32, %struct.EState* }
%struct.UpsertState = type { i32, i32, %struct.TupleTableSlot*, %struct.List*, %struct.TupleTableSlot*, %struct.List* }
%struct.timeval = type { i64, i64 }
%struct.ExplainState = type { %struct.StringInfoData*, %struct.StringInfoData*, i8, i8, i8, i8, i8, i8, i8, i8, i8, i8, i8, i8, i8, i8, i32, %struct.PlannedStmt*, %struct.List*, i32, i32, %struct.List*, %class.PlanInformation*, %struct.DN_RunInfo, i32*, i8*, i8, i8* }
%class.PlanInformation = type { %class.PlanTable*, %class.PlanTable*, %class.PlanTable*, %class.PlanTable*, %class.PlanTable*, %class.PlanTable*, %class.PlanTable*, %class.PlanTable*, %class.PlanTable*, %class.PlanTable*, i32, %class.PlanTable*, i8, i32 }
%class.PlanTable = type <{ %struct.StringInfoData*, i64, i32, [4 x i8], %struct.tupleDesc*, i8, i8, i8, i8, i8, [3 x i8], %struct.StringInfoData, i8, [7 x i8], double, i32, i32, i32*, i32, i8, [3 x i8], %struct.PlanTableMultiData**, i32, i8, i8, i8, i8, i64**, i8**, %struct.MultiInfo***, [14 x i16], [4 x i8] }>
%struct.PlanTableMultiData = type { %struct.PlanTableData*, i8* }
%struct.PlanTableData = type { [32 x i8], i32, [31 x i8], i64, i32, [31 x i8], [256 x i8], [64 x i8], [31 x i8], [64 x i8], %struct.StringInfoData*, double, double }
%struct.MultiInfo = type { i64*, i8* }
%struct.DN_RunInfo = type { i8, i32, i32* }
%struct.VecForeignScanState = type { %struct.ForeignScanState, %class.VectorBatch*, %class.VectorBatch*, %struct.MemoryContextData*, i8, i64*, i8* }
%struct.TruncateStmt = type { i32, %struct.List*, i8, i32, i8 }
%struct.VacuumStmt = type { i32, i32, i32, i32, i64, i64, %struct.RangeVar*, %struct.List*, %struct.RelationData*, %struct.PartitionData*, %struct.RelationData*, %struct.PartitionData*, i8, %struct.List*, i8*, i8, i8, i8, i32, i32, i32, %struct._DestReceiver*, i32, %struct.HeapTupleData**, %struct.tupleDesc*, i32, [3 x %struct.GlobalStatInfoEx], i32, %struct.List*, i8, %struct.List*, i8, i32, %struct.AdaptMem, i32, i8 }
%struct.RangeVar = type { i32, i8*, i8*, i8*, i8*, i8*, i32, i8, %struct.Alias*, i32, i8, i8, %struct.List*, i8, %struct.List*, i32, i32, i8 }
%struct.PartitionData = type { %struct.RelFileNode, %struct.SMgrRelationData*, i32, i8, i8, i64, i64, %struct.FormData_pg_partition*, i32, %struct.List*, %struct.Bitmapset*, i32, %struct.LockInfoData, i32, %struct.varlena*, %struct.PgStat_TableStatus*, %struct.PartitionData*, i8, %struct.RelationData*, %struct.PartitionMap*, %struct.LocalPartitionEntry*, i64 }
%struct.RelFileNode = type { i32, i32, i32, i16, i16 }
%struct.SMgrRelationData = type { %struct.RelFileNodeBackend, %struct.SMgrRelationData**, i32, i32, i32, i32, i32, i32, i32*, i32, i32, %struct._MdfdVec**, %struct.SegmentDesc**, %struct.SegSpace*, i8*, %struct.dlist_node, %struct.dlist_head, %struct.dlist_node, i8 }
%struct.RelFileNodeBackend = type { %struct.RelFileNode, i32 }
%struct._MdfdVec = type opaque
%struct.SegmentDesc = type opaque
%struct.SegSpace = type opaque
%struct.dlist_head = type { %struct.dlist_node }
%struct.dlist_node = type { %struct.dlist_node*, %struct.dlist_node* }
%struct.FormData_pg_partition = type { %struct.nameData, i8, i32, i32, i32, i8, i32, i32, double, double, i32, i32, i32, i32, i8, i32, i32, i32, i32, i32, i32, i64, i64 }
%struct.LockInfoData = type { %struct.LockRelId }
%struct.LockRelId = type { i32, i32, i32 }
%struct.varlena = type { [4 x i8], [0 x i8] }
%struct.PgStat_TableStatus = type { i32, i8, i32, %struct.PgStat_TableXactStatus*, %struct.PgStat_TableCounts, i8, float, float, i32*, i32 }
%struct.PgStat_TableXactStatus = type { i64, i64, i64, i64, i8, i64, i64, i64, i64, i64, i64, i64, i64, i32, %struct.PgStat_TableXactStatus*, %struct.PgStat_TableStatus*, %struct.PgStat_TableXactStatus* }
%struct.PgStat_TableCounts = type { i64, i64, i64, i64, i64, i64, i64, i64, i8, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64 }
%struct.PartitionMap = type { i32, i32, i8 }
%struct.LocalPartitionEntry = type { %struct.LocalBaseEntry, %struct.PartitionData* }
%struct.LocalBaseEntry = type { i32, %struct.Dlelem, i8 }
%struct.Dlelem = type { %struct.Dlelem*, %struct.Dlelem*, i8*, %struct.Dllist* }
%struct.Dllist = type { %struct.Dlelem*, %struct.Dlelem*, i64 }
%struct._DestReceiver = type { void (%struct.TupleTableSlot*, %struct._DestReceiver*)*, void (%struct._DestReceiver*, i32, %struct.tupleDesc*)*, void (%struct._DestReceiver*)*, void (%struct._DestReceiver*)*, i32, void (%class.VectorBatch*, %struct._DestReceiver*)*, void (%struct._DestReceiver*)*, i8, %struct.MemoryContextData* }
%struct.GlobalStatInfoEx = type { i32, double, double, i8, i8, double, i64, i64, i32, double, double*, double*, i32, %struct.HeapTupleData**, %struct.tupleDesc* }
%struct.AdaptMem = type { i32, i32 }
%struct.MotMemoryDetail = type { i64, i64, i64 }
%struct.MotSessionMemoryDetail = type { i64, i64, i64, i64, i64 }
%struct.MergeState = type { %struct.List*, %struct.List* }
%struct.RelationData = type { %struct.RelFileNode, %struct.SMgrRelationData*, i32, i32, i8, i8, i8, i8, i8, i64, i64, %struct.FormData_pg_class*, %struct.tupleDesc*, i32, i8, %struct.LockInfoData, %struct.RuleLock*, %struct.MemoryContextData*, %struct.TriggerDesc*, %struct.RlsPoliciesDesc*, %struct.List*, i32, i32, i32, %struct.Bitmapset*, %struct.Bitmapset*, %struct.Bitmapset*, %struct.Bitmapset*, i8*, i32, %struct.varlena*, i32, %struct.FormData_pg_index*, %struct.HeapTupleData*, %struct.FormData_pg_am*, i32, i32, i8, %struct.MemoryContextData*, %struct.RelationAmInfo*, i32*, i32*, i32*, %struct.FmgrInfo*, i16*, %struct.List*, %struct.List*, i32*, i32*, i16*, i8*, i32*, i32, %struct.FdwRoutine*, i32, i32, i64, i64, i64, i32, i32, %struct.RelationBucketKey*, %struct.PartitionMap*, i32, i32, i8, %struct.PgStat_TableStatus*, %struct.RelationLocInfo*, %struct.PartitionMap*, %struct.RelationData*, %struct.dlist_node, i32, i8, i8, %struct.LocalRelationEntry* }
%struct.FormData_pg_class = type { %struct.nameData, i32, i32, i32, i32, i32, i32, i32, double, double, i32, i32, i32, i32, i32, i32, i32, i8, i8, i8, i8, i16, i16, i8, i8, i8, i8, i8, i8, i8, i8, i8, i32, i8, i64, i32, %struct.int2vector }
%struct.int2vector = type { i32, i32, i32, i32, i32, i32, [0 x i16] }
%struct.RuleLock = type { i32, %struct.RewriteRule** }
%struct.RewriteRule = type { i32, i32, i16, %struct.Node*, %struct.List*, i8, i8 }
%struct.RlsPoliciesDesc = type opaque
%struct.FormData_pg_index = type { i32, i32, i16, i8, i8, i8, i8, i8, i8, i8, i8, i8, %struct.int2vector }
%struct.FormData_pg_am = type { %struct.nameData, i16, i16, i8, i8, i8, i8, i8, i8, i8, i8, i8, i8, i8, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32 }
%struct.RelationAmInfo = type { %struct.FmgrInfo, %struct.FmgrInfo, %struct.FmgrInfo, %struct.FmgrInfo, %struct.FmgrInfo, %struct.FmgrInfo, %struct.FmgrInfo, %struct.FmgrInfo, %struct.FmgrInfo, %struct.FmgrInfo, %struct.FmgrInfo, %struct.FmgrInfo, %struct.FmgrInfo, %struct.FmgrInfo, %struct.FmgrInfo, %struct.FmgrInfo }
%struct.RelationBucketKey = type { %struct.int2vector*, i32* }
%struct.RelationLocInfo = type { i32, i8, %struct.List*, %struct.List*, %struct.ListCell*, %struct.nameData, i16*, i32 }
%struct.LocalRelationEntry = type { %struct.LocalBaseEntry, %struct.RelationData* }
%struct.HTAB = type opaque
%struct.ParamListInfoData = type { void (%struct.ParamListInfoData*, i32)*, i8*, void (%struct.ParseState*, i8*)*, i8*, i32, i8, [0 x %struct.ParamExternData] }
%struct.ParseState = type { %struct.ParseState*, i8*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, i8, %struct.List*, %struct.List*, %struct.CommonTableExpr*, %struct.List*, i32, %struct.List*, i32, %struct.List*, %struct.Node*, i8, i8, i8, i8, i8, i8, i8, i8, %struct.RelationData*, %struct.RangeTblEntry*, i8, i8, %struct.List*, i32, %struct.SelectStmt*, %struct.List*, %struct.WithClause*, i8, %struct.Node* (%struct.ParseState*, %struct.ColumnRef*)*, %struct.Node* (%struct.ParseState*, %struct.ColumnRef*, %struct.Node*)*, %struct.Node* (%struct.ParseState*, %struct.ColumnRef*)*, %struct.Node* (%struct.ParseState*, %struct.ColumnRef*)*, %struct.Node* (%struct.ParseState*, %struct.ParamRef*)*, %struct.Node* (%struct.ParseState*, %struct.Param*, i32, i32, i32)*, %struct.Node* (%struct.ParseState*, %struct.Node*, %struct.Node*, i32*, i32*)*, void (%struct.ParseState*, i32, i32, i32, i8*)*, i8*, i8*, %struct.List*, i8*, i8*, %struct.List*, %struct.List*, %struct.List*, i8, i8, i8, i32, i8, i8, i8, %struct.PlusJoinRTEInfo* }
%struct.CommonTableExpr = type { i32, i8*, %struct.List*, i32, %struct.Node*, i32, i8, i32, %struct.List*, %struct.List*, %struct.List*, %struct.List*, i8, i8, i8, %struct.StartWithOptions* }
%struct.StartWithOptions = type { i32, %struct.List*, %struct.List*, i32, %struct.Node*, %struct.Node*, i8 }
%struct.SelectStmt = type { i32, %struct.List*, %struct.IntoClause*, %struct.List*, %struct.List*, %struct.Node*, %struct.Node*, %struct.List*, %struct.Node*, %struct.List*, %struct.WithClause*, %struct.List*, %struct.List*, %struct.Node*, %struct.Node*, %struct.List*, %struct.HintState*, i32, i8, %struct.SelectStmt*, %struct.SelectStmt*, i8 }
%struct.IntoClause = type { i32, %struct.RangeVar*, %struct.List*, %struct.List*, i32, i8, i8*, i8, i8, i8, %struct.DistributeBy*, %struct.PGXCSubCluster* }
%struct.DistributeBy = type { i32, i32, %struct.List*, %struct.DistState*, i32 }
%struct.DistState = type { i32, i8, %struct.List*, i8* }
%struct.PGXCSubCluster = type { i32, i32, %struct.List* }
%struct.WithClause = type { i32, %struct.List*, i8, i32, %struct.StartWithClause* }
%struct.StartWithClause = type { i32, %struct.Node*, %struct.Node*, %struct.Node*, i8, i8, i8 }
%struct.ColumnRef = type { i32, %struct.List*, i8, i32, i32 }
%struct.ParamRef = type { i32, i32, i32 }
%struct.Param = type { %struct.Expr, i32, i32, i32, i32, i32, i32, i32, i32 }
%struct.PlusJoinRTEInfo = type { i8, %struct.List* }
%struct.ParamExternData = type { i64, i8, i16, i32, %struct.Cursor_Data, %struct.TableOfInfo* }
%struct.TableOfInfo = type { i8, i32, i32, %struct.HTAB* }
%struct.ParamExecData = type { i8*, i32, i64, i8, i8*, i8, i8 }
%struct.BloomFilterControl = type { %"class.filter::BloomFilter"**, i32 }
%"class.filter::BloomFilter" = type { i32 (...)**, i8*, i8* }
%"struct.JitExec::JitContext" = type opaque
%struct.PruningResult = type <{ i32, i32, %struct.PruningBoundary*, %struct.Bitmapset*, i32, [4 x i8], %struct.Bitmapset*, %struct.List*, %struct.List*, %struct.Param*, %struct.OpExpr*, %struct.Expr*, i8, [7 x i8] }>
%struct.PruningBoundary = type { i32, i32, i64*, i8*, i64*, i8* }
%struct.OpExpr = type { %struct.Expr, i32, i32, i32, i8, i32, i32, %struct.List*, i32 }
%struct.Instrumentation = type { i8, i8, i8, i8, %struct.timeval, %struct.timeval, double, double, %struct.BufferUsage, double, double, double, double, double, double, %struct.BufferUsage, %struct.CPUUsage, %struct.CPUUsage, %struct.SortHashInfo, %struct.NetWorkPerfData, %struct.StreamSendData, %struct.RoughCheckInfo, %struct.MemoryInfo, i64, i64, i64, i64, i64, double, double, double, double, double, i32, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i8, i32, i32, i8, i32, i8, i8, i32, i64, i32, i32, [65 x i8], [65 x i8], [65 x i8], [1025 x i8], i32, i64, %struct.RecursiveInfo }
%struct.BufferUsage = type { i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, %struct.timeval, %struct.timeval }
%struct.CPUUsage = type { double }
%struct.SortHashInfo = type { i32, i32, i64, i32, i32, i32, i64, i64, i64, i64, i8, i32, i32, double, double, i64, i64, i64, i64, i64, i64, i64, i32, i32, i32 }
%struct.NetWorkPerfData = type { i64, %struct.timeval, %struct.timeval, %struct.timeval, %struct.timeval, %struct.timeval, %struct.timeval, double, double, double }
%struct.StreamSendData = type { %struct.timeval, %struct.timeval, %struct.timeval, %struct.timeval, %struct.timeval, %struct.timeval, %struct.timeval, %struct.timeval, %struct.timeval, %struct.timeval, double, double, double, double, double, i8 }
%struct.RoughCheckInfo = type { i64, i64, i64 }
%struct.MemoryInfo = type { i64, i64, i64, i32, %struct.MemoryContextData*, %struct.List* }
%struct.RecursiveInfo = type { i32, [201 x i64], i8 }
%struct.Bitmapset = type { i32, [0 x i32] }
%struct.HbktScanSlot = type { i32 }
%struct.ProjectionInfo = type { i32, %struct.List*, %struct.ExprContext*, %struct.TupleTableSlot*, i32*, i8, i8, i32, i32*, i32*, i32*, i32, i32, i32, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, %struct.List*, i8, %class.VectorBatch*, i1 (%struct.ExprContext*, %class.VectorBatch*)*, %class.VectorBatch* }
%struct.HashJoinTableData = type opaque
%struct.HashJoinTupleData = type opaque
%struct.BloomFilterRuntime = type { %struct.List*, %struct.List*, %"class.filter::BloomFilter"** }
%struct.JoinStateLog = type { i32, %struct.hashCell*, i8 }
%class.hashOpSource = type { %class.hashSource, %struct.PlanState* }
%class.hashSource = type { i32 (...)** }
%struct.ReCheckCellLoc = type { %struct.hashCell*, i32, i32 }
%class.hashFileSource = type { %class.hashSource, i32, i64*, i64, i64*, i64*, i32, i32, i32*, %class.VectorBatch*, %struct.hashCell*, %struct.MemoryContextData*, i8**, %struct.TupleTableSlot*, %struct.MinimalTupleData*, i32, i32, i32, i64*, i8*, i32, i64 (i64*)**, [2 x { i64, i64 }], [2 x { i64, i64 }], { i64, i64 }, { i64, i64 }, { i64, i64 }, { i64, i64 }, { i64, i64 }, { i64, i64 }, { i64, i64 }, { i64, i64 }, { i64, i64 } }
%struct.NumericData = type { i32, %union.NumericChoice }
%union.NumericChoice = type { %struct.NumericLong }
%struct.NumericLong = type { i16, i16, [1 x i16] }
%struct.ExprContext = type { i32, %struct.TupleTableSlot*, %struct.TupleTableSlot*, %struct.TupleTableSlot*, %struct.MemoryContextData*, %struct.MemoryContextData*, %struct.ParamExecData*, %struct.ParamListInfoData*, i64*, i8*, i64, i8, %class.ScalarVector*, i64, i8, %struct.EState*, %struct.ExprContext_CB*, %class.VectorBatch*, %class.VectorBatch*, %class.VectorBatch*, %class.VectorBatch*, i32, i8, %class.ScalarVector*, %class.ScalarVector*, i8, %struct.Cursor_Data, i32, %struct.PLpgSQL_execstate*, i8, i8*, i32, i8 }
%struct.ExprContext_CB = type { %struct.ExprContext_CB*, void (i64)*, %struct.ResourceOwnerData*, i64 }
%struct.ResourceOwnerData = type opaque
%struct.PLpgSQL_execstate = type { %struct.PLpgSQL_function*, i64, i8, i32, i64, i8, i32, i32, i8, i8, i8, %struct.tupleDesc*, %struct.tupleDesc*, i8*, %struct.ErrorData*, %struct.Tuplestorestate*, %struct.MemoryContextData*, %struct.ResourceOwnerData*, %struct.ReturnSetInfo*, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, %struct.PLpgSQL_datum**, %struct.EState*, %struct.ResourceOwnerData*, %struct.ParamListInfoData*, %struct.SPITupleTable*, i32, i32, %struct.ExprContext*, %struct.PLpgSQL_expr*, %struct.PLpgSQL_stmt*, i8*, i8*, %struct.List*, i8*, %struct.PLpgSQL_stmt*, i32, %struct.Cursor_Data*, i32, i64, i32, i8 }
%struct.PLpgSQL_function = type { i8*, i32, i32, %struct.OverrideSearchPath*, i32, i64, %struct.ItemPointerData, i8, i8, i32, %struct.PLpgSQL_func_hashkey*, %struct.MemoryContextData*, i32, i32, i8, %struct.FmgrInfo, i32, i8, i8, i8, i32, [8192 x i32], i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, %struct.List*, i32, i32, %struct.PLpgSQL_datum**, i8*, %struct.PLpgSQL_stmt_block*, %struct.List*, %struct.PLpgSQL_execstate*, i64, i8, %struct.RelationData*, %struct.DebugInfo*, %struct.PLpgSQL_nsitem*, i8, i8, i8 }
%struct.OverrideSearchPath = type { %struct.List*, i8, i8, i8 }
%struct.PLpgSQL_func_hashkey = type { i32, i8, i32, i32, [8192 x i32], i32 }
%struct.PLpgSQL_stmt_block = type { i32, i32, i8*, i8, %struct.List*, i32, i32*, %struct.PLpgSQL_exception_block*, i8* }
%struct.PLpgSQL_exception_block = type { %struct.List* }
%struct.DebugInfo = type { i32, i8, i8, i32, void (%struct.PLpgSQL_function*, %struct.PLpgSQL_execstate*)*, void (%struct.PLpgSQL_function*)*, %struct.PLpgSQL_function*, %struct.MemoryContextData*, %struct.DebugInfo*, %struct.DebugInfoComm*, %struct.List*, %struct.PLpgSQL_stmt*, %struct.PLpgSQL_execstate* }
%struct.DebugInfoComm = type { i32, i8*, i8*, i32, i32, i32, i32 }
%struct.PLpgSQL_nsitem = type { i32, i32, %struct.PLpgSQL_nsitem*, i8*, i8*, [0 x i8] }
%struct.ErrorData = type { i32, i8, i8, i8, i8, i8, i8, i8*, i32, i8*, i8*, i32, i32, i8*, i8*, i8*, i8*, i8*, i32, i32, i8*, i32, i8*, i32, i8, i8, i8*, i8* }
%struct.ReturnSetInfo = type { i32, %struct.ExprContext*, %struct.tupleDesc*, i32, i32, i32, %struct.Tuplestorestate*, %struct.tupleDesc* }
%struct.PLpgSQL_datum = type { i32, i32, i8 }
%struct.SPITupleTable = type { %struct.MemoryContextData*, i32, i32, %struct.tupleDesc*, %struct.HeapTupleData** }
%struct.PLpgSQL_expr = type { i32, i32, i8, i8*, %struct._SPI_plan*, %struct.Bitmapset*, %struct.PLpgSQL_function*, %struct.PLpgSQL_nsitem*, %struct.Expr*, i32, i32, i32, i8, i8, %struct.CachedPlanSource*, %struct.CachedPlan*, i64, %struct.ExprState*, i8, i64, i8, i8, i32, i32, i8, i8, i32, i8, i32 }
%struct._SPI_plan = type { i32, i8, i8, %struct.List*, %struct.MemoryContextData*, i32, i32, i32*, void (%struct.ParseState*, i8*)*, i8*, i32, i32, %struct.List* }
%struct.CachedPlanSource = type <{ i32, [4 x i8], %struct.Node*, i8*, i8*, i32*, i8*, i32, [4 x i8], void (%struct.ParseState*, i8*)*, i8*, i32, i32, i8, i8, [6 x i8], %struct.tupleDesc*, %struct.MemoryContextData*, %struct.List*, %struct.List*, %struct.List*, %struct.OverrideSearchPath*, %struct.MemoryContextData*, %struct.CachedPlan*, i8, i8, i8, i8, i32, %"struct.JitExec::JitContext"*, i32, [4 x i8], %struct.CachedPlanSource*, double, double, i32, [4 x i8], %struct.GPCSource, %struct.SPISign, i8, [7 x i8], i8*, i8, [7 x i8], %struct.CachedPlan*, %struct.ExecNodes*, i8, [7 x i8], i8*, i8*, i8, i8, i8, i8, [4 x i8] }>
%struct.GPCSource = type { %class.GPCPlanStatus, %struct.GPCKey* }
%class.GPCPlanStatus = type { i32, i32, i32, i32 }
%struct.GPCKey = type { i32, i8*, %struct.GPCEnv, %struct.SPISign }
%struct.GPCEnv = type { i8, %struct.GPCPlainEnv, i32, i32*, %struct.OverrideSearchPath*, [64 x i8], [64 x i8], [64 x i8], i8, i32 }
%struct.GPCPlainEnv = type { i32, i16, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, double, double, double, double, double, double, double, double, double, double, double }
%struct.SPISign = type { i32, i32, i32, i32 }
%struct.CachedPlan = type { i32, %struct.List*, i8, i8, i8, i32, %"struct.JitExec::JitContext"*, i8, i32, i64, i32, i32, i8, %struct.MemoryContextData*, i32, i8, i32 }
%struct.PLpgSQL_stmt = type { i32, i32 }
%struct.varattrib_1b = type { i8, [0 x i8] }
%struct.anon = type { i32, [0 x i8] }
%class.HashAggRunner = type <{ %class.BaseAggRunner, %struct.AggStateLog, %class.hashSource*, [1000 x i64], i32, i8, [3 x i8], i64, i64, double, double, %struct.MemoryContextData*, %struct.HashSegTbl*, i32, i32, i64, { i64, i64 }, i32, [4 x i8] }>
%class.BaseAggRunner = type { %class.hashBasedOperator.base, { i64, i64 }, %struct.VecAggState*, i32, i32, i8*, i32*, i32*, i32*, i8, i8, i32, %struct.finalAggInfo*, i32, %class.VectorBatch*, %class.VectorBatch*, %class.VectorBatch*, i32, [1000 x %struct.hashCell*], %struct.SortDistinct*, %class.VarBuf*, i32, %struct.ExprContext* }
%struct.VecAggState = type { %struct.AggState, i8*, %struct.VecAggInfo*, i8*, i8*, i8*, i8*, i8* }
%struct.AggState = type { %struct.ScanState, %struct.List*, i32, %struct.AggStatePerPhaseData*, i32, i32, %struct.FmgrInfo*, %struct.AggStatePerAggData*, %struct.MemoryContextData**, %struct.ExprContext*, %struct.AggStatePerAggData*, i8, i8, i32, i32, %struct.Bitmapset*, %struct.List*, i32, %struct.AggStatePerPhaseData*, %struct.Tuplesortstate*, %struct.Tuplesortstate*, %struct.TupleTableSlot*, %struct.AggStatePerGroupData*, %struct.HeapTupleData*, %struct.TupleHashTableData*, %struct.TupleTableSlot*, %struct.List*, i8, %struct.HASH_SEQ_STATUS, i8, i8*, %struct.FmgrInfo* }
%struct.AggStatePerAggData = type opaque
%struct.AggStatePerPhaseData = type opaque
%struct.Tuplesortstate = type opaque
%struct.AggStatePerGroupData = type opaque
%struct.TupleHashTableData = type { %struct.HTAB*, i32, i16*, %struct.FmgrInfo*, %struct.FmgrInfo*, %struct.MemoryContextData*, %struct.MemoryContextData*, i64, %struct.TupleTableSlot*, %struct.TupleTableSlot*, %struct.FmgrInfo*, %struct.FmgrInfo*, i64, i8, i8 }
%struct.HASH_SEQ_STATUS = type { %struct.HTAB*, i32, %struct.HASHELEMENT* }
%struct.HASHELEMENT = type { %struct.HASHELEMENT*, i32 }
%struct.VecAggInfo = type { %struct.FunctionCallInfoData, %struct.FunctionCallInfoData, %class.ScalarVector* (%struct.FunctionCallInfoData*)**, %class.ScalarVector* (%struct.FunctionCallInfoData*)**, i64 (%struct.FunctionCallInfoData*)** }
%struct.finalAggInfo = type { i32, %struct.VecAggInfo* }
%struct.SortDistinct = type { %class.Batchsortstate**, %class.VectorBatch**, i64*, %struct.hashCell*, i32* }
%class.Batchsortstate = type { %class.VecStore.base, %struct.MemoryContextData*, i32, %struct.RemoteQueryState*, %class.VectorBatch**, i32*, %struct.VecStreamState*, i32, %struct.ScanKeyData*, %struct.SortSupportData*, i64, i8, i8, i8, i32, %struct.MultiColumnsData, i8*, i64, i32, i32, i32, %struct.LogicalTapeSet*, i32, i8*, i32*, i32*, i32*, i64*, i32, i32, i32, i32, i32*, i32*, i32*, i32*, i32, i32, i8, i64, i32, i8, i64, i64, %struct.PGRUsage, i8*, i8*, { i64, i64 }, i32 (%struct.MultiColumns*, %struct.MultiColumns*, %class.Batchsortstate*)*, void (%class.Batchsortstate*, %struct.MultiColumns*, i8*)*, void (%class.Batchsortstate*, i32, %struct.MultiColumns*)*, void (%class.Batchsortstate*, %struct.MultiColumns*, i32, i32)*, i32 (%class.Batchsortstate*, i32, i1)*, void (%class.Batchsortstate*)*, void (%class.Batchsortstate*, %class.VectorBatch*, i32, i32)* }
%class.VecStore.base = type <{ %struct.MultiColumnsData, i64, i64, %class.ScalarVector*, i32, [4 x i8], %struct.tupleDesc*, i64, i64, i64, i8, i8, [6 x i8], i64, i32, i32, i32 }>
%struct.RemoteQueryState = type { %struct.ScanState, i32, %struct.pgxc_node_handle**, i32, i32, i32, i32, i32, %struct.tupleDesc*, i32, i32, i32, i32, i8*, i8*, i8*, i8*, i8*, i32, i8, i8, %struct.RemoteDataRowData, %struct.RowStoreManagerData*, i64, i8, i32*, i32, %struct._IO_FILE*, i64, i8*, i8*, i32, %struct.pgxc_node_handle**, i8*, i32, i32*, i32, i32, i8, %struct.Tuplestorestate*, i32, i64, i64, i8*, i8*, i1 (%struct.RemoteQueryState*, %struct.TupleTableSlot*, %struct.ParallelFunctionState*)*, i8, %struct.RemoteErrorData, i8, [2 x i64], [2 x i64], i32, i32, i8*, i8, %struct.NodeIdxInfo*, i32, i8, i8*, i8*, %struct.ParallelFunctionState*, i8 }
%struct.RemoteDataRowData = type { i8*, i32, i32 }
%struct.RowStoreManagerData = type { %struct.MemoryContextData*, %struct.ResourceOwnerData*, %struct.Bank**, i32, %struct.Bank**, i32 }
%struct.Bank = type { %struct.List*, %struct.BufFile*, i64, i64, i32, i64, i32, i64, i32 }
%struct.BufFile = type opaque
%struct._IO_FILE = type { i32, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, %struct._IO_marker*, %struct._IO_FILE*, i32, i32, i64, i16, i8, [1 x i8], i8*, i64, %struct._IO_codecvt*, %struct._IO_wide_data*, %struct._IO_FILE*, i8*, i64, i32, [20 x i8] }
%struct._IO_marker = type opaque
%struct._IO_codecvt = type opaque
%struct._IO_wide_data = type opaque
%struct.pgxc_node_handle = type { i32, i32, i32, i32, %struct.gsocket, i8, i32, %struct.RemoteQueryState*, %struct.StreamState*, i8*, i8*, i8*, i64, i64, i64, i8*, i64, i64, i64, i64, i32, i8*, i32, i32, %struct.PoolConnInfo, i8, %struct.pg_conn*, %struct.MessageCommLog*, i64 }
%struct.gsocket = type { i16, i16, i16, i16 }
%struct.StreamState = type { %struct.ScanState, %struct.pgxc_node_handle**, i32, i32, i8, i8, i32, i8*, i8*, i8*, i8, %struct.StreamDataBuf, %class.StreamConsumer*, i1 (%struct.StreamState*)*, i1 (%struct.StreamState*)*, %struct.RemoteErrorData, i64, %struct.StreamNetCtl, %struct.StreamSharedContext*, %struct.TupleVector*, i32, i64*, i8*, i8 }
%struct.StreamDataBuf = type { i8*, i32, i32 }
%class.StreamConsumer = type { %class.StreamObj, %struct.List*, i32, i8, %struct.StreamConnInfo*, %union.pthread_mutex_t, %union.pthread_cond_t, %struct.StreamSharedContext* }
%class.StreamObj = type { %class.StreamNodeGroup*, %class.StreamTransport**, %struct.StreamPair*, i32, %struct.StreamKey, %struct.MemoryContextData*, i32, i32, %struct.Stream*, i32, i64, i32, i8, i8, i32, %struct.ParallelDesc }
%class.StreamNodeGroup = type <{ %struct.List*, %struct.List*, %struct.List*, %struct.MemoryContextData*, %struct.ErrorData*, %struct.PortalData*, i64, i32, i32, %struct.StreamNode*, %struct.List*, i32, i32, i32, [4 x i8], %union.pthread_mutex_t, %union.pthread_cond_t, i8, i8, [2 x i8], i32, i8, i8, [6 x i8], %union.pthread_mutex_t, i32, [4 x i8] }>
%struct.PortalData = type { i8*, i8*, %struct.MemoryContextData*, %struct.ResourceOwnerData*, void (%struct.PortalData*)*, i64, i64, i8*, i8*, %struct.List*, %struct.CachedPlan*, %struct.ParamListInfoData*, i32, i32, i32, i32, i32, i8, i8, %struct.QueryDesc*, %struct.TableScanDescData*, %struct.tupleDesc*, i16*, %struct.Tuplestorestate*, %struct.MemoryContextData*, i8, i8, i8, i64, i8, i64, i8, i32, [4 x i8*], i32, i32, %struct.MemoryContextData*, i8, %struct.PortalStream, i8, i8 }
%struct.QueryDesc = type { i32, %struct.PlannedStmt*, %struct.Node*, i8*, %struct.SnapshotData*, %struct.SnapshotData*, %struct._DestReceiver*, %struct.ParamListInfoData*, i32, %struct.tupleDesc*, %struct.EState*, %struct.PlanState*, %struct.Instrumentation*, i8, %"struct.JitExec::JitContext"* }
%struct.PortalStream = type { %class.StreamNodeGroup*, %struct.MemoryContextData*, %struct.MemoryContextData*, i64 }
%struct.StreamNode = type { %class.StreamObj*, %struct.List*, i32, i8* }
%class.StreamTransport = type { i32 (...)**, [64 x i8], i32, i32, i8, %struct.Port*, %struct.StreamBuffer* }
%struct.Port = type { i32, i8, i32, %struct.SockAddr, %struct.SockAddr, i8*, i8*, i32, i8*, %struct.libcommaddrinfo*, %struct.gsocket, i32, i8*, i8*, i8*, %struct.List*, %struct.HbaLine*, [4 x i8], i64, i32, i32, i32, i32, i32, i32, i32, %struct.pg_gssinfo*, %struct.ssl_st*, %struct.x509_st*, i8*, i64, [9 x i8], i8, %struct.MessageCommLog*, i8*, %struct.gss_ctx_id_struct*, %struct.gss_cred_id_struct*, %struct.gss_name_struct*, %struct.gss_buffer_desc_struct }
%struct.SockAddr = type { %struct.sockaddr_storage, i32 }
%struct.sockaddr_storage = type { i16, [118 x i8], i64 }
%struct.libcommaddrinfo = type { i8*, [64 x i8], i32, i32, i32, i32, %struct.CommStreamKey, i32, i8, i32, %struct.libcommaddrinfo*, %struct.gsocket }
%struct.CommStreamKey = type { i64, i32, i32, i32 }
%struct.HbaLine = type { i32, i32, %struct.List*, %struct.List*, %struct.sockaddr_storage, %struct.sockaddr_storage, i32, i8*, i32, i8*, i8*, i8, i8*, i32, i8*, i8*, i8*, i8*, i8*, i8*, i8, i8*, i8*, i8, i8 }
%struct.pg_gssinfo = type { %struct.gss_buffer_desc_struct, %struct.gss_cred_id_struct*, %struct.gss_ctx_id_struct*, %struct.gss_name_struct* }
%struct.ssl_st = type opaque
%struct.x509_st = type opaque
%struct.gss_ctx_id_struct = type opaque
%struct.gss_cred_id_struct = type opaque
%struct.gss_name_struct = type opaque
%struct.gss_buffer_desc_struct = type { i64, i8* }
%struct.StreamBuffer = type { [8192 x i8], i32, i32, i32, i8 }
%struct.StreamPair = type { %struct.StreamKey, %struct.List*, %struct.List*, i32, i32 }
%struct.StreamKey = type { i64, i32, i32 }
%struct.Stream = type <{ %struct.Scan, i32, [4 x i8], i8*, %struct.ExecNodes*, %struct.List*, i8, [7 x i8], %struct.SimpleSort*, i8, [3 x i8], %struct.ParallelDesc, i8*, %struct.List*, i32, [4 x i8], %struct.ExecNodes*, i8, [7 x i8] }>
%struct.SimpleSort = type { i32, i32, i16*, i32*, i32*, i8*, i8 }
%struct.ParallelDesc = type { i32, i32, i32 }
%struct.StreamConnInfo = type { %union.StreamIdentity, [64 x i8], i32, i32, i32 }
%union.StreamIdentity = type { %struct.anon.8, [4 x i8] }
%struct.anon.8 = type { i32 }
%union.pthread_mutex_t = type { %struct.__pthread_mutex_s, [8 x i8] }
%struct.__pthread_mutex_s = type { i32, i32, i32, i32, i32, i32, %struct.__pthread_internal_list }
%struct.__pthread_internal_list = type { %struct.__pthread_internal_list*, %struct.__pthread_internal_list* }
%union.pthread_cond_t = type { %struct.__pthread_cond_s }
%struct.__pthread_cond_s = type { %union.anon.4, %union.anon.6, [2 x i32], [2 x i32], i32, i32, [2 x i32] }
%union.anon.4 = type { i64 }
%union.anon.6 = type { i64 }
%struct.StreamNetCtl = type { %union.StreamNetCtlLayer }
%union.StreamNetCtlLayer = type { %struct.anon.11 }
%struct.anon.11 = type { i32*, i32*, %struct.gsocket* }
%struct.StreamSharedContext = type { %struct.MemoryContextData*, %class.VectorBatch***, %struct.TupleVector***, %struct.StringInfoData***, i32**, i8**, i32*, %struct.CommStreamKey, i8, %struct.hash_entry**, %struct.hash_entry*** }
%struct.hash_entry = type opaque
%struct.TupleVector = type { %struct.TupleTableSlot**, i32 }
%struct.PoolConnInfo = type { %struct.nameData, i32 }
%struct.pg_conn = type opaque
%struct.MessageCommLog = type { %struct.MessageIpcLog, %struct.MessageIpcLog }
%struct.MessageIpcLog = type { i8, i32, i32, i32, i32, i8, i32, i32, [30 x i8] }
%struct.RemoteErrorData = type { i32, i8*, i8*, i32, i32 }
%struct.NodeIdxInfo = type { i32, i32 }
%struct.ParallelFunctionState = type { i8*, %struct.ExecNodes*, %struct.tupleDesc*, %struct.Tuplestorestate*, i64, i8*, %struct.MemoryContextData*, i8, i8, i8, i8 }
%struct.VecStreamState = type { %struct.StreamState, %class.VectorBatch*, void (%struct.VecStreamState*, %class.VectorBatch*)*, i8, i32, i32, i32* }
%struct.SortSupportData = type { %struct.MemoryContextData*, i32, i8, i8, i16, i8*, i32 (i64, i64, %struct.SortSupportData*)*, i8, i64 (i64, %struct.SortSupportData*)*, i1 (i32, %struct.SortSupportData*)*, i32 (i64, i64, %struct.SortSupportData*)* }
%struct.MultiColumnsData = type { %struct.MultiColumns*, i32, i32, i32 }
%struct.MultiColumns = type { i64*, i8*, i32, i32 }
%struct.LogicalTapeSet = type opaque
%struct.PGRUsage = type { %struct.timeval, %struct.rusage }
%struct.rusage = type { %struct.timeval, %struct.timeval, %union.anon.12, %union.anon.13, %union.anon.14, %union.anon.15, %union.anon.16, %union.anon.17, %union.anon.18, %union.anon.19, %union.anon.20, %union.anon.21, %union.anon.22, %union.anon.23, %union.anon.24, %union.anon.25 }
%union.anon.12 = type { i64 }
%union.anon.13 = type { i64 }
%union.anon.14 = type { i64 }
%union.anon.15 = type { i64 }
%union.anon.16 = type { i64 }
%union.anon.17 = type { i64 }
%union.anon.18 = type { i64 }
%union.anon.19 = type { i64 }
%union.anon.20 = type { i64 }
%union.anon.21 = type { i64 }
%union.anon.22 = type { i64 }
%union.anon.23 = type { i64 }
%union.anon.24 = type { i64 }
%union.anon.25 = type { i64 }
%struct.AggStateLog = type { i8, %struct.hashCell*, i32, i32 }
%struct.HashSegTbl = type { i32, %struct.hashCell** }
%class.SortAggRunner = type { %class.BaseAggRunner, i8, i8, i8, i32, i32, %struct.GroupintAtomContainer*, %class.Batchsortstate*, %class.Batchsortstate*, %class.VectorBatch*, %class.hashSource*, { i64, i64 } }
%struct.GroupintAtomContainer = type { [2000 x %struct.hashCell*], %class.VarBuf*, %class.VarBuf*, i32 }
%struct.bictl = type { i64, %struct.MemoryContextData* }
%"class.filter::BloomFilterImpl" = type { %"class.filter::BloomFilter", %"class.filter::BitSet"*, i64, i64, i64, i64, i64, %struct.ValueBit*, i64, i64, i8, i8, i32, i32, i32, i32, %struct.MemoryContextData* }
%"class.filter::BitSet" = type { i32 (...)**, i64*, i64 }
%class.SonicEncodingDatumArray = type { %class.SonicDatumArray }
%class.SonicDatumArray = type { i32 (...)**, %struct.MemoryContextData*, %struct.DatumDesc, i8, i32, i32, i32, i32, %struct.atom**, %struct.atom*, i32, i32, i8*, i8* }
%struct.DatumDesc = type { i32, i32, i32, i32 }
%struct.atom = type { i8*, i8* }
%class.SonicHashAgg = type <{ %class.SonicHash, { i64, i64 }, %struct.VecAggState*, i32, i32, i64, i64, i32, [4 x i8], i64, i32, [4 x i8], %struct.ExprContext*, %class.SonicHashSource*, %class.SonicHashPartition**, %class.SonicHashPartition**, i16, i16, i32, i16, [6 x i8], i16*, i16*, i16, i16, [4 x i8], i8*, i16*, i8, [7 x i8], %struct.finalAggInfo*, %struct.FmgrInfo*, %class.VectorBatch*, %class.VectorBatch*, %class.VectorBatch*, %class.VectorBatch*, i8, [3 x i8], i32, %class.SonicDatumArray*, %class.SonicDatumArray*, i8, [7 x i8], { i64, i64 }, double, double, { i64, i64 }*, { i64, i64 }*, [1000 x i16], i16, [1000 x i16], i16, [1000 x i32], [1000 x i32], [4 x i8] }>
%class.SonicHash = type { i32 (...)**, %class.SonicDatumArray**, %class.SonicDatumArray*, i8*, i8, i32, i64, i64, { i64, i64 }*, %struct.hashStateLog, i8, i8, %"struct.SonicHash::SonicHashInputOpAttr", %struct.FmgrInfo*, %struct.SonicHashMemoryControl, [16384 x i32], [1000 x i32], [1000 x i32], [1000 x i16], i16, [1000 x i8], [1000 x i64], [1000 x i8], [1000 x %struct.ArrayIdx] }
%struct.hashStateLog = type { i32, i8 }
%"struct.SonicHash::SonicHashInputOpAttr" = type { i8, i16*, i16*, i16, { i64, i64 }*, { i64, i64 }*, %struct.FmgrInfo*, %class.VectorBatch*, i16, %struct.tupleDesc* }
%struct.SonicHashMemoryControl = type { i8, i8, i32, i64, i32, i64, i64, i64, %struct.MemoryContextData*, %struct.MemoryContextData* }
%struct.ArrayIdx = type { i32, i16 }
%class.SonicHashSource = type { i32 (...)** }
%class.SonicHashPartition = type { %class.SonicHashSource, %struct.MemoryContextData*, i32, i16, i64, i64*, i64 }

$_ZZ18getScaleMultiplieriE6values = comdat any

@Int64MultiOutOfBound = external dso_local local_unnamed_addr constant [20 x i64], align 8
@ScaleMultipler = external dso_local local_unnamed_addr constant [20 x i64], align 8
@assert_enabled = external thread_local local_unnamed_addr global i8, align 1
@.str = private unnamed_addr constant [29 x i8] c"!(scale >= 0 && scale <= 38)\00", align 1
@.str.1 = private unnamed_addr constant [16 x i8] c"FailedAssertion\00", align 1
@.str.2 = private unnamed_addr constant [41 x i8] c"../../..//src/include/utils/biginteger.h\00", align 1
@_ZZ18getScaleMultiplieriE6values = linkonce_odr dso_local local_unnamed_addr constant [39 x i128] [i128 1, i128 10, i128 100, i128 1000, i128 10000, i128 100000, i128 1000000, i128 10000000, i128 100000000, i128 1000000000, i128 10000000000, i128 100000000000, i128 1000000000000, i128 10000000000000, i128 100000000000000, i128 1000000000000000, i128 10000000000000000, i128 100000000000000000, i128 1000000000000000000, i128 10000000000000000000, i128 100000000000000000000, i128 1000000000000000000000, i128 10000000000000000000000, i128 100000000000000000000000, i128 1000000000000000000000000, i128 10000000000000000000000000, i128 100000000000000000000000000, i128 1000000000000000000000000000, i128 10000000000000000000000000000, i128 100000000000000000000000000000, i128 1000000000000000000000000000000, i128 10000000000000000000000000000000, i128 100000000000000000000000000000000, i128 1000000000000000000000000000000000, i128 10000000000000000000000000000000000, i128 100000000000000000000000000000000000, i128 1000000000000000000000000000000000000, i128 10000000000000000000000000000000000000, i128 100000000000000000000000000000000000000], comdat, align 16

; Function Attrs: nofree norecurse nounwind ssp
define dso_local %class.ScalarVector* @llvmJittedFunc(%struct.FuncExprState* nocapture readnone %0, %class.HashJoinTbl* nocapture readnone %1, %struct.NumericData* nocapture readnone %2, %struct.ExprContext* nocapture readonly %3, i8* nocapture readnone %4, %class.ScalarVector* returned %5) local_unnamed_addr #0 {
  %7 = getelementptr inbounds %struct.ExprContext, %struct.ExprContext* %3, i64 0, i32 17
  %8 = load %class.VectorBatch*, %class.VectorBatch** %7, align 8
  %9 = getelementptr inbounds %class.VectorBatch, %class.VectorBatch* %8, i64 0, i32 4
  %10 = load %class.ScalarVector*, %class.ScalarVector** %9, align 8
  %11 = getelementptr %class.ScalarVector, %class.ScalarVector* %10, i64 25, i32 0
  %12 = load i32, i32* %11, align 8
  %13 = getelementptr %class.ScalarVector, %class.ScalarVector* %10, i64 25, i32 5
  %14 = load i64*, i64** %13, align 8
  %15 = getelementptr inbounds %class.ScalarVector, %class.ScalarVector* %5, i64 0, i32 5
  %16 = load i64*, i64** %15, align 8
  %17 = getelementptr inbounds %class.ScalarVector, %class.ScalarVector* %5, i64 0, i32 3
  %18 = load i8*, i8** %17, align 8
  %19 = icmp sgt i32 %12, 0
  br i1 %19, label %20, label %36

20:                                               ; preds = %6
  %21 = zext i32 %12 to i64
  br label %22

22:                                               ; preds = %22, %20
  %23 = phi i64 [ 0, %20 ], [ %34, %22 ]
  %24 = getelementptr i64, i64* %14, i64 %23
  %25 = load i64, i64* %24, align 8
  %26 = shl i64 %25, 1
  %27 = add i64 %26, 5
  %28 = icmp slt i64 %27, 10
  %29 = zext i1 %28 to i64
  %30 = getelementptr i64, i64* %16, i64 %23
  store i64 %29, i64* %30, align 8
  %31 = getelementptr i8, i8* %18, i64 %23
  %32 = load i8, i8* %31, align 1
  %33 = and i8 %32, -2
  store i8 %33, i8* %31, align 1
  %34 = add nuw nsw i64 %23, 1
  %35 = icmp eq i64 %34, %21
  br i1 %35, label %36, label %22

36:                                               ; preds = %22, %6
  %37 = getelementptr inbounds %class.ScalarVector, %class.ScalarVector* %5, i64 0, i32 0
  store i32 %12, i32* %37, align 8
  %38 = getelementptr inbounds %class.ScalarVector, %class.ScalarVector* %5, i64 0, i32 1, i32 0
  store i32 20, i32* %38, align 4
  ret %class.ScalarVector* %5
}

; Function Attrs: norecurse nounwind readonly ssp
define dso_local i64 @LLVMIRmemcmp(i8* nocapture readonly %0, i8* nocapture readonly %1, i32 %2) local_unnamed_addr #1 {
  br label %4

4:                                                ; preds = %11, %3
  %5 = phi i8* [ %0, %3 ], [ %13, %11 ]
  %6 = phi i8* [ %1, %3 ], [ %12, %11 ]
  %7 = phi i32 [ %2, %3 ], [ %14, %11 ]
  %8 = load i8, i8* %5, align 1
  %9 = load i8, i8* %6, align 1
  %10 = icmp eq i8 %8, %9
  br i1 %10, label %11, label %16

11:                                               ; preds = %4
  %12 = getelementptr i8, i8* %6, i64 1
  %13 = getelementptr i8, i8* %5, i64 1
  %14 = add i32 %7, -1
  %15 = icmp eq i32 %14, 0
  br i1 %15, label %16, label %4

16:                                               ; preds = %4, %11
  %17 = phi i64 [ 1, %11 ], [ 0, %4 ]
  ret i64 %17
}

; Function Attrs: norecurse nounwind readonly ssp
define dso_local [2 x i64] @LLVMIRrtrim1(i64 %0) local_unnamed_addr #1 {
  %2 = inttoptr i64 %0 to %struct.varattrib_1b*
  %3 = getelementptr inbounds %struct.varattrib_1b, %struct.varattrib_1b* %2, i64 0, i32 0
  %4 = load i8, i8* %3, align 1
  %5 = zext i8 %4 to i32
  %6 = and i32 %5, 1
  %7 = icmp eq i32 %6, 0
  br i1 %7, label %11, label %8

8:                                                ; preds = %1
  %9 = getelementptr inbounds %struct.varattrib_1b, %struct.varattrib_1b* %2, i64 0, i32 1, i64 0
  %10 = lshr i32 %5, 1
  br label %17

11:                                               ; preds = %1
  %12 = inttoptr i64 %0 to %struct.anon*
  %13 = getelementptr inbounds %struct.anon, %struct.anon* %12, i64 0, i32 1, i64 0
  %14 = getelementptr inbounds %struct.anon, %struct.anon* %12, i64 0, i32 0
  %15 = load i32, i32* %14, align 4
  %16 = lshr i32 %15, 2
  br label %17

17:                                               ; preds = %11, %8
  %18 = phi i32 [ -4, %11 ], [ -1, %8 ]
  %19 = phi i32 [ %16, %11 ], [ %10, %8 ]
  %20 = phi i8* [ %13, %11 ], [ %9, %8 ]
  %21 = add nsw i32 %19, %18
  br label %22

22:                                               ; preds = %26, %17
  %23 = phi i32 [ %21, %17 ], [ %24, %26 ]
  %24 = add nsw i32 %23, -1
  %25 = icmp sgt i32 %23, 0
  br i1 %25, label %26, label %31

26:                                               ; preds = %22
  %27 = zext i32 %24 to i64
  %28 = getelementptr i8, i8* %20, i64 %27
  %29 = load i8, i8* %28, align 1
  %30 = icmp eq i8 %29, 32
  br i1 %30, label %22, label %31

31:                                               ; preds = %26, %22
  %32 = ptrtoint i8* %20 to i64
  %33 = zext i32 %23 to i64
  %34 = insertvalue [2 x i64] undef, i64 %33, 0
  %35 = insertvalue [2 x i64] %34, i64 %32, 1
  ret [2 x i64] %35
}

; Function Attrs: norecurse nounwind readonly ssp
define dso_local [2 x i64] @LLVMIRbtrim1(i64 %0) local_unnamed_addr #1 {
  %2 = inttoptr i64 %0 to %struct.varattrib_1b*
  %3 = getelementptr inbounds %struct.varattrib_1b, %struct.varattrib_1b* %2, i64 0, i32 0
  %4 = load i8, i8* %3, align 1
  %5 = zext i8 %4 to i32
  %6 = and i32 %5, 1
  %7 = icmp eq i32 %6, 0
  br i1 %7, label %11, label %8

8:                                                ; preds = %1
  %9 = getelementptr inbounds %struct.varattrib_1b, %struct.varattrib_1b* %2, i64 0, i32 1, i64 0
  %10 = lshr i32 %5, 1
  br label %17

11:                                               ; preds = %1
  %12 = inttoptr i64 %0 to %struct.anon*
  %13 = getelementptr inbounds %struct.anon, %struct.anon* %12, i64 0, i32 1, i64 0
  %14 = getelementptr inbounds %struct.anon, %struct.anon* %12, i64 0, i32 0
  %15 = load i32, i32* %14, align 4
  %16 = lshr i32 %15, 2
  br label %17

17:                                               ; preds = %11, %8
  %18 = phi i32 [ -4, %11 ], [ -1, %8 ]
  %19 = phi i32 [ %16, %11 ], [ %10, %8 ]
  %20 = phi i8* [ %13, %11 ], [ %9, %8 ]
  %21 = add nsw i32 %19, %18
  br label %22

22:                                               ; preds = %26, %17
  %23 = phi i32 [ %21, %17 ], [ %24, %26 ]
  %24 = add nsw i32 %23, -1
  %25 = icmp sgt i32 %23, 0
  br i1 %25, label %26, label %43

26:                                               ; preds = %22
  %27 = zext i32 %24 to i64
  %28 = getelementptr i8, i8* %20, i64 %27
  %29 = load i8, i8* %28, align 1
  %30 = icmp eq i8 %29, 32
  br i1 %30, label %22, label %31

31:                                               ; preds = %26
  %32 = zext i32 %23 to i64
  br label %33

33:                                               ; preds = %38, %31
  %34 = phi i64 [ 0, %31 ], [ %39, %38 ]
  %35 = getelementptr i8, i8* %20, i64 %34
  %36 = load i8, i8* %35, align 1
  %37 = icmp eq i8 %36, 32
  br i1 %37, label %38, label %41

38:                                               ; preds = %33
  %39 = add nuw nsw i64 %34, 1
  %40 = icmp eq i64 %39, %32
  br i1 %40, label %43, label %33

41:                                               ; preds = %33
  %42 = trunc i64 %34 to i32
  br label %43

43:                                               ; preds = %22, %38, %41
  %44 = phi i32 [ %42, %41 ], [ %23, %38 ], [ 0, %22 ]
  %45 = sub i32 %23, %44
  %46 = zext i32 %44 to i64
  %47 = getelementptr i8, i8* %20, i64 %46
  %48 = ptrtoint i8* %47 to i64
  %49 = zext i32 %45 to i64
  %50 = insertvalue [2 x i64] undef, i64 %49, 0
  %51 = insertvalue [2 x i64] %50, i64 %48, 1
  ret [2 x i64] %51
}

; Function Attrs: norecurse nounwind readonly ssp
define dso_local i64 @LLVMIRbpchareq(i32 %0, i8* nocapture readonly %1, i32 %2, i8* nocapture readonly %3) local_unnamed_addr #1 {
  br label %5

5:                                                ; preds = %9, %4
  %6 = phi i32 [ %0, %4 ], [ %7, %9 ]
  %7 = add i32 %6, -1
  %8 = icmp sgt i32 %7, -1
  br i1 %8, label %9, label %14

9:                                                ; preds = %5
  %10 = zext i32 %7 to i64
  %11 = getelementptr i8, i8* %1, i64 %10
  %12 = load i8, i8* %11, align 1
  %13 = icmp eq i8 %12, 32
  br i1 %13, label %5, label %14

14:                                               ; preds = %9, %5
  br label %15

15:                                               ; preds = %14, %19
  %16 = phi i32 [ %17, %19 ], [ %2, %14 ]
  %17 = add i32 %16, -1
  %18 = icmp sgt i32 %17, -1
  br i1 %18, label %19, label %24

19:                                               ; preds = %15
  %20 = zext i32 %17 to i64
  %21 = getelementptr i8, i8* %3, i64 %20
  %22 = load i8, i8* %21, align 1
  %23 = icmp eq i8 %22, 32
  br i1 %23, label %15, label %24

24:                                               ; preds = %19, %15
  %25 = icmp eq i32 %6, %16
  br i1 %25, label %26, label %40

26:                                               ; preds = %24
  %27 = icmp eq i32 %6, 0
  br i1 %27, label %40, label %28

28:                                               ; preds = %26, %35
  %29 = phi i8* [ %37, %35 ], [ %1, %26 ]
  %30 = phi i8* [ %36, %35 ], [ %3, %26 ]
  %31 = phi i32 [ %38, %35 ], [ %6, %26 ]
  %32 = load i8, i8* %29, align 1
  %33 = load i8, i8* %30, align 1
  %34 = icmp eq i8 %32, %33
  br i1 %34, label %35, label %40

35:                                               ; preds = %28
  %36 = getelementptr i8, i8* %30, i64 1
  %37 = getelementptr i8, i8* %29, i64 1
  %38 = add i32 %31, -1
  %39 = icmp eq i32 %38, 0
  br i1 %39, label %40, label %28

40:                                               ; preds = %28, %35, %24, %26
  %41 = phi i64 [ 1, %26 ], [ 0, %24 ], [ 0, %28 ], [ 1, %35 ]
  ret i64 %41
}

; Function Attrs: norecurse nounwind readonly ssp
define dso_local i64 @LLVMIRbpcharne(i32 %0, i8* nocapture readonly %1, i32 %2, i8* nocapture readonly %3) local_unnamed_addr #1 {
  br label %5

5:                                                ; preds = %9, %4
  %6 = phi i32 [ %0, %4 ], [ %7, %9 ]
  %7 = add i32 %6, -1
  %8 = icmp sgt i32 %7, -1
  br i1 %8, label %9, label %14

9:                                                ; preds = %5
  %10 = zext i32 %7 to i64
  %11 = getelementptr i8, i8* %1, i64 %10
  %12 = load i8, i8* %11, align 1
  %13 = icmp eq i8 %12, 32
  br i1 %13, label %5, label %14

14:                                               ; preds = %9, %5
  br label %15

15:                                               ; preds = %14, %19
  %16 = phi i32 [ %17, %19 ], [ %2, %14 ]
  %17 = add i32 %16, -1
  %18 = icmp sgt i32 %17, -1
  br i1 %18, label %19, label %24

19:                                               ; preds = %15
  %20 = zext i32 %17 to i64
  %21 = getelementptr i8, i8* %3, i64 %20
  %22 = load i8, i8* %21, align 1
  %23 = icmp eq i8 %22, 32
  br i1 %23, label %15, label %24

24:                                               ; preds = %19, %15
  %25 = icmp eq i32 %6, %16
  br i1 %25, label %26, label %40

26:                                               ; preds = %24
  %27 = icmp eq i32 %6, 0
  br i1 %27, label %40, label %28

28:                                               ; preds = %26, %35
  %29 = phi i8* [ %37, %35 ], [ %1, %26 ]
  %30 = phi i8* [ %36, %35 ], [ %3, %26 ]
  %31 = phi i32 [ %38, %35 ], [ %6, %26 ]
  %32 = load i8, i8* %29, align 1
  %33 = load i8, i8* %30, align 1
  %34 = icmp eq i8 %32, %33
  br i1 %34, label %35, label %40

35:                                               ; preds = %28
  %36 = getelementptr i8, i8* %30, i64 1
  %37 = getelementptr i8, i8* %29, i64 1
  %38 = add i32 %31, -1
  %39 = icmp eq i32 %38, 0
  br i1 %39, label %40, label %28

40:                                               ; preds = %28, %35, %24, %26
  %41 = phi i64 [ 0, %26 ], [ 1, %24 ], [ 1, %28 ], [ 0, %35 ]
  ret i64 %41
}

; Function Attrs: norecurse nounwind readonly ssp
define dso_local i64 @LLVMIRtexteq(i32 %0, i8* nocapture readonly %1, i32 %2, i8* nocapture readonly %3) local_unnamed_addr #1 {
  br label %5

5:                                                ; preds = %9, %4
  %6 = phi i32 [ %0, %4 ], [ %7, %9 ]
  %7 = add i32 %6, -1
  %8 = icmp sgt i32 %7, -1
  br i1 %8, label %9, label %14

9:                                                ; preds = %5
  %10 = zext i32 %7 to i64
  %11 = getelementptr i8, i8* %1, i64 %10
  %12 = load i8, i8* %11, align 1
  %13 = icmp eq i8 %12, 32
  br i1 %13, label %5, label %14

14:                                               ; preds = %9, %5
  br label %15

15:                                               ; preds = %14, %19
  %16 = phi i32 [ %17, %19 ], [ %2, %14 ]
  %17 = add i32 %16, -1
  %18 = icmp sgt i32 %17, -1
  br i1 %18, label %19, label %24

19:                                               ; preds = %15
  %20 = zext i32 %17 to i64
  %21 = getelementptr i8, i8* %3, i64 %20
  %22 = load i8, i8* %21, align 1
  %23 = icmp eq i8 %22, 32
  br i1 %23, label %15, label %24

24:                                               ; preds = %19, %15
  %25 = icmp eq i32 %6, %16
  br i1 %25, label %26, label %40

26:                                               ; preds = %24
  %27 = icmp eq i32 %6, 0
  br i1 %27, label %40, label %28

28:                                               ; preds = %26, %35
  %29 = phi i8* [ %37, %35 ], [ %1, %26 ]
  %30 = phi i8* [ %36, %35 ], [ %3, %26 ]
  %31 = phi i32 [ %38, %35 ], [ %6, %26 ]
  %32 = load i8, i8* %29, align 1
  %33 = load i8, i8* %30, align 1
  %34 = icmp eq i8 %32, %33
  br i1 %34, label %35, label %40

35:                                               ; preds = %28
  %36 = getelementptr i8, i8* %30, i64 1
  %37 = getelementptr i8, i8* %29, i64 1
  %38 = add i32 %31, -1
  %39 = icmp eq i32 %38, 0
  br i1 %39, label %40, label %28

40:                                               ; preds = %28, %35, %24, %26
  %41 = phi i64 [ 1, %26 ], [ 0, %24 ], [ 0, %28 ], [ 1, %35 ]
  ret i64 %41
}

; Function Attrs: norecurse nounwind readonly ssp
define dso_local i64 @LLVMIRtextneq(i32 %0, i8* nocapture readonly %1, i32 %2, i8* nocapture readonly %3) local_unnamed_addr #1 {
  br label %5

5:                                                ; preds = %9, %4
  %6 = phi i32 [ %0, %4 ], [ %7, %9 ]
  %7 = add i32 %6, -1
  %8 = icmp sgt i32 %7, -1
  br i1 %8, label %9, label %14

9:                                                ; preds = %5
  %10 = zext i32 %7 to i64
  %11 = getelementptr i8, i8* %1, i64 %10
  %12 = load i8, i8* %11, align 1
  %13 = icmp eq i8 %12, 32
  br i1 %13, label %5, label %14

14:                                               ; preds = %9, %5
  br label %15

15:                                               ; preds = %14, %19
  %16 = phi i32 [ %17, %19 ], [ %2, %14 ]
  %17 = add i32 %16, -1
  %18 = icmp sgt i32 %17, -1
  br i1 %18, label %19, label %24

19:                                               ; preds = %15
  %20 = zext i32 %17 to i64
  %21 = getelementptr i8, i8* %3, i64 %20
  %22 = load i8, i8* %21, align 1
  %23 = icmp eq i8 %22, 32
  br i1 %23, label %15, label %24

24:                                               ; preds = %19, %15
  %25 = icmp eq i32 %6, %16
  br i1 %25, label %26, label %40

26:                                               ; preds = %24
  %27 = icmp eq i32 %6, 0
  br i1 %27, label %40, label %28

28:                                               ; preds = %26, %35
  %29 = phi i8* [ %37, %35 ], [ %1, %26 ]
  %30 = phi i8* [ %36, %35 ], [ %3, %26 ]
  %31 = phi i32 [ %38, %35 ], [ %6, %26 ]
  %32 = load i8, i8* %29, align 1
  %33 = load i8, i8* %30, align 1
  %34 = icmp eq i8 %32, %33
  br i1 %34, label %35, label %40

35:                                               ; preds = %28
  %36 = getelementptr i8, i8* %30, i64 1
  %37 = getelementptr i8, i8* %29, i64 1
  %38 = add i32 %31, -1
  %39 = icmp eq i32 %38, 0
  br i1 %39, label %40, label %28

40:                                               ; preds = %28, %35, %24, %26
  %41 = phi i64 [ 0, %26 ], [ 1, %24 ], [ 1, %28 ], [ 0, %35 ]
  ret i64 %41
}

; Function Attrs: norecurse nounwind readonly ssp
define dso_local [2 x i64] @LLVMIRsubstring_ASCII(i32 %0, i8* %1, i32 %2, i32 %3) local_unnamed_addr #1 {
  %5 = add i32 %3, %2
  %6 = icmp eq i32 %0, 0
  br i1 %6, label %7, label %9

7:                                                ; preds = %4
  %8 = ptrtoint i8* %1 to i64
  br label %95

9:                                                ; preds = %4
  %10 = sext i32 %0 to i64
  %11 = getelementptr i8, i8* %1, i64 %10
  %12 = icmp sgt i32 %2, 1
  %13 = icmp ugt i8* %11, %1
  %14 = and i1 %12, %13
  br i1 %14, label %15, label %52

15:                                               ; preds = %9, %46
  %16 = phi i8* [ %48, %46 ], [ %1, %9 ]
  %17 = phi i32 [ %34, %46 ], [ 1, %9 ]
  %18 = load i8, i8* %16, align 1
  %19 = sext i8 %18 to i32
  %20 = and i32 %19, 128
  %21 = icmp eq i32 %20, 0
  br i1 %21, label %32, label %22

22:                                               ; preds = %15
  %23 = and i32 %19, 224
  %24 = icmp eq i32 %23, 192
  br i1 %24, label %32, label %25

25:                                               ; preds = %22
  %26 = and i32 %19, 240
  %27 = icmp eq i32 %26, 224
  br i1 %27, label %32, label %28

28:                                               ; preds = %25
  %29 = and i32 %19, 248
  %30 = icmp eq i32 %29, 240
  %31 = select i1 %30, i32 4, i32 1
  br label %32

32:                                               ; preds = %28, %25, %22, %15
  %33 = phi i32 [ 1, %15 ], [ 2, %22 ], [ 3, %25 ], [ %31, %28 ]
  %34 = add i32 %33, %17
  %35 = icmp slt i32 %34, %5
  br i1 %35, label %39, label %36

36:                                               ; preds = %32
  %37 = sub i32 %5, %17
  %38 = sext i32 %37 to i64
  br label %46

39:                                               ; preds = %32
  %40 = icmp slt i32 %34, %2
  br i1 %40, label %44, label %41

41:                                               ; preds = %39
  %42 = sub i32 %2, %17
  %43 = sext i32 %42 to i64
  br label %46

44:                                               ; preds = %39
  %45 = zext i32 %33 to i64
  br label %46

46:                                               ; preds = %36, %44, %41
  %47 = phi i64 [ %38, %36 ], [ %45, %44 ], [ %43, %41 ]
  %48 = getelementptr i8, i8* %16, i64 %47
  %49 = icmp slt i32 %34, %2
  %50 = icmp ult i8* %48, %11
  %51 = and i1 %49, %50
  br i1 %51, label %15, label %52

52:                                               ; preds = %46, %9
  %53 = phi i8* [ %1, %9 ], [ %48, %46 ]
  %54 = phi i1 [ %13, %9 ], [ %50, %46 ]
  br i1 %54, label %55, label %59

55:                                               ; preds = %52
  %56 = icmp sgt i32 %5, %2
  %57 = icmp ult i8* %53, %11
  %58 = and i1 %56, %57
  br i1 %58, label %61, label %89

59:                                               ; preds = %52
  %60 = ptrtoint i8* %1 to i64
  br label %95

61:                                               ; preds = %55, %78
  %62 = phi i8* [ %86, %78 ], [ %53, %55 ]
  %63 = phi i32 [ %80, %78 ], [ %2, %55 ]
  %64 = load i8, i8* %62, align 1
  %65 = sext i8 %64 to i32
  %66 = and i32 %65, 128
  %67 = icmp eq i32 %66, 0
  br i1 %67, label %78, label %68

68:                                               ; preds = %61
  %69 = and i32 %65, 224
  %70 = icmp eq i32 %69, 192
  br i1 %70, label %78, label %71

71:                                               ; preds = %68
  %72 = and i32 %65, 240
  %73 = icmp eq i32 %72, 224
  br i1 %73, label %78, label %74

74:                                               ; preds = %71
  %75 = and i32 %65, 248
  %76 = icmp eq i32 %75, 240
  %77 = select i1 %76, i32 4, i32 1
  br label %78

78:                                               ; preds = %74, %71, %68, %61
  %79 = phi i32 [ 1, %61 ], [ 2, %68 ], [ 3, %71 ], [ %77, %74 ]
  %80 = add i32 %79, %63
  %81 = icmp slt i32 %80, %5
  %82 = sub i32 %5, %63
  %83 = sext i32 %82 to i64
  %84 = zext i32 %79 to i64
  %85 = select i1 %81, i64 %84, i64 %83
  %86 = getelementptr i8, i8* %62, i64 %85
  %87 = icmp ult i8* %86, %11
  %88 = and i1 %81, %87
  br i1 %88, label %61, label %89

89:                                               ; preds = %78, %55
  %90 = phi i8* [ %53, %55 ], [ %86, %78 ]
  %91 = ptrtoint i8* %90 to i64
  %92 = ptrtoint i8* %53 to i64
  %93 = sub i64 %91, %92
  %94 = and i64 %93, 4294967295
  br label %95

95:                                               ; preds = %89, %59, %7
  %96 = phi i64 [ 0, %7 ], [ 0, %59 ], [ %94, %89 ]
  %97 = phi i64 [ %8, %7 ], [ %60, %59 ], [ %92, %89 ]
  %98 = insertvalue [2 x i64] undef, i64 %96, 0
  %99 = insertvalue [2 x i64] %98, i64 %97, 1
  ret [2 x i64] %99
}

; Function Attrs: norecurse nounwind readonly ssp
define dso_local [2 x i64] @LLVMIRsubstring_UTF8(i32 %0, i8* %1, i32 %2, i32 %3) local_unnamed_addr #1 {
  %5 = add i32 %3, %2
  %6 = icmp eq i32 %0, 0
  br i1 %6, label %7, label %9

7:                                                ; preds = %4
  %8 = ptrtoint i8* %1 to i64
  br label %81

9:                                                ; preds = %4
  %10 = sext i32 %0 to i64
  %11 = getelementptr i8, i8* %1, i64 %10
  %12 = icmp sgt i32 %2, 1
  %13 = icmp ugt i8* %11, %1
  %14 = and i1 %12, %13
  br i1 %14, label %15, label %39

15:                                               ; preds = %9, %32
  %16 = phi i8* [ %34, %32 ], [ %1, %9 ]
  %17 = phi i32 [ %35, %32 ], [ 1, %9 ]
  %18 = load i8, i8* %16, align 1
  %19 = sext i8 %18 to i32
  %20 = and i32 %19, 128
  %21 = icmp eq i32 %20, 0
  br i1 %21, label %32, label %22

22:                                               ; preds = %15
  %23 = and i32 %19, 224
  %24 = icmp eq i32 %23, 192
  br i1 %24, label %32, label %25

25:                                               ; preds = %22
  %26 = and i32 %19, 240
  %27 = icmp eq i32 %26, 224
  br i1 %27, label %32, label %28

28:                                               ; preds = %25
  %29 = and i32 %19, 248
  %30 = icmp eq i32 %29, 240
  %31 = select i1 %30, i64 4, i64 1
  br label %32

32:                                               ; preds = %28, %25, %22, %15
  %33 = phi i64 [ 1, %15 ], [ 2, %22 ], [ 3, %25 ], [ %31, %28 ]
  %34 = getelementptr i8, i8* %16, i64 %33
  %35 = add nuw nsw i32 %17, 1
  %36 = icmp slt i32 %35, %2
  %37 = icmp ult i8* %34, %11
  %38 = and i1 %36, %37
  br i1 %38, label %15, label %39

39:                                               ; preds = %32, %9
  %40 = phi i8* [ %1, %9 ], [ %34, %32 ]
  %41 = phi i1 [ %13, %9 ], [ %37, %32 ]
  br i1 %41, label %42, label %46

42:                                               ; preds = %39
  %43 = icmp sgt i32 %5, %2
  %44 = icmp ult i8* %40, %11
  %45 = and i1 %43, %44
  br i1 %45, label %48, label %72

46:                                               ; preds = %39
  %47 = ptrtoint i8* %1 to i64
  br label %81

48:                                               ; preds = %42, %65
  %49 = phi i8* [ %67, %65 ], [ %40, %42 ]
  %50 = phi i32 [ %68, %65 ], [ %2, %42 ]
  %51 = load i8, i8* %49, align 1
  %52 = sext i8 %51 to i32
  %53 = and i32 %52, 128
  %54 = icmp eq i32 %53, 0
  br i1 %54, label %65, label %55

55:                                               ; preds = %48
  %56 = and i32 %52, 224
  %57 = icmp eq i32 %56, 192
  br i1 %57, label %65, label %58

58:                                               ; preds = %55
  %59 = and i32 %52, 240
  %60 = icmp eq i32 %59, 224
  br i1 %60, label %65, label %61

61:                                               ; preds = %58
  %62 = and i32 %52, 248
  %63 = icmp eq i32 %62, 240
  %64 = select i1 %63, i64 4, i64 1
  br label %65

65:                                               ; preds = %61, %58, %55, %48
  %66 = phi i64 [ 1, %48 ], [ 2, %55 ], [ 3, %58 ], [ %64, %61 ]
  %67 = getelementptr i8, i8* %49, i64 %66
  %68 = add nsw i32 %50, 1
  %69 = icmp slt i32 %68, %5
  %70 = icmp ult i8* %67, %11
  %71 = and i1 %69, %70
  br i1 %71, label %48, label %72

72:                                               ; preds = %65, %42
  %73 = phi i8* [ %40, %42 ], [ %67, %65 ]
  %74 = ptrtoint i8* %73 to i64
  %75 = ptrtoint i8* %40 to i64
  %76 = sub i64 %74, %75
  %77 = icmp ugt i8* %73, %11
  %78 = add i64 %76, 4294967295
  %79 = select i1 %77, i64 %78, i64 %76
  %80 = and i64 %79, 4294967295
  br label %81

81:                                               ; preds = %72, %46, %7
  %82 = phi i64 [ 0, %7 ], [ 0, %46 ], [ %80, %72 ]
  %83 = phi i64 [ %8, %7 ], [ %47, %46 ], [ %75, %72 ]
  %84 = insertvalue [2 x i64] undef, i64 %82, 0
  %85 = insertvalue [2 x i64] %84, i64 %83, 1
  ret [2 x i64] %85
}

; Function Attrs: ssp
define dso_local i64 @LLVMIRtextgt(i32 %0, i8* %1, i32 %2, i8* %3, i32 %4) local_unnamed_addr #2 {
  br label %6

6:                                                ; preds = %10, %5
  %7 = phi i32 [ %0, %5 ], [ %8, %10 ]
  %8 = add i32 %7, -1
  %9 = icmp sgt i32 %8, -1
  br i1 %9, label %10, label %15

10:                                               ; preds = %6
  %11 = zext i32 %8 to i64
  %12 = getelementptr i8, i8* %1, i64 %11
  %13 = load i8, i8* %12, align 1
  %14 = icmp eq i8 %13, 32
  br i1 %14, label %6, label %15

15:                                               ; preds = %10, %6
  br label %16

16:                                               ; preds = %15, %20
  %17 = phi i32 [ %18, %20 ], [ %2, %15 ]
  %18 = add i32 %17, -1
  %19 = icmp sgt i32 %18, -1
  br i1 %19, label %20, label %25

20:                                               ; preds = %16
  %21 = zext i32 %18 to i64
  %22 = getelementptr i8, i8* %3, i64 %21
  %23 = load i8, i8* %22, align 1
  %24 = icmp eq i8 %23, 32
  br i1 %24, label %16, label %25

25:                                               ; preds = %20, %16
  %26 = icmp slt i32 %7, %17
  %27 = select i1 %26, i32 %7, i32 %17
  %28 = icmp sgt i32 %27, 0
  br i1 %28, label %29, label %49

29:                                               ; preds = %25
  %30 = zext i32 %27 to i64
  br label %33

31:                                               ; preds = %43
  %32 = icmp ult i64 %45, %30
  br i1 %32, label %33, label %49

33:                                               ; preds = %29, %31
  %34 = phi i64 [ 0, %29 ], [ %45, %31 ]
  %35 = getelementptr i8, i8* %1, i64 %34
  %36 = load i8, i8* %35, align 1
  %37 = add i8 %36, -48
  %38 = icmp ult i8 %37, 10
  br i1 %38, label %39, label %46

39:                                               ; preds = %33
  %40 = getelementptr i8, i8* %3, i64 %34
  %41 = load i8, i8* %40, align 1
  %42 = icmp slt i8 %36, %41
  br i1 %42, label %51, label %43

43:                                               ; preds = %39
  %44 = icmp sgt i8 %36, %41
  %45 = add nuw nsw i64 %34, 1
  br i1 %44, label %51, label %31

46:                                               ; preds = %33
  %47 = tail call i32 @_Z10varstr_cmpPciS_ij(i8* nonnull %1, i32 %7, i8* %3, i32 %17, i32 %4)
  %48 = icmp sgt i32 %47, 0
  br label %51

49:                                               ; preds = %31, %25
  %50 = icmp sgt i32 %7, %17
  br label %51

51:                                               ; preds = %43, %39, %49, %46
  %52 = phi i1 [ %48, %46 ], [ %50, %49 ], [ true, %43 ], [ false, %39 ]
  %53 = zext i1 %52 to i64
  ret i64 %53
}

declare dso_local i32 @_Z10varstr_cmpPciS_ij(i8*, i32, i8*, i32, i32) local_unnamed_addr #3

; Function Attrs: ssp
define dso_local i64 @LLVMIRtextlt(i32 %0, i8* %1, i32 %2, i8* %3, i32 %4) local_unnamed_addr #2 {
  br label %6

6:                                                ; preds = %10, %5
  %7 = phi i32 [ %0, %5 ], [ %8, %10 ]
  %8 = add i32 %7, -1
  %9 = icmp sgt i32 %8, -1
  br i1 %9, label %10, label %15

10:                                               ; preds = %6
  %11 = zext i32 %8 to i64
  %12 = getelementptr i8, i8* %1, i64 %11
  %13 = load i8, i8* %12, align 1
  %14 = icmp eq i8 %13, 32
  br i1 %14, label %6, label %15

15:                                               ; preds = %10, %6
  br label %16

16:                                               ; preds = %15, %20
  %17 = phi i32 [ %18, %20 ], [ %2, %15 ]
  %18 = add i32 %17, -1
  %19 = icmp sgt i32 %18, -1
  br i1 %19, label %20, label %25

20:                                               ; preds = %16
  %21 = zext i32 %18 to i64
  %22 = getelementptr i8, i8* %3, i64 %21
  %23 = load i8, i8* %22, align 1
  %24 = icmp eq i8 %23, 32
  br i1 %24, label %16, label %25

25:                                               ; preds = %20, %16
  %26 = icmp slt i32 %7, %17
  %27 = select i1 %26, i32 %7, i32 %17
  %28 = icmp sgt i32 %27, 0
  br i1 %28, label %29, label %49

29:                                               ; preds = %25
  %30 = zext i32 %27 to i64
  br label %33

31:                                               ; preds = %43
  %32 = icmp ult i64 %45, %30
  br i1 %32, label %33, label %49

33:                                               ; preds = %29, %31
  %34 = phi i64 [ 0, %29 ], [ %45, %31 ]
  %35 = getelementptr i8, i8* %1, i64 %34
  %36 = load i8, i8* %35, align 1
  %37 = add i8 %36, -48
  %38 = icmp ult i8 %37, 10
  br i1 %38, label %39, label %46

39:                                               ; preds = %33
  %40 = getelementptr i8, i8* %3, i64 %34
  %41 = load i8, i8* %40, align 1
  %42 = icmp slt i8 %36, %41
  br i1 %42, label %49, label %43

43:                                               ; preds = %39
  %44 = icmp sgt i8 %36, %41
  %45 = add nuw nsw i64 %34, 1
  br i1 %44, label %49, label %31

46:                                               ; preds = %33
  %47 = tail call i32 @_Z10varstr_cmpPciS_ij(i8* nonnull %1, i32 %7, i8* %3, i32 %17, i32 %4)
  %48 = icmp slt i32 %47, 0
  br label %49

49:                                               ; preds = %39, %43, %31, %25, %46
  %50 = phi i1 [ %48, %46 ], [ %26, %25 ], [ true, %39 ], [ false, %43 ], [ %26, %31 ]
  %51 = zext i1 %50 to i64
  ret i64 %51
}

; Function Attrs: norecurse nounwind readonly ssp
define dso_local i64 @int4_bool2(%struct.FunctionCallInfoData* nocapture readonly %0) local_unnamed_addr #1 {
  %2 = getelementptr inbounds %struct.FunctionCallInfoData, %struct.FunctionCallInfoData* %0, i64 0, i32 7
  %3 = load i64*, i64** %2, align 8
  %4 = load i64, i64* %3, align 8
  %5 = trunc i64 %4 to i32
  %6 = icmp ne i32 %5, 0
  %7 = zext i1 %6 to i64
  ret i64 %7
}

; Function Attrs: norecurse nounwind readonly ssp
define dso_local i64 @LLVMIRtextlike(i32 %0, i8* nocapture readonly %1, i32 %2, i8* nocapture readonly %3) local_unnamed_addr #1 {
  %5 = add i32 %2, -1
  %6 = sext i32 %5 to i64
  %7 = getelementptr i8, i8* %3, i64 %6
  %8 = load i8, i8* %7, align 1
  %9 = icmp eq i8 %8, 37
  br i1 %9, label %10, label %12

10:                                               ; preds = %4
  %11 = icmp sgt i32 %5, %0
  br i1 %11, label %41, label %24

12:                                               ; preds = %4
  %13 = load i8, i8* %3, align 1
  %14 = icmp eq i8 %13, 37
  br i1 %14, label %15, label %22

15:                                               ; preds = %12
  %16 = icmp sgt i32 %5, %0
  br i1 %16, label %41, label %17

17:                                               ; preds = %15
  %18 = getelementptr i8, i8* %3, i64 1
  %19 = sub i32 %0, %5
  %20 = sext i32 %19 to i64
  %21 = getelementptr i8, i8* %1, i64 %20
  br label %24

22:                                               ; preds = %12
  %23 = icmp eq i32 %0, %2
  br i1 %23, label %24, label %41

24:                                               ; preds = %22, %17, %10
  %25 = phi i32 [ %5, %10 ], [ %5, %17 ], [ %2, %22 ]
  %26 = phi i8* [ %1, %10 ], [ %21, %17 ], [ %1, %22 ]
  %27 = phi i8* [ %3, %10 ], [ %18, %17 ], [ %3, %22 ]
  %28 = icmp eq i32 %25, 0
  br i1 %28, label %41, label %29

29:                                               ; preds = %24, %36
  %30 = phi i8* [ %38, %36 ], [ %27, %24 ]
  %31 = phi i8* [ %37, %36 ], [ %26, %24 ]
  %32 = phi i32 [ %39, %36 ], [ %25, %24 ]
  %33 = load i8, i8* %31, align 1
  %34 = load i8, i8* %30, align 1
  %35 = icmp eq i8 %33, %34
  br i1 %35, label %36, label %41

36:                                               ; preds = %29
  %37 = getelementptr i8, i8* %31, i64 1
  %38 = getelementptr i8, i8* %30, i64 1
  %39 = add i32 %32, -1
  %40 = icmp eq i32 %39, 0
  br i1 %40, label %41, label %29

41:                                               ; preds = %29, %36, %24, %22, %15, %10
  %42 = phi i64 [ 0, %10 ], [ 0, %15 ], [ 0, %22 ], [ 1, %24 ], [ 0, %29 ], [ 1, %36 ]
  ret i64 %42
}

; Function Attrs: nounwind readonly ssp
define dso_local i64 @LLVMIRtextnotlike(i8* nocapture %0, i32 %1, i8* nocapture %2, i32 %3) local_unnamed_addr #4 {
  %5 = icmp sgt i32 %1, 0
  %6 = icmp sgt i32 %3, 0
  %7 = and i1 %5, %6
  br i1 %7, label %8, label %50

8:                                                ; preds = %4, %42
  %9 = phi i8* [ %43, %42 ], [ %0, %4 ]
  %10 = phi i32 [ %46, %42 ], [ %3, %4 ]
  %11 = phi i8* [ %45, %42 ], [ %2, %4 ]
  %12 = phi i32 [ %44, %42 ], [ %1, %4 ]
  %13 = load i8, i8* %11, align 1
  %14 = icmp eq i8 %13, 37
  br i1 %14, label %15, label %39

15:                                               ; preds = %8, %21
  %16 = phi i8* [ %18, %21 ], [ %11, %8 ]
  %17 = phi i32 [ %19, %21 ], [ %10, %8 ]
  %18 = getelementptr i8, i8* %16, i64 1
  %19 = add i32 %17, -1
  %20 = icmp sgt i32 %19, 0
  br i1 %20, label %21, label %72

21:                                               ; preds = %15
  %22 = load i8, i8* %18, align 1
  %23 = icmp eq i8 %22, 37
  br i1 %23, label %15, label %24

24:                                               ; preds = %21
  %25 = icmp sgt i32 %12, 0
  br i1 %25, label %26, label %72

26:                                               ; preds = %24, %35
  %27 = phi i8* [ %36, %35 ], [ %9, %24 ]
  %28 = phi i32 [ %37, %35 ], [ %12, %24 ]
  %29 = load i8, i8* %27, align 1
  %30 = icmp eq i8 %29, %22
  br i1 %30, label %31, label %35

31:                                               ; preds = %26
  %32 = tail call i64 @LLVMIRtextnotlike(i8* nonnull %27, i32 %28, i8* nonnull %18, i32 %19)
  %33 = trunc i64 %32 to i32
  %34 = icmp eq i32 %33, 0
  br i1 %34, label %35, label %69

35:                                               ; preds = %31, %26
  %36 = getelementptr i8, i8* %27, i64 1
  %37 = add nsw i32 %28, -1
  %38 = icmp sgt i32 %28, 1
  br i1 %38, label %26, label %72

39:                                               ; preds = %8
  %40 = load i8, i8* %9, align 1
  %41 = icmp eq i8 %13, %40
  br i1 %41, label %42, label %72

42:                                               ; preds = %39
  %43 = getelementptr i8, i8* %9, i64 1
  %44 = add nsw i32 %12, -1
  %45 = getelementptr i8, i8* %11, i64 1
  %46 = add nsw i32 %10, -1
  %47 = icmp sgt i32 %12, 1
  %48 = icmp sgt i32 %10, 1
  %49 = and i1 %47, %48
  br i1 %49, label %8, label %50

50:                                               ; preds = %42, %4
  %51 = phi i8* [ %2, %4 ], [ %45, %42 ]
  %52 = phi i32 [ %3, %4 ], [ %46, %42 ]
  %53 = phi i1 [ %5, %4 ], [ %47, %42 ]
  br i1 %53, label %72, label %54

54:                                               ; preds = %50
  %55 = icmp sgt i32 %52, 0
  br i1 %55, label %56, label %65

56:                                               ; preds = %54, %61
  %57 = phi i32 [ %63, %61 ], [ %52, %54 ]
  %58 = phi i8* [ %62, %61 ], [ %51, %54 ]
  %59 = load i8, i8* %58, align 1
  %60 = icmp eq i8 %59, 37
  br i1 %60, label %61, label %65

61:                                               ; preds = %56
  %62 = getelementptr i8, i8* %58, i64 1
  %63 = add nsw i32 %57, -1
  %64 = icmp sgt i32 %57, 1
  br i1 %64, label %56, label %65

65:                                               ; preds = %56, %61, %54
  %66 = phi i32 [ %52, %54 ], [ %63, %61 ], [ %57, %56 ]
  %67 = icmp slt i32 %66, 1
  %68 = select i1 %67, i64 1, i64 -1
  br label %72

69:                                               ; preds = %31
  %70 = shl i64 %32, 32
  %71 = ashr exact i64 %70, 32
  br label %72

72:                                               ; preds = %39, %15, %35, %24, %69, %65, %50
  %73 = phi i64 [ 0, %50 ], [ %68, %65 ], [ %71, %69 ], [ -1, %24 ], [ -1, %35 ], [ 1, %15 ], [ 0, %39 ]
  ret i64 %73
}

; Function Attrs: norecurse nounwind readonly ssp
define dso_local [2 x i64] @LLVMIRBINum2int8(i64 %0) local_unnamed_addr #1 {
  %2 = inttoptr i64 %0 to %struct.NumericData*
  %3 = getelementptr inbounds %struct.NumericData, %struct.NumericData* %2, i64 0, i32 1, i32 0, i32 0
  %4 = load i16, i16* %3, align 4
  %5 = getelementptr inbounds %struct.NumericData, %struct.NumericData* %2, i64 0, i32 1, i32 0, i32 1
  %6 = bitcast i16* %5 to i64*
  %7 = load i64, i64* %6, align 2
  %8 = and i16 %4, 255
  %9 = zext i16 %8 to i64
  %10 = insertvalue [2 x i64] undef, i64 %9, 0
  %11 = insertvalue [2 x i64] %10, i64 %7, 1
  ret [2 x i64] %11
}

; Function Attrs: norecurse nounwind readonly ssp
define dso_local i1 @GetStatusofHashAggRunner(%class.HashAggRunner* nocapture readonly %0) local_unnamed_addr #1 {
  %2 = getelementptr inbounds %class.HashAggRunner, %class.HashAggRunner* %0, i64 0, i32 0, i32 9
  %3 = load i8, i8* %2, align 8, !range !2
  %4 = icmp ne i8 %3, 0
  ret i1 %4
}

; Function Attrs: norecurse nounwind readonly ssp
define dso_local i32 @GetmkeyofSortAggRunner(%class.SortAggRunner* nocapture readonly %0) local_unnamed_addr #1 {
  %2 = getelementptr inbounds %class.SortAggRunner, %class.SortAggRunner* %0, i64 0, i32 0, i32 0, i32 19
  %3 = load i32, i32* %2, align 4
  ret i32 %3
}

; Function Attrs: norecurse nounwind readonly ssp
define dso_local i64 @BigI64pos(%struct.bictl* nocapture readonly %0) local_unnamed_addr #1 {
  %2 = getelementptr inbounds %struct.bictl, %struct.bictl* %0, i64 0, i32 0
  %3 = load i64, i64* %2, align 8
  ret i64 %3
}

; Function Attrs: norecurse nounwind readnone ssp
define dso_local i64 @Bi64LoadGlobalVar(i64 %0, i32 %1) local_unnamed_addr #5 {
  %3 = sext i32 %1 to i64
  %4 = getelementptr [20 x i64], [20 x i64]* @Int64MultiOutOfBound, i64 0, i64 %3
  %5 = load i64, i64* %4, align 8
  %6 = icmp ult i64 %5, %0
  br i1 %6, label %7, label %10

7:                                                ; preds = %2
  %8 = uitofp i64 %0 to double
  %9 = fptoui double %8 to i64
  br label %18

10:                                               ; preds = %2
  %11 = getelementptr [20 x i64], [20 x i64]* @ScaleMultipler, i64 0, i64 %3
  %12 = load i64, i64* %11, align 8
  %13 = icmp ult i64 %12, %0
  br i1 %13, label %14, label %18

14:                                               ; preds = %10
  %15 = uitofp i64 %0 to double
  %16 = fmul double %15, 2.000000e+00
  %17 = fptoui double %16 to i64
  br label %18

18:                                               ; preds = %10, %14, %7
  %19 = phi i64 [ %9, %7 ], [ %17, %14 ], [ undef, %10 ]
  ret i64 %19
}

; Function Attrs: ssp
define dso_local i64 @Simplebi64add64CodeGen(i32 %0, %struct.NumericData* nocapture readonly %1, %struct.NumericData* nocapture readonly %2) local_unnamed_addr #2 {
  %4 = getelementptr inbounds %struct.NumericData, %struct.NumericData* %1, i64 0, i32 1, i32 0, i32 0
  %5 = load i16, i16* %4, align 4
  %6 = getelementptr inbounds %struct.NumericData, %struct.NumericData* %2, i64 0, i32 1, i32 0, i32 0
  %7 = load i16, i16* %6, align 4
  %8 = getelementptr inbounds %struct.NumericData, %struct.NumericData* %1, i64 0, i32 1, i32 0, i32 1
  %9 = bitcast i16* %8 to i64*
  %10 = load i64, i64* %9, align 2
  %11 = getelementptr inbounds %struct.NumericData, %struct.NumericData* %2, i64 0, i32 1, i32 0, i32 1
  %12 = bitcast i16* %11 to i64*
  %13 = load i64, i64* %12, align 2
  %14 = and i16 %5, 255
  %15 = and i16 %7, 255
  %16 = icmp ugt i16 %14, %15
  %17 = select i1 %16, i16 %14, i16 %15
  %18 = zext i16 %17 to i32
  switch i32 %0, label %36 [
    i32 0, label %19
    i32 2, label %21
  ]

19:                                               ; preds = %3
  %20 = add i64 %13, %10
  br label %51

21:                                               ; preds = %3
  %22 = zext i16 %15 to i32
  %23 = sub nsw i32 %18, %22
  %24 = load i8, i8* @assert_enabled, align 1, !range !2
  %25 = icmp eq i8 %24, 0
  %26 = icmp ult i32 %23, 39
  %27 = or i1 %26, %25
  br i1 %27, label %29, label %28

28:                                               ; preds = %21
  tail call void @_Z20ExceptionalConditionPKcS0_S0_i(i8* getelementptr inbounds ([29 x i8], [29 x i8]* @.str, i64 0, i64 0), i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([41 x i8], [41 x i8]* @.str.2, i64 0, i64 0), i32 114)
  br label %29

29:                                               ; preds = %21, %28
  %30 = sext i32 %23 to i64
  %31 = getelementptr [39 x i128], [39 x i128]* @_ZZ18getScaleMultiplieriE6values, i64 0, i64 %30
  %32 = load i128, i128* %31, align 16
  %33 = trunc i128 %32 to i64
  %34 = mul i64 %13, %33
  %35 = add i64 %34, %10
  br label %51

36:                                               ; preds = %3
  %37 = zext i16 %14 to i32
  %38 = sub nsw i32 %18, %37
  %39 = load i8, i8* @assert_enabled, align 1, !range !2
  %40 = icmp eq i8 %39, 0
  %41 = icmp ult i32 %38, 39
  %42 = or i1 %41, %40
  br i1 %42, label %44, label %43

43:                                               ; preds = %36
  tail call void @_Z20ExceptionalConditionPKcS0_S0_i(i8* getelementptr inbounds ([29 x i8], [29 x i8]* @.str, i64 0, i64 0), i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.1, i64 0, i64 0), i8* getelementptr inbounds ([41 x i8], [41 x i8]* @.str.2, i64 0, i64 0), i32 114)
  br label %44

44:                                               ; preds = %36, %43
  %45 = sext i32 %38 to i64
  %46 = getelementptr [39 x i128], [39 x i128]* @_ZZ18getScaleMultiplieriE6values, i64 0, i64 %45
  %47 = load i128, i128* %46, align 16
  %48 = trunc i128 %47 to i64
  %49 = mul i64 %10, %48
  %50 = add i64 %49, %13
  br label %51

51:                                               ; preds = %44, %29, %19
  %52 = phi i64 [ %50, %44 ], [ %35, %29 ], [ %20, %19 ]
  ret i64 %52
}

; Function Attrs: nounwind ssp
define dso_local i1 @match_key(%class.VectorBatch* nocapture readonly %0, i32 %1, %struct.hashCell* nocapture readonly %2, i32 %3) local_unnamed_addr #6 {
  %5 = icmp sgt i32 %3, 0
  br i1 %5, label %6, label %37

6:                                                ; preds = %4
  %7 = getelementptr inbounds %class.VectorBatch, %class.VectorBatch* %0, i64 0, i32 4
  %8 = load %class.ScalarVector*, %class.ScalarVector** %7, align 8
  %9 = sext i32 %1 to i64
  %10 = zext i32 %3 to i64
  br label %11

11:                                               ; preds = %34, %6
  %12 = phi i64 [ 0, %6 ], [ %35, %34 ]
  %13 = getelementptr %class.ScalarVector, %class.ScalarVector* %8, i64 %12, i32 3
  %14 = load i8*, i8** %13, align 8
  %15 = getelementptr i8, i8* %14, i64 %9
  %16 = load i8, i8* %15, align 1
  %17 = getelementptr %struct.hashCell, %struct.hashCell* %2, i64 0, i32 1, i64 %12, i32 1
  %18 = load i8, i8* %17, align 8
  %19 = or i8 %18, %16
  %20 = and i8 %19, 1
  %21 = icmp eq i8 %20, 0
  br i1 %21, label %22, label %30, !prof !3, !misexpect !4

22:                                               ; preds = %11
  %23 = getelementptr %class.ScalarVector, %class.ScalarVector* %8, i64 %12, i32 5
  %24 = load i64*, i64** %23, align 8
  %25 = getelementptr i64, i64* %24, i64 %9
  %26 = load i64, i64* %25, align 8
  %27 = getelementptr %struct.hashCell, %struct.hashCell* %2, i64 0, i32 1, i64 %12, i32 0
  %28 = load i64, i64* %27, align 8
  %29 = icmp eq i64 %26, %28
  br i1 %29, label %34, label %37

30:                                               ; preds = %11
  %31 = and i8 %16, 1
  %32 = and i8 %31, %18
  %33 = icmp eq i8 %32, 0
  br i1 %33, label %37, label %34, !prof !5, !misexpect !6

34:                                               ; preds = %22, %30
  %35 = add nuw nsw i64 %12, 1
  %36 = icmp eq i64 %35, %10
  br i1 %36, label %37, label %11

37:                                               ; preds = %22, %34, %30, %4
  %38 = phi i1 [ true, %4 ], [ false, %30 ], [ false, %22 ], [ true, %34 ]
  ret i1 %38
}

; Function Attrs: norecurse nounwind readonly ssp
define dso_local i64 @getnumBitsBFImpl(%"class.filter::BloomFilterImpl"* nocapture readonly %0) local_unnamed_addr #1 {
  %2 = getelementptr inbounds %"class.filter::BloomFilterImpl", %"class.filter::BloomFilterImpl"* %0, i64 0, i32 2
  %3 = load i64, i64* %2, align 8
  ret i64 %3
}

; Function Attrs: norecurse nounwind readonly ssp
define dso_local %struct.MemoryContextData* @getSonicContext(%class.SonicEncodingDatumArray* nocapture readonly %0) local_unnamed_addr #1 {
  %2 = getelementptr inbounds %class.SonicEncodingDatumArray, %class.SonicEncodingDatumArray* %0, i64 0, i32 0, i32 1
  %3 = load %struct.MemoryContextData*, %struct.MemoryContextData** %2, align 8
  ret %struct.MemoryContextData* %3
}

; Function Attrs: norecurse nounwind readonly ssp
define dso_local %struct.MemoryContextData* @getHashContext(%class.SonicHashAgg* nocapture readonly %0) local_unnamed_addr #1 {
  %2 = getelementptr inbounds %class.SonicHashAgg, %class.SonicHashAgg* %0, i64 0, i32 0, i32 14, i32 8
  %3 = load %struct.MemoryContextData*, %struct.MemoryContextData** %2, align 8
  ret %struct.MemoryContextData* %3
}

; Function Attrs: norecurse nounwind readonly ssp
define dso_local %class.SonicDatumArray** @getSonicDataArray(%class.SonicHashAgg* nocapture readonly %0) local_unnamed_addr #1 {
  %2 = getelementptr inbounds %class.SonicHashAgg, %class.SonicHashAgg* %0, i64 0, i32 0, i32 1
  %3 = load %class.SonicDatumArray**, %class.SonicDatumArray*** %2, align 8
  ret %class.SonicDatumArray** %3
}

; Function Attrs: norecurse nounwind readonly ssp
define dso_local i8 @getDataNthNullFlag(i32 %0, %class.SonicEncodingDatumArray* nocapture readonly %1) local_unnamed_addr #1 {
  %3 = getelementptr inbounds %class.SonicEncodingDatumArray, %class.SonicEncodingDatumArray* %1, i64 0, i32 0, i32 7
  %4 = load i32, i32* %3, align 8
  %5 = lshr i32 %0, %4
  %6 = getelementptr inbounds %class.SonicEncodingDatumArray, %class.SonicEncodingDatumArray* %1, i64 0, i32 0, i32 5
  %7 = load i32, i32* %6, align 8
  %8 = add i32 %7, -1
  %9 = and i32 %8, %0
  %10 = getelementptr inbounds %class.SonicEncodingDatumArray, %class.SonicEncodingDatumArray* %1, i64 0, i32 0, i32 8
  %11 = load %struct.atom**, %struct.atom*** %10, align 8
  %12 = sext i32 %5 to i64
  %13 = getelementptr %struct.atom*, %struct.atom** %11, i64 %12
  %14 = load %struct.atom*, %struct.atom** %13, align 8
  %15 = getelementptr inbounds %struct.atom, %struct.atom* %14, i64 0, i32 1
  %16 = load i8*, i8** %15, align 8
  %17 = sext i32 %9 to i64
  %18 = getelementptr i8, i8* %16, i64 %17
  %19 = load i8, i8* %18, align 1
  ret i8 %19
}

declare dso_local void @_Z20ExceptionalConditionPKcS0_S0_i(i8*, i8*, i8*, i32) local_unnamed_addr #3

attributes #0 = { nofree norecurse nounwind ssp "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="non-leaf" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+crc,+lse,+neon" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { norecurse nounwind readonly ssp "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="non-leaf" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+crc,+lse,+neon" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { ssp "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="non-leaf" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+crc,+lse,+neon" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="non-leaf" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+crc,+lse,+neon" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind readonly ssp "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="non-leaf" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+crc,+lse,+neon" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { norecurse nounwind readnone ssp "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="non-leaf" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+crc,+lse,+neon" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #6 = { nounwind ssp "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="non-leaf" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+crc,+lse,+neon" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.1 (openEuler 10.0.1-1.oe1 8ce601c3ffe76c3a905ca6c867afe8d18becb409)"}
!2 = !{i8 0, i8 2}
!3 = !{!"branch_weights", i32 2000, i32 1}
!4 = !{!"misexpect", i64 0, i64 2000, i64 1}
!5 = !{!"branch_weights", i32 4004000, i32 1}
!6 = !{!"misexpect", i64 1, i64 2000, i64 1}
