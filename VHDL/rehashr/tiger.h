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

// This implementation is based on Tom St Denis free implementation.

#ifndef ___TIGER_H___
#define ___TIGER_H___

#include "hashalgo.h"

class CTigerHash : public CHashAlgorithm
{
public:
	CTigerHash();
	~CTigerHash();

	const char *GetName() { return "Tiger"; }
	const char *GetShortName() { return "tiger"; }
	UINTPREF GetLength() { return 24; }
	UINTPREF GetInternalLength() { return 64; }

	void Init(RH_DATA_INFO *pInfo);
	void Update(const UWORD8 *pBuf, UINTPREF uLen);
	void Final();
	void GetHash(UWORD8 *pHash) { memcpy(pHash, m_final, 24); }

private:
	void _Compress();
	void _Round(UWORD64 *a, UWORD64 *b, UWORD64 *c, UWORD64 x, UWORD64 mul);
	void _Pass(UWORD64 *a, UWORD64 *b, UWORD64 *c, UWORD64 *x, UWORD64 mul);
	void _KeySchedule(UWORD64 *x);

	UWORD64 m_state[3], m_length;
	UWORD32 m_curlen;
	UWORD8 m_buf[64];

	UWORD8 m_final[24];
};

#endif
