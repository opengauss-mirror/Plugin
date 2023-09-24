//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CDatumGenericSPQDB.cpp
//
//	@doc:
//		Implementation of SPQDB generic datum
//---------------------------------------------------------------------------

#include "naucrates/base/CDatumGenericSPQDB.h"

#include "spqos/base.h"
#include "spqos/common/clibwrapper.h"
#include "spqos/string/CWStringDynamic.h"

#include "spqopt/base/COptCtxt.h"
#include "spqopt/mdcache/CMDAccessor.h"
#include "naucrates/md/CMDIdSPQDB.h"
#include "naucrates/md/IMDType.h"
#include "naucrates/statistics/CScaleFactorUtils.h"

using namespace spqnaucrates;
using namespace spqmd;

// selectivities needed for LIKE predicate statistics evaluation
const CDouble CDatumGenericSPQDB::DefaultFixedCharSelectivity(0.20);
const CDouble CDatumGenericSPQDB::DefaultCharRangeSelectivity(0.25);
const CDouble CDatumGenericSPQDB::DefaultAnyCharSelectivity(0.99);
const CDouble CDatumGenericSPQDB::DefaultCdbRanchorSelectivity(0.95);
const CDouble CDatumGenericSPQDB::DefaultCdbRolloffSelectivity(0.14);

//---------------------------------------------------------------------------
//	@function:
//		CDatumGenericSPQDB::CDatumGenericSPQDB
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CDatumGenericSPQDB::CDatumGenericSPQDB(CMemoryPool *mp, IMDId *mdid,
									 INT type_modifier, const void *src,
									 ULONG size, BOOL is_null,
									 LINT stats_comp_val_int,
									 CDouble stats_comp_val_double)
	: m_mp(mp),
	  m_size(size),
	  m_bytearray_value(NULL),
	  m_is_null(is_null),
	  m_mdid(mdid),
	  m_type_modifier(type_modifier),
	  m_cached_type(NULL),
	  m_stats_comp_val_int(stats_comp_val_int),
	  m_stats_comp_val_double(stats_comp_val_double)
{
	SPQOS_ASSERT(NULL != mp);
	SPQOS_ASSERT(mdid->IsValid());

	if (!IsNull())
	{
		SPQOS_ASSERT(0 < size);

		m_bytearray_value = SPQOS_NEW_ARRAY(m_mp, BYTE, size);
		(void) clib::Memcpy(m_bytearray_value, src, size);
	}
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumGenericSPQDB::~CDatumGenericSPQDB
//
//	@doc:
//		Dtor
//
//---------------------------------------------------------------------------
CDatumGenericSPQDB::~CDatumGenericSPQDB()
{
	SPQOS_DELETE_ARRAY(m_bytearray_value);
	m_mdid->Release();
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumGenericSPQDB::IsNull
//
//	@doc:
//		Accessor of is null
//
//---------------------------------------------------------------------------
BOOL
CDatumGenericSPQDB::IsNull() const
{
	return m_is_null;
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumGenericSPQDB::Size
//
//	@doc:
//		Accessor of size
//
//---------------------------------------------------------------------------
ULONG
CDatumGenericSPQDB::Size() const
{
	return m_size;
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumGenericSPQDB::MDId
//
//	@doc:
//		Accessor of the type information
//
//---------------------------------------------------------------------------
IMDId *
CDatumGenericSPQDB::MDId() const
{
	return m_mdid;
}


INT
CDatumGenericSPQDB::TypeModifier() const
{
	return m_type_modifier;
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumGenericSPQDB::HashValue
//
//	@doc:
//		Hash function
//
//---------------------------------------------------------------------------
ULONG
CDatumGenericSPQDB::HashValue() const
{
	ULONG hash = 0;
	if (IsNull())
	{
		hash = spqos::HashValue<ULONG>(&hash);
	}
	else
	{
		hash = spqos::HashValue<BYTE>(&m_bytearray_value[0]);
		ULONG size = Size();
		for (ULONG i = 1; i < size; i++)
		{
			hash = spqos::CombineHashes(
				hash, spqos::HashValue<BYTE>(&m_bytearray_value[i]));
		}
	}

	return spqos::CombineHashes(m_mdid->HashValue(), hash);
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumGenericSPQDB::GetMDName
//
//	@doc:
//		Return string representation
//
//---------------------------------------------------------------------------
const CWStringConst *
CDatumGenericSPQDB::GetStrRepr(CMemoryPool *mp) const
{
	CWStringDynamic str(mp);

	if (IsNull())
	{
		str.AppendFormat(SPQOS_WSZ_LIT("null"));
		return SPQOS_NEW(mp) CWStringConst(mp, str.GetBuffer());
	}

	// pretty print datums that can be mapped to LINTs or CDoubles
	if (IsDatumMappableToLINT())
	{
		str.AppendFormat(SPQOS_WSZ_LIT("%0.3f"), (double) GetLINTMapping());
		return SPQOS_NEW(mp) CWStringConst(mp, str.GetBuffer());
	}
	else if (IsDatumMappableToDouble())
	{
		str.AppendFormat(SPQOS_WSZ_LIT("%0.3f"), GetDoubleMapping().Get());
		return SPQOS_NEW(mp) CWStringConst(mp, str.GetBuffer());
	}

	// print hex representation of bytes
	ULONG size = Size();
	for (ULONG i = 0; i < size; i++)
	{
		str.AppendFormat(SPQOS_WSZ_LIT("%02X"), m_bytearray_value[i]);
	}

	return SPQOS_NEW(mp) CWStringConst(mp, str.GetBuffer());
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumGenericSPQDB::Matches
//
//	@doc:
//		Matches the values of datums
//
//---------------------------------------------------------------------------
BOOL
CDatumGenericSPQDB::Matches(const IDatum *datum) const
{
	if (!datum->MDId()->Equals(m_mdid) || (datum->Size() != Size()))
	{
		return false;
	}

	const CDatumGenericSPQDB *datum_generic =
		dynamic_cast<const CDatumGenericSPQDB *>(datum);

	if (datum_generic->IsNull() && IsNull())
	{
		return true;
	}

	if (!datum_generic->IsNull() && !IsNull())
	{
		if (0 == clib::Memcmp(datum_generic->m_bytearray_value,
							  m_bytearray_value, Size()))
		{
			return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumGenericSPQDB::MakeCopy
//
//	@doc:
//		Returns a copy of the datum
//
//---------------------------------------------------------------------------
IDatum *
CDatumGenericSPQDB::MakeCopy(CMemoryPool *mp) const
{
	m_mdid->AddRef();

	// CDatumGenericSPQDB makes a copy of the buffer
	return SPQOS_NEW(mp) CDatumGenericSPQDB(
		mp, m_mdid, m_type_modifier, m_bytearray_value, m_size, m_is_null,
		m_stats_comp_val_int, m_stats_comp_val_double);
}


//---------------------------------------------------------------------------
//	@function:
//		CDatumGenericSPQDB::OsPrint
//
//	@doc:
//		Debug print
//
//---------------------------------------------------------------------------
IOstream &
CDatumGenericSPQDB::OsPrint(IOstream &os) const
{
	const CWStringConst *str = GetStrRepr(m_mp);
	os << str->GetBuffer();
	SPQOS_DELETE(str);

	return os;
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumGenericSPQDB::IsDatumMappableToDouble
//
//	@doc:
//		For statistics computation, can this datum be mapped to a CDouble
//
//---------------------------------------------------------------------------
BOOL
CDatumGenericSPQDB::IsDatumMappableToDouble() const
{
	return CMDTypeGenericSPQDB::HasByte2DoubleMapping(this->MDId());
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumGenericSPQDB::IsDatumMappableToLINT
//
//	@doc:
//		For statistics computation, can this datum be mapped to a LINT
//
//---------------------------------------------------------------------------
BOOL
CDatumGenericSPQDB::IsDatumMappableToLINT() const
{
	if (NULL == m_cached_type)
	{
		m_cached_type = COptCtxt::PoctxtFromTLS()->Pmda()->RetrieveType(MDId());
	}
	return CMDTypeGenericSPQDB::HasByte2IntMapping(m_cached_type);
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumGenericSPQDB::MakeCopyOfValue
//
//	@doc:
//		For statistics computation, return the byte array representation of
//		the datum
//---------------------------------------------------------------------------
const BYTE *
CDatumGenericSPQDB::GetByteArrayValue() const
{
	return m_bytearray_value;
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumGenericSPQDB::StatsAreEqual
//
//	@doc:
//		Are datums statistically equal?
//
//---------------------------------------------------------------------------
BOOL
CDatumGenericSPQDB::StatsAreEqual(const IDatum *datum) const
{
	// if mapping exists, use that to compute equality
	if (IsDatumMappableToLINT() || IsDatumMappableToDouble())
	{
		return IDatum::StatsAreEqual(datum);
	}

	// take special care of nulls
	if (IsNull() || datum->IsNull())
	{
		return IsNull() && datum->IsNull();
	}

	// fall back to memcmp
	const CDatumGenericSPQDB *datum_generic_spqdb =
		dynamic_cast<const CDatumGenericSPQDB *>(datum);

	ULONG size = this->Size();
	if (size == datum_generic_spqdb->Size())
	{
		const BYTE *s1 = m_bytearray_value;
		const BYTE *s2 = datum_generic_spqdb->m_bytearray_value;
		return (clib::Memcmp(s1, s2, size) == 0);
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumGenericSPQDB::MakeCopyOfValue
//
//	@doc:
//		Accessor of byte array
//
//---------------------------------------------------------------------------
BYTE *
CDatumGenericSPQDB::MakeCopyOfValue(CMemoryPool *mp, ULONG *dest_length) const
{
	ULONG length = 0;
	BYTE *dest = NULL;

	if (!IsNull())
	{
		length = this->Size();
		;
		SPQOS_ASSERT(length > 0);
		dest = SPQOS_NEW_ARRAY(mp, BYTE, length);
		(void) clib::Memcpy(dest, this->m_bytearray_value, length);
	}

	*dest_length = length;
	return dest;
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumGenericSPQDB::NeedsPadding
//
//	@doc:
//		Does the datum need to be padded before statistical derivation
//
//---------------------------------------------------------------------------
BOOL
CDatumGenericSPQDB::NeedsPadding() const
{
	return MDId()->Equals(&CMDIdSPQDB::m_mdid_bpchar);
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumGenericSPQDB::MakePaddedDatum
//
//	@doc:
//		Return the padded datum
//
//---------------------------------------------------------------------------
IDatum *
CDatumGenericSPQDB::MakePaddedDatum(CMemoryPool *mp, ULONG col_len) const
{
	// in SPQDB the first four bytes of the datum are used for the header
	const ULONG adjusted_col_width = col_len + SPQDB_DATUM_HDRSZ;

	if (this->IsNull() || (spqos::ulong_max == col_len))
	{
		return this->MakeCopy(mp);
	}

	const ULONG datum_len = this->Size();
	if (spqos::ulong_max != adjusted_col_width && datum_len < adjusted_col_width)
	{
		const BYTE *original = this->GetByteArrayValue();
		BYTE *dest = NULL;

		dest = SPQOS_NEW_ARRAY(m_mp, BYTE, adjusted_col_width);
		(void) clib::Memcpy(dest, original, datum_len);

		// datum's length smaller than column's size, therefore pad the input datum
		(void) clib::Memset(dest + datum_len, ' ',
							adjusted_col_width - datum_len);

		// create a new datum
		this->MDId()->AddRef();
		CDatumGenericSPQDB *datum_new = SPQOS_NEW(m_mp) CDatumGenericSPQDB(
			mp, this->MDId(), this->TypeModifier(), dest, adjusted_col_width,
			this->IsNull(), this->GetLINTMapping(), 0 /* dValue */
		);

		// clean up the input byte array as the constructor creates a copy
		SPQOS_DELETE_ARRAY(dest);

		return datum_new;
	}

	return this->MakeCopy(mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumGenericSPQDB::GetLikePredicateScaleFactor
//
//	@doc:
//		Return the scale factor of the like predicate by checking the pattern
//		that is being matched in the LIKE predicate
//---------------------------------------------------------------------------
CDouble
CDatumGenericSPQDB::GetLikePredicateScaleFactor() const
{
	if (this->IsNull())
	{
		return CDouble(1.0);
	}

	const ULONG datum_len = this->Size();
	const BYTE *dest = this->GetByteArrayValue();

	ULONG pos = 0;

	// skip any leading %; it's already factored into initial selectivity (DDefaultScaleFactorLike).
	// In SPQDB the first four bytes of the datum are used for the header
	for (pos = SPQDB_DATUM_HDRSZ; pos < datum_len; pos++)
	{
		if ('%' != dest[pos] && '_' != dest[pos])
		{
			break;
		}
	}

	CDouble selectivity(1.0);
	CDouble fixed_char_selectivity =
		CDatumGenericSPQDB::DefaultFixedCharSelectivity;
	while (pos < datum_len)
	{
		// % and _ are wildcard characters in LIKE
		if ('_' == dest[pos])
		{
			selectivity =
				selectivity * CDatumGenericSPQDB::DefaultAnyCharSelectivity;
		}
		else if ('%' != dest[pos])
		{
			if ('\\' == dest[pos])
			{
				// backslash quotes the next character
				pos++;
				if (pos >= datum_len)
				{
					break;
				}
			}

			selectivity = selectivity * fixed_char_selectivity;
			fixed_char_selectivity =
				fixed_char_selectivity +
				(1.0 - fixed_char_selectivity) *
					CDatumGenericSPQDB::DefaultCdbRolloffSelectivity;
		}

		pos++;
	}

	selectivity = selectivity * GetTrailingWildcardSelectivity(dest, pos);

	return 1 / std::max(selectivity,
						1 / CScaleFactorUtils::DDefaultScaleFactorLike);
}

//---------------------------------------------------------------------------
//	@function:
//		CDatumGenericSPQDB::GetTrailingWildcardSelectivity
//
//	@doc:
//		Return the selectivity of the trailing wildcards
//
//---------------------------------------------------------------------------
CDouble
CDatumGenericSPQDB::GetTrailingWildcardSelectivity(const BYTE *dest,
												  ULONG pos) const
{
	SPQOS_ASSERT(NULL != dest);

	// If no trailing wildcard, reduce selectivity
	BOOL wildcard = (0 < pos) && ('%' != dest[pos - 1]);
	BOOL backslash = (2 <= pos) && ('\\' == dest[pos - 2]);
	if (wildcard || backslash)
	{
		return CDatumGenericSPQDB::DefaultCdbRanchorSelectivity;
	}

	return CDouble(1.0);
}

// EOF
