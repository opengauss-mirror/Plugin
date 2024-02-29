#include "postgres.h"

#include "ivfflat.h"
#include "storage/buf/bufmgr.h"
#include "vector.h"

/*
 * Allocate a vector array
 */
VectorArray
VectorArrayInit(int maxlen, int dimensions)
{
	VectorArray res = (VectorArray)palloc(sizeof(VectorArrayData));

	res->length = 0;
	res->maxlen = maxlen;
	res->dim = dimensions;
	res->items = (Vector *)palloc_extended(maxlen * VECTOR_SIZE(dimensions), MCXT_ALLOC_ZERO | MCXT_ALLOC_HUGE);
	return res;
}

/*
 * Free a vector array
 */
void
VectorArrayFree(VectorArray arr)
{
	pfree(arr->items);
	pfree(arr);
}

/*
 * Print vector array - useful for debugging
 */
void
PrintVectorArray(char *msg, VectorArray arr)
{
	int			i;

	for (i = 0; i < arr->length; i++)
		PrintVector(msg, VectorArrayGet(arr, i));
}

/*
 * Get the number of lists in the index
 */
int
IvfflatGetLists(Relation index)
{
	IvfflatOptions *opts = (IvfflatOptions *) index->rd_options;

	if (opts)
		return opts->lists;

	return IVFFLAT_DEFAULT_LISTS;
}

/*
 * Get proc
 */
FmgrInfo *
IvfflatOptionalProcInfo(Relation rel, uint16 procnum)
{
	if (!OidIsValid(index_getprocid(rel, 1, procnum)))
		return NULL;

	return index_getprocinfo(rel, 1, procnum);
}

/*
 * Divide by the norm
 *
 * Returns false if value should not be indexed
 *
 * The caller needs to free the pointer stored in value
 * if it's different than the original value
 */
bool
IvfflatNormValue(FmgrInfo *procinfo, Oid collation, Datum *value, Vector * result)
{
	Vector	   *v;
	int			i;
	double		norm;

	norm = DatumGetFloat8(FunctionCall1Coll(procinfo, collation, *value));

	if (norm > 0)
	{
		v = DatumGetVector(*value);

		if (result == NULL)
			result = InitVector(v->dim);

		for (i = 0; i < v->dim; i++)
			result->x[i] = v->x[i] / norm;

		*value = PointerGetDatum(result);

		return true;
	}

	return false;
}

/*
 * New buffer
 */
Buffer
IvfflatNewBuffer(Relation index, ForkNumber forkNum)
{
	Buffer		buf = ReadBufferExtended(index, forkNum, P_NEW, RBM_NORMAL, NULL);

	LockBuffer(buf, BUFFER_LOCK_EXCLUSIVE);
	return buf;
}

/*
 * Init page
 */
void
IvfflatInitPage(Buffer buf, Page page)
{
	PageInit(page, BufferGetPageSize(buf), sizeof(IvfflatPageOpaqueData));
	IvfflatPageGetOpaque(page)->nextblkno = InvalidBlockNumber;
	IvfflatPageGetOpaque(page)->page_id = IVFFLAT_PAGE_ID;
}

/*
 * Init and register page
 */
void
IvfflatInitRegisterPage(Relation index, Buffer *buf, Page *page)
{
	*page = BufferGetPage(*buf);
	IvfflatInitPage(*buf, *page);
}

/*
 * Commit buffer
 */
void
IvfflatCommitBuffer(Buffer buf)
{
	MarkBufferDirty(buf);
	UnlockReleaseBuffer(buf);
}

/*
 * Add a new page
 *
 * The order is very important!!
 */
void
IvfflatAppendPage(Relation index, Buffer *buf, Page *page, ForkNumber forkNum)
{
	/* Get new buffer */
	Buffer		newbuf = IvfflatNewBuffer(index, forkNum);
	Page		newpage = BufferGetPage(newbuf);

	/* Update the previous buffer */
	IvfflatPageGetOpaque(*page)->nextblkno = BufferGetBlockNumber(newbuf);

	/* Init new page */
	IvfflatInitPage(newbuf, newpage);

	/* Commit */
	MarkBufferDirty(*buf);
	MarkBufferDirty(newbuf);

	/* Unlock */
	UnlockReleaseBuffer(*buf);

	*page = BufferGetPage(newbuf);
	*buf = newbuf;
}

/*
 * Update the start or insert page of a list
 */
void
IvfflatUpdateList(Relation index, ListInfo listInfo,
				  BlockNumber insertPage, BlockNumber originalInsertPage,
				  BlockNumber startPage, ForkNumber forkNum)
{
	Buffer		buf;
	Page		page;
	IvfflatList list;
	bool		changed = false;

	buf = ReadBufferExtended(index, forkNum, listInfo.blkno, RBM_NORMAL, NULL);
	LockBuffer(buf, BUFFER_LOCK_EXCLUSIVE);
	page = BufferGetPage(buf);
	list = (IvfflatList) PageGetItem(page, PageGetItemId(page, listInfo.offno));

	if (BlockNumberIsValid(insertPage) && insertPage != list->insertPage)
	{
		/* Skip update if insert page is lower than original insert page  */
		/* This is needed to prevent insert from overwriting vacuum */
		if (!BlockNumberIsValid(originalInsertPage) || insertPage >= originalInsertPage)
		{
			list->insertPage = insertPage;
			changed = true;
		}
	}

	if (BlockNumberIsValid(startPage) && startPage != list->startPage)
	{
		list->startPage = startPage;
		changed = true;
	}

	/* Only commit if changed */
	if (changed)
		IvfflatCommitBuffer(buf);
	else
	{
		UnlockReleaseBuffer(buf);
	}
}
