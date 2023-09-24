//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2008 Greenplum, Inc.
//
//	@filename:
//		utils.cpp
//
//	@doc:
//		Various utilities which are not necessarily spqos specific
//---------------------------------------------------------------------------
#include "spqos/utils.h"

#include "spqos/types.h"

// using 16 addresses a line fits exactly into 80 characters
#define SPQOS_MEM_BPL 16


// number of stack frames to search for addresses
#define SPQOS_SEARCH_STACK_FRAMES 16


using namespace spqos;

//---------------------------------------------------------------------------
//	SPQOS versions of standard streams
//	Do not reference std::(w)cerr/(w)cout directly;
//---------------------------------------------------------------------------
COstreamBasic spqos::oswcerr(&std::wcerr);
COstreamBasic spqos::oswcout(&std::wcout);


//---------------------------------------------------------------------------
//	@function:
//		spqos::Print
//
//	@doc:
//		Print wide-character string
//
//---------------------------------------------------------------------------
void
spqos::Print(WCHAR *wsz)
{
	std::wcout << wsz;
}


//---------------------------------------------------------------------------
//	@function:
//		spqos::HexDump
//
//	@doc:
//		Generic memory dumper; produces regular hex dump
//
//---------------------------------------------------------------------------
IOstream &
spqos::HexDump(IOstream &os, const void *pv, ULLONG size)
{
	for (ULONG i = 0; i < 1 + (size / SPQOS_MEM_BPL); i++)
	{
		// starting address of line
		BYTE *buf = ((BYTE *) pv) + (SPQOS_MEM_BPL * i);
		os << (void *) buf << "  ";
		os << COstream::EsmHex;

		// individual bytes
		for (ULONG j = 0; j < SPQOS_MEM_BPL; j++)
		{
			if (buf[j] < 16)
			{
				os << "0";
			}

			os << (ULONG) buf[j] << " ";

			// separator in middle of line
			if (j + 1 == SPQOS_MEM_BPL / 2)
			{
				os << "- ";
			}
		}

		// blank between hex and text dump
		os << " ";

		// text representation
		for (ULONG j = 0; j < SPQOS_MEM_BPL; j++)
		{
			// print only 'visible' characters
			if (buf[j] >= 0x20 && buf[j] <= 0x7f)
			{
				// cast to CHAR to avoid stream from (mis-)interpreting BYTE
				os << (CHAR) buf[j];
			}
			else
			{
				os << ".";
			}
		}
		os << COstream::EsmDec << std::endl;
	}
	return os;
}


//---------------------------------------------------------------------------
//	@function:
//		spqos::HashByteArray
//
//	@doc:
//		Generic hash function for an array of BYTEs
//		Taken from D. E. Knuth;
//
//---------------------------------------------------------------------------
ULONG
spqos::HashByteArray(const BYTE *byte_array, ULONG size)
{
	ULONG hash = size;

	for (ULONG i = 0; i < size; ++i)
	{
		BYTE b = byte_array[i];
		hash = ((hash << 5) ^ (hash >> 27)) ^ b;
	}

	return hash;
}


//---------------------------------------------------------------------------
//	@function:
//		spqos::CombineHashes
//
//	@doc:
//		Combine ULONG-based hash values
//
//---------------------------------------------------------------------------
ULONG
spqos::CombineHashes(ULONG hash1, ULONG hash2)
{
	ULONG hashes[2];
	hashes[0] = hash1;
	hashes[1] = hash2;

	return HashByteArray((BYTE *) hashes,
						 SPQOS_ARRAY_SIZE(hashes) * sizeof(hashes[0]));
}


//---------------------------------------------------------------------------
//	@function:
//		spqos::Add
//
//	@doc:
//		Add two unsigned long long values, throw an exception if overflow occurs,
//
//---------------------------------------------------------------------------
ULLONG
spqos::Add(ULLONG first, ULLONG second)
{
	if (first > spqos::ullong_max - second)
	{
		// if addition result overflows, we have (a + b > spqos::ullong_max),
		// then we need to check for  (a > spqos::ullong_max - b)
		SPQOS_RAISE(CException::ExmaSystem, CException::ExmiOverflow);
	}

	ULLONG res = first + second;

	return res;
}


//---------------------------------------------------------------------------
//	@function:
//		spqos::Multiply
//
//	@doc:
//		Multiply two unsigned long long values, throw an exception if overflow occurs,
//
//---------------------------------------------------------------------------
ULLONG
spqos::Multiply(ULLONG first, ULLONG second)
{
	if (0 < second && first > spqos::ullong_max / second)
	{
		// if multiplication result overflows, we have (a * b > spqos::ullong_max),
		// then we need to check for  (a > spqos::ullong_max / b)
		SPQOS_RAISE(CException::ExmaSystem, CException::ExmiOverflow);
	}
	ULLONG res = first * second;

	return res;
}

// EOF
