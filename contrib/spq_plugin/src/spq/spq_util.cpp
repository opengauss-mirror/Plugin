/* -------------------------------------------------------------------------
 *
 * spq_util.cpp
 * 	  Internal utility support functions for Greenplum Database/PostgreSQL.
 *
 * Portions Copyright (c) 2005-2011, Greenplum inc
 * Portions Copyright (c) 2012-Present VMware, Inc. or its affiliates.
 *
 *
 * IDENTIFICATION
 * 	    spq/spq_util.cpp
 * -------------------------------------------------------------------------
 */
#include "postgres.h"
#include "spq/spq_util.h"
#include "spq/spq_hash.h"
#include "knl/knl_session.h"
#include "knl/knl_thread.h"
#include "utils/builtins.h"

/*
 * Given total number of primary segment databases and a number of
 * segments to "skip" - this routine creates a boolean map (array) the
 * size of total number of segments and randomly selects several
 * entries (total number of total_to_skip) to be marked as
 * "skipped". This is used for external tables with the 'gpfdist'
 * protocol where we want to get a number of *random* segdbs to
 * connect to a gpfdist client.
 *
 * Caller of this function should pfree skip_map when done with it.
 */
bool *makeRandomSegMap(int total_primaries, int total_to_skip)
{
    int randint;     /* some random int representing a seg    */
    int skipped = 0; /* num segs already marked to be skipped */
    bool *skip_map;

    skip_map = (bool *)palloc(total_primaries * sizeof(bool));
    MemSet(skip_map, false, total_primaries * sizeof(bool));

    while (total_to_skip != skipped) {
        /*
         * create a random int between 0 and (total_primaries - 1).
         */
        randint = spqhashrandomseg(total_primaries);

        /*
         * mark this random index 'true' in the skip map (marked to be
         * skipped) unless it was already marked.
         */
        if (skip_map[randint] == false) {
            skip_map[randint] = true;
            skipped++;
        }
    }

    return skip_map;
}
bool *spq_xforms(void)
{
    return u_sess->attr.attr_spq.spq_optimizer_xforms;
}

int getSpqsegmentCount(void)
{
    int dop = u_sess->opt_cxt.query_dop > 0 ?  u_sess->opt_cxt.query_dop: 1;
    return t_thrd.spq_ctx.num_nodes * dop;
}