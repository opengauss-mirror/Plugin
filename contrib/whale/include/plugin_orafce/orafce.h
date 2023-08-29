#ifndef __ORAFCE__
#define __ORAFCE__

#include "postgres.h"
#include "catalog/catversion.h"
#include "nodes/pg_list.h"
#include <sys/time.h>
#include "plugin_utils/datetime.h"
#include "utils/datum.h"
#include "plugin_utils/date.h"
#define MAX_MULTIBYTE_CHAR_LEN  4
#include "storage/lock/lwlock.h"

#include "commands/extension.h"
#include "plugin_orafce/pipe.h"

#ifndef PLVLEX_D
#include "plugin_orafce/plvlex.h"
#endif

typedef LWLock *LWLockId;

#define TextPCopy(t) \
	DatumGetTextP(datumCopy(PointerGetDatum(t), false, -1))

#define PG_GETARG_IF_EXISTS(n, type, defval) \
	((PG_NARGS() > (n) && !PG_ARGISNULL(n)) ? PG_GETARG_##type(n) : (defval))

/* alignment of this struct must fit for all types */
typedef union vardata
{
	char	c;
	short	s;
	int		i;
	long	l;
	float	f;
	double	d;
	void   *p;
} vardata;

extern int ora_instr(text *txt, text *pattern, int start, int nth);
extern int ora_mb_strlen(text *str, char **sizes, int **positions);
extern int ora_mb_strlen1(text *str);


/*
 * Version compatibility
 */

extern Oid	equality_oper_funcid(Oid argtype);

/*
 * Date utils
 */
#define STRING_PTR_FIELD_TYPE const char *const

extern STRING_PTR_FIELD_TYPE ora_days[];

extern int ora_seq_search(const char *name, STRING_PTR_FIELD_TYPE array[], size_t max);

#ifdef _MSC_VER

#define int2size(v)			(size_t)(v)
#define size2int(v)			(int)(v)

#else

#define int2size(v)			v
#define size2int(v)			v

#endif

#endif

typedef enum {
    IT_NO_MORE_ITEMS = 0,
    IT_NUMBER = 9,
    IT_VARCHAR = 11,
    IT_DATE = 12,
    IT_TIMESTAMPTZ = 13,
    IT_BYTEA = 23,
    IT_RECORD = 24
} message_data_type;

typedef struct {
    int32 size;
    message_data_type type;
    Oid tupType;
} message_data_item;

typedef struct {
    int32 size;
    int32 items_count;
    message_data_item *next;
} message_buffer;

typedef struct WeekDays
{
    int	encoding;
    const char *names[7];
} WeekDays;

typedef struct OraFileSlot
{
    FILE   *file;
    int	   max_linesize;
    int	   encoding;
    int32  id;
} OraFileSlot;

typedef struct {
    char day;
    char month;
} holiday_desc;

typedef struct {
	size_t size;
	void* first_byte_ptr;
	bool dispossible;
/*	int16 context; */
} list_item;

typedef struct {
	bool is_valid;
	bool registered;
	char *pipe_name;
	char *creator;
	Oid  uid;
	struct _queue_item *items;
	int16 count;
	int16 limit;
	int size;
} orafce_pipe;

#define MAX_SLOTS		50			/* Orafce 10g supports 50 files */
#define MAX_holidays   30
#define MAX_EXCEPTIONS 5

