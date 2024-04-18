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

#ifndef ___ABSTRACT_HASH_ALGORITHM_H___
#define ___ABSTRACT_HASH_ALGORITHM_H___

#include "rhcommon.h"

typedef struct _RH_DATA_INFO
{
	const char *pszFileName;
	UINTPREF uDataLen;
} RH_DATA_INFO;

class CHashAlgorithm // Abstract hash algorithm class
{
public:
	virtual const char *GetName() = 0; // Full name of the algorithm
	virtual const char *GetShortName() = 0; // Short name of the algorithm
	virtual UINTPREF GetLength() = 0; // Number of bytes of the digest
	virtual UINTPREF GetInternalLength() = 0; // Internal buffer size

	virtual void Init(RH_DATA_INFO *pInfo) = 0; // Initialize
	virtual void Update(const UWORD8 *pBuf, UINTPREF uLen) = 0;
	virtual void Final() = 0; // Call this to finish hashing
	virtual void GetHash(UWORD8 *pHash) = 0; // Get the final digest
};

void FillAlgorithmList(CHashAlgorithm **pList);

#endif // ___ABSTRACT_HASH_ALGORITHM_H___
