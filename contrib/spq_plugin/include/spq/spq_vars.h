/* -------------------------------------------------------------------------
 *
 * spq_vars.h
 * 	  definitions for OpenGauss-SPQ-specific global variables
 *
 * Portions Copyright (c) 2003-2010, Greenplum inc
 * Portions Copyright (c) 2012-Present VMware, Inc. or its affiliates.
 * Portions Copyright (c) 2022 Huawei Technologies Co.,Ltd.
 *
 * IDENTIFICATION
 * 	    src/include/spq/spq_vars.h
 *
 * NOTES
 * 	  See guc_spq.cpp for variable external specification.
 *
 * -------------------------------------------------------------------------
 */
 
#ifndef SPQ_VARS_H
#define SPQ_VARS_H

#define DEFAULT_PACKET_SIZE 8192
#define SPQ_MIN_PACKET_SIZE 512
#define SPQ_MAX_PACKET_SIZE 65507 /* Max payload for IPv4/UDP (subtract 20 more for IPv6 without extensions) */
 
typedef enum GaussVars_Verbosity {
    GAUSSVARS_VERBOSITY_UNDEFINED = 0,
    GAUSSVARS_VERBOSITY_OFF,
    GAUSSVARS_VERBOSITY_TERSE,
    GAUSSVARS_VERBOSITY_VERBOSE,
    GAUSSVARS_VERBOSITY_DEBUG,
} GaussVars_Verbosity;

#endif /* SPQ_VARS_H */