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

#ifndef ___CL_UTIL_H___
#define ___CL_UTIL_H___

#include "rhcommon.h"

// Test if pszString is an argument
bool isArgument(char *pszString);

// Removes all unnecessary characters from an argument string
void fmtArgument(char *pszArg, char *pszDest);

// Format pszPath such that it is compatible with the OS
void fmtPath(char *pszPath);

// Append a path character (for example '/') to the string
// if necessary
void catdirsep(char *pszPath);

// Extract path and file
void pathonly(char *pszPath);
void fileonly(char *pszPath);

// Test if pszPath is a path navigation string (like "." and "..")
bool ispathnav(char *pszPath);

// Test if pszPath contains a path (so !file)
bool haspath(char *pszPath);

// For Unix/Linux: strlwr function
#if (RH_TARGET_SYSTEM != RH_TARGET_SYSTEM_WINDOWS)
char *_strlwr(char *s);
#endif

// Secure strcpy and strcat versions
void rhstrcpy(char *szDest, const char *szSource, UINTPREF nMaxBuf);
void rhstrcat(char *szDest, const char *szAppend, UINTPREF nMaxBuf);

// Make full path
void rhfullpathex(const char *pszRelative, char *pszAbsolute);

#endif // ___CL_UTIL_H___
