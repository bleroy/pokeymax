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

// This free implementation is based on Wei Dai's modified md2.c from Andrew
// M. Kuchling.

#ifndef ___MD2_H___
#define ___MD2_H___

#include "hashalgo.h"

class CMD2Hash : public CHashAlgorithm
{
public:
	CMD2Hash();
	~CMD2Hash();

	const char *GetName() { return "MD2"; }
	const char *GetShortName() { return "md2"; }
	UINTPREF GetLength() { return 16; }
	UINTPREF GetInternalLength() { return 48; }

	void Init(RH_DATA_INFO *pInfo);
	void Update(const UWORD8 *pBuf, UINTPREF uLen);
	void Final();
	void GetHash(UWORD8 *pHash) { memcpy(pHash, m_final, 16); }

private:
	UWORD8 m_X[48], m_C[16], m_buf[16];
	unsigned int m_count;

	UWORD8 m_final[16];
};

#endif
