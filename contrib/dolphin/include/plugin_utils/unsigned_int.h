#include "plugin_postgres.h"

PG_FUNCTION_INFO_V1_PUBLIC(i4toui1);
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
PG_FUNCTION_INFO_V1_PUBLIC(uint12cmp);
PG_FUNCTION_INFO_V1_PUBLIC(uint14cmp);
PG_FUNCTION_INFO_V1_PUBLIC(uint18cmp);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int1cmp);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int2cmp);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int4cmp);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int8cmp);
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
PG_FUNCTION_INFO_V1_PUBLIC(uint24cmp);
PG_FUNCTION_INFO_V1_PUBLIC(uint28cmp);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int2cmp);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int4cmp);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int8cmp);
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
PG_FUNCTION_INFO_V1_PUBLIC(uint48cmp);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int4cmp);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int8cmp);
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
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int8cmp);
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
PG_FUNCTION_INFO_V1_PUBLIC(uint1_pl_int1);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_mi_int1);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_mul_int1);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_div_int1);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int1_eq);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int1_ne);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int1_lt);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int1_le);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int1_gt);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int1_ge);
PG_FUNCTION_INFO_V1_PUBLIC(int1_pl_uint1);
PG_FUNCTION_INFO_V1_PUBLIC(int1_mi_uint1);
PG_FUNCTION_INFO_V1_PUBLIC(int1_mul_uint1);
PG_FUNCTION_INFO_V1_PUBLIC(int1_div_uint1);
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint1_eq);
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint1_ne);
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint1_lt);
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint1_le);
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint1_gt);
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint1_ge);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_pl_int2);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_mi_int2);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_mul_int2);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_div_int2);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int2_eq);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int2_ne);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int2_lt);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int2_le);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int2_gt);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int2_ge);
PG_FUNCTION_INFO_V1_PUBLIC(int2_pl_uint2);
PG_FUNCTION_INFO_V1_PUBLIC(int2_mi_uint2);
PG_FUNCTION_INFO_V1_PUBLIC(int2_mul_uint2);
PG_FUNCTION_INFO_V1_PUBLIC(int2_div_uint2);
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint2_eq);
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint2_ne);
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint2_lt);
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint2_le);
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint2_gt);
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint2_ge);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_pl_int4);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_mi_int4);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_mul_int4);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_div_int4);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int4_eq);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int4_ne);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int4_lt);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int4_le);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int4_gt);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int4_ge);
PG_FUNCTION_INFO_V1_PUBLIC(int4_pl_uint4);
PG_FUNCTION_INFO_V1_PUBLIC(int4_mi_uint4);
PG_FUNCTION_INFO_V1_PUBLIC(int4_mul_uint4);
PG_FUNCTION_INFO_V1_PUBLIC(int4_div_uint4);
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint4_eq);
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint4_ne);
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint4_lt);
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint4_le);
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint4_gt);
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint4_ge);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_pl_int8);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_mi_int8);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_mul_int8);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_div_int8);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int8_eq);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int8_ne);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int8_lt);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int8_le);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int8_gt);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int8_ge);
PG_FUNCTION_INFO_V1_PUBLIC(int8_pl_uint8);
PG_FUNCTION_INFO_V1_PUBLIC(int8_mi_uint8);
PG_FUNCTION_INFO_V1_PUBLIC(int8_mul_uint8);
PG_FUNCTION_INFO_V1_PUBLIC(int8_div_uint8);
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint8_eq);
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint8_ne);
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint8_lt);
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint8_le);
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint8_gt);
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint8_ge);
PG_FUNCTION_INFO_V1_PUBLIC(i1toui4);
PG_FUNCTION_INFO_V1_PUBLIC(ui4toi1);
PG_FUNCTION_INFO_V1_PUBLIC(uint8um);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int1_mod);
PG_FUNCTION_INFO_V1_PUBLIC(int1_uint1_mod);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int2_mod);
PG_FUNCTION_INFO_V1_PUBLIC(int2_uint2_mod);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int4_mod);
PG_FUNCTION_INFO_V1_PUBLIC(int4_uint4_mod);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_int8_mod);
PG_FUNCTION_INFO_V1_PUBLIC(int8_uint8_mod);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_xor_int1);
PG_FUNCTION_INFO_V1_PUBLIC(int1_xor_uint1);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_xor_int2);
PG_FUNCTION_INFO_V1_PUBLIC(int2_xor_uint2);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_xor_int4);
PG_FUNCTION_INFO_V1_PUBLIC(int4_xor_uint4);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_xor_int8);
PG_FUNCTION_INFO_V1_PUBLIC(int8_xor_uint8);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_or_int1);
PG_FUNCTION_INFO_V1_PUBLIC(int1_or_uint1);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_or_int2);
PG_FUNCTION_INFO_V1_PUBLIC(int2_or_uint2);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_or_int4);
PG_FUNCTION_INFO_V1_PUBLIC(int4_or_uint4);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_or_int8);
PG_FUNCTION_INFO_V1_PUBLIC(int8_or_uint8);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_and_int1);
PG_FUNCTION_INFO_V1_PUBLIC(int1_and_uint1);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_and_int2);
PG_FUNCTION_INFO_V1_PUBLIC(int2_and_uint2);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_and_int4);
PG_FUNCTION_INFO_V1_PUBLIC(int4_and_uint4);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_and_int8);
PG_FUNCTION_INFO_V1_PUBLIC(int8_and_uint8);
PG_FUNCTION_INFO_V1_PUBLIC(i2_cast_ui1);
PG_FUNCTION_INFO_V1_PUBLIC(i4_cast_ui1);
PG_FUNCTION_INFO_V1_PUBLIC(i8_cast_ui1);
PG_FUNCTION_INFO_V1_PUBLIC(i2_cast_ui2);
PG_FUNCTION_INFO_V1_PUBLIC(i4_cast_ui2);
PG_FUNCTION_INFO_V1_PUBLIC(i8_cast_ui2);
PG_FUNCTION_INFO_V1_PUBLIC(i2_cast_ui4);
PG_FUNCTION_INFO_V1_PUBLIC(i4_cast_ui4);
PG_FUNCTION_INFO_V1_PUBLIC(i8_cast_ui4);
PG_FUNCTION_INFO_V1_PUBLIC(i2_cast_ui8);
PG_FUNCTION_INFO_V1_PUBLIC(i4_cast_ui8);
PG_FUNCTION_INFO_V1_PUBLIC(i8_cast_ui8);
PG_FUNCTION_INFO_V1_PUBLIC(f4_cast_ui1);
PG_FUNCTION_INFO_V1_PUBLIC(f8_cast_ui1);
PG_FUNCTION_INFO_V1_PUBLIC(f4_cast_ui2);
PG_FUNCTION_INFO_V1_PUBLIC(f8_cast_ui2);
PG_FUNCTION_INFO_V1_PUBLIC(f4_cast_ui4);
PG_FUNCTION_INFO_V1_PUBLIC(f8_cast_ui4);
PG_FUNCTION_INFO_V1_PUBLIC(f4_cast_ui8);
PG_FUNCTION_INFO_V1_PUBLIC(f8_cast_ui8);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint2_eq);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint2_lt);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint2_le);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint2_gt);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint2_ge);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint4_eq);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint4_lt);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint4_le);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint4_gt);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint4_ge);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint8_eq);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint8_lt);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint8_le);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint8_gt);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_uint8_ge);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint4_eq);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint4_lt);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint4_le);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint4_gt);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint4_ge);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint8_eq);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint8_lt);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint8_le);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint8_gt);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_uint8_ge);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint8_eq);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint8_lt);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint8_le);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint8_gt);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_uint8_ge);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int2_eq);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int2_lt);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int2_le);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int2_gt);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int2_ge);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int4_eq);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int4_lt);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int4_le);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int4_gt);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int4_ge);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int8_eq);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int8_lt);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int8_le);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int8_gt);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_int8_ge);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int4_eq);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int4_lt);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int4_le);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int4_gt);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int4_ge);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int8_eq);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int8_lt);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int8_le);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int8_gt);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_int8_ge);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int8_eq);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int8_lt);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int8_le);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int8_gt);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_int8_ge);
PG_FUNCTION_INFO_V1_PUBLIC(uint1_sortsupport);
PG_FUNCTION_INFO_V1_PUBLIC(uint2_sortsupport);
PG_FUNCTION_INFO_V1_PUBLIC(uint4_sortsupport);
PG_FUNCTION_INFO_V1_PUBLIC(uint8_sortsupport);

extern "C" DLL_PUBLIC Datum i4toui1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int1_mod(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint1_mod(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int2_mod(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint2_mod(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int4_mod(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint4_mod(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int8_mod(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint8_mod(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8um(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ui4toi1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i1toui4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_pl_int1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_mi_int1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_mul_int1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_div_int1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int1_eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int1_ne(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int1_lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int1_le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int1_gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int1_ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_pl_uint1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_mi_uint1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_mul_uint1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_div_uint1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint1_eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint1_ne(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint1_lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint1_le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint1_gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_uint1_ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_pl_int2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_mi_int2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_mul_int2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_div_int2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int2_eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int2_ne(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int2_lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int2_le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int2_gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int2_ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_pl_uint2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_mi_uint2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_mul_uint2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_div_uint2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint2_eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint2_ne(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint2_lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint2_le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint2_gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_uint2_ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_pl_int4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_mi_int4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_mul_int4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_div_int4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int4_eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int4_ne(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int4_lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int4_le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int4_gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int4_ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_pl_uint4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_mi_uint4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_mul_uint4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_div_uint4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint4_eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint4_ne(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint4_lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint4_le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint4_gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_uint4_ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_pl_int8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_mi_int8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_mul_int8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_div_int8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int8_eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int8_ne(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int8_lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int8_le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int8_gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_int8_ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_pl_uint8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_mi_uint8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_mul_uint8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_div_uint8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint8_eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint8_ne(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint8_lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint8_le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint8_gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_uint8_ge(PG_FUNCTION_ARGS);
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
extern "C" DLL_PUBLIC Datum uint12cmp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint14cmp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint18cmp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int1cmp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int2cmp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int4cmp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int8cmp(PG_FUNCTION_ARGS);
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
extern "C" DLL_PUBLIC Datum uint24cmp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint28cmp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int2cmp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int4cmp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int8cmp(PG_FUNCTION_ARGS);
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
extern "C" DLL_PUBLIC Datum uint48cmp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int4cmp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int8cmp(PG_FUNCTION_ARGS);
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
extern "C" DLL_PUBLIC Datum uint8out(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8recv(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8send(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8and(PG_FUNCTION_ARGS);
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
extern "C" DLL_PUBLIC Datum uint8_int8cmp(PG_FUNCTION_ARGS);
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
extern "C" DLL_PUBLIC Datum uint1_xor_int1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_xor_uint1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_xor_int2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_xor_uint2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_xor_int4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_xor_uint4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_xor_int8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_xor_uint8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_or_int1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_or_uint1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_or_int2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_or_uint2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_or_int4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_or_uint4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_or_int8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_or_uint8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_and_int1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int1_and_uint1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_and_int2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int2_and_uint2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_and_int4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int4_and_uint4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_and_int8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum int8_and_uint8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i2_cast_ui1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i4_cast_ui1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i8_cast_ui1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i2_cast_ui2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i4_cast_ui2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i8_cast_ui2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i2_cast_ui4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i4_cast_ui4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i8_cast_ui4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i2_cast_ui8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i4_cast_ui8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum i8_cast_ui8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum f4_cast_ui1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum f8_cast_ui1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum f4_cast_ui2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum f8_cast_ui2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum f4_cast_ui4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum f8_cast_ui4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum f4_cast_ui8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum f8_cast_ui8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint2_eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint2_lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint2_le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint2_gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint2_ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint4_eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint4_lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint4_le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint4_gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint4_ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint8_eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint8_lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint8_le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint8_gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_uint8_ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint4_eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint4_lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint4_le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint4_gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint4_ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint8_eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint8_lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint8_le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint8_gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_uint8_ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint8_eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint8_lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint8_le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint8_gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_uint8_ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int2_eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int2_lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int2_le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int2_gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int2_ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int4_eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int4_lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int4_le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int4_gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int4_ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int8_eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int8_lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int8_le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int8_gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_int8_ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int4_eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int4_lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int4_le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int4_gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int4_ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int8_eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int8_lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int8_le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int8_gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_int8_ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int8_eq(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int8_lt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int8_le(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int8_gt(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_int8_ge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint1_sortsupport(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint2_sortsupport(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint4_sortsupport(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum uint8_sortsupport(PG_FUNCTION_ARGS);