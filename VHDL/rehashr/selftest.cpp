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

// A very small self-test.

#include "selftest.h"

#include "md2.h"
#include "md5.h"

static const UWORD8 MD2HASH_NUMBERS[16] = {
	0xd5, 0x97, 0x6f, 0x79, 0xd8, 0x3d, 0x3a, 0x0d,
	0xc9, 0x80, 0x6c, 0x3c, 0x66, 0xf3, 0xef, 0xd8 };
static const UWORD8 MD5HASH_NOTHING[16] = {
	0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04,
	0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e };
static const UWORD8 MD5HASH_NUMBERS[16] = {
	0x57, 0xed, 0xf4, 0xa2, 0x2b, 0xe3, 0xc9, 0x55,
	0xac, 0x49, 0xda, 0x2e, 0x21, 0x07, 0xb6, 0x7a };
static const UWORD8 MD5HMAC_PAD[16] = {
	0x56, 0xbe, 0x34, 0x52, 0x1d, 0x14, 0x4c, 0x88,
	0xdb, 0xb8, 0xc7, 0x33, 0xf0, 0xe8, 0xb3, 0xf6 };

bool rhSelfTest()
{
	UWORD8 pBuf[128];
	UINTPREF uErrors = 0;
	CHashAlgorithm *pAlgo = NULL;
	CHMAC *pHMAC = NULL;
	RH_DATA_INFO rhData;
	UINTPREF u;

	printf("ReHash self-test started."); rhNewLine();

	rhData.pszFileName = "Test";
	rhData.uDataLen = 0;

	// Test if the types were compiled correctly
	printf("Testing types..."); rhNewLine();
	u = _rhTestTypes();
	if(u != 0)
	{
		if(u != 1)
		{
			printf("%u-bit type not defined correctly.", u); rhNewLine();
		}
		else
		{
			printf("Preferred type not defined correctly."); rhNewLine();
		}

		uErrors++;
	}
	else
	{
		printf("Types test passed."); rhNewLine();
	}

	printf("Testing MD2 hash..."); rhNewLine();
	pAlgo = new CMD2Hash;
	if(pAlgo == NULL)
	{
		printf("Error: Cannot select MD2 algorihtm."); rhNewLine();
		uErrors++;
	}
	else
	{
		pAlgo->Init(&rhData);
		pAlgo->Update((UWORD8 *)"12345678901234567890123456789012345678901234567890123456789012345678901234567890", 80);
		pAlgo->Final();
		pAlgo->GetHash(pBuf);
		if(memcmp(pBuf, MD2HASH_NUMBERS, 16) != 0)
		{
			printf("Error: MD2 hash not correct for numbers vector."); rhNewLine();
			uErrors++;
		}

		delete pAlgo;
		pAlgo = NULL;
	}
	printf("MD2 test finished."); rhNewLine();

	printf("Testing MD5 hash..."); rhNewLine();
	pAlgo = new CMD5Hash;
	if(pAlgo == NULL)
	{
		printf("Error: Cannot select MD5 algorithm."); rhNewLine();
		uErrors++;
	}
	else // MD5 loaded successfully
	{
		pAlgo->Init(&rhData);
		pAlgo->Final();
		pAlgo->GetHash(pBuf);
		if(memcmp(pBuf, MD5HASH_NOTHING, 16) != 0)
		{
			printf("Error: MD5 hash not correct for vector <nothing>."); rhNewLine();
			uErrors++;
		}

		pAlgo->Init(&rhData);
		pAlgo->Update((UWORD8 *)"12345678901234567890123456789012345678901234567890123456789012345678901234567890", 80);
		pAlgo->Final();
		pAlgo->GetHash(pBuf);
		if(memcmp(pBuf, MD5HASH_NUMBERS, 16) != 0)
		{
			printf("Error: MD5 hash not correct for the numbers vector."); rhNewLine();
			uErrors++;
		}

		pHMAC = new CHMAC;
		pHMAC->SetHashAlgorithm(pAlgo);
		memset(pBuf, 0xAA, 16);
		pHMAC->SetKey(pBuf, 16);
		pHMAC->Init(&rhData);
		memset(pBuf, 0xDD, 50);
		pHMAC->Update(pBuf, 50);
		pHMAC->Final();
		pHMAC->GetHash(pBuf);

		if(memcmp(pBuf, MD5HMAC_PAD, 16) != 0)
		{
			printf("Error: MD5-HMAC not correct for 0xAA-0xDD vector."); rhNewLine();
			uErrors++;
		}

		delete pHMAC; pHMAC = NULL;

		delete pAlgo; pAlgo = NULL;
	}
	printf("MD5 test finished."); rhNewLine();

	rhNewLine();
	printf("----------------------------------------------------------------");
	rhNewLine();
	printf("There were %u errors.", uErrors);
	rhNewLine();

	if(uErrors == 0) return true;
	return false;
}

// For ReHash it's essential that the types UWORD8, UWORD16, UWORD32 and
// UWORD64 are defined correctly
// This function should not output anything to the console
UINTPREF _rhTestTypes()
{
	UWORD8 u8; UWORD16 u16; UWORD32 u32; UWORD64 u64; UINTPREF upref;

	u8 = 0xFF; // Simple arithmetic test
	if((u8 - 1) != 0xFE) return 8;
	u8++; // Overflow test
	if(u8 != 0) return 8;
	u8 = 0x80; u8 <<= 1; // Shift test
	if(u8 != 0) return 8;
	u8 >>= 1; // Unshift bit loss test
	if(u8 != 0) return 8;
	u8 = 0; u8--; // Negative overflow test
	if(u8 != 0xFF) return 8;

	u16 = 0xFFFF; // Simple arithmetic test
	if((u16 - 1) != 0xFFFE) return 16;
	u16++; // Overflow test
	if(u16 != 0) return 16;
	u16 = 0x8000; u16 <<= 1; // Shift test
	if(u16 != 0) return 16;
	u16 >>= 1; // Unshift bit loss test
	if(u16 != 0) return 16;
	u16 = 0; u16--; // Negative overflow test
	if(u16 != 0xFFFF) return 16;

	u32 = CONST32(0xFFFFFFFF); // Simple arithmetic test
	if((u32 - 1) != CONST32(0xFFFFFFFE)) return 32;
	u32++; // Overflow test
	if(u32 != 0) return 32;
	u32 = CONST32(0x80000000); u32 <<= 1; // Shift test
	if(u32 != 0) return 32;
	u32 >>= 1; // Unshift bit loss test
	if(u32 != 0) return 32;
	u32 = 0; u32--; // Negative overflow test
	if(u32 != CONST32(0xFFFFFFFF)) return 32;

	u64 = CONST64(0xFFFFFFFFFFFFFFFF); // Simple arithmetic test
	if((u64 - 1) != CONST64(0xFFFFFFFFFFFFFFFE)) return 64;
	u64++; // Overflow test
	if(u64 != 0) return 64;
	u64 = CONST64(0x8000000000000000); u64 <<= 1; // Shift test
	if(u64 != 0) return 64;
	u64 >>= 1; // Unshift bit loss test
	if(u64 != 0) return 64;
	u64 = 0; u64--; // Negative overflow test
	if(u64 != CONST64(0xFFFFFFFFFFFFFFFF)) return 64;

	// Some tests for the preferred variable type
	upref = 0xFF; upref++; // Must not overflow here
	if(upref == 0) return 1;
	upref = 0xFFFF; upref++; // Must not overflow here
	if(upref == 0) return 1;
	upref = 0; upref--; // Test negative overflow
	if(upref <= 0xFFFF) return 1;
	upref = 1 << 31; // Test shifting
	if(upref == 0) return 1;

	return 0;
}
