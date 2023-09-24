/* -------------------------------------------------------------------------
 * spqplugin.h
 *        prototypes for functions in contrib/spq/spqplugin.cpp
 *
 * Portions Copyright (c) 2023 Huawei Technologies Co.,Ltd.
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *        contrib/spq/spqplugin.h
 *
 * -------------------------------------------------------------------------
 */
#ifndef SPQPLUGIN_H
#define SPQPLUGIN_H

extern "C" void _PG_init(void);
extern "C" void _PG_fini(void);
extern "C" void spqplugin_invoke(void);

#endif // SPQPLUGIN_H
