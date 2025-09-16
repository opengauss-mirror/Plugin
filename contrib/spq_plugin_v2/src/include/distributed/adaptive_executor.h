#ifndef ADAPTIVE_EXECUTOR_H
#define ADAPTIVE_EXECUTOR_H

#include "distributed/multi_physical_planner.h"

extern uint64 ExecuteTaskList(RowModifyLevel modLevel, List* taskList);
extern uint64 ExecuteUtilityTaskList(List* utilityTaskList, bool localExecutionSupported);
extern uint64 ExecuteUtilityTaskListExtended(List* utilityTaskList, int poolSize,
                                             bool localExecutionSupported);
extern uint64 ExecuteTaskListOutsideTransaction(RowModifyLevel modLevel, List* taskList,
                                                int targetPoolSize, List* jobIdList);

#endif /* ADAPTIVE_EXECUTOR_H */
