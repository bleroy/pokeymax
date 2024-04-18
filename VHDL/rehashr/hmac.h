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

#ifndef ___HMAC_H___
#define ___HMAC_H___

#include "rhcommon.h"
#include "hashalgo.h"

#define HMAC_MAX_BLOCK 256

#define HMAC_IPAD 0x36
#define HMAC_OPAD 0x5C

// Only declare when the abstract hash algorithm class has been declared

class CHMAC
{
public:
	CHMAC();
	~CHMAC();

	void SetHashAlgorithm(CHashAlgorithm *pHashAlgorithm);

	void SetKey(UWORD8 *pKey, UINTPREF uKeySize);
	void Init(RH_DATA_INFO *pInfo);
	void Update(const UWORD8 *pBuf, UINTPREF uLen);
	void Final();
	void GetHash(UWORD8 *pHash)
	{
		memcpy(pHash, m_final, m_pAlgo->GetLength());
	}

private:
	CHashAlgorithm *m_pAlgo;
	UINTPREF m_lenBlock;

	UWORD8 m_pKey[HMAC_MAX_BLOCK];
	UINTPREF m_uKeySize;

	UWORD8 m_final[HMAC_MAX_BLOCK];
};

void FillHMACList(CHMAC **pList, CHashAlgorithm **pAlgorithms);

#endif // ___HMAC_H___
