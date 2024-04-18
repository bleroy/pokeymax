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

#ifndef ___HASH_OUTPUT_H___
#define ___HASH_OUTPUT_H___

#include "rhcommon.h"

#define HASHOUTPUT_NONE 0
#define HASHOUTPUT_TEXT 1
#define HASHOUTPUT_HTML 2
#define HASHOUTPUT_XML  3
#define HASHOUTPUT_CSV  4
#define HASHOUTPUT_LAST 5

#define RH_MAX_CHAR_CODE 12
#define RH_MAX_HASH_STRING 128
#define RH_MAX_OUTPUT_BUFFER 1024

class CHashOutput
{
public:
	CHashOutput();
	virtual ~CHashOutput();

	void SetOutputStyleA(const char *pszSchemeName);
	void SetOutputStyleN(int nStyle);

	void InitOutput(void *pManager);
	void NewDataSource(const char *pFileDesc, bool bHMAC, char *pHMACKey);
	void Output(const char *pHashName, UWORD8 *pHash, UINTPREF uLen);
	void CloseDataSource();
	void CloseOutput();

private:
	void _OutputHashBytes(UWORD8 *pHash, UINTPREF uLen);

	UINTPREF m_nMaxHashName;

	INTPREF m_nOutputStyle;
	INTPREF m_nByteSpaces;
	INTPREF m_nCombineWords;
	char m_szCharCode[RH_MAX_CHAR_CODE];
	bool m_bPadToFullWord;
	bool m_bBase64;

	char *m_pNewLine;

	char m_szHashBuffer[RH_MAX_HASH_STRING];
	char m_szBuffer[RH_MAX_OUTPUT_BUFFER];
	char m_szByteBuffer[16];
};

#endif // ___HASH_OUTPUT_H___
