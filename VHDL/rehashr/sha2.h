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

// This free implementation is based on Dr Brian Gladmans free implementation.
// The source has been heavily modified to integrate to ReHash.

// Here's the original header:

/*
 ---------------------------------------------------------------------------
 Copyright (c) 2002, Dr Brian Gladman <brg@gladman.me.uk>, Worcester, UK.
 All rights reserved.

 LICENSE TERMS

 The free distribution and use of this software in both source and binary
 form is allowed (with or without changes) provided that:

   1. distributions of this source code include the above copyright
      notice, this list of conditions and the following disclaimer;

   2. distributions in binary form include the above copyright
      notice, this list of conditions and the following disclaimer
      in the documentation and/or other associated materials;

   3. the copyright holder's name is not used to endorse products
      built using this software without specific written permission.

 ALTERNATIVELY, provided that this notice is retained in full, this product
 may be distributed under the terms of the GNU General Public License (GPL),
 in which case the provisions of the GPL apply INSTEAD OF those given above.

 DISCLAIMER

 This software is provided 'as is' with no explicit or implied warranties
 in respect of its properties, including, but not limited to, correctness
 and/or fitness for purpose.
 ---------------------------------------------------------------------------
 Issue Date: 26/08/2003

 This is a byte oriented version of SHA2 that operates on arrays of bytes
 stored in memory. This code implements sha256, sha384 and sha512 but the
 latter two functions rely on efficient 64-bit integer operations that
 may not be very efficient on 32-bit machines

 My thanks to Erik Andersen <andersen@codepoet.org> for testing this code
 on big-endian systems and for his assistance with corrections
*/

#ifndef ___SHA2_H___
#define ___SHA2_H___

#include "hashalgo.h"

#define SHA256_DIGEST_SIZE  32
#define SHA384_DIGEST_SIZE  48
#define SHA512_DIGEST_SIZE  64

#define SHA256_BLOCK_SIZE   64
#define SHA384_BLOCK_SIZE  128
#define SHA512_BLOCK_SIZE  128

class CSHA256Hash : public CHashAlgorithm
{
public:
	CSHA256Hash();
	~CSHA256Hash();

	const char *GetName() { return "SHA256"; }
	const char *GetShortName() { return "sha256"; }
	UINTPREF GetLength() { return 32; }
	UINTPREF GetInternalLength() { return 64; }

	void Init(RH_DATA_INFO *pInfo);
	void Update(const UWORD8 *pBuf, UINTPREF uLen);
	void Final();
	void GetHash(UWORD8 *pHash) { memcpy(pHash, m_final, 32); }

private:
	void _Compile();

	UWORD32 m_count[2];
	UWORD32 m_hash[8];
	UWORD32 m_wbuf[16];

	UWORD8 m_final[32];
};

class CSHA384Hash : public CHashAlgorithm
{
public:
	CSHA384Hash();
	~CSHA384Hash();

	const char *GetName() { return "SHA384"; }
	const char *GetShortName() { return "sha384"; }
	UINTPREF GetLength() { return 48; }
	UINTPREF GetInternalLength() { return 128; }

	void Init(RH_DATA_INFO *pInfo);
	void Update(const UWORD8 *pBuf, UINTPREF uLen);
	void Final();
	void GetHash(UWORD8 *pHash) { memcpy(pHash, m_final, 48); }

private:
	void _Compile();

	UWORD64 m_count[2];
	UWORD64 m_hash[8];
	UWORD64 m_wbuf[16];

	UWORD8 m_final[48];
};

class CSHA512Hash : public CHashAlgorithm
{
public:
	CSHA512Hash();
	~CSHA512Hash();

	const char *GetName() { return "SHA512"; }
	const char *GetShortName() { return "sha512"; }
	UINTPREF GetLength() { return 64; }
	UINTPREF GetInternalLength() { return 128; }

	void Init(RH_DATA_INFO *pInfo);
	void Update(const UWORD8 *pBuf, UINTPREF uLen);
	void Final();
	void GetHash(UWORD8 *pHash) { memcpy(pHash, m_final, 64); }

private:
	void _Compile();

	UWORD64 m_count[2];
	UWORD64 m_hash[8];
	UWORD64 m_wbuf[16];

	UWORD8 m_final[64];
};

#endif // ___SHA2_H___
