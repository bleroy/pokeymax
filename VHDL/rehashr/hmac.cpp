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

#include "hmac.h"

static const char g_pNothing[4] = { 0, 0, 0, 0 };

CHMAC::CHMAC()
{
	m_pAlgo = NULL;
}

CHMAC::~CHMAC()
{
	m_pAlgo = NULL;
}

void CHMAC::SetHashAlgorithm(CHashAlgorithm *pHashAlgorithm)
{
	RH_ASSERT(pHashAlgorithm != NULL);
	m_pAlgo = pHashAlgorithm;

	m_lenBlock = m_pAlgo->GetInternalLength();
	RH_ASSERT(m_lenBlock != 0);

	RH_ASSERT(m_pAlgo->GetInternalLength() >= m_pAlgo->GetLength());

	if(m_lenBlock > HMAC_MAX_BLOCK)
	{
		RH_TRACE0("Increase HMAC_MAX_INTERNAL!");
		exit(RH_BUFFER_OVERFLOW);
	}
}

void CHMAC::SetKey(UWORD8 *pKey, UINTPREF uKeySize)
{
	RH_DATA_INFO rhData;

	RH_ASSERT(pKey != NULL);

	rhData.pszFileName = g_pNothing;
	rhData.uDataLen = uKeySize;

	if(uKeySize > m_lenBlock)
	{
		m_pAlgo->Init(&rhData);
		m_pAlgo->Update(pKey, uKeySize);
		m_pAlgo->Final();

		m_pAlgo->GetHash(m_pKey);
		m_uKeySize = m_pAlgo->GetLength();
	}
	else
	{
		memcpy(m_pKey, pKey, uKeySize);
		m_uKeySize = uKeySize;
	}
}

void CHMAC::Init(RH_DATA_INFO *pInfo)
{
	UWORD8 pTmp[HMAC_MAX_BLOCK];
	UINTPREF i;

	RH_ASSERT(m_pAlgo != NULL);

	memset(pTmp, HMAC_IPAD, m_lenBlock); // Construct ipad of length B
	for(i = 0; i < m_uKeySize; i++)
		pTmp[i] ^= m_pKey[i]; // XOR in the key

	m_pAlgo->Init(pInfo);

	// Hash: k XOR ipad
	m_pAlgo->Update(pTmp, m_lenBlock);
}

void CHMAC::Update(const UWORD8 *pBuf, UINTPREF uLen)
{
	RH_ASSERT(m_pAlgo != NULL);

	m_pAlgo->Update(pBuf, uLen);
}

void CHMAC::Final()
{
	UWORD8 pInner[HMAC_MAX_BLOCK];
	UWORD8 pTmp[HMAC_MAX_BLOCK];
	UINTPREF i;
	RH_DATA_INFO rhData;

	RH_ASSERT(m_pAlgo != NULL);

	m_pAlgo->Final();
	m_pAlgo->GetHash(pInner);

	memset(pTmp, HMAC_OPAD, m_lenBlock);
	for(i = 0; i < m_uKeySize; i++)
		pTmp[i] ^= m_pKey[i];

	rhData.pszFileName = g_pNothing;
	rhData.uDataLen = m_uKeySize + m_pAlgo->GetLength();

	m_pAlgo->Init(&rhData);
	m_pAlgo->Update(pTmp, m_lenBlock); // k XOR opad
	m_pAlgo->Update(pInner, m_pAlgo->GetLength()); // Inner hash
	m_pAlgo->Final();
	m_pAlgo->GetHash(m_final);
}

void FillHMACList(CHMAC **pList, CHashAlgorithm **pAlgorithms)
{
	UINTPREF i = 0;

	RH_ASSERT(pList != NULL);
	RH_ASSERT(pAlgorithms != NULL);

	while(1)
	{
		if(pAlgorithms[i] == NULL) break;
		if(pList[i] != NULL) break;

		pList[i] = new CHMAC;
		pList[i]->SetHashAlgorithm(pAlgorithms[i]);

		i++;
		pList[i] = NULL;
	}
}
