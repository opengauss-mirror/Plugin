/*
 *
 * Shared memory control - based on alocating chunks aligned on
 * asize array (fibonachi), and dividing free bigger block.
 *
 */

#include "postgres.h"
#include "plugin_orafce/shmmc.h"
#include "stdlib.h"
#include "string.h"
#include "plugin_postgres.h"

#include "stdint.h"

#define LIST_ITEMS 512

int context;

typedef struct {
    int list_c;
    size_t max_size;
    vardata data[1]; /* flexible array member */
} mem_desc;

#define MAX_SIZE 82688

static size_t asize[] = {32,   64,   96,   160,   256,   416,   672,   1088, 1760,
                         2848, 4608, 7456, 12064, 19520, 31584, 51104, 82688};

int cycle = 0;

/* align requested size */

static int ptr_comp(const void *a, const void *b)
{
    ptrdiff_t d;

    list_item *_a = (list_item *)a;
    list_item *_b = (list_item *)b;

    d = (uintptr_t)_a->first_byte_ptr - (uintptr_t)_b->first_byte_ptr;

    return d > 0 ? 1 : (d < 0 ? -1 : 0);
}

char *ora_copystring(char *str)
{
    size_t len;
    char *result;

    len = strlen(str);
    if (NULL != (result = (char *)ora_salloc(len + 1))) {
        errno_t sret = memcpy_s(result, len + 1, str, len + 1);
        securec_check(sret, "", "");
    } else
        ereport(ERROR, (errcode(ERRCODE_OUT_OF_MEMORY), errmsg("out of memory"),
                        errdetail("Failed while allocation block %d bytes in shared memory.", (int)len + 1),
                        errhint("Increase SHMEMMSGSZ and recompile package.")));

    return result;
}

char *ora_scstring(text *str)
{
    int len;
    char *result;

    len = VARSIZE_ANY_EXHDR(str);

    if (NULL != (result = (char *)ora_salloc(len + 1))) {
        errno_t sret = memcpy_s(result, len, VARDATA_ANY(str), len);
        securec_check(sret, "", "");
        result[len] = '\0';
    } else
        ereport(ERROR, (errcode(ERRCODE_OUT_OF_MEMORY), errmsg("out of memory"),
                        errdetail("Failed while allocation block %d bytes in shared memory.", (int)len + 1),
                        errhint("Increase SHMEMMSGSZ and recompile package.")));

    return result;
}

/*
 * Compact the list of slots, by merging adjacent unused slots into larger
 * slots.
 */
static void defragmentation()
{
    int src, target;
    errno_t sret;

    /* Sort the array to pointer order */
    qsort(GetSessionContext()->list, *(GetSessionContext()->list_c), sizeof(list_item), ptr_comp);

    /* Merge adjacent dispossible slots, and move up other slots */
    target = 0;
    for (src = 0; src < *(GetSessionContext()->list_c); src++) {
        if (target > 0 && GetSessionContext()->list[src].dispossible &&
            GetSessionContext()->list[target - 1].dispossible) {
            GetSessionContext()->list[target - 1].size += GetSessionContext()->list[src].size;
        } else {
            if (src != target)
                sret = memcpy_s(&(GetSessionContext()->list[target]), sizeof(list_item),
                                &(GetSessionContext()->list[src]), sizeof(list_item));
            securec_check(sret, "", "");
            target++;
        }
    }
    *(GetSessionContext()->list_c) = target;
}

static size_t align_size(size_t size)
{
    int i;

    /* default, we can allocate max MAX_SIZE memory block */

    for (i = 0; i < 17; i++) {
        if (asize[i] >= size) {
            return asize[i];
        }
    }

    ereport(ERROR, (errcode(ERRCODE_OUT_OF_MEMORY), errmsg("too much large memory block request"),
                    errdetail("Failed while allocation block %lu bytes in shared memory.", (unsigned long)size),
                    errhint("Increase MAX_SIZE constant, fill table a_size and recompile package.")));

    return 0;
}

/*
  initialize shared memory. It works in two modes, create and no create.
  No create is used for mounting shared memory buffer. Top of memory is
  used for list_item array.
*/

void ora_sinit(void *ptr, size_t size, bool create)
{
    if (GetSessionContext()->list == NULL) {
        mem_desc *m = (mem_desc *)ptr;
        GetSessionContext()->list = (list_item *)m->data;
        GetSessionContext()->list_c = &m->list_c;
        GetSessionContext()->max_size = m->max_size = size;

        if (create) {
            GetSessionContext()->list[0].size = size - sizeof(list_item) * LIST_ITEMS - sizeof(mem_desc);
            GetSessionContext()->list[0].first_byte_ptr = ((char *)&m->data) + sizeof(list_item) * LIST_ITEMS;
            GetSessionContext()->list[0].dispossible = true;
            *(GetSessionContext()->list_c) = 1;
        }
    }
}

void *ora_salloc(size_t size)
{
    size_t aligned_size;
    int repeat_c;
    void *ptr = NULL;

    aligned_size = align_size(size);

    for (repeat_c = 0; repeat_c < 2; repeat_c++) {
        size_t max_min = GetSessionContext()->max_size;
        int select = -1;
        int i;

        /* find first good free block */
        for (i = 0; i < *(GetSessionContext()->list_c); i++) {
            if (GetSessionContext()->list[i].dispossible) {
                /* If this block is just the right size, return it */
                if (GetSessionContext()->list[i].size == aligned_size) {
                    GetSessionContext()->list[i].dispossible = false;
                    ptr = GetSessionContext()->list[i].first_byte_ptr;
                    /* list[i].context = context; */

                    return ptr;
                }

                if (GetSessionContext()->list[i].size > aligned_size &&
                    GetSessionContext()->list[i].size < max_min) {
                    max_min = GetSessionContext()->list[i].size;
                    select = i;
                }
            }
        }

        /* If no suitable free slot found, defragment and try again. */
        if (select == -1 || *(GetSessionContext()->list_c) == LIST_ITEMS) {
            defragmentation();
            continue;
        }

        /*
         * A slot larger than required was found. Divide it to avoid wasting
         * space, and return the slot of the right size.
         */
        GetSessionContext()->list[*(GetSessionContext()->list_c)].size =
            GetSessionContext()->list[select].size - aligned_size;
        GetSessionContext()->list[*(GetSessionContext()->list_c)].first_byte_ptr =
            (char *)(GetSessionContext()->list[select].first_byte_ptr) + aligned_size;
        GetSessionContext()->list[*(GetSessionContext()->list_c)].dispossible = true;
        GetSessionContext()->list[select].size = aligned_size;
        GetSessionContext()->list[select].dispossible = false;
        /* list[select].context = context; */
        ptr = GetSessionContext()->list[select].first_byte_ptr;
        *(GetSessionContext()->list_c) += 1;
        break;
    }

    return ptr;
}

void ora_sfree(void *ptr)
{
    int i;
    errno_t rc = EOK;

    /*
        if (cycle++ % 100 == 0)
        {
            size_t suma = 0;
            for (i = 0; i < *list_c; i++)
                if (list[i].dispossible)
                    suma += list[i].size;
            elog(NOTICE, "=============== FREE MEM REPORT === %10d ================", suma);
        }
    */

    for (i = 0; i < *(GetSessionContext()->list_c); i++)
        if (GetSessionContext()->list[i].first_byte_ptr == ptr) {
            GetSessionContext()->list[i].dispossible = true;
            /* list[i].context = -1; */
            rc = memset_s(GetSessionContext()->list[i].first_byte_ptr, GetSessionContext()->list[i].size, '#',
                          GetSessionContext()->list[i].size);
            securec_check(rc, "\0", "\0");
            return;
        }

    ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR), errmsg("corrupted pointer"),
                    errdetail("Failed while reallocating memory block in shared memory."),
                    errhint("Report this bug to autors.")));
}

void *ora_srealloc(void *ptr, size_t size)
{
    void *result;
    size_t aux_s = 0;
    int i;

    for (i = 0; i < *(GetSessionContext()->list_c); i++)
        if (GetSessionContext()->list[i].first_byte_ptr == ptr) {
            if (align_size(size) <= GetSessionContext()->list[i].size)
                return ptr;
            aux_s = GetSessionContext()->list[i].size;
        }

    if (aux_s == 0)
        ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR), errmsg("corrupted pointer"),
                        errdetail("Failed while reallocating memory block in shared memory."),
                        errhint("Report this bug to autors.")));

    if (NULL != (result = (char *)ora_salloc(size))) {
        errno_t sret = memcpy_s(result, aux_s, ptr, aux_s);
        securec_check(sret, "", "");
        ora_sfree(ptr);
    }

    return result;
}

/*
 *  alloc shared memory, raise exception if not
 */

void *salloc(size_t size)
{
    void *result;

    if (NULL == (result = ora_salloc(size)))
        ereport(ERROR, (errcode(ERRCODE_OUT_OF_MEMORY), errmsg("out of memory"),
                        errdetail("Failed while allocation block %lu bytes in shared memory.", (unsigned long)size),
                        errhint("Increase SHMEMMSGSZ and recompile package.")));

    return result;
}

void *srealloc(void *ptr, size_t size)
{
    void *result;

    if (NULL == (result = ora_srealloc(ptr, size)))
        ereport(ERROR, (errcode(ERRCODE_OUT_OF_MEMORY), errmsg("out of memory"),
                        errdetail("Failed while reallocation block %lu bytes in shared memory.", (unsigned long)size),
                        errhint("Increase SHMEMMSGSZ and recompile package.")));

    return result;
}
