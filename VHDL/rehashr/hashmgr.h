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

#ifndef ___HASH_MANAGER_H___
#define ___HASH_MANAGER_H___

#include "rhcommon.h"
#include "hashalgo.h"
#include "hashoutput.h"
#include "hmac.h"

// HM_BUFFER_SIZE must be at least 64 and at least RH_MAX_PATH
#define HM_BUFFER_SIZE 16384

#define HM_NULL       0
#define HM_RECURSIVE  1
#define HM_SHORTNAMES 2
#define HM_LAST       3

class CHashManager
{
public:
	CHashManager();
	virtual ~CHashManager();

	void SetOutputStyle(const char *pszOutputSchemeName);
	void SetOption(INTPREF nOption, bool bFlag);
	void SetHMAC(bool bHMAC, UWORD8 *pKey, UINTPREF uKeySize);

	void PrepareHashing();
	void EndHashing();

	bool HashPath(const char *pszPath, const char *pszMask);
	INTPREF HashFile(const char *pszFile);

	INTPREF FindAlgorithm(const char *pszName, bool bShortName);
	bool SelectAlgorithm(INTPREF nAlgorithm, bool bSelect = true);
	void SelectAllAlgorithms(bool bSelect = true);

	CHashAlgorithm *m_pAlgorithms[RH_MAX_ALGORITHMS];
	bool m_bUseAlgorithm[RH_MAX_ALGORITHMS];

private:
	CHMAC *m_pHMACs[RH_MAX_ALGORITHMS];
	bool m_bHMAC;
	CHashOutput m_output;

	bool m_bRecursiveScanning;
	bool m_bTimings;
	bool m_bShortNames;

	char m_szHMACKey[RH_MAX_STD_BUFFER];
};

#endif // ___HASH_MANAGER_H___
