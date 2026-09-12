/*
 * AGE-internal 64-bit integer parsing.
 *
 * ag_scanint8 is AGE's own strict integer scanner (Apache AGE error
 * wording and semantics). It intentionally does not share a name with
 * the host scanint8, whose openGauss signature and error text differ.
 * The host declarations for the SQL-callable int8 functions come from
 * the server header.
 */
#ifndef AG_PLUGIN_UTILS_INT8_H
#define AG_PLUGIN_UTILS_INT8_H

#include "fmgr.h"
#include "utils/int8.h"

extern bool ag_scanint8(const char *str, bool errorOK, int64 *result);

#endif /* AG_PLUGIN_UTILS_INT8_H */
