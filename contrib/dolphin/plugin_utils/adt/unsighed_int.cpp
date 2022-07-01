/* -------------------------------------------------------------------------
 *
 * varlena.c
 *	  Functions for the variable-length built-in types.
 *
 * Portions Copyright (c) 1996-2012, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 * Portions Copyright (c) 2021, openGauss Contributors
 *
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/varlena.c
 *
 * -------------------------------------------------------------------------
 */
#include "postgres.h"
#include "knl/knl_variable.h"

#include <limits.h>
#include <stdlib.h>
#include <cstring>
#include <cmath>

#include "access/hash.h"
#include "access/tuptoaster.h"
#include "catalog/pg_collation.h"
#include "catalog/pg_type.h"
#include "common/int.h"
#include "lib/hyperloglog.h"
#include "libpq/md5.h"
#include "libpq/pqformat.h"
#include "miscadmin.h"
#include "plugin_parser/scansup.h"
#include "port/pg_bswap.h"
#include "regex/regex.h"
#include "utils/builtins.h"
#include "utils/bytea.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"
#include "utils/numeric.h"
#include "utils/pg_locale.h"
#include "plugin_parser/parser.h"
#include "utils/int8.h"
#include "utils/sortsupport.h"
#include "executor/node/nodeSort.h"
#include "plugin_utils/my_locale.h"
#include "pgxc/groupmgr.h"
#include "plugin_postgres.h"
#include "parser/parse_coerce.h"
#include "catalog/pg_type.h"
#include "workload/cpwlm.h"
#include "utils/varbit.h"
#include "plugin_commands/mysqlmode.h"


#define INTEGER_DIV_INTEGER(arg1, arg2)	\
	float8 result;				\
	if (arg2 == 0) {			\
		ereport(ERROR,		\
			 (errcode(ERRCODE_DIVISION_BY_ZERO),	\
				 errmsg("division by zero")));		\
		/* ensure compiler realizes we mustn't reach the division (gcc bug) */	\
		PG_RETURN_NULL();	\
	}						\
	result = (arg1 * 1.0) / (arg2 * 1.0);				\
	PG_RETURN_FLOAT8(result)


PG_FUNCTION_INFO_V1_PUBLIC(i4toui1);
extern "C" DLL_PUBLIC Datum i4toui1(PG_FUNCTION_ARGS);


PG_FUNCTION_INFO_V1_PUBLIC(uint1in);
PG_FUNCTION_INFO_V1_PUBLIC(uint1out);
PG_FUNCTION_INFO_V1_PUBLIC(uint1recv);
PG_FUNCTION_INFO_V1_PUBLIC(uint1send);
PG_FUNCTION_INFO_V1_PUBLIC(uint1eq);
PG_FUNCTION_INFO_V1_PUBLIC(uint1ne);
PG_FUNCTION_INFO_V1_PUBLIC(uint1lt);
PG_FUNCTION_INFO_V1_PUBLIC(uint1le);
PG_FUNCTION_INFO_V1_PUBLIC(uint1gt);
PG_FUNCTION_INFO_V1_PUBLIC(uint1ge);
PG_FUNCTION_INFO_V1_PUBLIC(uint1cmp);
PG_FUNCTION_INFO_V1_PUBLIC(ui1toi2);
PG_FUNCTION_INFO_V1_PUBLIC(i2toui1);
PG_FUNCTION_INFO_V1_PUBLIC(ui1toi4);

PG_FUNCTION_INFO_V1_PUBLIC(ui1toi8);
PG_FUNCTION_INFO_V1_PUBLIC(i8toui1);
PG_FUNCTION_INFO_V1_PUBLIC(ui1tof4);
PG_FUNCTION_INFO_V1_PUBLIC(f4toui1);
PG_FUNCTION_INFO_V1_PUBLIC(ui1tof8);
PG_FUNCTION_INFO_V1_PUBLIC(f8toui1);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_bool);
PG_FUNCTION_INFO_V1_PUBLIC(bool_uint1);
PG_FUNCTION_INFO_V1_PUBLIC(uint1and);
PG_FUNCTION_INFO_V1_PUBLIC(uint1or);
PG_FUNCTION_INFO_V1_PUBLIC(uint1xor);
PG_FUNCTION_INFO_V1_PUBLIC(uint1not);
PG_FUNCTION_INFO_V1_PUBLIC(uint1shl);
PG_FUNCTION_INFO_V1_PUBLIC(uint1shr);
PG_FUNCTION_INFO_V1_PUBLIC(uint1um);
PG_FUNCTION_INFO_V1_PUBLIC(uint1pl);
PG_FUNCTION_INFO_V1_PUBLIC(uint1mi);
PG_FUNCTION_INFO_V1_PUBLIC(uint1mul);
PG_FUNCTION_INFO_V1_PUBLIC(uint1div);
PG_FUNCTION_INFO_V1_PUBLIC(uint1abs);
PG_FUNCTION_INFO_V1_PUBLIC(uint1mod);
PG_FUNCTION_INFO_V1_PUBLIC(uint1larger);
PG_FUNCTION_INFO_V1_PUBLIC(uint1smaller);
PG_FUNCTION_INFO_V1_PUBLIC(uint1up);
PG_FUNCTION_INFO_V1_PUBLIC(hashuint1);
PG_FUNCTION_INFO_V1_PUBLIC(uint2in);
PG_FUNCTION_INFO_V1_PUBLIC(uint2out);
PG_FUNCTION_INFO_V1_PUBLIC(uint2recv);
PG_FUNCTION_INFO_V1_PUBLIC(uint2send);
PG_FUNCTION_INFO_V1_PUBLIC(uint2eq);
PG_FUNCTION_INFO_V1_PUBLIC(uint2ne);
PG_FUNCTION_INFO_V1_PUBLIC(uint2lt);
PG_FUNCTION_INFO_V1_PUBLIC(uint2le);
PG_FUNCTION_INFO_V1_PUBLIC(uint2gt);
PG_FUNCTION_INFO_V1_PUBLIC(uint2ge);
PG_FUNCTION_INFO_V1_PUBLIC(uint2cmp);
PG_FUNCTION_INFO_V1_PUBLIC(ui1toui2);
PG_FUNCTION_INFO_V1_PUBLIC(ui2toui1);
PG_FUNCTION_INFO_V1_PUBLIC(ui2toi2);
PG_FUNCTION_INFO_V1_PUBLIC(i2toui2);
PG_FUNCTION_INFO_V1_PUBLIC(ui2toi4);
PG_FUNCTION_INFO_V1_PUBLIC(i4toui2);
PG_FUNCTION_INFO_V1_PUBLIC(ui2toi8);
PG_FUNCTION_INFO_V1_PUBLIC(i8toui2);
PG_FUNCTION_INFO_V1_PUBLIC(ui2tof4);
PG_FUNCTION_INFO_V1_PUBLIC(f4toui2);
PG_FUNCTION_INFO_V1_PUBLIC(ui2tof8);
PG_FUNCTION_INFO_V1_PUBLIC(f8toui2);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_bool);
PG_FUNCTION_INFO_V1_PUBLIC(bool_uint2);
PG_FUNCTION_INFO_V1_PUBLIC(uint2and);
PG_FUNCTION_INFO_V1_PUBLIC(uint2or);
PG_FUNCTION_INFO_V1_PUBLIC(uint2xor);
PG_FUNCTION_INFO_V1_PUBLIC(uint2not);
PG_FUNCTION_INFO_V1_PUBLIC(uint2shl);
PG_FUNCTION_INFO_V1_PUBLIC(uint2shr);
PG_FUNCTION_INFO_V1_PUBLIC(uint2um);
PG_FUNCTION_INFO_V1_PUBLIC(uint2up);
PG_FUNCTION_INFO_V1_PUBLIC(uint2pl);
PG_FUNCTION_INFO_V1_PUBLIC(uint2mi);
PG_FUNCTION_INFO_V1_PUBLIC(uint2mul);
PG_FUNCTION_INFO_V1_PUBLIC(uint2div);
PG_FUNCTION_INFO_V1_PUBLIC(uint2abs);
PG_FUNCTION_INFO_V1_PUBLIC(uint2mod);
PG_FUNCTION_INFO_V1_PUBLIC(uint2larger);
PG_FUNCTION_INFO_V1_PUBLIC(uint2smaller);
PG_FUNCTION_INFO_V1_PUBLIC(uint2inc);
PG_FUNCTION_INFO_V1_PUBLIC(uint4in);
PG_FUNCTION_INFO_V1_PUBLIC(uint4out);
PG_FUNCTION_INFO_V1_PUBLIC(uint4recv);
PG_FUNCTION_INFO_V1_PUBLIC(uint4send);
PG_FUNCTION_INFO_V1_PUBLIC(uint4eq);
PG_FUNCTION_INFO_V1_PUBLIC(uint4ne);
PG_FUNCTION_INFO_V1_PUBLIC(uint4lt);
PG_FUNCTION_INFO_V1_PUBLIC(uint4le);
PG_FUNCTION_INFO_V1_PUBLIC(uint4gt);
PG_FUNCTION_INFO_V1_PUBLIC(uint4ge);
PG_FUNCTION_INFO_V1_PUBLIC(uint4cmp);
PG_FUNCTION_INFO_V1_PUBLIC(ui1toui4);
PG_FUNCTION_INFO_V1_PUBLIC(ui4toui1);
PG_FUNCTION_INFO_V1_PUBLIC(ui4toi2);
PG_FUNCTION_INFO_V1_PUBLIC(i2toui4);
PG_FUNCTION_INFO_V1_PUBLIC(ui4toui2);
PG_FUNCTION_INFO_V1_PUBLIC(ui2toui4);
PG_FUNCTION_INFO_V1_PUBLIC(ui4toi4);
PG_FUNCTION_INFO_V1_PUBLIC(i4toui4);
PG_FUNCTION_INFO_V1_PUBLIC(ui4toi8);
PG_FUNCTION_INFO_V1_PUBLIC(i8toui4);
PG_FUNCTION_INFO_V1_PUBLIC(ui4tof4);
PG_FUNCTION_INFO_V1_PUBLIC(f4toui4);
PG_FUNCTION_INFO_V1_PUBLIC(ui4tof8);
PG_FUNCTION_INFO_V1_PUBLIC(f8toui4);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_bool);
PG_FUNCTION_INFO_V1_PUBLIC(bool_uint4);
PG_FUNCTION_INFO_V1_PUBLIC(uint4and);
PG_FUNCTION_INFO_V1_PUBLIC(uint4or);
PG_FUNCTION_INFO_V1_PUBLIC(uint4xor);
PG_FUNCTION_INFO_V1_PUBLIC(uint4not);
PG_FUNCTION_INFO_V1_PUBLIC(uint4shl);
PG_FUNCTION_INFO_V1_PUBLIC(uint4shr);
PG_FUNCTION_INFO_V1_PUBLIC(uint4um);
PG_FUNCTION_INFO_V1_PUBLIC(uint4up);
PG_FUNCTION_INFO_V1_PUBLIC(uint4pl);
PG_FUNCTION_INFO_V1_PUBLIC(uint4mi);
PG_FUNCTION_INFO_V1_PUBLIC(uint4mul);
PG_FUNCTION_INFO_V1_PUBLIC(uint4div);
PG_FUNCTION_INFO_V1_PUBLIC(uint4abs);
PG_FUNCTION_INFO_V1_PUBLIC(uint4mod);
PG_FUNCTION_INFO_V1_PUBLIC(uint4larger);
PG_FUNCTION_INFO_V1_PUBLIC(uint4smaller);
PG_FUNCTION_INFO_V1_PUBLIC(uint4inc);
PG_FUNCTION_INFO_V1_PUBLIC(uint8in);
PG_FUNCTION_INFO_V1_PUBLIC(uint8out);
PG_FUNCTION_INFO_V1_PUBLIC(uint8recv);
PG_FUNCTION_INFO_V1_PUBLIC(uint8send);
PG_FUNCTION_INFO_V1_PUBLIC(uint8and);	
PG_FUNCTION_INFO_V1_PUBLIC(uint8_and_int8);	
PG_FUNCTION_INFO_V1_PUBLIC(int8_and_uint8);	
PG_FUNCTION_INFO_V1_PUBLIC(uint8or);		
PG_FUNCTION_INFO_V1_PUBLIC(uint8xor);		
PG_FUNCTION_INFO_V1_PUBLIC(uint8not);		
PG_FUNCTION_INFO_V1_PUBLIC(uint8shl);		
PG_FUNCTION_INFO_V1_PUBLIC(uint8shr);		
PG_FUNCTION_INFO_V1_PUBLIC(uint8eq);		
PG_FUNCTION_INFO_V1_PUBLIC(uint8ne);		
PG_FUNCTION_INFO_V1_PUBLIC(uint8lt);		
PG_FUNCTION_INFO_V1_PUBLIC(uint8le);		
PG_FUNCTION_INFO_V1_PUBLIC(uint8gt);		
PG_FUNCTION_INFO_V1_PUBLIC(uint8ge);		
PG_FUNCTION_INFO_V1_PUBLIC(uint8cmp);		
PG_FUNCTION_INFO_V1_PUBLIC(i1toui8);		
PG_FUNCTION_INFO_V1_PUBLIC(ui8toi1);		
PG_FUNCTION_INFO_V1_PUBLIC(ui1toui8);		
PG_FUNCTION_INFO_V1_PUBLIC(ui8toui1);		
PG_FUNCTION_INFO_V1_PUBLIC(i2toui8);		
PG_FUNCTION_INFO_V1_PUBLIC(ui8toi2);		
PG_FUNCTION_INFO_V1_PUBLIC(ui2toui8);		
PG_FUNCTION_INFO_V1_PUBLIC(ui8toui2);		
PG_FUNCTION_INFO_V1_PUBLIC(i4toui8);		
PG_FUNCTION_INFO_V1_PUBLIC(ui8toi4);		
PG_FUNCTION_INFO_V1_PUBLIC(ui4toui8);		
PG_FUNCTION_INFO_V1_PUBLIC(ui8toui4);		
PG_FUNCTION_INFO_V1_PUBLIC(i8toui8);		
PG_FUNCTION_INFO_V1_PUBLIC(ui8toi8);		
PG_FUNCTION_INFO_V1_PUBLIC(f4toui8);		
PG_FUNCTION_INFO_V1_PUBLIC(ui8tof4);		
PG_FUNCTION_INFO_V1_PUBLIC(f8toui8);		
PG_FUNCTION_INFO_V1_PUBLIC(ui8tof8);		
PG_FUNCTION_INFO_V1_PUBLIC(uint8_bool);		
PG_FUNCTION_INFO_V1_PUBLIC(bool_uint8);		
PG_FUNCTION_INFO_V1_PUBLIC(uint8up);		
PG_FUNCTION_INFO_V1_PUBLIC(uint8pl);		
PG_FUNCTION_INFO_V1_PUBLIC(uint8mi);		
PG_FUNCTION_INFO_V1_PUBLIC(uint8mul);		
PG_FUNCTION_INFO_V1_PUBLIC(uint8div);		
PG_FUNCTION_INFO_V1_PUBLIC(uint8abs);		
PG_FUNCTION_INFO_V1_PUBLIC(uint8mod);		
PG_FUNCTION_INFO_V1_PUBLIC(uint8larger);	
PG_FUNCTION_INFO_V1_PUBLIC(uint8smaller);	
PG_FUNCTION_INFO_V1_PUBLIC(uint8inc);	
PG_FUNCTION_INFO_V1_PUBLIC(i1toui1);
PG_FUNCTION_INFO_V1_PUBLIC(i1toui2);
PG_FUNCTION_INFO_V1_PUBLIC(hashuint2);
PG_FUNCTION_INFO_V1_PUBLIC(hashuint4);

PG_FUNCTION_INFO_V1_PUBLIC(hashuint8);
PG_FUNCTION_INFO_V1_PUBLIC(ui2toi1);

PG_FUNCTION_INFO_V1_PUBLIC(ui1toi1);


PG_FUNCTION_INFO_V1_PUBLIC(uint1_pl_int1  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_mi_int1  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_mul_int1 );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_div_int1 );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int1_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int1_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int1_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int1_le  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int1_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int1_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint1_pl_int2  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_mi_int2  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_mul_int2 );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_div_int2 );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int2_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int2_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int2_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int2_le  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int2_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int2_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint1_pl_int4  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_mi_int4  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_mul_int4 );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_div_int4 );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int4_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int4_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int4_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int4_le  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int4_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int4_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint1_pl_int8  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_mi_int8  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_mul_int8 );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_div_int8 );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int8_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int8_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int8_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int8_le  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int8_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int8_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(int1_pl_uint1  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_mi_uint1  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_mul_uint1 );
PG_FUNCTION_INFO_V1_PUBLIC(int1_div_uint1 );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint1_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint1_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint1_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint1_le  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint1_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint1_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(int2_pl_uint1  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_mi_uint1  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_mul_uint1 );
PG_FUNCTION_INFO_V1_PUBLIC(int2_div_uint1 );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint1_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint1_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint1_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint1_le  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint1_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint1_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(int4_pl_uint1  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_mi_uint1  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_mul_uint1 );
PG_FUNCTION_INFO_V1_PUBLIC(int4_div_uint1 );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint1_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint1_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint1_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint1_le  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint1_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint1_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(int8_pl_uint1  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_mi_uint1  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_mul_uint1 );
PG_FUNCTION_INFO_V1_PUBLIC(int8_div_uint1 );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint1_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint1_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint1_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint1_le  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint1_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint1_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint1_pl_uint2 );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_mi_uint2 );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_mul_uint2);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_div_uint2);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint2_eq );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint2_ne );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint2_lt );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint2_le );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint2_gt );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint2_ge );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint1_pl_uint4 );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_mi_uint4 );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_mul_uint4);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_div_uint4);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint4_eq );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint4_ne );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint4_lt );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint4_le );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint4_gt );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint4_ge );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint1_pl_uint8 );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_mi_uint8 );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_mul_uint8);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_div_uint8);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint8_ne );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint8_eq );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint8_lt );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint8_le );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint8_gt );
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint8_ge );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint2_pl_int1  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_mi_int1  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_mul_int1 );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_div_int1 );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int1_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int1_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int1_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int1_le  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int1_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int1_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint2_pl_int2  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_mi_int2  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_mul_int2 );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_div_int2 );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int2_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int2_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int2_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int2_le  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int2_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int2_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint2_pl_int4  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_mi_int4  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_mul_int4 );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_div_int4 );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int4_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int4_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int4_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int4_le  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int4_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int4_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint2_pl_int8  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_mi_int8  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_mul_int8 );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_div_int8 );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int8_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int8_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int8_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int8_le  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int8_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int8_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(int1_pl_uint2  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_mi_uint2  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_mul_uint2 );
PG_FUNCTION_INFO_V1_PUBLIC(int1_div_uint2 );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint2_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint2_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint2_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint2_le  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint2_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint2_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(int2_pl_uint2  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_mi_uint2  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_mul_uint2 );
PG_FUNCTION_INFO_V1_PUBLIC(int2_div_uint2 );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint2_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint2_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint2_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint2_le  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint2_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint2_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(int4_pl_uint2  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_mi_uint2  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_mul_uint2 );
PG_FUNCTION_INFO_V1_PUBLIC(int4_div_uint2 );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint2_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint2_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint2_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint2_le  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint2_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint2_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(int8_pl_uint2  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_mi_uint2  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_mul_uint2 );
PG_FUNCTION_INFO_V1_PUBLIC(int8_div_uint2 );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint2_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint2_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint2_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint2_le  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint2_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint2_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint2_pl_uint1 );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_mi_uint1 );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_mul_uint1);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_div_uint1);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint1_eq );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint1_ne );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint1_lt );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint1_le );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint1_gt );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint1_ge );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint2_pl_uint4 );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_mi_uint4 );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_mul_uint4);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_div_uint4);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint4_eq );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint4_ne );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint4_lt );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint4_le );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint4_gt );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint4_ge );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint2_pl_uint8 );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_mi_uint8 );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_mul_uint8);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_div_uint8);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint8_ne );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint8_eq );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint8_lt );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint8_le );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint8_gt );
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint8_ge );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint4_pl_int1  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_mi_int1  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_mul_int1 );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_div_int1 );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int1_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int1_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int1_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int1_le  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int1_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int1_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint4_pl_int2  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_mi_int2  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_mul_int2 );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_div_int2 );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int2_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int2_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int2_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int2_le  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int2_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int2_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint4_pl_int4  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_mi_int4  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_mul_int4 );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_div_int4 );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int4_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int4_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int4_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int4_le  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int4_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int4_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint4_pl_int8  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_mi_int8  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_mul_int8 );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_div_int8 );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int8_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int8_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int8_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int8_le  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int8_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int8_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(int1_pl_uint4  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_mi_uint4  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_mul_uint4 );
PG_FUNCTION_INFO_V1_PUBLIC(int1_div_uint4 );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint4_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint4_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint4_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint4_le  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint4_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint4_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(int2_pl_uint4  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_mi_uint4  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_mul_uint4 );
PG_FUNCTION_INFO_V1_PUBLIC(int2_div_uint4 );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint4_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint4_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint4_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint4_le  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint4_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint4_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(int4_pl_uint4  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_mi_uint4  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_mul_uint4 );
PG_FUNCTION_INFO_V1_PUBLIC(int4_div_uint4 );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint4_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint4_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint4_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint4_le  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint4_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint4_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(int8_pl_uint4  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_mi_uint4  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_mul_uint4 );
PG_FUNCTION_INFO_V1_PUBLIC(int8_div_uint4 );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint4_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint4_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint4_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint4_le  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint4_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint4_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint4_pl_uint1 );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_mi_uint1 );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_mul_uint1);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_div_uint1);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint1_eq );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint1_ne );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint1_lt );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint1_le );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint1_gt );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint1_ge );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint4_pl_uint2 );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_mi_uint2 );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_mul_uint2);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_div_uint2);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint2_eq );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint2_ne );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint2_lt );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint2_le );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint2_gt );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint2_ge );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint4_pl_uint8 );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_mi_uint8 );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_mul_uint8);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_div_uint8);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint8_ne );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint8_eq );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint8_lt );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint8_le );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint8_gt );
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint8_ge );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint8_pl_int1  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_mi_int1  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_mul_int1 );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_div_int1 );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int1_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int1_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int1_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int1_le  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int1_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int1_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint8_pl_int2  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_mi_int2  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_mul_int2 );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_div_int2 );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int2_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int2_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int2_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int2_le  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int2_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int2_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint8_pl_int4  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_mi_int4  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_mul_int4 );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_div_int4 );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int4_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int4_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int4_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int4_le  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int4_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int4_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint8_pl_int8  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_mi_int8  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_mul_int8 );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_div_int8 );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int8_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int8_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int8_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int8_le  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int8_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int8_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(int1_pl_uint8  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_mi_uint8  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_mul_uint8 );
PG_FUNCTION_INFO_V1_PUBLIC(int1_div_uint8 );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint8_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint8_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint8_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint8_le  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint8_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint8_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(int2_pl_uint8  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_mi_uint8  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_mul_uint8 );
PG_FUNCTION_INFO_V1_PUBLIC(int2_div_uint8 );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint8_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint8_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint8_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint8_le  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint8_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint8_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(int4_pl_uint8  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_mi_uint8  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_mul_uint8 );
PG_FUNCTION_INFO_V1_PUBLIC(int4_div_uint8 );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint8_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint8_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint8_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint8_le  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint8_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint8_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(int8_pl_uint8  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_mi_uint8  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_mul_uint8 );
PG_FUNCTION_INFO_V1_PUBLIC(int8_div_uint8 );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint8_eq  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint8_ne  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint8_lt  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint8_le  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint8_gt  );
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint8_ge  );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint8_pl_uint1 );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_mi_uint1 );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_mul_uint1);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_div_uint1);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint1_eq );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint1_ne );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint1_lt );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint1_le );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint1_gt );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint1_ge );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint8_pl_uint2 );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_mi_uint2 );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_mul_uint2);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_div_uint2);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint2_eq );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint2_ne );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint2_lt );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint2_le );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint2_gt );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint2_ge );
                                           
PG_FUNCTION_INFO_V1_PUBLIC(uint8_pl_uint4 );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_mi_uint4 );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_mul_uint4);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_div_uint4);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint4_ne );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint4_eq );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint4_lt );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint4_le );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint4_gt );
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint4_ge );
PG_FUNCTION_INFO_V1_PUBLIC(i1toui4 );
PG_FUNCTION_INFO_V1_PUBLIC(ui4toi1 );

PG_FUNCTION_INFO_V1_PUBLIC(uint8um );

PG_FUNCTION_INFO_V1_PUBLIC(uint1_int1_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int2_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int4_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int8_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint2_mod) ;
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint4_mod) ;
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint8_mod) ;
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint1_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint1_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint1_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint1_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int1_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int2_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int4_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int8_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint1_mod) ;
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint4_mod) ;
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint8_mod) ;
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint2_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint2_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint2_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint2_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int1_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int2_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int4_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int8_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint1_mod) ;
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint2_mod) ;


PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint8_mod) ;
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint4_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint4_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint4_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint4_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int1_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int2_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int4_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int8_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint8_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint8_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint8_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint8_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint4_mod)  ;
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint1_mod) ;
PG_FUNCTION_INFO_V1_PUBLIC(uint8_uint2_mod) ;

extern "C" DLL_PUBLIC Datum uint1_int1_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum uint1_int2_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum uint1_int4_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum uint1_int8_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum uint1_uint2_mod(PG_FUNCTION_ARGS) ;
 extern "C" DLL_PUBLIC Datum uint1_uint4_mod(PG_FUNCTION_ARGS) ;
 extern "C" DLL_PUBLIC Datum uint1_uint8_mod(PG_FUNCTION_ARGS) ;
 extern "C" DLL_PUBLIC Datum int1_uint1_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum int2_uint1_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum int4_uint1_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum int8_uint1_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum uint2_int1_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum uint2_int2_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum uint2_int4_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum uint2_int8_mod(PG_FUNCTION_ARGS)  ;
  extern "C" DLL_PUBLIC Datum uint2_uint1_mod(PG_FUNCTION_ARGS) ;
 extern "C" DLL_PUBLIC Datum uint2_uint4_mod(PG_FUNCTION_ARGS) ;
 extern "C" DLL_PUBLIC Datum uint2_uint8_mod(PG_FUNCTION_ARGS) ;
 extern "C" DLL_PUBLIC Datum int1_uint2_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum int2_uint2_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum int4_uint2_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum int8_uint2_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum uint4_int1_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum uint4_int2_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum uint4_int4_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum uint4_int8_mod(PG_FUNCTION_ARGS)  ;
  extern "C" DLL_PUBLIC Datum uint4_uint1_mod(PG_FUNCTION_ARGS) ;
   extern "C" DLL_PUBLIC Datum uint4_uint2_mod(PG_FUNCTION_ARGS) ;
 extern "C" DLL_PUBLIC Datum uint4_uint8_mod(PG_FUNCTION_ARGS) ;
 extern "C" DLL_PUBLIC Datum int1_uint4_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum int2_uint4_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum int4_uint4_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum int8_uint4_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum uint8_int1_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum uint8_int2_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum uint8_int4_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum uint8_int8_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum int1_uint8_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum int2_uint8_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum int4_uint8_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum int8_uint8_mod(PG_FUNCTION_ARGS)  ;
extern "C" DLL_PUBLIC Datum uint8_uint1_mod(PG_FUNCTION_ARGS)  ;
 extern "C" DLL_PUBLIC Datum uint8_uint2_mod(PG_FUNCTION_ARGS) ;
  extern "C" DLL_PUBLIC Datum uint8_uint4_mod(PG_FUNCTION_ARGS) ;
  

extern "C" DLL_PUBLIC Datum uint8um  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui4toi1  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i1toui4  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_pl_int1  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_mi_int1  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_mul_int1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_div_int1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int1_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int1_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int1_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int1_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int1_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int1_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint1_pl_int2  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_mi_int2  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_mul_int2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_div_int2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int2_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int2_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int2_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int2_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int2_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int2_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint1_pl_int4  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_mi_int4  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_mul_int4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_div_int4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int4_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int4_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int4_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int4_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int4_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int4_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint1_pl_int8  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_mi_int8  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_mul_int8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_div_int8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int8_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int8_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int8_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int8_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int8_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int8_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum int1_pl_uint1  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_mi_uint1  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_mul_uint1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_div_uint1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint1_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint1_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint1_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint1_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint1_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint1_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum int2_pl_uint1  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_mi_uint1  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_mul_uint1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_div_uint1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint1_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint1_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint1_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint1_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint1_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint1_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum int4_pl_uint1  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_mi_uint1  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_mul_uint1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_div_uint1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint1_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint1_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint1_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint1_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint1_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint1_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum int8_pl_uint1  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_mi_uint1  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_mul_uint1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_div_uint1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint1_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint1_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint1_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint1_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint1_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint1_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint1_pl_uint2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_mi_uint2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_mul_uint2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_div_uint2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint2_eq (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint2_ne (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint2_lt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint2_le (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint2_gt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint2_ge (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint1_pl_uint4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_mi_uint4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_mul_uint4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_div_uint4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint4_eq (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint4_ne (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint4_lt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint4_le (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint4_gt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint4_ge (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint1_pl_uint8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_mi_uint8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_mul_uint8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_div_uint8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint8_ne (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint8_eq (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint8_lt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint8_le (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint8_gt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint8_ge (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint2_pl_int1  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_mi_int1  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_mul_int1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_div_int1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int1_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int1_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int1_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int1_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int1_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int1_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint2_pl_int2  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_mi_int2  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_mul_int2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_div_int2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int2_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int2_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int2_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int2_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int2_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int2_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint2_pl_int4  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_mi_int4  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_mul_int4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_div_int4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int4_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int4_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int4_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int4_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int4_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int4_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint2_pl_int8  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_mi_int8  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_mul_int8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_div_int8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int8_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int8_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int8_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int8_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int8_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int8_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum int1_pl_uint2  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_mi_uint2  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_mul_uint2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_div_uint2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint2_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint2_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint2_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint2_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint2_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint2_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum int2_pl_uint2  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_mi_uint2  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_mul_uint2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_div_uint2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint2_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint2_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint2_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint2_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint2_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint2_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum int4_pl_uint2  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_mi_uint2  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_mul_uint2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_div_uint2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint2_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint2_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint2_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint2_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint2_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint2_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum int8_pl_uint2  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_mi_uint2  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_mul_uint2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_div_uint2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint2_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint2_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint2_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint2_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint2_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint2_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint2_pl_uint1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_mi_uint1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_mul_uint1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_div_uint1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint1_eq (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint1_ne (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint1_lt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint1_le (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint1_gt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint1_ge (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint2_pl_uint4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_mi_uint4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_mul_uint4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_div_uint4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint4_eq (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint4_ne (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint4_lt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint4_le (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint4_gt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint4_ge (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint2_pl_uint8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_mi_uint8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_mul_uint8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_div_uint8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint8_ne (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint8_eq (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint8_lt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint8_le (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint8_gt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint8_ge (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint4_pl_int1  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_mi_int1  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_mul_int1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_div_int1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int1_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int1_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int1_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int1_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int1_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int1_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint4_pl_int2  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_mi_int2  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_mul_int2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_div_int2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int2_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int2_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int2_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int2_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int2_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int2_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint4_pl_int4  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_mi_int4  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_mul_int4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_div_int4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int4_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int4_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int4_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int4_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int4_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int4_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint4_pl_int8  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_mi_int8  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_mul_int8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_div_int8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int8_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int8_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int8_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int8_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int8_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int8_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum int1_pl_uint4  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_mi_uint4  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_mul_uint4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_div_uint4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint4_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint4_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint4_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint4_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint4_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint4_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum int2_pl_uint4  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_mi_uint4  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_mul_uint4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_div_uint4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint4_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint4_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint4_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint4_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint4_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint4_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum int4_pl_uint4  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_mi_uint4  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_mul_uint4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_div_uint4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint4_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint4_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint4_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint4_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint4_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint4_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum int8_pl_uint4  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_mi_uint4  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_mul_uint4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_div_uint4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint4_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint4_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint4_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint4_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint4_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint4_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint4_pl_uint1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_mi_uint1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_mul_uint1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_div_uint1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint1_eq (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint1_ne (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint1_lt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint1_le (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint1_gt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint1_ge (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint4_pl_uint2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_mi_uint2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_mul_uint2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_div_uint2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint2_eq (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint2_ne (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint2_lt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint2_le (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint2_gt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint2_ge (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint4_pl_uint8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_mi_uint8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_mul_uint8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_div_uint8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint8_ne (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint8_eq (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint8_lt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint8_le (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint8_gt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint8_ge (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint8_pl_int1  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_mi_int1  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_mul_int1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_div_int1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int1_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int1_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int1_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int1_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int1_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int1_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint8_pl_int2  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_mi_int2  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_mul_int2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_div_int2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int2_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int2_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int2_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int2_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int2_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int2_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint8_pl_int4  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_mi_int4  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_mul_int4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_div_int4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int4_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int4_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int4_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int4_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int4_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int4_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint8_pl_int8  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_mi_int8  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_mul_int8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_div_int8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int8_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int8_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int8_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int8_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int8_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int8_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum int1_pl_uint8  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_mi_uint8  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_mul_uint8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_div_uint8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint8_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint8_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint8_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint8_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint8_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint8_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum int2_pl_uint8  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_mi_uint8  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_mul_uint8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_div_uint8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint8_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint8_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint8_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint8_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint8_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint8_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum int4_pl_uint8  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_mi_uint8  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_mul_uint8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_div_uint8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint8_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint8_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint8_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint8_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint8_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint8_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum int8_pl_uint8  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_mi_uint8  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_mul_uint8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_div_uint8 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint8_eq  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint8_ne  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint8_lt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint8_le  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint8_gt  (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint8_ge  (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint8_pl_uint1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_mi_uint1 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_mul_uint1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_div_uint1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_uint1_eq (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_uint1_ne (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_uint1_lt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_uint1_le (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_uint1_gt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_uint1_ge (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint8_pl_uint2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_mi_uint2 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_mul_uint2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_div_uint2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_uint2_eq (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_uint2_ne (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_uint2_lt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_uint2_le (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_uint2_gt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_uint2_ge (PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint8_pl_uint4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_mi_uint4 (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_mul_uint4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_div_uint4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_uint4_ne (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_uint4_eq (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_uint4_lt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_uint4_le (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_uint4_gt (PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_uint4_ge (PG_FUNCTION_ARGS);


extern "C" DLL_PUBLIC Datum ui1toi1(PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum ui2toi1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum hashuint2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum hashuint4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum hashuint8(PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum i1toui2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i1toui1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1in(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1out(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1recv(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1send(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1ne(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1cmp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui1toi2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i2toui1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui1toi4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui1toi8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i8toui1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui1tof4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum f4toui1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui1tof8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum f8toui1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_bool(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bool_uint1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1and(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1or(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1xor(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1not(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1shl(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1shr(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1um(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1pl(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1mi(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1mul(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1div(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1abs(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1mod(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1larger(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1smaller(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1up(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum hashuint1(PG_FUNCTION_ARGS);

extern "C" DLL_PUBLIC Datum uint2in(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2out(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2recv(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2send(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2ne(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2cmp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui1toui2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui2toui1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui2toi2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i2toui2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui2toi4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i4toui2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui2toi8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i8toui2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui2tof4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum f4toui2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui2tof8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum f8toui2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_bool(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bool_uint2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2and(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2or(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2xor(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2not(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2shl(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2shr(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2um(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2up(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2pl(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2mi(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2mul(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2div(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2abs(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2mod(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2larger(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2smaller(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2inc(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4in(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4out(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4recv(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4send(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4ne(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4cmp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui1toui4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui4toui1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui4toi2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i2toui4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui4toui2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui2toui4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui4toi4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i4toui4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui4toi8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i8toui4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui4tof4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum f4toui4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui4tof8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum f8toui4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_bool(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum bool_uint4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4and(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4or(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4xor(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4not(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4shl(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4shr(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4um(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4up(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4pl(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4mi(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4mul(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4div(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4abs(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4mod(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4larger(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4smaller(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4inc(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8in(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum  uint8out(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8recv(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8send(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8and(PG_FUNCTION_ARGS);	
extern "C" DLL_PUBLIC Datum uint8_and_int8(PG_FUNCTION_ARGS);	
extern "C" DLL_PUBLIC Datum int8_and_uint8(PG_FUNCTION_ARGS);	
extern "C" DLL_PUBLIC Datum uint8or(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum uint8xor(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum uint8not(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum uint8shl(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum uint8shr(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum uint8eq(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum uint8ne(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum uint8lt(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum uint8le(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum uint8gt(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum uint8ge(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum uint8cmp(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum i1toui8(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum ui8toi1(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum ui1toui8(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum ui8toui1(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum i2toui8(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum ui8toi2(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum ui2toui8(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum ui8toui2(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum i4toui8(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum ui8toi4(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum ui4toui8(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum ui8toui4(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum i8toui8(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum ui8toi8(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum f4toui8(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum ui8tof4(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum f8toui8(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum ui8tof8(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum uint8_bool(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum bool_uint8(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum uint8up(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum uint8pl(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum uint8mi(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum uint8mul(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum uint8div(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum uint8abs(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum uint8mod(PG_FUNCTION_ARGS);		
extern "C" DLL_PUBLIC Datum uint8larger(PG_FUNCTION_ARGS);	
extern "C" DLL_PUBLIC Datum uint8smaller(PG_FUNCTION_ARGS);	
extern "C" DLL_PUBLIC Datum uint8inc(PG_FUNCTION_ARGS);	    





Datum
uint1in(PG_FUNCTION_ARGS)
{
	char	   *num = PG_GETARG_CSTRING(0);

	PG_RETURN_UINT8((uint8)pg_atoui(num, sizeof(uint8), '\0'));
}

/*
 *		uint1out			- converts uint8 to "num"
 */
Datum
uint1out(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT8(0);
	char	   *result = (char *) palloc(5);	/* sign, 3 digits, '\0' */

	pg_uctoa(arg1, result);
	PG_RETURN_CSTRING(result);
}

/*
 *		uint1recv			- converts external binary format to uint8
 */
Datum
uint1recv(PG_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) PG_GETARG_POINTER(0);

	PG_RETURN_UINT8((uint8) pq_getmsgint(buf, sizeof(uint8)));
}

/*
 *		int1send			- converts uint8 to binary format
 */
Datum
uint1send(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendint(&buf, arg1, sizeof(uint8));
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

Datum
uint1and(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_UINT8(arg1 & arg2);
}

Datum
uint1or(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_UINT8(arg1 | arg2);
}

Datum
uint1xor(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    uint8 arg2 = PG_GETARG_UINT8(1);

    PG_RETURN_UINT8(arg1 ^ arg2);
}

Datum
uint1not(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);

    PG_RETURN_UINT8((uint8)(~arg1));
}

Datum
uint1shl(PG_FUNCTION_ARGS)
{
    uint8 arg1 = PG_GETARG_UINT8(0);
    int32 arg2 = PG_GETARG_INT32(1);

    PG_RETURN_UINT8((uint8)(arg1 << arg2));
}

Datum
uint1shr(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	int32 arg2 = PG_GETARG_INT32(1);

	PG_RETURN_UINT8(arg1 >> arg2);
}

/*
 * adapt mysql, add a new data type uint1 
  *		uint1eq			- returns 1 if arg1 == arg2
  *		uint1ne			- returns 1 if arg1 != arg2
  *		uint1lt			- returns 1 if arg1 < arg2
  *		uint1le			- returns 1 if arg1 <= arg2
  *		uint1gt			- returns 1 if arg1 > arg2
  *		uint1ge			- returns 1 if arg1 >= arg2
  */
Datum
uint1eq(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
uint1ne(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
uint1lt(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
uint1le(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
uint1gt(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL( arg1 > arg2);
}

Datum
uint1ge(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL( arg1 >= arg2);
}
Datum
uint1cmp(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	if (arg1 > arg2)
		PG_RETURN_INT32(1);
	else if (arg1 == arg2)
		PG_RETURN_INT32(0);
	else
		PG_RETURN_INT32(-1);
}

/*
 *		===================
 *		CONVERSION ROUTINES
 *		===================
 */
Datum
ui1toi2(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);

	PG_RETURN_INT16((int16) arg1);
}

Datum
i2toui1(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);

	if (arg1 < 0 || arg1 > UCHAR_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("tinyint unsigned out of range")));

	PG_RETURN_UINT8((uint8) arg1);
}

Datum
ui1toi4(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);

	PG_RETURN_INT32((int32) arg1);
}

Datum
i4toui1(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);

	if (arg1 < 0 || arg1 > UCHAR_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("tinyint unsigned out of range")));

	PG_RETURN_UINT8((uint8) arg1);
}

Datum
ui1toi8(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	
	PG_RETURN_INT64((int64) arg1);
}

Datum
i8toui1(PG_FUNCTION_ARGS)
{
	int64	arg1 = PG_GETARG_INT64(0);

	if (arg1 <  0 || arg1 > UCHAR_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),				
				 errmsg("tinyint unsigned out of range")));

	PG_RETURN_UINT8((uint8) arg1);
}

Datum
ui1tof4(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);

	PG_RETURN_FLOAT4((float4) arg1);
}

Datum
ui1tof8(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);

	PG_RETURN_FLOAT8((float8) arg1);
}

Datum
f4toui1(PG_FUNCTION_ARGS)
{
	float4		arg1 = PG_GETARG_FLOAT4(0);

	if (arg1 < 0 || arg1 > UCHAR_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("tinyint unsigned out of range")));
	PG_RETURN_UINT8((uint8) arg1);
}

Datum
f8toui1(PG_FUNCTION_ARGS)
{
	float8		arg1 = PG_GETARG_FLOAT8(0);

	if (arg1 < 0 || arg1 > UCHAR_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("tinyint unsignedout of range")));

	PG_RETURN_UINT8((uint8) arg1);
}


/* OPERATE INT1 */
Datum
uint1um(PG_FUNCTION_ARGS)
{
	uint16 result = PG_GETARG_UINT8(0);

	PG_RETURN_INT16(0-result);
}

Datum
uint1up(PG_FUNCTION_ARGS)
{
	uint8 arg = PG_GETARG_UINT8(0);

	PG_RETURN_UINT8(arg);
}

Datum
uint1pl(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	uint16 result;

	result = arg1 + arg2;

    if (result > UCHAR_MAX) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    }

    PG_RETURN_UINT8((uint8)result);

}

Datum
uint1mi(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint8 arg2 = PG_GETARG_UINT8(1);

	uint8 result;

    if (arg1 < arg2) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    }

    result = arg1 - arg2;

    PG_RETURN_UINT8(result);


}

Datum
uint1mul(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint8 arg2 = PG_GETARG_UINT8(1);

	uint16 result16;


    result16 = (uint16)arg1 * (uint16)arg2;

    if ((result16 < 0) || (result16 > UCHAR_MAX)) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("tinyint unsigned out of range")));
    }

    PG_RETURN_UINT8((uint8)result16);

}

Datum
uint1div(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
    float8 result;

    if (arg2 == 0) {
        ereport(ERROR, (errcode(ERRCODE_DIVISION_BY_ZERO), errmsg("division by zero")));
        PG_RETURN_NULL();
    }

    result = (arg1 * 1.0) / (arg2 * 1.0);

    PG_RETURN_FLOAT8(result);
}

Datum
uint1abs(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	
	PG_RETURN_UINT8(arg1);
}

Datum
uint1mod(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}


	PG_RETURN_UINT8(arg1 % arg2);
}

Datum
uint1larger(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint8 arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_UINT8((arg1 > arg2) ? arg1 : arg2);
}

Datum
uint1smaller(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint8 arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_UINT8((arg1 < arg2) ? arg1 : arg2);
}


Datum
hashuint1(PG_FUNCTION_ARGS)
{
	return hash_uint32((int32) PG_GETARG_UINT8(0));
}

Datum
hashuint2(PG_FUNCTION_ARGS)
{
	return hash_uint32((int32) PG_GETARG_UINT16(0));
}

Datum
hashuint4(PG_FUNCTION_ARGS)
{
	return hash_uint32(PG_GETARG_UINT32(0));
}


Datum
hashuint8(PG_FUNCTION_ARGS)
{
	uint64		val = PG_GETARG_UINT64(0);
	uint32		lohalf = (uint32) val;
	uint32		hihalf = (uint32) (val >> 32);

	lohalf ^=  hihalf;

	return hash_uint32(lohalf);
}

Datum
uint2in(PG_FUNCTION_ARGS)
{
	char	   *num = PG_GETARG_CSTRING(0);

	PG_RETURN_UINT16((uint16)pg_atoui(num, sizeof(uint16), '\0'));
}

Datum
uint2out(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	char	   *result = (char *) palloc(7);	/* sign, 5 digits, '\0' */

	pg_ltoa((int32)arg1, result);
	PG_RETURN_CSTRING(result);
}

Datum
uint2recv(PG_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) PG_GETARG_POINTER(0);

	PG_RETURN_UINT16((uint16) pq_getmsgint(buf, sizeof(uint16)));
}

Datum
uint2send(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendint(&buf, arg1, sizeof(uint16));
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

Datum
uint2and(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_UINT16(arg1 & arg2);
}

Datum
uint2or(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_UINT16(arg1 | arg2);
}

Datum
uint2xor(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    uint16 arg2 = PG_GETARG_UINT16(1);

    PG_RETURN_UINT16(arg1 ^ arg2);
}

Datum
uint2not(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);

    PG_RETURN_UINT16((uint16)(~arg1));
}

Datum
uint2shl(PG_FUNCTION_ARGS)
{
    uint16 arg1 = PG_GETARG_UINT16(0);
    int32 arg2 = PG_GETARG_INT32(1);

    PG_RETURN_UINT16((uint16)(arg1 << arg2));
}

Datum
uint2shr(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	int32 arg2 = PG_GETARG_INT32(1);

	PG_RETURN_UINT16(arg1 >> arg2);
}

/*
 * adapt mysql, add a new data type uint2 
  *		uint2eq			- returns 1 if arg1 == arg2
  *		uint2ne			- returns 1 if arg1 != arg2
  *		uint2lt			- returns 1 if arg1 < arg2
  *		uint2le			- returns 1 if arg1 <= arg2
  *		uint2gt			- returns 1 if arg1 > arg2
  *		uint2ge			- returns 1 if arg1 >= arg2
  */
Datum
uint2eq(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
uint2ne(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
uint2lt(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
uint2le(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
uint2gt(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL( arg1 > arg2);
}

Datum
uint2ge(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL( arg1 >= arg2);
}
Datum
uint2cmp(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	if (arg1 > arg2)
		PG_RETURN_INT32(1);
	else if (arg1 == arg2)
		PG_RETURN_INT32(0);
	else
		PG_RETURN_INT32(-1);
}

/*
 *		===================
 *		CONVERSION ROUTINES
 *		===================
 */
Datum
ui1toui2(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);

	PG_RETURN_UINT16((uint16) arg1);
}

Datum
ui2toui1(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);

	if (arg1 > UCHAR_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("tinyint unsigned out of range")));

	PG_RETURN_UINT8((uint8) arg1);
}

Datum
ui2toi2(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);

	if (arg1 < SHRT_MIN || arg1 > SHRT_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint unsigned out of range")));

	PG_RETURN_INT16((int16) arg1);
}

Datum
i2toui2(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);

	if (arg1 < 0 || arg1 > USHRT_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint unsigned out of range")));

	PG_RETURN_UINT16((uint16) arg1);
}
Datum
ui2toi4(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);

	PG_RETURN_INT32((int32) arg1);
}

Datum
i4toui2(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);

	if (arg1 < 0 || arg1 > USHRT_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint unsigned out of range")));

	PG_RETURN_UINT16((uint16) arg1);
}

Datum
ui2toi8(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	
	PG_RETURN_INT64((int64) arg1);
}

Datum
i8toui2(PG_FUNCTION_ARGS)
{
	int64	arg1 = PG_GETARG_INT64(0);

	if (arg1 <  0 || arg1 > USHRT_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint unsigned out of range")));

	PG_RETURN_UINT16((uint16) arg1);
}

Datum
ui2tof4(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);

	PG_RETURN_FLOAT4((float4) arg1);
}

Datum
ui2tof8(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);

	PG_RETURN_FLOAT8((float8) arg1);
}

Datum
f4toui2(PG_FUNCTION_ARGS)
{
	float4		arg1 = PG_GETARG_FLOAT4(0);

	if (arg1 < 0 || arg1 > USHRT_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint unsigned out of range")));

	PG_RETURN_UINT16((uint16) arg1);
}

Datum
f8toui2(PG_FUNCTION_ARGS)
{
	float8		arg1 = PG_GETARG_FLOAT8(0);

	if (arg1 < 0 || arg1 > USHRT_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint unsigned out of range")));

	PG_RETURN_UINT16((uint16) arg1);
}

Datum
uint2um(PG_FUNCTION_ARGS)
{
	int32 arg = PG_GETARG_UINT16(0);
	int32  result;

	result = -arg;

	PG_RETURN_INT32(result);
}

Datum
uint2up(PG_FUNCTION_ARGS)
{
	uint16 arg = PG_GETARG_UINT16(0);

	PG_RETURN_UINT16(arg);
}

Datum
uint2pl(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint32 result;

	result = arg1 + arg2;


	PG_RETURN_UINT32(result);

}

Datum
uint2mi(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint16 arg2 = PG_GETARG_UINT16(1);

	uint16 result;
	if (arg1 < arg2) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("smallint unsigned value out of range")));
    }
	result = arg1 - arg2;
	
	PG_RETURN_UINT16(result);

}

Datum
uint2mul(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
/* BEGIN <GAUSSDBV100R003C10> : compatible with Mysql's datatype bigint unsigned */
	uint32 result32;

	result32 = (uint32) arg1 * (uint32) arg2;

	PG_RETURN_UINT32(result32);
/* END <l00219320> */
}

Datum
uint2div(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint2abs(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	
	PG_RETURN_UINT16(arg1);
}

Datum
uint2mod(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	/* No overflow is possible */

	PG_RETURN_UINT16(arg1 % arg2);
}

Datum
uint2larger(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint16 arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_UINT16((arg1 > arg2) ? arg1 : arg2);
}

Datum
uint2smaller(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint16 arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_UINT16((arg1 < arg2) ? arg1 : arg2);
}

Datum
uint2inc(PG_FUNCTION_ARGS)
{
	uint16		arg = PG_GETARG_UINT16(0);
	int32		result;

	result = arg + 1;
	
	/* Overflow check */
	if (result > USHRT_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint unsigned out of range")));

	PG_RETURN_UINT16((uint16)result);
}

/*
 * @@GaussDB@@
 * Brief		:  adapt mysql, add a new data type uint4
 * Description	: 
 * Notes		: 
 * Author		: l00219320
 */ 

Datum
uint4in(PG_FUNCTION_ARGS)
{
	char	   *num = PG_GETARG_CSTRING(0);

	PG_RETURN_UINT32((uint32)pg_atoui(num, sizeof(uint32), '\0'));
}

Datum
uint4out(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	char	   *result = (char *) palloc(12);	/* sign, 10 digits, '\0' */

	pg_lltoa((int64)arg1, result);
	PG_RETURN_CSTRING(result);
}

Datum
uint4recv(PG_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) PG_GETARG_POINTER(0);

	PG_RETURN_UINT32((uint32) pq_getmsgint(buf, sizeof(uint32)));
}

Datum
uint4send(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendint(&buf, arg1, sizeof(uint32));
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

Datum
uint4and(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    PG_RETURN_UINT32(arg1 & arg2);
}

Datum
uint4or(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    PG_RETURN_UINT32(arg1 | arg2);
}

Datum
uint4xor(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    uint32 arg2 = PG_GETARG_UINT32(1);

    PG_RETURN_UINT32(arg1 ^ arg2);
}

Datum
uint4not(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);

    PG_RETURN_UINT32((uint32)(~arg1));
}

Datum
uint4shl(PG_FUNCTION_ARGS)
{
    uint32 arg1 = PG_GETARG_UINT32(0);
    int32 arg2 = PG_GETARG_INT32(1);

    PG_RETURN_UINT32((uint32)(arg1 << arg2));
}

Datum
uint4shr(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	int32 arg2 = PG_GETARG_INT32(1);

	PG_RETURN_UINT32(arg1 >> arg2);
}

/*
 * adapt mysql, add a new data type uint4 
  *		uint4eq			- returns 1 if arg1 == arg2
  *		uint4ne			- returns 1 if arg1 != arg2
  *		uint4lt			- returns 1 if arg1 < arg2
  *		uint4le			- returns 1 if arg1 <= arg2
  *		uint4gt			- returns 1 if arg1 > arg2
  *		uint4ge			- returns 1 if arg1 >= arg2
  */
Datum
uint4eq(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
uint4ne(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
uint4lt(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
uint4le(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
uint4gt(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL( arg1 > arg2);
}

Datum
uint4ge(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL( arg1 >= arg2);
}
Datum
uint4cmp(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	if (arg1 > arg2)
		PG_RETURN_INT32(1);
	else if (arg1 == arg2)
		PG_RETURN_INT32(0);
	else
		PG_RETURN_INT32(-1);
}


 
 Datum
ui4toui1(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);


	if (arg1 > UCHAR_MAX)

		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("tinyint unsigned out of range")));

	PG_RETURN_UINT8((uint8) arg1);
}

 Datum
ui1toui4(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);

	PG_RETURN_UINT32((uint32) arg1);
}


Datum
ui4toi1(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);

	if (arg1 > SCHAR_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("tinyint unsigned out of range")));

	PG_RETURN_INT8((int8) arg1);
}

Datum
i1toui4(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);

	if (arg1 < 0)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("int unsigned out of range")));

	PG_RETURN_UINT32((uint32) arg1);
}

Datum
ui2toi1(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);

	if (arg1 > SCHAR_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("tinyint unsigned out of range")));

	PG_RETURN_INT8((int8) arg1);
}

Datum
i1toui2(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);

	if (arg1 < 0)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint unsigned out of range")));

	PG_RETURN_UINT16((uint16) arg1);
}

Datum
ui1toi1(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);

	if (arg1 > SCHAR_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("tinyint unsigned out of range")));

	PG_RETURN_INT8((int8) arg1);
}

Datum
i1toui1(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);

	if (arg1 < 0)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("tinyint unsigned out of range")));

	PG_RETURN_UINT8((uint8) arg1);
}


Datum
ui4toui2(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);

	if (arg1 > USHRT_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint unsigned out of range")));

	PG_RETURN_UINT16((uint16) arg1);
}

Datum
ui2toui4(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);

	PG_RETURN_UINT32((uint32) arg1);
}

Datum
ui4toi2(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);

	/* BEGIN <z00216470> : clear coverity warning 117449 */
	if (arg1 > SHRT_MAX)
	/* END <z00216470> */
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint unsigned out of range")));

	PG_RETURN_INT16((int16) arg1);
}

Datum
i2toui4(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);

	/* BEGIN <z00216470> : clear coverity warning 117220 */
	if (arg1 < 0)
	/* END <z00216470> */
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("int unsigned out of range")));

	PG_RETURN_UINT32((uint32) arg1);
}
Datum
ui4toi4(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);

	if (arg1 > INT_MAX)
	{
		ereport(ERROR,
			(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
			 errmsg("int  unsigned out of range")));
	}

	PG_RETURN_INT32((int32) arg1);
}

Datum
i4toui4(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);

	if (arg1 < 0)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("int unsigned out of range")));

	PG_RETURN_UINT32((uint32) arg1);
}

Datum
ui4toi8(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	
	PG_RETURN_INT64((int64) arg1);
}

Datum
i8toui4(PG_FUNCTION_ARGS)
{
	int64	arg1 = PG_GETARG_INT64(0);

	if (arg1 <  0 || arg1 > UINT_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("int unsigned out of range")));

	PG_RETURN_UINT32((uint32) arg1);
}

Datum
ui4tof4(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);

	PG_RETURN_FLOAT4((float4) arg1);
}

Datum
f4toui4(PG_FUNCTION_ARGS)
{
	float4		arg = PG_GETARG_FLOAT4(0);
	float8		darg;

	/* Round arg to nearest integer (but it's still in float form) */
	darg = rint(arg);

	if (darg < 0 || darg > UINT_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("int unsigned out of range")));

	PG_RETURN_UINT32(darg);
}

Datum
ui4tof8(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);

	PG_RETURN_FLOAT8((float8) arg1);
}

Datum
f8toui4(PG_FUNCTION_ARGS)
{
	float8		arg1 = PG_GETARG_FLOAT8(0);

	if (arg1 < 0 || arg1 > UINT_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("int unsigned out of range")));

	PG_RETURN_UINT32((uint32) arg1);
}


Datum
uint4um(PG_FUNCTION_ARGS)
{
	int64 arg = PG_GETARG_UINT32(0);
	int64  result;

	result = -arg;

	PG_RETURN_INT64(result);
}


Datum
uint8um(PG_FUNCTION_ARGS)
{
	int128 arg = PG_GETARG_UINT32(0);
	int128  result;

	result = -arg;

	PG_RETURN_INT128(result);
}


Datum
uint4up(PG_FUNCTION_ARGS)
{
	uint32 arg = PG_GETARG_UINT32(0);

	PG_RETURN_UINT32(arg);
}

Datum
uint4pl(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	uint64 result;

	result = (uint64)arg1 + (uint64)arg2;

	PG_RETURN_UINT64(result);
}

Datum
uint4mi(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint32 arg2 = PG_GETARG_UINT32(1);

	uint32 result;
    if (arg1 < arg2) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }
	result = arg1 - arg2;
	
	PG_RETURN_UINT32(result);
}

Datum
uint4mul(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	
	uint64 result64;

	result64 = (uint64) arg1 * (uint64) arg2;

	PG_RETURN_UINT64(result64);
}

Datum
uint4div(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint4abs(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	
	PG_RETURN_UINT32(arg1);
}

Datum
uint4mod(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	/* No overflow is possible */

	PG_RETURN_UINT32(arg1 % arg2);
}

Datum
uint4larger(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint32 arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_UINT32((arg1 > arg2) ? arg1 : arg2);
}

Datum
uint4smaller(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint32 arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_UINT32((arg1 < arg2) ? arg1 : arg2);
}

Datum
uint4inc(PG_FUNCTION_ARGS)
{
	uint32		arg = PG_GETARG_UINT32(0);

	if (arg >= UINT_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("int unsigned out of range")));

	PG_RETURN_UINT32((uint32)(arg + 1));
}
Datum
int1_pl_uint1(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	int16 result = arg1 + arg2;
	
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("int unsigned  value out of range")));
    }

	PG_RETURN_INT16(result);
}

Datum
int1_pl_uint2(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint64 result = arg1 + arg2;

	PG_RETURN_UINT64(result);
}



Datum
int1_pl_uint4(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	uint64 result = arg1 + arg2;

	PG_RETURN_UINT64(result);
}


Datum
uint1_pl_int1(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	int8 arg2 = PG_GETARG_INT8(1);
	int16  result = arg1 + arg2;

    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }

	PG_RETURN_INT16(result);	
}

Datum
uint1_pl_uint2(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint32 result;

	result = (uint32)arg1 +(uint32)arg2;

	PG_RETURN_UINT32(result);
}

Datum
uint1_pl_int2(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	int16 arg2 = PG_GETARG_INT16(1);
	int32  result;

	result = (uint32)arg1 + arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("int unsigned value out of range")));
    }

	PG_RETURN_INT32(result);	
}

Datum
int2_pl_uint1(PG_FUNCTION_ARGS)
{
	int16 arg1 = PG_GETARG_INT16(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	int32  result;

	result = arg1 + (int32)arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("int unsigned value out of range")));
    }

	PG_RETURN_INT32(result);	
}

Datum
uint1_pl_int4(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	int32 arg2 = PG_GETARG_INT32(1);
	int64  result;

	result = (int64)arg1 + arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("int unsigned value out of range")));
    }

	PG_RETURN_INT64(result);	
}

Datum
int4_pl_uint1(PG_FUNCTION_ARGS)
{
	int32 arg1 = PG_GETARG_INT32(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	int64  result;

	result = arg1 + (int64)arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }
    

	PG_RETURN_INT64(result);	
}

Datum
uint1_pl_uint4(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	uint64 result;

	result = (uint64)arg1 + (uint64)arg2;

	PG_RETURN_UINT64(result);
}

Datum
uint2_pl_int1(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	int8 arg2 = PG_GETARG_INT8(1);
	uint64  result;

	result = (uint64)arg1 + arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint2_pl_uint1(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	uint32 result;

	result = (uint32)arg1 + (uint32)arg2;

	PG_RETURN_UINT32(result);
}

Datum
uint2_pl_int2(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	int16 arg2 = PG_GETARG_INT16(1);
	uint64  result;

	result = (uint64)arg1 + arg2;

	PG_RETURN_UINT64(result);	
}

Datum
int2_pl_uint2(PG_FUNCTION_ARGS)
{
	int16 arg1 = PG_GETARG_INT16(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint64  result;

	result = arg1 + (uint64)arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint2_pl_int4(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	int32 arg2 = PG_GETARG_INT32(1);
	uint64  result;

	result = (uint64)arg1 + arg2;

	PG_RETURN_UINT64(result);	
}

Datum
int4_pl_uint2(PG_FUNCTION_ARGS)
{
	int32 arg1 = PG_GETARG_INT32(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint64  result;

	result = arg1 + (uint64)arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint2_pl_uint4(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	uint64 result;

	result = (uint64)arg1 + (uint64)arg2;

	PG_RETURN_UINT64(result);
}

Datum
uint4_pl_int1(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	int8 arg2 = PG_GETARG_INT8(1);
	uint64  result;

	result = (uint64)arg1 + arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint4_pl_uint1(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	uint64 result;

	result = (uint64)arg1 + (uint64)arg2;

	PG_RETURN_UINT64(result);
}

Datum
uint4_pl_int2(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	int16 arg2 = PG_GETARG_INT16(1);
	uint64  result;

	result = (uint64)arg1 + arg2;

	PG_RETURN_UINT64(result);	
}

Datum
int2_pl_uint4(PG_FUNCTION_ARGS)
{
	int16 arg1 = PG_GETARG_INT16(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	uint64  result;

	result = arg1 + (uint64)arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint4_pl_uint2(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint64 result;

	result = (uint64)arg1 + (uint64)arg2;

	PG_RETURN_UINT64(result);
}

Datum
uint4_pl_int4(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	int32 arg2 = PG_GETARG_INT32(1);
	uint64  result;

	result = (uint64)arg1 + arg2;

	PG_RETURN_UINT64(result);	
}


Datum
int4_pl_uint4(PG_FUNCTION_ARGS)
{

	int32 arg1 = PG_GETARG_INT32(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	uint64  result;

	result = arg1 + (uint64)arg2;

	PG_RETURN_UINT64(result);	
}
/*datatype minus*/
Datum
int1_mi_uint1(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	int16 result = arg1 - arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }
    

	PG_RETURN_INT16(result);
}

Datum
int1_mi_uint2(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint64 result = arg1 - arg2;

	PG_RETURN_UINT64(result);
}


Datum
int1_mi_uint4(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	uint64 result = arg1 - arg2;

	PG_RETURN_UINT64(result);
}


Datum
uint1_mi_int1(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	int8 arg2 = PG_GETARG_INT8(1);
	int16  result;

	result = (int16)arg1 - arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }

	PG_RETURN_INT16(result);	
}

Datum
uint1_mi_uint2(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint64 result;

	result = (uint64)arg1 - (uint64)arg2;

	PG_RETURN_UINT64(result);
}

Datum
uint1_mi_int2(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	int16 arg2 = PG_GETARG_INT16(1);
	int32  result;

	result = (int32)arg1 - arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }

	PG_RETURN_INT32(result);	
}

Datum
int2_mi_uint1(PG_FUNCTION_ARGS)
{
	int16 arg1 = PG_GETARG_INT16(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	int32  result;

	result = arg1 - (int32)arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }
    

	PG_RETURN_INT32(result);	
}

Datum
uint1_mi_int4(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	int32 arg2 = PG_GETARG_INT32(1);
	int64  result;

	result = (int64)arg1 - arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }

	PG_RETURN_INT64(result);	
}

Datum
int4_mi_uint1(PG_FUNCTION_ARGS)
{
	int32 arg1 = PG_GETARG_INT32(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	int64  result;

	result = arg1 - (int64)arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }

	PG_RETURN_INT64(result);	
}

Datum
uint1_mi_uint4(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	int64 result;

	result = (int64)arg1 - (int64)arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }
    

	PG_RETURN_INT64(result);
}

Datum
uint2_mi_int1(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	int8 arg2 = PG_GETARG_INT8(1);
	uint64  result;

	result = (uint64)arg1 - arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint2_mi_uint1(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	uint64 result;

	result = (uint64)arg1 - (uint64)arg2;

	PG_RETURN_UINT64(result);
}

Datum
uint2_mi_int2(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	int16 arg2 = PG_GETARG_INT16(1);
	uint64  result;

	result = (uint64)arg1 - arg2;

	PG_RETURN_UINT64(result);	
}

Datum
int2_mi_uint2(PG_FUNCTION_ARGS)
{
	int16 arg1 = PG_GETARG_INT16(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint64  result;

	result = arg1 - (uint64)arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint2_mi_int4(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	int32 arg2 = PG_GETARG_INT32(1);
	uint64  result;

	result = (uint64)arg1 - arg2;

	PG_RETURN_UINT64(result);	
}

Datum
int4_mi_uint2(PG_FUNCTION_ARGS)
{
	int32 arg1 = PG_GETARG_INT32(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint64  result;

	result = arg1 - (uint64)arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint2_mi_uint4(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	uint64 result;

	result = (uint64)arg1 - (uint64)arg2;

	PG_RETURN_UINT64(result);
}

Datum
uint4_mi_int1(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	int8 arg2 = PG_GETARG_INT8(1);
	uint64  result;

	result = (uint64)arg1 - arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint4_mi_uint1(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	uint64 result;

	result = (uint64)arg1 - (uint64)arg2;

	PG_RETURN_UINT64(result);
}

Datum
uint4_mi_int2(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	int16 arg2 = PG_GETARG_INT16(1);
	uint64  result;

	result = (uint64)arg1 - arg2;

	PG_RETURN_UINT64(result);	
}

Datum
int2_mi_uint4(PG_FUNCTION_ARGS)
{
	int16 arg1 = PG_GETARG_INT16(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	uint64  result;

	result = arg1 - (uint64)arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint4_mi_uint2(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint64 result;

	result = (uint64)arg1 - (uint64)arg2;

	PG_RETURN_UINT64(result);
}

Datum
uint4_mi_int4(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	int32 arg2 = PG_GETARG_INT32(1);
	uint64  result;

	result = (uint64)arg1 - arg2;

	PG_RETURN_UINT64(result);	
}


Datum
int4_mi_uint4(PG_FUNCTION_ARGS)
{

	int32 arg1 = PG_GETARG_INT32(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	uint64  result;

	result = arg1 - (uint64)arg2;

	PG_RETURN_UINT64(result);	
}

/*datatype multiply*/
Datum
int1_mul_uint1(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	int16 result;
	
	result = arg1 * (int16)arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }

	PG_RETURN_INT16(result);
}

Datum
int1_mul_uint2(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint64 result;

	result = arg1 * (uint64)arg2;

	PG_RETURN_UINT64(result);
}

Datum
int1_mul_int2(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	int16 arg2 = PG_GETARG_INT16(1);
	int32 result;
	result = arg1 *(int32) arg2;

	 PG_RETURN_INT32(result);
}

Datum
int2_mul_int1(PG_FUNCTION_ARGS)
{
	int16 arg1 = PG_GETARG_INT16(0);
	int8 arg2 = PG_GETARG_INT8(1);
	int32 result;

	result = (int32)arg1 * arg2;

	 PG_RETURN_INT32(result);
}

Datum
int1_mul_int4(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	int32 arg2 = PG_GETARG_INT32(1);
	int32 result;

	result = arg1 * arg2;

	if (!(arg1 >= (int32) SHRT_MIN && arg1 <= (int32) SHRT_MAX) &&
		result / arg1 != arg2)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));

	PG_RETURN_INT32(result);
}

Datum
int4_mul_int1(PG_FUNCTION_ARGS)
{
	int32 arg1 = PG_GETARG_INT32(0);
	int8 arg2 = PG_GETARG_INT8(1);
	int64 result;
	
	result = (int64)arg1 * (int64)arg2;
	
	if (!(arg1 >= (int32) SHRT_MIN && arg1 <= (int32) SHRT_MAX) &&
		result / arg1 != arg2)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));

	PG_RETURN_INT32((int32)result);
}

Datum
int1_mul_uint4(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	uint64 result;

	result = arg1 * (uint64)arg2;

	PG_RETURN_UINT64(result);
}


Datum
uint1_mul_int1(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	int8 arg2 = PG_GETARG_INT8(1);
	uint64  result;
	
    if (arg2 < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }

	result = (uint64)arg1 * arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint1_mul_uint2(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint32 result;

	result = (uint32)arg1 * (uint32)arg2;

	 PG_RETURN_UINT32(result);
}

Datum
uint1_mul_int2(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	int16 arg2 = PG_GETARG_INT16(1);
	uint64  result;
    if (arg2 < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }

	result = (uint64)arg1 * arg2;

	PG_RETURN_UINT64(result);	
}

Datum
int2_mul_uint1(PG_FUNCTION_ARGS)
{
	int16 arg1 = PG_GETARG_INT16(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	int32  result;

	result = arg1 * (int32)arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }
    

	PG_RETURN_INT32(result);
}

Datum
uint1_mul_int4(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	int32 arg2 = PG_GETARG_INT32(1);
	uint64  result;
    if (arg2 < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }

	result = (uint64)arg1 * arg2;

	PG_RETURN_UINT64(result);	
}

Datum
int4_mul_uint1(PG_FUNCTION_ARGS)
{
	int32 arg1 = PG_GETARG_INT32(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	int64  result;

	result = (int64)arg1 * (int64)arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }

	PG_RETURN_INT64(result);	
}

Datum
uint1_mul_uint4(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	uint64 result;

	result = (uint64)arg1 * (uint64)arg2;

	 PG_RETURN_UINT64(result);
}

Datum
uint2_mul_int1(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	int8 arg2 = PG_GETARG_INT8(1);
	uint64  result;

	result = (uint64)arg1 * (uint64)arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint2_mul_uint1(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	uint32 result;

	result = (uint32)arg1 * (uint32)arg2;

	PG_RETURN_UINT32(result);
}

Datum
uint2_mul_int2(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	int16 arg2 = PG_GETARG_INT16(1);
	uint64  result;

	result = (uint64)arg1 * (uint64)arg2;

	PG_RETURN_UINT64(result);	
}

Datum
int2_mul_uint2(PG_FUNCTION_ARGS)
{
	int16 arg1 = PG_GETARG_INT16(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint64  result;

	result = (uint64)arg1 * (uint64)arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint2_mul_int4(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	int32 arg2 = PG_GETARG_INT32(1);
	uint64  result;

	result = (uint64)arg1 * (uint64)arg2;

	PG_RETURN_UINT64(result);	
}

Datum
int4_mul_uint2(PG_FUNCTION_ARGS)
{
	int32 arg1 = PG_GETARG_INT32(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint64  result;

	result = (uint64)arg1 * (uint64)arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint2_mul_uint4(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	uint64 result;

	result = (uint64)arg1 * (uint64)arg2;

	 PG_RETURN_UINT64(result);
}

Datum
uint4_mul_int1(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	int8 arg2 = PG_GETARG_INT8(1);
	uint64  result;

	result = (uint64)arg1 * (uint64)arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint4_mul_uint1(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	uint64 result;

	result = (uint64)arg1 * (uint64)arg2;

       PG_RETURN_UINT64(result);
}

Datum
uint4_mul_int2(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	int16 arg2 = PG_GETARG_INT16(1);
	uint64  result;

	result = (uint64)arg1 * (uint64)arg2;

	PG_RETURN_UINT64(result);	
}

Datum
int2_mul_uint4(PG_FUNCTION_ARGS)
{
	int16 arg1 = PG_GETARG_INT16(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	uint64  result;

	result = (uint64)arg1 * (uint64)arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint4_mul_uint2(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint64 result;

	result = (uint64)arg1 * (uint64)arg2;

      PG_RETURN_UINT64(result);
}

Datum
uint4_mul_int4(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	int32 arg2 = PG_GETARG_INT32(1);
	uint64  result;

	result = (uint64)arg1 * (uint64)arg2;

	PG_RETURN_UINT64(result);	
}


Datum
int4_mul_uint4(PG_FUNCTION_ARGS)
{

	int32 arg1 = PG_GETARG_INT32(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	uint64  result;

	result = (uint64)arg1 * (uint64)arg2;

	PG_RETURN_UINT64(result);	
}

/*datatype divide*/
Datum
int1_div_uint1(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
int1_div_uint2(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
int1_div_int2(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	int16 arg2 = PG_GETARG_INT16(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
int2_div_int1(PG_FUNCTION_ARGS)
{
	int16 arg1 = PG_GETARG_INT16(0);
	int8 arg2 = PG_GETARG_INT8(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
int1_div_int4(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	int32 arg2 = PG_GETARG_INT32(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
int4_div_int1(PG_FUNCTION_ARGS)
{
	int32 arg1 = PG_GETARG_INT32(0);
	int8 arg2 = PG_GETARG_INT8(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
int1_div_uint4(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}


Datum
uint1_div_int1(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	int8 arg2 = PG_GETARG_INT8(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint1_div_uint2(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint1_div_int2(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	int16 arg2 = PG_GETARG_INT16(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
int2_div_uint1(PG_FUNCTION_ARGS)
{
	int16 arg1 = PG_GETARG_INT16(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint1_div_int4(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	int32 arg2 = PG_GETARG_INT32(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
int4_div_uint1(PG_FUNCTION_ARGS)
{
	int32 arg1 = PG_GETARG_INT32(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	INTEGER_DIV_INTEGER(arg1, arg2);	
}

Datum
uint1_div_uint4(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint2_div_int1(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	int8 arg2 = PG_GETARG_INT8(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint2_div_uint1(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint2_div_int2(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	int16 arg2 = PG_GETARG_INT16(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
int2_div_uint2(PG_FUNCTION_ARGS)
{
	int16 arg1 = PG_GETARG_INT16(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint2_div_int4(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	int32 arg2 = PG_GETARG_INT32(1);
	INTEGER_DIV_INTEGER(arg1, arg2);	
}

Datum
int4_div_uint2(PG_FUNCTION_ARGS)
{
	int32 arg1 = PG_GETARG_INT32(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint2_div_uint4(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint4_div_int1(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	int8 arg2 = PG_GETARG_INT8(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint4_div_uint1(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint4_div_int2(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	int16 arg2 = PG_GETARG_INT16(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
int2_div_uint4(PG_FUNCTION_ARGS)
{
	int16 arg1 = PG_GETARG_INT16(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint4_div_uint2(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint4_div_int4(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	int32 arg2 = PG_GETARG_INT32(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}


Datum
int4_div_uint4(PG_FUNCTION_ARGS)
{

	int32 arg1 = PG_GETARG_INT32(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}
/* END <l00219320> */

/* BEGIN <z00216470> : GAUSSDBV100R003C10-192
 * comparison functions for btree access method
 */
Datum
int1_int2_eq(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	int16		arg2 = PG_GETARG_INT16(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
int1_int2_ne(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	int16		arg2 = PG_GETARG_INT16(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
int1_int2_lt(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	int16		arg2 = PG_GETARG_INT16(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
int1_int2_le(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	int16		arg2 = PG_GETARG_INT16(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
int1_int2_gt(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	int16		arg2 = PG_GETARG_INT16(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
int1_int2_ge(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	int16		arg2 = PG_GETARG_INT16(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
int2_int1_eq(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
int2_int1_ne(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
int2_int1_lt(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	int8		arg2 = PG_GETARG_INT8(1);
	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
int2_int1_le(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
int2_int1_gt(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
int2_int1_ge(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
int1_int4_eq(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
int1_int4_ne(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
int1_int4_lt(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
int1_int4_le(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
int1_int4_gt(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
int1_int4_ge(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
int4_int1_eq(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
int4_int1_ne(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
int4_int1_lt(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
int4_int1_le(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
int4_int1_gt(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
int4_int1_ge(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
int1_uint1_eq(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
int1_uint1_ne(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
int1_uint1_lt(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
int1_uint1_le(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
int1_uint1_gt(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
int1_uint1_ge(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
uint1_int1_eq(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
uint1_int1_ne(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
uint1_int1_lt(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
uint1_int1_le(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
uint1_int1_gt(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
uint1_int1_ge(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
int1_uint2_eq(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
int1_uint2_ne(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
int1_uint2_lt(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
int1_uint2_le(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
int1_uint2_gt(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
int1_uint2_ge(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
uint2_int1_eq(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
uint2_int1_ne(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
uint2_int1_lt(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
uint2_int1_le(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
uint2_int1_gt(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
uint2_int1_ge(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int8		arg2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
int1_uint4_eq(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	PG_RETURN_BOOL((int64)arg1 == (int64)arg2);
}

Datum
int1_uint4_ne(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	/* BEGIN <z00216470> : DTS2013121701740 */
	PG_RETURN_BOOL((int64)arg1 != (int64)arg2);
	/* END <z00216470> */
}

Datum
int1_uint4_lt(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	PG_RETURN_BOOL((int64)arg1 < (int64)arg2);
}

Datum
int1_uint4_le(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	/* BEGIN <z00216470> : DTS2013121701740 */
	PG_RETURN_BOOL((int64)arg1 <= (int64)arg2);
	/* END <z00216470> */
}

Datum
int1_uint4_gt(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	PG_RETURN_BOOL((int64)arg1 > (int64)arg2);
}

Datum
int1_uint4_ge(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	PG_RETURN_BOOL((int64)arg1 >= (int64)arg2);
}

Datum
uint4_int1_eq(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int8		arg2 = PG_GETARG_INT8(1);
	PG_RETURN_BOOL((int64)arg1 == (int64)arg2);
}

Datum
uint4_int1_ne(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int8		arg2 = PG_GETARG_INT8(1);
	/* BEGIN <z00216470> : DTS2013121701740 */
	PG_RETURN_BOOL((int64)arg1 != (int64)arg2);
	/* END <z00216470> */
}

Datum
uint4_int1_lt(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int8		arg2 = PG_GETARG_INT8(1);
	PG_RETURN_BOOL((int64)arg1 < (int64)arg2);
}

Datum
uint4_int1_le(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int8		arg2 = PG_GETARG_INT8(1);
	/* BEGIN <z00216470> : DTS2013121701740 */
	PG_RETURN_BOOL((int64)arg1 <= (int64)arg2);
	/* END <z00216470> */
}

Datum
uint4_int1_gt(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int8		arg2 = PG_GETARG_INT8(1);
	PG_RETURN_BOOL((int64)arg1 > (int64)arg2);
}

Datum
uint4_int1_ge(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int8		arg2 = PG_GETARG_INT8(1);
	PG_RETURN_BOOL((int64)arg1 >= (int64)arg2);
}

Datum
uint1_uint2_eq(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
uint1_uint2_ne(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
uint1_uint2_lt(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
uint1_uint2_le(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
uint1_uint2_gt(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
uint1_uint2_ge(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
uint2_uint1_eq(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
uint2_uint1_ne(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
uint2_uint1_lt(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
uint2_uint1_le(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
uint2_uint1_gt(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
uint2_uint1_ge(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
uint1_uint4_eq(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
uint1_uint4_ne(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
uint1_uint4_lt(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
uint1_uint4_le(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
uint1_uint4_gt(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
uint1_uint4_ge(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
uint4_uint1_eq(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
uint4_uint1_ne(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
uint4_uint1_lt(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
uint4_uint1_le(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
uint4_uint1_gt(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
uint4_uint1_ge(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
uint1_int2_eq(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	int16		arg2 = PG_GETARG_INT16(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
uint1_int2_ne(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	int16		arg2 = PG_GETARG_INT16(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
uint1_int2_lt(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	int16		arg2 = PG_GETARG_INT16(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
uint1_int2_le(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	int16		arg2 = PG_GETARG_INT16(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
uint1_int2_gt(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	int16		arg2 = PG_GETARG_INT16(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
uint1_int2_ge(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	int16		arg2 = PG_GETARG_INT16(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
int2_uint1_eq(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
int2_uint1_ne(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
int2_uint1_lt(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
int2_uint1_le(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
int2_uint1_gt(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
int2_uint1_ge(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
uint1_int4_eq(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
uint1_int4_ne(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
uint1_int4_lt(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
uint1_int4_le(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
uint1_int4_gt(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
uint1_int4_ge(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
int4_uint1_eq(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 == arg2);
}
/* BEGIN <DTS2014021003725 >: modified by l00219320*/
Datum
int4_uint1_ne(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
int4_uint1_lt(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
int4_uint1_le(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
int4_uint1_gt(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
int4_uint1_ge(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	uint8		arg2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}
/* END <DTS2014021003725 >: modified by l00219320*/
Datum
uint2_uint4_eq(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
uint2_uint4_ne(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
uint2_uint4_lt(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
uint2_uint4_le(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
uint2_uint4_gt(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
uint2_uint4_ge(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
uint4_uint2_eq(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
uint4_uint2_ne(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
uint4_uint2_lt(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
uint4_uint2_le(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
uint4_uint2_gt(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
uint4_uint2_ge(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
uint2_int2_eq(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int16		arg2 = PG_GETARG_INT16(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
uint2_int2_ne(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int16		arg2 = PG_GETARG_INT16(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
uint2_int2_lt(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int16		arg2 = PG_GETARG_INT16(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
uint2_int2_le(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int16		arg2 = PG_GETARG_INT16(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
uint2_int2_gt(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int16		arg2 = PG_GETARG_INT16(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
uint2_int2_ge(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int16		arg2 = PG_GETARG_INT16(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
int2_uint2_eq(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
int2_uint2_ne(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
int2_uint2_lt(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
int2_uint2_le(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
int2_uint2_gt(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
int2_uint2_ge(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
uint2_int4_eq(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
uint2_int4_ne(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
uint2_int4_lt(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
uint2_int4_le(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
uint2_int4_gt(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
uint2_int4_ge(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int32		arg2 = PG_GETARG_INT32(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
int4_uint2_eq(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
int4_uint2_ne(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
int4_uint2_lt(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
int4_uint2_le(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
int4_uint2_gt(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
int4_uint2_ge(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	uint16		arg2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
uint4_int2_eq(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int16		arg2 = PG_GETARG_INT16(1);
	PG_RETURN_BOOL((int64)arg1 == (int64)arg2);
}

Datum
uint4_int2_ne(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int16		arg2 = PG_GETARG_INT16(1);
	PG_RETURN_BOOL((int64)arg1 != (int64)arg2);
}

Datum
uint4_int2_lt(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int16		arg2 = PG_GETARG_INT16(1);
	PG_RETURN_BOOL((int64)arg1 < (int64)arg2);
}

Datum
uint4_int2_le(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int16		arg2 = PG_GETARG_INT16(1);
	/* BEGIN <z00216470> : DTS2013121701740 */
	PG_RETURN_BOOL((int64)arg1 <= (int64)arg2);
	/* END <z00216470> */
}

Datum
uint4_int2_gt(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int16		arg2 = PG_GETARG_INT16(1);
	PG_RETURN_BOOL((int64)arg1 > (int64)arg2);
}

Datum
uint4_int2_ge(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int16		arg2 = PG_GETARG_INT16(1);
	PG_RETURN_BOOL((int64)arg1 >= (int64)arg2);
}

Datum
int2_uint4_eq(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	PG_RETURN_BOOL((int64)arg1 == (int64)arg2);
}

Datum
int2_uint4_ne(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	PG_RETURN_BOOL((int64)arg1 != (int64)arg2);
}

Datum
int2_uint4_lt(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	PG_RETURN_BOOL((int64)arg1 < (int64)arg2);
}

Datum
int2_uint4_le(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	PG_RETURN_BOOL((int64)arg1 <= (int64)arg2);
}

Datum
int2_uint4_gt(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	PG_RETURN_BOOL((int64)arg1 > (int64)arg2);
}

Datum
int2_uint4_ge(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	PG_RETURN_BOOL((int64)arg1 >= (int64)arg2);
}

Datum
uint4_int4_eq(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int32		arg2 = PG_GETARG_INT32(1);
	PG_RETURN_BOOL((int64)arg1 == (int64)arg2);
}

Datum
uint4_int4_ne(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int32		arg2 = PG_GETARG_INT32(1);
	PG_RETURN_BOOL((int64)arg1 != (int64)arg2);
}

Datum
uint4_int4_lt(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int32		arg2 = PG_GETARG_INT32(1);
	PG_RETURN_BOOL((int64)arg1 < (int64)arg2);
}

Datum
uint4_int4_le(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int32		arg2 = PG_GETARG_INT32(1);
	PG_RETURN_BOOL((int64)arg1 <= (int64)arg2);
}

Datum
uint4_int4_gt(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int32		arg2 = PG_GETARG_INT32(1);
	PG_RETURN_BOOL((int64)arg1 > (int64)arg2);
}

Datum
uint4_int4_ge(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int32		arg2 = PG_GETARG_INT32(1);
	PG_RETURN_BOOL((int64)arg1 >= (int64)arg2);
}

Datum
int4_uint4_eq(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	PG_RETURN_BOOL((int64)arg1 == (int64)arg2);
}

Datum
int4_uint4_ne(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	PG_RETURN_BOOL((int64)arg1 != (int64)arg2);
}

Datum
int4_uint4_lt(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	PG_RETURN_BOOL((int64)arg1 < (int64)arg2);
}

Datum
int4_uint4_le(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	PG_RETURN_BOOL((int64)arg1 <= (int64)arg2);
}

Datum
int4_uint4_gt(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	PG_RETURN_BOOL((int64)arg1 > (int64)arg2);
}

Datum
int4_uint4_ge(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	PG_RETURN_BOOL((int64)arg1 >= (int64)arg2);
}

bool
scanuint8(const char *str, bool errorOK, uint64 *result)
{
	const char *ptr = str;
	uint64		tmp = 0;

	/* skip leading spaces */
	while (*ptr && isspace((unsigned char) *ptr))
		ptr++;
	
	/* handle sign */
	if (*ptr == '-')
	{
				ereport(ERROR,
						(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
					   errmsg("value \"%s\" is out of range for type bigint unsigned",
							  str)));
	}
	else if (*ptr == '+')
		ptr++;
	
	/* require at least one digit */
	if (!isdigit((unsigned char) *ptr))
	{
		if (errorOK)
			return false;
		else
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
					 errmsg("invalid input syntax for integer: \"%s\"",
							str)));
	}

	/* process digits */
	while (*ptr && isdigit((unsigned char) *ptr))
	{
		uint64		newtmp = tmp * 10 + (*ptr++ - '0');

		if ((newtmp / 10) != tmp)		/* overflow? */
		{
			if (errorOK)
				return false;
			else
				ereport(ERROR,
						(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
					   errmsg("value \"%s\" is out of range for type bigint unsigned",
							  str)));
		}
		tmp = newtmp;
	}

	*result = tmp;

	return true;
}

Datum
uint8in(PG_FUNCTION_ARGS)
{
	char	   *str = PG_GETARG_CSTRING(0);
	uint64		result;
	(void) scanuint8(str, false, &result);
	PG_RETURN_UINT64(result);
}

void
pg_ulltoa(uint64 value, char *a)
{
	char	   *start = a;

	/* Compute the result string backwards. */
	do
	{
		uint64		remainder;
		uint64		oldval = value;

		value /= 10;
		remainder = oldval - value * 10;
		*a++ = '0' + remainder;
	} while (value != 0);


	/* Add trailing NUL byte, and back up 'a' to the last character. */
	*a-- = '\0';

	/* Reverse string. */
	while (start < a)
	{
		char		swap = *start;

		*start++ = *a;
		*a-- = swap;
	}
}

/* uint8out()
 */
Datum
uint8out(PG_FUNCTION_ARGS)
{
	uint64		val = PG_GETARG_UINT64(0);
	char		buf[MAXINT8LEN + 1];
	char	   *result;

	pg_ulltoa(val, buf);
	result = pstrdup(buf);
	PG_RETURN_CSTRING(result);
}

/*
 *		uint8recv			- converts external binary format to int8
 */
Datum
uint8recv(PG_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) PG_GETARG_POINTER(0);

	PG_RETURN_UINT64(pg_getmsguint64(buf));
}
void
pq_senduint64(StringInfo buf, uint64 i)
{
	uint32		n32;

	/* High order half first, since we're doing MSB-first */
	n32 = (uint32) (i >> 32);
	n32 = htonl(n32);
	appendBinaryStringInfo(buf, (char *) &n32, 4);

	/* Now the low order half */
	n32 = (uint32) i;
	n32 = htonl(n32);
	appendBinaryStringInfo(buf, (char *) &n32, 4);
}

/*
 *		uint8send			- converts int8 to binary format
 */
Datum
uint8send(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_senduint64(&buf, arg1);
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

Datum
uint8and(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);

    PG_RETURN_UINT64(arg1 & arg2);
}

Datum
uint8_and_int8(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	int64 arg2 = PG_GETARG_INT64(1);

	PG_RETURN_UINT64(arg1 & arg2);
}

Datum
int8_and_uint8(PG_FUNCTION_ARGS)
{
	int64 arg1 = PG_GETARG_INT64(0);
	uint64 arg2 = PG_GETARG_UINT64(1);

    PG_RETURN_UINT64(arg1 & arg2);
}

Datum
uint8or(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);

    PG_RETURN_UINT64(arg1 | arg2);
}

Datum
uint8xor(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    uint64 arg2 = PG_GETARG_UINT64(1);

    PG_RETURN_UINT64(arg1 ^ arg2);
}

Datum
uint8not(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);

    PG_RETURN_UINT64((uint64)(~arg1));
}

Datum
uint8shl(PG_FUNCTION_ARGS)
{
    uint64 arg1 = PG_GETARG_UINT64(0);
    int32 arg2 = PG_GETARG_INT32(1);

    PG_RETURN_UINT64((uint64)(arg1 << arg2));
}

Datum
uint8shr(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	int32 arg2 = PG_GETARG_INT32(1);

	PG_RETURN_UINT64(arg1 >> arg2);
}

/*
 * adapt mysql, add a new data type uint8 
  *		uint8eq			- returns 1 if arg1 == arg2
  *		uint8ne			- returns 1 if arg1 != arg2
  *		uint8lt			- returns 1 if arg1 < arg2
  *		uint8le			- returns 1 if arg1 <= arg2
  *		uint8gt			- returns 1 if arg1 > arg2
  *		uint8ge			- returns 1 if arg1 >= arg2
  */
Datum
uint8eq(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint64		arg2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
uint8ne(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint64		arg2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
uint8lt(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint64		arg2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
uint8le(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint64		arg2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
uint8gt(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint64		arg2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL( arg1 > arg2);
}

Datum
uint8ge(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint64		arg2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL( arg1 >= arg2);
}
Datum
uint8cmp(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	uint64		arg2 = PG_GETARG_UINT64(1);

	if (arg1 > arg2)
		PG_RETURN_INT32(1);
	else if (arg1 == arg2)
		PG_RETURN_INT32(0);
	else
		PG_RETURN_INT32(-1);
}

/*
 *		===================
 *		CONVERSION ROUTINES
 *		===================
 */
 
 Datum
ui8toui1(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);


	if (arg1 > UCHAR_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("tinyint unsigned out of range")));

	PG_RETURN_UINT8((uint8) arg1);
}

 Datum
ui1toui8(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);

	PG_RETURN_UINT64((uint64) arg1);
}

Datum
ui8toi1(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);

	if (arg1 > SCHAR_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("tinyint out of range")));

	PG_RETURN_INT8((int8) arg1);
}

Datum
i1toui8(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);

	if (arg1 < 0)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint unsigned out of range")));

	PG_RETURN_UINT64((uint64) arg1);
}


Datum
ui8toui2(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);

	if (arg1 > USHRT_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint unsigned out of range")));

	PG_RETURN_UINT16((uint16) arg1);
}

Datum
ui2toui8(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);

	PG_RETURN_UINT64((uint64) arg1);
}

Datum
ui8toi2(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);

	if (arg1 > SHRT_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint out of range")));

	PG_RETURN_INT16((int16) arg1);
}

Datum
i2toui8(PG_FUNCTION_ARGS)
{
	int16		arg1 = PG_GETARG_INT16(0);

	if (arg1 < 0)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint unsigned out of range")));

	PG_RETURN_UINT64((uint64) arg1);
}
Datum
ui8toi4(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);

	if (arg1 > INT_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("int  out of range")));

	PG_RETURN_INT32((int32) arg1);
}

Datum
i4toui8(PG_FUNCTION_ARGS)
{
	int32		arg1 = PG_GETARG_INT32(0);

	if (arg1 < 0)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint unsigned out of range")));

	PG_RETURN_UINT64((uint64) arg1);
}

Datum
ui8toui4(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);

	if (arg1 > UINT_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("int unsigned out of range")));

	PG_RETURN_UINT32((uint32) arg1);
}

Datum
ui4toui8(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);

	PG_RETURN_UINT64((uint64) arg1);
}

Datum
ui8toi8(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);
	int64		result;

	/*
	 * Does it fit in an int64?  Avoid assuming that we have handy constants
	 * defined for the range boundaries, instead test for overflow by
	 * reverse-conversion.
	 */
	result = (int64) arg1;

	if ((float8) result != arg1)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));

	PG_RETURN_INT64((int64) arg1);
}

Datum
i8toui8(PG_FUNCTION_ARGS)
{
	int64	arg1 = PG_GETARG_INT64(0);

	if (arg1 <  0 )
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint unsigned out of range")));

	PG_RETURN_UINT64((uint64) arg1);
}

Datum
ui8tof4(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);

	PG_RETURN_FLOAT4((float4) arg1);
}

Datum
f4toui8(PG_FUNCTION_ARGS)
{
	float4		arg1 = PG_GETARG_FLOAT4(0);
      float8   arg;
	uint64 result;
	
	arg = rint(arg1);
	result = (uint64) arg;

	if ((float8) result != arg)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint unsigned out of range")));

	PG_RETURN_UINT64(arg1);
}

Datum
ui8tof8(PG_FUNCTION_ARGS)
{
	uint64		arg1 = PG_GETARG_UINT64(0);

	PG_RETURN_FLOAT8((float8) arg1);
}

Datum
f8toui8(PG_FUNCTION_ARGS)
{
	float8		arg1 = PG_GETARG_FLOAT8(0);
      float8   arg;
	uint64 result;
	arg = rint(arg1);

	/*
	 * Does it fit in an int64?  Avoid assuming that we have handy constants
	 * defined for the range boundaries, instead test for overflow by
	 * reverse-conversion.
	 */
	result = (uint64) arg;

	if ((float8) result != arg)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint unsigned out of range")));

	PG_RETURN_UINT64(arg1);
}


Datum
uint8up(PG_FUNCTION_ARGS)
{
	uint64 arg = PG_GETARG_UINT64(0);

	PG_RETURN_UINT64(arg);
}

Datum
uint8pl(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result;

	result = arg1 +arg2; 

	PG_RETURN_UINT64(result);
}

Datum
uint8mi(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result; 
	if (arg1 < arg2) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("bigint unsigned value out of range")));
    }
	result = arg1 - arg2;
	
	PG_RETURN_UINT64(result);
}

Datum
uint8mul(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result64;

	result64 = arg1 *  arg2;

	PG_RETURN_UINT64(result64);
}

Datum
uint8div(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint8abs(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	
	PG_RETURN_UINT64(arg1);
}

Datum
uint8mod(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}



	PG_RETURN_UINT64(arg1 % arg2);
}
Datum
uint8larger(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint64 arg2 = PG_GETARG_UINT64(1);

	PG_RETURN_UINT64((arg1 > arg2) ? arg1 : arg2);
}

Datum
uint8smaller(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint64 arg2 = PG_GETARG_UINT64(1);

	PG_RETURN_UINT64((arg1 < arg2) ? arg1 : arg2);
}

Datum
uint8inc(PG_FUNCTION_ARGS)
{
	uint64		arg = PG_GETARG_UINT64(0);

	/* Overflow check */
	if (arg >= ULONG_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint unsigned out of range")));

	PG_RETURN_UINT64(arg + 1);
}

Datum
int1_pl_uint8(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result;
	result = arg1 + arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint8_pl_int1(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	int8 arg2 = PG_GETARG_INT8(1);

	uint64 result;
	result = arg1 + arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
int2_pl_uint8(PG_FUNCTION_ARGS)
{
	int16 arg1 = PG_GETARG_INT16(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result;
	result = arg1 + arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint8_pl_int2(PG_FUNCTION_ARGS)
{
	
	uint64 arg1 = PG_GETARG_UINT64(0);
	int16 arg2 = PG_GETARG_INT16(1);
	uint64 result;
	result = arg1 + arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
int4_pl_uint8(PG_FUNCTION_ARGS)
{
	int32 arg1 = PG_GETARG_INT32(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result;
	result = arg1 + arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint8_pl_int4(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	int32 arg2 = PG_GETARG_INT32(1);

	uint64 result;
	result = arg1 + arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
int8_pl_uint8(PG_FUNCTION_ARGS)
{
	int64 arg1 = PG_GETARG_INT64(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result;
	result = arg1 + arg2;

	PG_RETURN_UINT64(result) ;
}
Datum
uint8_pl_int8(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	int64 arg2 = PG_GETARG_INT64(1);
	uint64 result;
	result = arg1 + arg2;

	PG_RETURN_UINT64(result) ;
}
Datum
uint1_pl_uint8(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	int128 result;
	result = arg1 + arg2;

	PG_RETURN_INT128(result) ;
}

Datum
uint8_pl_uint1(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint8 arg2 = PG_GETARG_UINT8(1);

	uint64 result;
	result = arg1 + arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint2_pl_uint8(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result;
	result = arg1 + arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint8_pl_uint2(PG_FUNCTION_ARGS)
{
	
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint64 result;
	result = arg1 + arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint4_pl_uint8(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result;
	result = arg1 + arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint8_pl_uint4(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint32 arg2 = PG_GETARG_UINT32(1);

	uint64 result;
	result = arg1 + arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
int1_mi_uint8(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result;
	result = arg1 - arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint8_mi_int1(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	int8 arg2 = PG_GETARG_INT8(1);

	uint64 result;
	result = arg1 - arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
int2_mi_uint8(PG_FUNCTION_ARGS)
{
	int16 arg1 = PG_GETARG_INT16(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result;
	result = arg1 - arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint8_mi_int2(PG_FUNCTION_ARGS)
{
	
	uint64 arg1 = PG_GETARG_UINT64(0);
	int16 arg2 = PG_GETARG_INT16(1);
	uint64 result;
	result = arg1 - arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
int4_mi_uint8(PG_FUNCTION_ARGS)
{
	int32 arg1 = PG_GETARG_INT32(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result;
	result = arg1 - arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint8_mi_int4(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	int32 arg2 = PG_GETARG_INT32(1);

	uint64 result;
	result = arg1 - arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
int8_mi_uint8(PG_FUNCTION_ARGS)
{
	int64 arg1 = PG_GETARG_INT64(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result;
	result = arg1 - arg2;

	PG_RETURN_UINT64(result) ;
}
Datum
uint8_mi_int8(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	int64 arg2 = PG_GETARG_INT64(1);
	uint64 result;
	result = arg1 - arg2;

	PG_RETURN_UINT64(result) ;
}
Datum
uint1_mi_uint8(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	int128 result;
	result = (int128)arg1 - (int128)arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }
    

	PG_RETURN_INT128(result) ;
}

Datum
uint8_mi_uint1(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint8 arg2 = PG_GETARG_UINT8(1);

	uint64 result;
	result = arg1 - arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint2_mi_uint8(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result;
	result = arg1 - arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint8_mi_uint2(PG_FUNCTION_ARGS)
{
	
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint64 result;
	result = arg1 - arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint4_mi_uint8(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result;
	result = arg1 - arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint8_mi_uint4(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint32 arg2 = PG_GETARG_UINT32(1);

	uint64 result;
	result = arg1 - arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
int1_mul_uint8(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result;
	result = arg1 * arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint8_mul_int1(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	int8 arg2 = PG_GETARG_INT8(1);

	uint64 result;
	result = arg1 * arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
int2_mul_uint8(PG_FUNCTION_ARGS)
{
	int16 arg1 = PG_GETARG_INT16(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result;
	result = arg1 * arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint8_mul_int2(PG_FUNCTION_ARGS)
{
	
	uint64 arg1 = PG_GETARG_UINT64(0);
	int16 arg2= PG_GETARG_INT16(1);
	uint64 result;
	result = arg1 * arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
int4_mul_uint8(PG_FUNCTION_ARGS)
{
	int32 arg1 = PG_GETARG_INT32(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result;
	result = arg1 * arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint8_mul_int4(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	int32 arg2 = PG_GETARG_INT32(1);

	uint64 result;
	result = arg1 * arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
int8_mul_uint8(PG_FUNCTION_ARGS)
{
	int64 arg1 = PG_GETARG_INT64(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result;
	result = arg1 * arg2;

	PG_RETURN_UINT64(result) ;
}
Datum
uint8_mul_int8(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	int64 arg2 = PG_GETARG_INT64(1);
	uint64 result;
	result = arg1 * arg2;

	PG_RETURN_UINT64(result) ;
}
Datum
uint1_mul_uint8(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	int128 result;
	result = arg1 * arg2;

	PG_RETURN_INT128(result) ;
}

Datum
uint8_mul_uint1(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint8 arg2 = PG_GETARG_UINT8(1);

	uint64 result;
	result = arg1 * arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint2_mul_uint8(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result;
	result = arg1 * arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint8_mul_uint2(PG_FUNCTION_ARGS)
{
	
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint16 arg2= PG_GETARG_UINT16(1);
	uint64 result;
	result = arg1 * arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint4_mul_uint8(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	uint64 result;
	result = arg1 * arg2;

	PG_RETURN_UINT64(result) ;
}

Datum
uint8_mul_uint4(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint32 arg2 = PG_GETARG_UINT32(1);

	uint64 result;
	result = arg1 * arg2;

	PG_RETURN_UINT64(result) ;
}
Datum
int1_div_uint8(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint8_div_int1(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	int8 arg2 = PG_GETARG_INT8(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
int2_div_uint8(PG_FUNCTION_ARGS)
{
	int16 arg1 = PG_GETARG_INT16(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint8_div_int2(PG_FUNCTION_ARGS)
{
	
	uint64 arg1 = PG_GETARG_UINT64(0);
	int16 arg2 = PG_GETARG_INT16(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
int4_div_uint8(PG_FUNCTION_ARGS)
{
	int32 arg1 = PG_GETARG_INT32(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint8_div_int4(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	int32 arg2 = PG_GETARG_INT32(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
int8_div_uint8(PG_FUNCTION_ARGS)
{
	int64 arg1 = PG_GETARG_INT64(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}
Datum
uint8_div_int8(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	int64 arg2 = PG_GETARG_INT64(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}
Datum
uint1_div_uint8(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint8_div_uint1(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint2_div_uint8(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint8_div_uint2(PG_FUNCTION_ARGS)
{
	
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint4_div_uint8(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint8_div_uint4(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
int1_uint8_eq(PG_FUNCTION_ARGS)
{
	int8		val1 = PG_GETARG_INT8(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL((uint64)val1 == val2);
}

Datum
uint8_int1_eq(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int8		val2 = PG_GETARG_INT8(1);
	if (val2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(val1 == (uint64)val2);
}


Datum
int8_uint1_eq(PG_FUNCTION_ARGS)
{
	int64		val1 = PG_GETARG_INT64(0);
	uint64		val2 = PG_GETARG_UINT8(1);
	if (val1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL((uint64)val1 == val2);
}


Datum
int1_uint8_ne(PG_FUNCTION_ARGS)
{
	int8		val1 = PG_GETARG_INT8(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	if (val1 < 0)
		PG_RETURN_BOOL(1);

	PG_RETURN_BOOL((uint64)val1 != val2);
}

Datum
uint8_int1_ne(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int8		val2 = PG_GETARG_INT8(1);
	if (val2 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL(val1 != (uint64)val2);
}

Datum
int1_uint8_lt(PG_FUNCTION_ARGS)
{
	int8		val1 = PG_GETARG_INT8(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL((uint64)val1 < val2);
}

Datum
uint8_int1_lt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int8		val2 = PG_GETARG_INT8(1);
	if (val2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(val1 < (uint64)val2);
}

Datum
uint8_int1_gt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int8		val2 = PG_GETARG_INT8(1);
	if (val2 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL(val1 > (uint64)val2);
}

Datum
int1_uint8_gt(PG_FUNCTION_ARGS)
{
	int8		val1 = PG_GETARG_INT8(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL((uint64)val1 > val2);
}

Datum
uint8_int1_le(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int8		val2 = PG_GETARG_INT8(1);
	if (val2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(val1 <= (uint64)val2);
}

Datum
int1_uint8_le(PG_FUNCTION_ARGS)
{
	int8		val1 = PG_GETARG_INT8(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL((uint64)val1 <= val2);
}

Datum
uint8_int1_ge(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int8		val2 = PG_GETARG_INT8(1);
	if (val2 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL(val1 >= (uint64)val2);
}

Datum
int1_uint8_ge(PG_FUNCTION_ARGS)
{
	int8		val1 = PG_GETARG_INT8(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	if (val1 < 0)
		PG_RETURN_BOOL(0);

	PG_RETURN_BOOL((uint64)val1 >= val2);
}

Datum
uint1_uint8_eq(PG_FUNCTION_ARGS)
{
	uint8		val1 = PG_GETARG_UINT8(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 == val2);
}
Datum
uint1_int8_eq(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT8(0);
	int64		val2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL((int64)val1 == val2);
}

Datum
uint8_uint1_eq(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint8		val2 = PG_GETARG_UINT8(1);


	PG_RETURN_BOOL(val1 == val2);
}


Datum
uint1_uint8_ne(PG_FUNCTION_ARGS)
{
	uint8		val1 = PG_GETARG_UINT8(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 != val2);
}


Datum
uint1_int8_ne(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT8(0);
	int64		val2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL((int64)val1 != val2);
}


Datum
int8_uint1_ne(PG_FUNCTION_ARGS)
{
	int64		val1 = PG_GETARG_INT64(0);
	uint64		val2 = PG_GETARG_UINT8(1);


	PG_RETURN_BOOL(val1 != (int64)val2);
}


Datum
uint8_uint1_ne(PG_FUNCTION_ARGS)
{
	
	uint64		val1 = PG_GETARG_UINT64(0);
	uint8		val2 = PG_GETARG_UINT8(1);

	PG_RETURN_BOOL(val1 != val2);
}

Datum
uint1_uint8_lt(PG_FUNCTION_ARGS)
{
	uint8		val1 = PG_GETARG_UINT8(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 < val2);
}
Datum
int8_uint1_lt(PG_FUNCTION_ARGS)
{
	int64		val1 = PG_GETARG_INT64(0);
	uint64		val2 = PG_GETARG_UINT8(1);


	PG_RETURN_BOOL(val1 < (int64)val2);
}


Datum
uint1_int8_lt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT8(0);
	int64		val2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL((int64)val1 < val2);
}

Datum
int8_int1_gt(PG_FUNCTION_ARGS)
{
	int64		val1 = PG_GETARG_UINT8(0);
	uint64		val2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL(val1 > (int64)val2);
}

Datum
uint1_int8_gt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT8(0);
	int64		val2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL((int64)val1 > val2);
}

Datum
int8_uint1_gt(PG_FUNCTION_ARGS)
{
	int64		val1 = PG_GETARG_INT64(0);
	uint64		val2 = PG_GETARG_UINT8(1);


	PG_RETURN_BOOL(val1 > (int64)val2);
}



Datum
uint1_int8_le(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT8(0);
	int64		val2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL((int64)val1 <= val2);
}

Datum
int8_uint1_le(PG_FUNCTION_ARGS)
{
	int64		val1 = PG_GETARG_INT64(0);
	uint64		val2 = PG_GETARG_UINT8(1);


	PG_RETURN_BOOL(val1 <= (int64)val2);
}


Datum
uint1_int8_ge(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT8(0);
	int64		val2 = PG_GETARG_INT8(1);

	PG_RETURN_BOOL((int64)val1 >= val2);
}


Datum
int8_uint1_ge(PG_FUNCTION_ARGS)
{
	int64		val1 = PG_GETARG_INT64(0);
	uint64		val2 = PG_GETARG_UINT8(1);


	PG_RETURN_BOOL(val1 >= (int64)val2);
}



Datum
uint8_uint1_lt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint8		val2 = PG_GETARG_UINT8(1);


	PG_RETURN_BOOL(val1 < val2);
}

Datum
uint8_uint1_gt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint8		val2 = PG_GETARG_UINT8(1);


	PG_RETURN_BOOL(val1 > val2);
}

Datum
uint1_uint8_gt(PG_FUNCTION_ARGS)
{
	uint8		val1 = PG_GETARG_UINT8(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 > val2);
}

Datum
uint8_uint1_le(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint8		val2 = PG_GETARG_UINT8(1);


	PG_RETURN_BOOL(val1 <= val2);
}

Datum
uint1_uint8_le(PG_FUNCTION_ARGS)
{
	uint8		val1 = PG_GETARG_UINT8(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 <= val2);
}

Datum
uint8_uint1_ge(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint8		val2 = PG_GETARG_UINT8(1);


	PG_RETURN_BOOL(val1 >= val2);
}

Datum
uint1_uint8_ge(PG_FUNCTION_ARGS)
{
	uint8		val1 = PG_GETARG_UINT8(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 >= val2);
}
Datum
uint2_int8_eq(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int64       arg2 = PG_GETARG_INT64(1);
	if (arg2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
uint2_int8_ne(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int64       arg2 = PG_GETARG_INT64(1);
	if (arg2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
uint2_int8_lt(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int64       arg2 = PG_GETARG_INT64(1);
	if (arg2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
uint2_int8_le(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int64       arg2 = PG_GETARG_INT64(1);
	if (arg2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
uint2_int8_gt(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int64       arg2 = PG_GETARG_INT64(1);
	if (arg2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
uint2_int8_ge(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int64       arg2 = PG_GETARG_INT64(1);
	if (arg2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 >= arg2);
}

Datum
int2_uint8_eq(PG_FUNCTION_ARGS)
{
	int16		val1 = PG_GETARG_INT16(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL((uint64)val1 == val2);
}

Datum
uint8_int2_eq(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int16		val2 = PG_GETARG_INT16(1);
	if (val2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(val1 == (uint64)val2);
}


Datum
int2_uint8_ne(PG_FUNCTION_ARGS)
{
	int16		val1 = PG_GETARG_INT16(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL((uint64)val1 != val2);
}

Datum
uint8_int2_ne(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int16		val2 = PG_GETARG_INT16(1);
	if (val2 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL(val1 != (uint64)val2);
}

Datum
int2_uint8_lt(PG_FUNCTION_ARGS)
{
	int16		val1 = PG_GETARG_INT16(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL((uint64)val1 < val2);
}

Datum
uint8_int2_lt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int16		val2 = PG_GETARG_INT16(1);
	if (val2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(val1 < (uint64)val2);
}

Datum
uint8_int2_gt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int16		val2 = PG_GETARG_INT16(1);
	if (val2 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL(val1 > (uint64)val2);
}

Datum
int2_uint8_gt(PG_FUNCTION_ARGS)
{
	int16		val1 = PG_GETARG_INT16(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL((uint64)val1 > val2);
}

Datum
uint8_int2_le(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int16		val2 = PG_GETARG_INT16(1);
	if (val2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(val1 <= (uint64)val2);
}

Datum
int2_uint8_le(PG_FUNCTION_ARGS)
{
	int16		val1 = PG_GETARG_INT16(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL((uint64)val1 <= val2);
}

Datum
uint8_int2_ge(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int16		val2 = PG_GETARG_INT16(1);
	if (val2 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL(val1 >= (uint64)val2);
}

Datum
int2_uint8_ge(PG_FUNCTION_ARGS)
{
	int16		val1 = PG_GETARG_INT16(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL((uint64)val1 >= val2);
}

Datum
uint2_uint8_eq(PG_FUNCTION_ARGS)
{
	uint16		val1 = PG_GETARG_UINT16(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 == val2);
}

Datum
uint8_uint2_eq(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint16		val2 = PG_GETARG_UINT16(1);


	PG_RETURN_BOOL(val1 == val2);
}


Datum
uint2_uint8_ne(PG_FUNCTION_ARGS)
{
	uint16		val1 = PG_GETARG_UINT16(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 != val2);
}

Datum
uint8_uint2_ne(PG_FUNCTION_ARGS)
{
	
	uint64		val1 = PG_GETARG_UINT64(0);
	uint16		val2 = PG_GETARG_UINT16(1);

	PG_RETURN_BOOL(val1 != val2);
}

Datum
uint2_uint8_lt(PG_FUNCTION_ARGS)
{
	uint16		val1 = PG_GETARG_UINT16(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 < val2);
}

Datum
uint8_uint2_lt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint16		val2 = PG_GETARG_UINT16(1);


	PG_RETURN_BOOL(val1 < val2);
}

Datum
uint8_uint2_gt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint16		val2 = PG_GETARG_UINT16(1);


	PG_RETURN_BOOL(val1 > val2);
}

Datum
uint2_uint8_gt(PG_FUNCTION_ARGS)
{
	uint16		val1 = PG_GETARG_UINT16(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 > val2);
}

Datum
uint8_uint2_le(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint16		val2 = PG_GETARG_UINT16(1);


	PG_RETURN_BOOL(val1 <= val2);
}

Datum
uint2_uint8_le(PG_FUNCTION_ARGS)
{
	uint16		val1 = PG_GETARG_UINT16(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 <= val2);
}

Datum
uint8_uint2_ge(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint16		val2 = PG_GETARG_UINT16(1);


	PG_RETURN_BOOL(val1 >= val2);
}

Datum
uint2_uint8_ge(PG_FUNCTION_ARGS)
{
	uint16		val1 = PG_GETARG_UINT16(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 >= val2);
}

Datum
int4_uint8_eq(PG_FUNCTION_ARGS)
{
	int32		val1 = PG_GETARG_INT32(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL((uint64)val1 == val2);
}

Datum
uint8_int4_eq(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int32		val2 = PG_GETARG_INT32(1);
	if (val2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(val1 == (uint64)val2);
}

Datum
int4_uint8_ne(PG_FUNCTION_ARGS)
{
	int32		val1 = PG_GETARG_INT32(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL((uint64)val1 != val2);
}

Datum
uint8_int4_ne(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int32		val2 = PG_GETARG_INT32(1);
	if (val2 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL(val1 != (uint64)val2);
}

Datum
int4_uint8_lt(PG_FUNCTION_ARGS)
{
	int32		val1 = PG_GETARG_INT32(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL((uint64)val1 < val2);
}

Datum
uint8_int4_lt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int32		val2 = PG_GETARG_INT32(1);
	if (val2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(val1 < (uint64)val2);
}

Datum
uint8_int4_gt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int32		val2 = PG_GETARG_INT32(1);
	if (val2 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL(val1 > (uint64)val2);
}

Datum
int4_uint8_gt(PG_FUNCTION_ARGS)
{
	int32		val1 = PG_GETARG_INT32(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL((uint64)val1 > val2);
}

Datum
uint8_int4_le(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int32		val2 = PG_GETARG_INT32(1);
	if (val2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(val1 <= (uint64)val2);
}

Datum
int4_uint8_le(PG_FUNCTION_ARGS)
{
	int32		val1 = PG_GETARG_INT32(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL((uint64)val1 <= val2);
}

Datum
uint8_int4_ge(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int32		val2 = PG_GETARG_INT32(1);
	if (val2 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL(val1 >= (uint64)val2);
}

Datum
int4_uint8_ge(PG_FUNCTION_ARGS)
{
	int32		val1 = PG_GETARG_INT32(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL((uint64)val1 >= val2);
}

Datum
uint4_uint8_eq(PG_FUNCTION_ARGS)
{
	uint32		val1 = PG_GETARG_UINT32(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 == val2);
}

Datum
uint8_uint4_eq(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint32		val2 = PG_GETARG_UINT32(1);


	PG_RETURN_BOOL(val1 == val2);
}


Datum
uint4_uint8_ne(PG_FUNCTION_ARGS)
{
	uint32		val1 = PG_GETARG_UINT32(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 != val2);
}

Datum
uint8_uint4_ne(PG_FUNCTION_ARGS)
{
	
	uint64		val1 = PG_GETARG_UINT64(0);
	uint32		val2 = PG_GETARG_UINT32(1);

	PG_RETURN_BOOL(val1 != val2);
}

Datum
uint4_uint8_lt(PG_FUNCTION_ARGS)
{
	uint32		val1 = PG_GETARG_UINT32(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 < val2);
}

Datum
uint8_uint4_lt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint32		val2 = PG_GETARG_UINT32(1);


	PG_RETURN_BOOL(val1 < val2);
}

Datum
uint8_uint4_gt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint32		val2 = PG_GETARG_UINT32(1);


	PG_RETURN_BOOL(val1 > val2);
}

Datum
uint4_uint8_gt(PG_FUNCTION_ARGS)
{
	uint32		val1 = PG_GETARG_UINT32(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 > val2);
}

Datum
uint8_uint4_le(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint32		val2 = PG_GETARG_UINT32(1);


	PG_RETURN_BOOL(val1 <= val2);
}

Datum
uint4_uint8_le(PG_FUNCTION_ARGS)
{
	uint32		val1 = PG_GETARG_UINT32(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 <= val2);
}

Datum
uint8_uint4_ge(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	uint32		val2 = PG_GETARG_UINT32(1);


	PG_RETURN_BOOL(val1 >= val2);
}

Datum
uint4_uint8_ge(PG_FUNCTION_ARGS)
{
	uint32		val1 = PG_GETARG_UINT32(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	PG_RETURN_BOOL(val1 >= val2);
}

Datum
int8_uint8_eq(PG_FUNCTION_ARGS)
{
	int64		val1 = PG_GETARG_INT64(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL((uint64)val1 == val2);
}

Datum
uint8_int8_eq(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int64		val2 = PG_GETARG_INT64(1);
	if (val2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(val1 == (uint64)val2);
}

Datum
int8_uint8_ne(PG_FUNCTION_ARGS)
{
	int64		val1 = PG_GETARG_INT64(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL((uint64)val1 != val2);
}

Datum
uint8_int8_ne(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int64		val2 = PG_GETARG_INT64(1);
	if (val2 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL(val1 != (uint64)val2);
}

Datum
int8_uint8_lt(PG_FUNCTION_ARGS)
{
	int64		val1 = PG_GETARG_INT64(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL((uint64)val1 < val2);
}

Datum
uint8_int8_lt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int64		val2 = PG_GETARG_INT64(1);
	if (val2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(val1 < (uint64)val2);
}

Datum
uint8_int8_gt(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int64		val2 = PG_GETARG_INT64(1);
	if (val2 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL(val1 > (uint64)val2);
}

Datum
int8_uint8_gt(PG_FUNCTION_ARGS)
{
	int64		val1 = PG_GETARG_INT64(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL((uint64)val1 > val2);
}

Datum
uint8_int8_le(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int64		val2 = PG_GETARG_INT64(1);
	if (val2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(val1 <= (uint64)val2);
}

Datum
int8_uint8_le(PG_FUNCTION_ARGS)
{
	int64		val1 = PG_GETARG_INT64(0);
	uint64		val2 = PG_GETARG_UINT64(1);
	if (val1 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL((uint64)val1 <= val2);
}

Datum
uint8_int8_ge(PG_FUNCTION_ARGS)
{
	uint64		val1 = PG_GETARG_UINT64(0);
	int64		val2 = PG_GETARG_INT64(1);
	if (val2 < 0)
		PG_RETURN_BOOL(1);
	PG_RETURN_BOOL(val1 >= (uint64)val2);
}

Datum
int8_uint8_ge(PG_FUNCTION_ARGS)
{
	int64		val1 = PG_GETARG_INT64(0);
	uint64		val2 = PG_GETARG_UINT64(1);

	if (val1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL((uint64)val1 >= val2);
}


Datum
int1_pl_int8(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	int64 arg2 = PG_GETARG_INT64(1);
	int64		result;

	result = arg1 + arg2;

	if (SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	PG_RETURN_INT64(result);
}

Datum
int8_pl_int1(PG_FUNCTION_ARGS)
{
	int64 arg1 = PG_GETARG_INT64(0);
	int8 arg2 = PG_GETARG_INT8(1);
	int64		result;

	result = arg1 + arg2;

	if (SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	PG_RETURN_INT64(result);
}

Datum
int1_mul_int8(PG_FUNCTION_ARGS)
{
	int8 arg1 = PG_GETARG_INT8(0);
	int64 arg2 = PG_GETARG_INT64(1);
	int64		result;

	result = arg1 * arg2;

	if (arg1 != (int64) ((int32) arg1) &&
		result / arg1 != arg2)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	PG_RETURN_INT64(result);	
}

Datum
int8_mul_int1(PG_FUNCTION_ARGS)
{

	int64 arg1 = PG_GETARG_INT64(0);
	int8 arg2 = PG_GETARG_INT8(1);
	int64		result;

	result = arg1 * arg2;

	if (arg1 != (int64) ((int32) arg1) &&
		result / arg1 != arg2)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	PG_RETURN_INT64(result);	
}

Datum
int8_mi_int1(PG_FUNCTION_ARGS)
{
	int64		arg1 = PG_GETARG_INT64(0);
	int8		arg2 = PG_GETARG_INT8(1);
	int64		result;

	result = arg1 - arg2;

	/*
	 * Overflow check.	If the inputs are of the same sign then their
	 * difference cannot overflow.	If they are of different signs then the
	 * result should be of the same sign as the first input.
	 */
	if (!SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	PG_RETURN_INT64(result);
}

Datum
int1_mi_int8(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	int64	arg2 = PG_GETARG_INT64(1);
	int64		result;

	result = arg1 - arg2;

	/*
	 * Overflow check.	If the inputs are of the same sign then their
	 * difference cannot overflow.	If they are of different signs then the
	 * result should be of the same sign as the first input.
	 */
	if (!SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	PG_RETURN_INT64(result);
}

Datum
int1_div_int8(PG_FUNCTION_ARGS)
{
	int8		arg1 = PG_GETARG_INT8(0);
	int64		arg2 = PG_GETARG_INT64(1);
	float8		result;

	if (arg2 == 0)
	{
		ereport(ERROR,
				(errcode(ERRCODE_DIVISION_BY_ZERO),
				 errmsg("division by zero")));
		/* ensure compiler realizes we mustn't reach the division (gcc bug) */
		PG_RETURN_NULL();
	}
	result = (arg1*1.0) / (arg2*1.0);
	/*
	 * Overflow check.	The only possible overflow case is for arg1 =
	 * INT64_MIN, arg2 = -1, where the correct result is -INT64_MIN, which
	 * can't be represented on a two's-complement machine.	Most machines
	 * produce INT64_MIN but it seems some produce zero.
	 */
	if (arg2 == -1 && arg1 < 0 && result <= 0)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	PG_RETURN_FLOAT8(result);
}

Datum
int8_div_int1(PG_FUNCTION_ARGS)
{
	int64	arg1 = PG_GETARG_INT64(0);
	int8		arg2 = PG_GETARG_INT8(1);
	float8		result;

	if (arg2 == 0)
	{
		ereport(ERROR,
				(errcode(ERRCODE_DIVISION_BY_ZERO),
				 errmsg("division by zero")));
		/* ensure compiler realizes we mustn't reach the division (gcc bug) */
		PG_RETURN_NULL();
	}
	result = (arg1*1.0) / (arg2*1.0);
	/*
	 * Overflow check.	The only possible overflow case is for arg1 =
	 * INT64_MIN, arg2 = -1, where the correct result is -INT64_MIN, which
	 * can't be represented on a two's-complement machine.	Most machines
	 * produce INT64_MIN but it seems some produce zero.
	 */
	if (arg2 == -1 && arg1 < 0 && result <= 0)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	PG_RETURN_FLOAT8(result);
}

Datum
uint1_pl_int8(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	int64 arg2 = PG_GETARG_INT64(1);
	int128  result;

	result = arg1 + arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }

	PG_RETURN_INT128(result);	
}

Datum
int8_pl_uint1(PG_FUNCTION_ARGS)
{
	int64 arg1 = PG_GETARG_INT64(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	uint64  result;

	result = arg1 + arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint1_mi_int8(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	int64 arg2 = PG_GETARG_INT64(1);
	int128  result;

	result = arg1 - arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }

	PG_RETURN_INT128(result);	
}

Datum
int8_mi_uint1(PG_FUNCTION_ARGS)
{
	int64 arg1 = PG_GETARG_INT64(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	int128  result;

	result = arg1 - arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }

	PG_RETURN_INT128(result);	
}

Datum
uint1_mul_int8(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	int64 arg2 = PG_GETARG_INT64(1);
	int128  result;
	
    if (arg2 < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }

	result = arg1 * arg2;

	PG_RETURN_INT128(result);	
}

Datum
int8_mul_uint1(PG_FUNCTION_ARGS)
{
	int64 arg1 = PG_GETARG_INT64(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	int128  result;

	result = arg1 * arg2;
    if (result < 0) {
        ereport(ERROR, (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE), errmsg("unsigned int value out of range")));
    }

	PG_RETURN_INT128(result);	
}

Datum
uint1_div_int8(PG_FUNCTION_ARGS)
{
	uint8		arg1 = PG_GETARG_UINT8(0);
	int64		arg2 = PG_GETARG_INT64(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
int8_div_uint1(PG_FUNCTION_ARGS)
{
	int64	arg1 = PG_GETARG_INT64(0);
	uint8		arg2 = PG_GETARG_UINT8(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint2_pl_int8(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	int64 arg2 = PG_GETARG_INT64(1);
	uint64  result;

	result = arg1 + arg2;

	PG_RETURN_UINT64(result);	
}

Datum
int8_pl_uint2(PG_FUNCTION_ARGS)
{
	int64 arg1 = PG_GETARG_INT64(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint64  result;

	result = arg1 + arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint2_mi_int8(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	int64 arg2 = PG_GETARG_INT64(1);
	uint64  result;

	result = arg1 - arg2;

	PG_RETURN_UINT64(result);	
}

Datum
int8_mi_uint2(PG_FUNCTION_ARGS)
{
	int64 arg1 = PG_GETARG_INT64(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint64  result;

	result = arg1 - arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint2_mul_int8(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	int64 arg2 = PG_GETARG_INT64(1);
	uint64  result;

	result = arg1 * arg2;

	PG_RETURN_UINT64(result);	
}

Datum
int8_mul_uint2(PG_FUNCTION_ARGS)
{
	int64 arg1 = PG_GETARG_INT64(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	uint64  result;

	result = arg1 * arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint2_div_int8(PG_FUNCTION_ARGS)
{
	uint16		arg1 = PG_GETARG_UINT16(0);
	int64		arg2 = PG_GETARG_INT64(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
int8_div_uint2(PG_FUNCTION_ARGS)
{
	int64	arg1 = PG_GETARG_INT64(0);
	uint16		arg2 = PG_GETARG_UINT16(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint4_pl_int8(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	int64 arg2 = PG_GETARG_INT64(1);
	uint64  result;

	result = arg1 + arg2;

	PG_RETURN_UINT64(result);	
}

Datum
int8_pl_uint4(PG_FUNCTION_ARGS)
{
	int64 arg1 = PG_GETARG_INT64(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	uint64  result;

	result = arg1 + arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint4_mi_int8(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	int64 arg2 = PG_GETARG_INT64(1);
	uint64  result;

	result = arg1 - arg2;

	PG_RETURN_UINT64(result);	
}

Datum
int8_mi_uint4(PG_FUNCTION_ARGS)
{
	int64 arg1 = PG_GETARG_INT64(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	uint64  result;

	result = arg1 - arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint4_mul_int8(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	int64 arg2 = PG_GETARG_INT64(1);
	uint64  result;

	result = arg1 * arg2;

	PG_RETURN_UINT64(result);	
}

Datum
int8_mul_uint4(PG_FUNCTION_ARGS)
{
	int64 arg1 = PG_GETARG_INT64(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	uint64  result;

	result = arg1 * arg2;

	PG_RETURN_UINT64(result);	
}

Datum
uint4_div_int8(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int64		arg2 = PG_GETARG_INT64(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
int8_div_uint4(PG_FUNCTION_ARGS)
{
	int64	arg1 = PG_GETARG_INT64(0);
	uint32		arg2 = PG_GETARG_UINT32(1);
	INTEGER_DIV_INTEGER(arg1, arg2);
}

Datum
uint4_int8_eq(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int64       arg2 = PG_GETARG_INT64(1);
	if (arg2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
uint4_int8_ne(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int64       arg2 = PG_GETARG_INT64(1);
	if (arg2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
uint4_int8_lt(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int64       arg2 = PG_GETARG_INT64(1);
	if (arg2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
uint4_int8_le(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int64       arg2 = PG_GETARG_INT64(1);
	if (arg2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
uint4_int8_gt(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int64       arg2 = PG_GETARG_INT64(1);
	if (arg2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
uint4_int8_ge(PG_FUNCTION_ARGS)
{
	uint32		arg1 = PG_GETARG_UINT32(0);
	int64       arg2 = PG_GETARG_INT64(1);
	if (arg2 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 >= arg2);
}
Datum
int8_uint2_eq(PG_FUNCTION_ARGS)
{
	
	int64       arg1 = PG_GETARG_INT64(0);
    uint16		arg2 = PG_GETARG_UINT64(1);
    
	if (arg1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
int8_uint2_ne(PG_FUNCTION_ARGS)
{
	int64       arg1 = PG_GETARG_INT64(0);
    uint16		arg2 = PG_GETARG_UINT64(1);
    
	if (arg1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
int8_uint2_lt(PG_FUNCTION_ARGS)
{
	int64       arg1 = PG_GETARG_INT64(0);
    uint16		arg2 = PG_GETARG_UINT64(1);
    
	if (arg1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
int8_uint2_le(PG_FUNCTION_ARGS)
{
	int64       arg1 = PG_GETARG_INT64(0);
    uint16		arg2 = PG_GETARG_UINT64(1);
    
	if (arg1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
int8_uint2_gt(PG_FUNCTION_ARGS)
{
	int64       arg1 = PG_GETARG_INT64(0);
    uint16		arg2 = PG_GETARG_UINT64(1);
    
	if (arg1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
int8_uint2_ge(PG_FUNCTION_ARGS)
{
	int64       arg1 = PG_GETARG_INT64(0);
    uint16		arg2 = PG_GETARG_UINT64(1);
	if (arg1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 >= arg2);
}
Datum
int8_uint4_eq(PG_FUNCTION_ARGS)
{
	
	int64       arg1 = PG_GETARG_INT64(0);
    uint32		arg2 = PG_GETARG_UINT64(1);
    
	if (arg1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 == arg2);
}

Datum
int8_uint4_ne(PG_FUNCTION_ARGS)
{
	int64       arg1 = PG_GETARG_INT64(0);
    uint32		arg2 = PG_GETARG_UINT64(1);
    
	if (arg1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 != arg2);
}

Datum
int8_uint4_lt(PG_FUNCTION_ARGS)
{
	int64       arg1 = PG_GETARG_INT64(0);
    uint32		arg2 = PG_GETARG_UINT64(1);
    
	if (arg1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 < arg2);
}

Datum
int8_uint4_le(PG_FUNCTION_ARGS)
{
	int64       arg1 = PG_GETARG_INT64(0);
    uint32		arg2 = PG_GETARG_UINT64(1);
    
	if (arg1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 <= arg2);
}

Datum
int8_uint4_gt(PG_FUNCTION_ARGS)
{
	int64       arg1 = PG_GETARG_INT64(0);
    uint32		arg2 = PG_GETARG_UINT64(1);
    
	if (arg1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 > arg2);
}

Datum
int8_uint4_ge(PG_FUNCTION_ARGS)
{
	int64       arg1 = PG_GETARG_INT64(0);
    uint32		arg2 = PG_GETARG_UINT64(1);
	if (arg1 < 0)
		PG_RETURN_BOOL(0);
	PG_RETURN_BOOL(arg1 >= arg2);
}
Datum bool_uint1(PG_FUNCTION_ARGS)
{
    if (PG_GETARG_BOOL(0) == false)
        PG_RETURN_UINT8(0);
    else
        PG_RETURN_UINT8(1);
}
Datum uint1_bool(PG_FUNCTION_ARGS)
{
    if (PG_GETARG_UINT8(0) == 0)
        PG_RETURN_BOOL(false);
    else
        PG_RETURN_BOOL(true);
}


Datum uint4_bool(PG_FUNCTION_ARGS)
{
    if (PG_GETARG_UINT32(0) == 0)
        PG_RETURN_BOOL(false);
    else
        PG_RETURN_BOOL(true);
}

/* Cast bool -> Uint4 */
Datum bool_uint4(PG_FUNCTION_ARGS)
{
    if (PG_GETARG_BOOL(0) == false)
        PG_RETURN_UINT32(0);
    else
        PG_RETURN_UINT32(1);
}

/* Cast uint2 -> bool */
Datum uint2_bool(PG_FUNCTION_ARGS)
{
    if (PG_GETARG_UINT16(0) == 0)
        PG_RETURN_BOOL(false);
    else
        PG_RETURN_BOOL(true);
}

/* Cast bool -> uint2 */
Datum bool_uint2(PG_FUNCTION_ARGS)
{
    if (PG_GETARG_BOOL(0) == false)
        PG_RETURN_UINT16(0);
    else
        PG_RETURN_UINT16(1);
}

Datum uint8_bool(PG_FUNCTION_ARGS)
{
    if (PG_GETARG_UINT64(0) == 0)
        PG_RETURN_BOOL(false);
    else
        PG_RETURN_BOOL(true);
}

/* Cast bool -> uint8 */
Datum bool_uint8(PG_FUNCTION_ARGS)
{
    if (PG_GETARG_BOOL(0) == false)
        PG_RETURN_UINT64(0);
    else
        PG_RETURN_UINT64(1);
}



Datum
uint1_int1_mod(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	int8 arg2 = PG_GETARG_INT8(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}



	PG_RETURN_INT64(arg1 % arg2);
}
Datum
uint1_int2_mod(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	int16 arg2 = PG_GETARG_INT16(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}




	PG_RETURN_INT64(arg1 % arg2);
}
Datum
uint1_int4_mod(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	int32 arg2 = PG_GETARG_INT32(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}




	PG_RETURN_INT64(arg1 % arg2);
}
Datum
uint1_int8_mod(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	int64 arg2 = PG_GETARG_INT64(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}




	PG_RETURN_INT64(arg1 % arg2);
}
Datum
uint1_uint2_mod(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	PG_RETURN_INT64(arg1 % arg2);
}
Datum
uint1_uint4_mod(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	PG_RETURN_INT64(arg1 % arg2);
}
Datum
uint1_uint8_mod(PG_FUNCTION_ARGS)
{
	uint8 arg1 = PG_GETARG_UINT8(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	PG_RETURN_INT64(arg1 % arg2);
}



Datum
int1_uint1_mod(PG_FUNCTION_ARGS)
{
	
	int8 arg1 = PG_GETARG_INT8(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}


	PG_RETURN_INT64(arg1 % arg2);
}

Datum
int2_uint1_mod(PG_FUNCTION_ARGS)
{
	
	int16 arg1 = PG_GETARG_INT16(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	else if (arg2 < 0)
	{
		PG_RETURN_INT64(arg1 % (-arg2));
	}


	PG_RETURN_UINT64(arg1 % arg2);
}

Datum
int4_uint1_mod(PG_FUNCTION_ARGS)
{
	
	int32 arg1 = PG_GETARG_INT32(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	else if (arg2 < 0)
	{
		PG_RETURN_INT64(arg1 % (-arg2));
	}


	PG_RETURN_UINT64(arg1 % arg2);
}
Datum
int8_uint1_mod(PG_FUNCTION_ARGS)
{
	
	int64 arg1 = PG_GETARG_INT64(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	PG_RETURN_UINT64(arg1 % arg2);
}

Datum
uint2_int1_mod(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	int8 arg2 = PG_GETARG_INT8(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}



	PG_RETURN_INT64(arg1 % arg2);
}
Datum
uint2_int2_mod(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	int16 arg2 = PG_GETARG_INT16(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}




	PG_RETURN_INT64(arg1 % arg2);
}
Datum
uint2_int4_mod(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	int32 arg2 = PG_GETARG_INT32(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}




	PG_RETURN_INT64(arg1 % arg2);
}
Datum
uint2_int8_mod(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	int64 arg2 = PG_GETARG_INT64(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}




	PG_RETURN_INT64(arg1 % arg2);
}
Datum
uint2_uint1_mod(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	PG_RETURN_INT64(arg1 % arg2);
}


Datum
uint2_uint4_mod(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	PG_RETURN_INT64(arg1 % arg2);
}
Datum
uint2_uint8_mod(PG_FUNCTION_ARGS)
{
	uint16 arg1 = PG_GETARG_UINT16(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	PG_RETURN_INT64(arg1 % arg2);
}



Datum
int1_uint2_mod(PG_FUNCTION_ARGS)
{
	
	int8 arg1 = PG_GETARG_INT8(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}


	PG_RETURN_INT64(arg1 % arg2);
}

Datum
int2_uint2_mod(PG_FUNCTION_ARGS)
{
	
	int16 arg1 = PG_GETARG_INT16(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	else if (arg2 < 0)
	{
		PG_RETURN_INT64(arg1 % (-arg2));
	}


	PG_RETURN_UINT64(arg1 % arg2);
}

Datum
int4_uint2_mod(PG_FUNCTION_ARGS)
{
	
	int32 arg1 = PG_GETARG_INT32(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	else if (arg2 < 0)
	{
		PG_RETURN_INT64(arg1 % (-arg2));
	}


	PG_RETURN_UINT64(arg1 % arg2);
}
Datum
int8_uint2_mod(PG_FUNCTION_ARGS)
{
	
	int64 arg1 = PG_GETARG_INT64(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	PG_RETURN_UINT64(arg1 % arg2);
}

Datum uint4_int1_mod(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	int8 arg2 = PG_GETARG_INT8(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}



	PG_RETURN_INT64(arg1 % arg2);
}
Datum uint4_int2_mod(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	int16 arg2 = PG_GETARG_INT16(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}




	PG_RETURN_INT64(arg1 % arg2);
}
Datum uint4_int4_mod(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	int32 arg2 = PG_GETARG_INT32(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}




	PG_RETURN_INT64(arg1 % arg2);
}
Datum uint4_int8_mod(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	int64 arg2 = PG_GETARG_INT64(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}




	PG_RETURN_INT64(arg1 % arg2);
}


Datum
uint4_uint1_mod(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	PG_RETURN_INT64(arg1 % arg2);
}
Datum
uint4_uint2_mod(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint16 arg2 = PG_GETARG_UINT16(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	PG_RETURN_INT64(arg1 % arg2);
}




Datum
uint4_uint8_mod(PG_FUNCTION_ARGS)
{
	uint32 arg1 = PG_GETARG_UINT32(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	PG_RETURN_INT64(arg1 % arg2);
}



Datum
int1_uint4_mod(PG_FUNCTION_ARGS)
{
	
	int8 arg1 = PG_GETARG_INT8(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}


	PG_RETURN_INT64(arg1 % arg2);
}

Datum
int2_uint4_mod(PG_FUNCTION_ARGS)
{
	
	int16 arg1 = PG_GETARG_INT16(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	else if (arg2 < 0)
	{
		PG_RETURN_INT64(arg1 % (-arg2));
	}


	PG_RETURN_UINT64(arg1 % arg2);
}

Datum
int4_uint4_mod(PG_FUNCTION_ARGS)
{
	
	int32 arg1 = PG_GETARG_INT32(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	else if (arg2 < 0)
	{
		PG_RETURN_INT64(arg1 % (-arg2));
	}


	PG_RETURN_UINT64(arg1 % arg2);
}
Datum
int8_uint4_mod(PG_FUNCTION_ARGS)
{
	
	int64 arg1 = PG_GETARG_INT64(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	PG_RETURN_UINT64(arg1 % arg2);
}

Datum
uint8_int1_mod(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	int8 arg2 = PG_GETARG_UINT8(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}



	PG_RETURN_INT64(arg1 % arg2);
}
Datum
uint8_int2_mod(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	int16 arg2 = PG_GETARG_INT16(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}




	PG_RETURN_INT64(arg1 % arg2);
}
Datum
uint8_int4_mod(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	int32 arg2 = PG_GETARG_INT32(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}




	PG_RETURN_INT64(arg1 % arg2);
}
Datum
uint8_int8_mod(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	int64 arg2 = PG_GETARG_INT64(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}




	PG_RETURN_INT64(arg1 % arg2);
}


Datum
int1_uint8_mod(PG_FUNCTION_ARGS)
{
	
	int8 arg1 = PG_GETARG_INT8(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}


	PG_RETURN_INT64(arg1 % arg2);
}

Datum
int2_uint8_mod(PG_FUNCTION_ARGS)
{
	
	int16 arg1 = PG_GETARG_INT16(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	else if (arg2 < 0)
	{
		PG_RETURN_INT64(arg1 % (-arg2));
	}


	PG_RETURN_UINT64(arg1 % arg2);
}

Datum
int4_uint8_mod(PG_FUNCTION_ARGS)
{
	
	int32 arg1 = PG_GETARG_INT32(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	else if (arg2 < 0)
	{
		PG_RETURN_INT64(arg1 % (-arg2));
	}


	PG_RETURN_UINT64(arg1 % arg2);
}
Datum
int8_uint8_mod(PG_FUNCTION_ARGS)
{
	
	int64 arg1 = PG_GETARG_INT64(0);
	uint64 arg2 = PG_GETARG_UINT64(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}

	PG_RETURN_UINT64(arg1 % arg2);
}
Datum
uint8_uint1_mod(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint8 arg2 = PG_GETARG_UINT8(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}



	PG_RETURN_INT64(arg1 % arg2);
}
Datum
uint8_uint2_mod(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	int16 arg2 = PG_GETARG_UINT16(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}




	PG_RETURN_INT64(arg1 % arg2);
}
Datum
uint8_uint4_mod(PG_FUNCTION_ARGS)
{
	uint64 arg1 = PG_GETARG_UINT64(0);
	uint32 arg2 = PG_GETARG_UINT32(1);
	
	if (arg2 == 0)
	{
		PG_RETURN_NULL();
	}




	PG_RETURN_INT64(arg1 % arg2);
}

