/*
 Copyright (c) 2003, Dominik Reichl <dominik.reichl@t-online.de>
 All rights reserved.

 LICENSE TERMS

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of ReichlSoft nor the names of its contributors may be used
   to endorse or promote products derived from this software without specific
   prior written permission.

 DISCLAIMER

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "fnvhash.h"

// This is the initialization constant for FNV.32-1 standard.
// If you need FNV.32-0, define it as 0x00000000.
#define FNV32_1_INIT  CONST32(0x811C9DC5)
#define FNV32_1_PRIME CONST32(0x01000193)

#define FNV64_1_INIT  CONST64(0x84222325CBF29CE4)
#define FNV64_1_PRIME CONST64(0x100000001B3)

/////////////////////////////////////////////////////////////////////////////

CFNV32Hash::CFNV32Hash()
{
}

CFNV32Hash::~CFNV32Hash()
{
}

void CFNV32Hash::Init(RH_DATA_INFO *pInfo)
{
	m_fnv32 = FNV32_1_INIT;
}

void CFNV32Hash::Update(const UWORD8 *pBuf, UINTPREF uLen)
{
	UINTPREF i;

	for(i = 0; i < uLen; i++)
	{
		m_fnv32 *= FNV32_1_PRIME;
		m_fnv32 ^= *pBuf++;
	}
}

void CFNV32Hash::Final()
{
}

/////////////////////////////////////////////////////////////////////////////

CFNV64Hash::CFNV64Hash()
{
}

CFNV64Hash::~CFNV64Hash()
{
}

void CFNV64Hash::Init(RH_DATA_INFO *pInfo)
{
	m_fnv64 = FNV64_1_INIT;
}

void CFNV64Hash::Update(const UWORD8 *pBuf, UINTPREF uLen)
{
	UINTPREF i;

	for(i = 0; i < uLen; i++)
	{
		m_fnv64 *= FNV64_1_PRIME;
		m_fnv64 ^= *pBuf++;
	}
}

void CFNV64Hash::Final()
{
}
