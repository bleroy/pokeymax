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

// For the complete header, including the original authors information,
// see the header file.

#include "fnpattern.h"

#define CH_SUB (FPAT_CLOSP)

#if (RH_TARGET_SYSTEM == RH_TARGET_SYSTEM_LINUX)
#define CH_QUOTE FPAT_QUOTE
#else /*DOS*/
#define CH_QUOTE FPAT_QUOTE2
#endif

bool fpattern_isvalid(const char *pat)
{
	INTPREF len;

	RH_ASSERT(pat != NULL);

	// Verify that the pattern is valid
	for(len = 0; pat[len] != '\0'; len++)
	{
		switch(pat[len])
		{
		case FPAT_SET_L:
			// Char set
			len++;
			if(pat[len] == FPAT_SET_NOT)
				len++; // Set negation

			while(pat[len] != FPAT_SET_R)
			{
				if(pat[len] == CH_QUOTE)
					len++; // Quoted char
				if(pat[len] == '\0')
					return false; // Missing closing bracket
				len++;

				if(pat[len] == FPAT_SET_THRU)
				{
					// Char range
					len++;
					if(pat[len] == CH_QUOTE)
						len++; // Quoted char
					if(pat[len] == '\0')
						return false; // Missing closing bracket
					len++;
				}

				if(pat[len] == '\0')
					return false; // Missing closing bracket
			}
			break;

		case CH_QUOTE:
			// Quoted char
			len++;
			if(pat[len] == '\0')
				return false; // Missing quoted char
			break;

		case FPAT_NOT:
			// Negated pattern
			len++;
			if(pat[len] == '\0')
				return false; // Missing subpattern
			break;

		default:
			// Valid character
			break;
		}
	}

	return true;
}

bool _fpattern_submatch(const char *pat, const char *fname)
{
	INTPREF fch;
	INTPREF pch;
	INTPREF i;
	INTPREF yes, match;
	INTPREF lo, hi;

	RH_ASSERT(pat != NULL);
	RH_ASSERT(fname != NULL);

	// Attempt to match subpattern against subfilename
	while(*pat != '\0')
	{
		fch = *fname;
		pch = *pat;
		pat++;

		switch(pch)
		{
		case FPAT_ANY:
			// Match a single char
			if(fch == '\0') return false;
			fname++;
			break;

		case FPAT_CLOS:
			// Match zero or more chars
			i = 0;
			while(fname[i] != '\0') i++;
			while(i >= 0)
			{
				if(_fpattern_submatch(pat, fname + i))
					return true;
				i--;
			}
			return false;
			break;

		case CH_SUB:
			// Match zero or more chars
			i = 0;
			while ((fname[i] != '\0') && (fname[i] != '.')) i++;
			while (i >= 0)
			{
				if(_fpattern_submatch(pat, fname+i))
					return true;
				i--;
			}
			return false;
			break;

		case CH_QUOTE:
			// Match a quoted char
			pch = *pat;
			if((lowcase(fch) != lowcase(pch)) || (pch == '\0'))
				return false;
			fname++;
			pat++;
			break;

		case FPAT_SET_L:
			// Match char set/range
			yes = true;
			if(*pat == FPAT_SET_NOT)
			{
				pat++;
				yes = false; // Set negation
			}

			// Look for [s], [-], [abc], [a-c]
			match = !yes;
			while((*pat != FPAT_SET_R) && (*pat != '\0'))
			{
				if(*pat == CH_QUOTE)
					pat++;	// Quoted char

				if(*pat == '\0')
					break;
				lo = *pat++;
				hi = lo;

				if(*pat == FPAT_SET_THRU)
				{
					// Range
					pat++;

					if(*pat == CH_QUOTE)
						pat++; // Quoted char

					if(*pat == '\0')
						break;
					hi = *pat++;
				}

				if(*pat == '\0')
					break;

				// Compare character to set range
				if ((lowcase(fch) >= lowcase(lo)) &&
					(lowcase(fch) <= lowcase(hi)))
					match = yes;
			}

			if(!match)
				return false;

			if(*pat == '\0')
				return false; // Missing closing bracket

			fname++;
			pat++;
			break;

		case FPAT_NOT:
			// Match only if rest of pattern does not match
			if(*pat == '\0')
				return false; // Missing subpattern
			i = _fpattern_submatch(pat, fname);
			return !i;

		default:
			// Match a (non-null) char exactly
			if (lowcase(fch) != lowcase(pch))
				return false;
			fname++;
			break;
		}
	}

	// Check for complete match
	if(*fname != '\0')
		return false;

	// Successful match
	return true;
}

bool fpattern_match(const char *pat, const char *fname)
{
	RH_ASSERT(pat != NULL);
	RH_ASSERT(fname != NULL);

	// Attempt to match pattern against filename
	if(fname[0] == '\0')
		return (pat[0] == '\0'); // Special case

	return _fpattern_submatch(pat, fname);
}
