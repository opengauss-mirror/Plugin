#include "plugin_postgres.h"
#include "fmgr.h"

/* from aggregate.c */
extern "C" DLL_PUBLIC Datum orafce_listagg1_transfn(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_wm_concat_transfn(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_listagg2_transfn(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_listagg_finalfn(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_median4_transfn(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_median4_finalfn(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_median8_transfn(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_median8_finalfn(PG_FUNCTION_ARGS);

/* from alert.c */
extern "C" DLL_PUBLIC Datum dbms_alert_register(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_alert_remove(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_alert_removeall(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_alert_set_defaults(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_alert_signal(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_alert_waitany(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_alert_waitone(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_alert_defered_signal(PG_FUNCTION_ARGS);

/* from assert.c */
extern "C" DLL_PUBLIC Datum dbms_assert_enquote_literal(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_assert_enquote_name(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_assert_noop(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_assert_qualified_sql_name(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_assert_schema_name(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_assert_simple_sql_name(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_assert_object_name(PG_FUNCTION_ARGS);

/* from convert.c */
extern "C" DLL_PUBLIC Datum orafce_to_char_int4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_to_char_int8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_to_char_float4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_to_char_float8(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_to_char_numeric(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_to_char_timestamp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_to_number(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_to_multi_byte(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_to_single_byte(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_unistr(PG_FUNCTION_ARGS);

/* from datefce.c */
extern "C" DLL_PUBLIC Datum next_day(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum next_day_by_index(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum months_between(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ora_to_date(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ora_date_trunc(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ora_date_round(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ora_timestamptz_trunc(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ora_timestamptz_round(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ora_timestamp_trunc(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ora_timestamp_round(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_sysdate(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_sessiontimezone(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_dbtimezone(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum add_months_whale(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum last_day_whale(PG_FUNCTION_ARGS);

/* from file.c */
extern "C" DLL_PUBLIC Datum utl_file_fopen(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum utl_file_is_open(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum utl_file_get_line(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum utl_file_get_nextline(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum utl_file_put(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum utl_file_put_line(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum utl_file_new_line(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum utl_file_putf(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum utl_file_fflush(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum utl_file_fclose(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum utl_file_fclose_all(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum utl_file_fremove(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum utl_file_frename(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum utl_file_fcopy(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum utl_file_fgetattr(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum utl_file_tmpdir(PG_FUNCTION_ARGS);

/* from others.c */
extern "C" DLL_PUBLIC Datum ora_nvl(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ora_nvl2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ora_set_nls_sort(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ora_lnnvl(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ora_decode(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_dump(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ora_get_major_version(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ora_get_major_version_num(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ora_get_full_version_num(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ora_get_platform(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum ora_get_status(PG_FUNCTION_ARGS);

/* from pipe.c */
extern "C" DLL_PUBLIC Datum dbms_pipe_pack_message_text(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_unpack_message_text(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_send_message(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_receive_message(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_unique_session_name(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_list_pipes(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_next_item_type(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_create_pipe(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_create_pipe_2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_create_pipe_1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_reset_buffer(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_purge(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_remove_pipe(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_pack_message_date(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_unpack_message_date(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_pack_message_timestamp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_unpack_message_timestamp(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_pack_message_number(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_unpack_message_number(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_pack_message_bytea(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_unpack_message_bytea(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_pack_message_record(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_unpack_message_record(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_pack_message_integer(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_pipe_pack_message_bigint(PG_FUNCTION_ARGS);

/* from plunit.c */
extern "C" DLL_PUBLIC Datum plunit_assert_true(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plunit_assert_true_message(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plunit_assert_false(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plunit_assert_false_message(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plunit_assert_null(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plunit_assert_null_message(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plunit_assert_not_null(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plunit_assert_not_null_message(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plunit_assert_equals(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plunit_assert_equals_message(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plunit_assert_equals_range(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plunit_assert_equals_range_message(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plunit_assert_not_equals(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plunit_assert_not_equals_message(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plunit_assert_not_equals_range(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plunit_assert_not_equals_range_message(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plunit_fail(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plunit_fail_message(PG_FUNCTION_ARGS);

/* from plvdate.c */
extern "C" DLL_PUBLIC Datum plvdate_add_bizdays(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvdate_nearest_bizday(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvdate_next_bizday(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvdate_bizdays_between(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvdate_prev_bizday(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvdate_isbizday(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvdate_set_nonbizday_dow(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvdate_unset_nonbizday_dow(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvdate_set_nonbizday_day(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvdate_unset_nonbizday_day(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvdate_use_easter(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvdate_using_easter(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvdate_use_great_friday(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvdate_using_great_friday(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvdate_include_start(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvdate_including_start(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvdate_default_holidays(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvdate_version(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvdate_days_inmonth(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvdate_isleapyear(PG_FUNCTION_ARGS);

/* from plvlec.c */

/* from plvstr.c */
extern "C" DLL_PUBLIC Datum plvstr_rvrs(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvstr_normalize(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvstr_is_prefix_text(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvstr_is_prefix_int(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvstr_is_prefix_int64(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvstr_lpart(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvstr_rpart(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvstr_lstrip(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvstr_rstrip(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvstr_left(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvstr_right(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvstr_substr2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvstr_substr3(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvstr_instr2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvstr_instr3(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvstr_instr4(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvstr_betwn_i(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvstr_betwn_c(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvstr_swap(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvchr_nth(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvchr_first(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvchr_last(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvchr_is_kind_i(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvchr_is_kind_a(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvchr_char_name(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_substr2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_substr3(PG_FUNCTION_ARGS);
 
/* from plvsubst.c */
extern "C" DLL_PUBLIC Datum plvsubst_string_array(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvsubst_string_string(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvsubst_setsubst(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvsubst_setsubst_default(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum plvsubst_subst(PG_FUNCTION_ARGS);

/* from putline.c */
extern "C" DLL_PUBLIC Datum dbms_output_enable(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_output_enable_default(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_output_disable(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_output_serveroutput(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_output_put(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_output_put_line(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_output_new_line(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_output_get_line(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_output_get_lines(PG_FUNCTION_ARGS);

/* from random.c */
extern "C" DLL_PUBLIC Datum dbms_random_initialize(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_random_normal(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_random_random(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_random_seed_int(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_random_seed_varchar(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_random_string(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_random_terminate(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_random_value(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_random_value_range(PG_FUNCTION_ARGS);

/* from utility.c */
extern "C" DLL_PUBLIC Datum dbms_utility_format_call_stack0(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_utility_format_call_stack1(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum dbms_utility_get_time(PG_FUNCTION_ARGS);

/* from oraguc.c */

/* from charpad.c */
extern "C" DLL_PUBLIC Datum orafce_lpad(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_rpad(PG_FUNCTION_ARGS);

/* from charlen.c */
extern "C" DLL_PUBLIC Datum orafce_bpcharlen(PG_FUNCTION_ARGS);

/* from varchar2.c */
extern "C" DLL_PUBLIC Datum varchar2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_concat2(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_varchar_transform(PG_FUNCTION_ARGS);

/* from orafce_replace_empty_strings.c */
extern "C" DLL_PUBLIC Datum orafce_replace_empty_strings(PG_FUNCTION_ARGS);
extern "C" DLL_PUBLIC Datum orafce_replace_null_strings(PG_FUNCTION_ARGS);
