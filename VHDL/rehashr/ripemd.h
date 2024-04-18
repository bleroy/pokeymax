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

// This free implementation is based on Tom St Denis free implementation, which
// is based on the free implementation of Antoon Bosselaers, ESAT-COSIC.

#ifndef ___RIPEMD_H___
#define ___RIPEMD_H___

#include "hashalgo.h"

class CRMD128Hash : public CHashAlgorithm
{
public:
	CRMD128Hash();
	~CRMD128Hash();

	const char *GetName() { return "RipeMD128"; }
	const char *GetShortName() { return "rmd128"; }
	UINTPREF GetLength() { return 16; }
	UINTPREF GetInternalLength() { return 64; }

	void Init(RH_DATA_INFO *pInfo);
	void Update(const UWORD8 *pBuf, UINTPREF uLen);
	void Final();
	void GetHash(UWORD8 *pHash) { memcpy(pHash, m_final, 16); }

private:
	void _Compress();

    UWORD64 m_length;
    UWORD8 m_buf[64];
    UWORD32 m_curlen, m_state[4];

	UWORD8 m_final[16];
};

class CRMD160Hash : public CHashAlgorithm
{
public:
	CRMD160Hash();
	~CRMD160Hash();

	const char *GetName() { return "RipeMD160"; }
	const char *GetShortName() { return "rmd160"; }
	UINTPREF GetLength() { return 20; }
	UINTPREF GetInternalLength() { return 64; }

	void Init(RH_DATA_INFO *pInfo);
	void Update(const UWORD8 *pBuf, UINTPREF uLen);
	void Final();
	void GetHash(UWORD8 *pHash) { memcpy(pHash, m_final, 20); }

private:
	void _Compress();

    UWORD64 m_length;
    UWORD8 m_buf[64];
    UWORD32 m_curlen, m_state[5];

	UWORD8 m_final[20];
};

#endif // ___RIPEMD_H___
