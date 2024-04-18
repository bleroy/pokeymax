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

#include "clutil.h"

bool isArgument(char *pszString)
{
	RH_ASSERT(pszString != NULL);
	RH_ASSERT(strlen(pszString) != 0);

	// The following characters must not be argument introducers:
	// '/'   -> Unix paths
	// '\\'  -> Windows paths
	// '_'   -> Possible path prefix
	// '.'   -> File without name or hidden (Unix)
	if((*pszString) == '-') return true;

	return false;
}

// Removes all unnecessary characters from an argument string
void fmtArgument(char *pszArg, char *pszDest)
{
	UINTPREF i;
	UINTPREF pos = 0;

	RH_ASSERT(pszArg != NULL);
	RH_ASSERT(pszDest != NULL);
	RH_ASSERT(strlen(pszArg) != 0);

	for(i = 0; i < strlen(pszArg); i++)
	{
		if(pszArg[i] == '-') continue; // Skip special characters
		if(pszArg[i] == '#') continue;
		if(pszArg[i] == ' ') continue;
		if(pszArg[i] == '<') continue;
		if(pszArg[i] == '>') continue;
		if(pszArg[i] == '(') continue;
		if(pszArg[i] == ')') continue;
		if(pszArg[i] == '[') continue;
		if(pszArg[i] == ']') continue;
		if(pszArg[i] == '/') continue;
		if(pszArg[i] == '\\') continue;
		if(pszArg[i] == '\"') continue; // Don't test for ':' !

		pszDest[pos] = pszArg[i];
		pos++;
	}

	pszDest[pos] = 0; // Terminate string
}

void fmtPath(char *pszPath)
{
	UINTPREF i;
	char chFind;
	char chReplace;

	RH_ASSERT(pszPath != NULL);
	RH_ASSERT(strlen(pszPath) != 0);

#if(RH_TARGET_SYSTEM == RH_TARGET_SYSTEM_WINDOWS)
	chFind    = '/';
	chReplace = '\\';
#elif(RH_TARGET_SYSTEM == RH_TARGET_SYSTEM_LINUX)
	chFind    = '\\';
	chReplace = '/';
#endif

	for(i = 0; i < strlen(pszPath); i++)
	{
		if(pszPath[i] == chFind)
			pszPath[i] = chReplace;
	}
}

void catdirsep(char *pszPath)
{
	UINTPREF i;

	RH_ASSERT(pszPath != NULL);
	RH_ASSERT(strlen(pszPath) != 0);

	i = strlen(pszPath);

	if(pszPath[i-1] == SZ_DIR_CHAR) return; // Nothing to do

	if(i < RH_MAX_PATH)
	{
		pszPath[i] = SZ_DIR_CHAR;
		pszPath[i+1] = 0;
	}
}

void pathonly(char *pszPath)
{
	UINTPREF i;
	bool bReplaced = false;

	RH_ASSERT(pszPath != NULL);
	RH_ASSERT(strlen(pszPath) != 0);

	i = strlen(pszPath) - 1;
	if(i == ((UINTPREF)-1)) return;
	while(1)
	{
		if(pszPath[i] == SZ_DIR_CHAR)
		{
			pszPath[i] = 0;
			bReplaced = true;
			break;
		}

		i--;
		if(i == (UINTPREF)-1) break;
	}

	if(bReplaced == false) pszPath[0] = 0;

	catdirsep(pszPath); // Append path character if necessary
}

void fileonly(char *pszPath)
{
	char szTemp[RH_MAX_PATH];
	UINTPREF i, j;

	RH_ASSERT(pszPath != NULL);

	i = strlen(pszPath) - 1;
	if(i == (unsigned int)-1) return;
	if(i <= 1) return;

	while(1) // Reverse scan for path delimiter
	{
		if(pszPath[i] == SZ_DIR_CHAR) break;
		i--;
		if(i == 0) break;
	}

	if(i == 0) return;

	j = 0;
	i++;
	for( ; i < strlen(pszPath); i++) // Copy only filename to new buffer
	{
		szTemp[j] = pszPath[i];
		j++;
	}
	szTemp[j] = 0;

	rhstrcpy(pszPath, szTemp, RH_MAX_PATH); // Store working buffer
}

// Is this a <..> or <.> path env descriptor?
bool ispathnav(char *pszPath)
{
	UINTPREF i;

	RH_ASSERT(pszPath != NULL);
	RH_ASSERT(strlen(pszPath) != 0);

	i = strlen(pszPath);
	if(i == 0) return false;
	if((pszPath[i-1] == '.') && (i == 1)) return true;
	if((pszPath[i-1] == '.') && (pszPath[i-2] == '.') && (i == 2)) return true;
	if((pszPath[i-1] == '.') && (pszPath[i-2] == SZ_DIR_CHAR)) return true;
	if((pszPath[i-1] == '.') && (pszPath[i-2] == '.') && (pszPath[i-3] == SZ_DIR_CHAR)) return true;

	return false;
}

bool haspath(char *pszPath)
{
	UINTPREF i = 0;
	bool bPath = false;

	RH_ASSERT(pszPath != NULL);

	while(1)
	{
		if(pszPath[i] == 0) break;

#if(RH_TARGET_SYSTEM == RH_TARGET_SYSTEM_WINDOWS)
		if(pszPath[i] == '\\') { bPath = true; break; }
#else
		if(pszPath[i] == '/') { bPath = true; break; }
#endif

		i++;
	}

	return bPath;
}

// strlwr for Linux
#if(RH_TARGET_SYSTEM != RH_TARGET_SYSTEM_WINDOWS)
char *_strlwr(char *s)
{
	char *cp;

	for(cp = s; *cp; cp++)
		*cp = tolower(*cp);

	return s;
}
#endif

// Secure string copy function
void rhstrcpy(char *szDest, const char *szSource, UINTPREF nMaxBuf)
{
	RH_ASSERT(szDest != NULL);
	RH_ASSERT(szSource != NULL);
	RH_ASSERT(nMaxBuf != (UINTPREF)-1);

	if(strlen(szSource) < nMaxBuf)
		strcpy(szDest, szSource);
	else
	{
		memcpy(szDest, szSource, nMaxBuf-1);
		szDest[nMaxBuf-1] = 0;
	}
}

// Secure string concatenation function
void rhstrcat(char *szDest, const char *szAppend, UINTPREF nMaxBuf)
{
	UINTPREF lDest = strlen(szDest);
	UINTPREF lAppend = strlen(szAppend);

	UINTPREF lSum = lDest + lAppend;

	RH_ASSERT(szDest != NULL);
	RH_ASSERT(szAppend != NULL);
	RH_ASSERT(nMaxBuf != (UINTPREF)0);
	RH_ASSERT(nMaxBuf != (UINTPREF)-1);

	if(lSum >= nMaxBuf)
		lAppend = nMaxBuf - (lDest + 1);

	if(lAppend > 0)
	{
		memcpy(szDest + lDest, szAppend, lAppend);
		szDest[lDest + lAppend] = 0;
	}
}

// Get the full path of a relative path
void rhfullpathex(const char *pszRelative, char *pszAbsolute)
{
	RH_ASSERT(pszRelative != NULL);
	RH_ASSERT(pszAbsolute != NULL);

#if (RH_TARGET_SYSTEM == RH_TARGET_SYSTEM_WINDOWS)
	_fullpath(pszAbsolute, pszRelative, RH_MAX_PATH);
#else
	realpath(pszRelative, pszAbsolute);
#endif
}
