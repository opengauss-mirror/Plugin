/* -------------------------------------------------------------------------
 *
 * fmgr_hook.h
 *	  Dolphin-side accessors for the per-call coercion context carried in
 *	  FunctionCallExtraData (see fmgr.h in the server).
 *
 * The new fmgr ABI dropped the ccontext member from FunctionCallInfoData;
 * the storage now lives in the optional per-call data.  These helpers
 * allocate that data on demand and read/write the coercion context,
 * restoring the capability for dolphin built-in functions.
 *
 * This header intentionally uses its own include guard so that the
 * definitions take effect regardless of the inclusion order between the
 * server's fmgr.h and dolphin's plugin_utils/fmgr.h.
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * -------------------------------------------------------------------------
 */
#ifndef DOLPHIN_FMGR_HOOK_H
#define DOLPHIN_FMGR_HOOK_H

/* Implemented in plugin_executor/execQual.cpp. */
extern void FunctionCallEnsureExtra(FunctionCallInfoData* fcinfo);

static inline CoercionContext FCINFO_GET_COERCION_CONTEXT(FunctionCallInfoData* fcinfo)
{
    return (fcinfo->extra != NULL) ? fcinfo->extra->ccontext : COERCION_UNKNOWN;
}

static inline void FCINFO_SET_COERCION_CONTEXT(FunctionCallInfoData* fcinfo, CoercionContext ccontext)
{
    FunctionCallEnsureExtra(fcinfo);
    fcinfo->extra->ccontext = ccontext;
}

#endif
