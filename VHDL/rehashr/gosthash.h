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

#ifndef ___GOSTHASH_H___
#define ___GOSTHASH_H___

#include "hashalgo.h"

class CGOSTHash : public CHashAlgorithm
{
public:
	CGOSTHash();
	~CGOSTHash();

	const char *GetName() { return "GOST-Hash"; }
	const char *GetShortName() { return "gost"; }
	UINTPREF GetLength() { return 32; }
	UINTPREF GetInternalLength() { return 32; }

	void Init(RH_DATA_INFO *pInfo);
	void Update(const UWORD8 *pBuf, UINTPREF uLen);
	void Final();

	void GetHash(UWORD8 *pHash) { memcpy(pHash, m_digest, 32); }

private:
	void _Compress(UWORD32 *h, UWORD32 *m);
	void _Bytes(const UWORD8 *pBuf, UWORD32 uBits);

	UWORD32 m_sum[8];
	UWORD32 m_hash[8];
	UWORD32 m_len[8];
	UWORD8 m_partial[32];
	UWORD32 m_partial_bytes;

	UWORD8 m_digest[32];
};

void _gostMakeTables();

#endif // ___GOSTHASH_H___
