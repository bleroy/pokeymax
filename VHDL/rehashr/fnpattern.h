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

// Heavily modified version of the filename matching algorithm implementation
// by David R. Tribble.

// Here's the original header:

/******************************************************************************
* fpattern.h
*	Functions for matching filename patterns to filenames.
*
* Usage
*	Filename patterns are composed of regular (printable) characters which
*	may comprise a filename as well as special pattern matching characters:
*
*	    .		Matches a period (.).
*			Note that a period in a filename is not treated any
*			differently than any other character.
*
*	    ?		Any.
*			Matches any single character except '/' or '\'.
*
*	    *		Closure.
*			Matches zero or more occurences of any characters other
*			than '/' or '\'.
*			Leading '*' characters are allowed.
*
*	    SUB		Substitute (^Z).
*			Similar to '*', this matches zero or more occurences of
*			any characters other than '/', '\', or '.'.
*			Leading '^Z' characters are allowed.
*
*	    [ab]	Set.
*			Matches the single character 'a' or 'b'.
*			If the dash '-' character is to be included, it must
*			immediately follow the opening bracket '['.
*			If the closing bracket ']' character is to be included,
*			it must be preceded by a quote '`'.
*
*	    [a-z]	Range.
*			Matches a single character in the range 'a' to 'z'.
*			Ranges and sets may be combined within the same set of
*			brackets.
*
*	    [!R]	Exclusive range.
*			Matches a single character not in the range 'R'.
*			If range 'R' includes the dash '-' character, the dash
*			must immediately follow the caret '!'.
*
*	    !		Not.
*			Makes the following pattern (up to the next '/') match
*			any filename except those what it would normally match.
*
*	    /		Path separator (UNIX and DOS).
*			Matches a '/' or '\' pathname (directory) separator.
*			Multiple separators are treated like a single
*			separator.
*			A leading separator indicates an absolute pathname.
*
*	    \		Path separator (DOS).
*			Same as the '/' character.
*			Note that this character must be escaped if used within
*			string constants ("\\").
*
*	    \		Quote (UNIX).
*			Makes the next character a regular (nonspecial)
*			character.
*			Note that to match the quote character itself, it must
*			be quoted.
*			Note that this character must be escaped if used within
*			string constants ("\\").
*
*	    `		Quote (DOS).
*			Makes the next character a regular (nonspecial)
*			character.
*			Note that to match the quote character itself, it must
*			be quoted.
*
*	Upper and lower case alphabetic characters are considered identical,
*	i.e., 'a' and 'A' match each other.
*	(What constitutes a lowercase letter depends on the current locale
*	settings.)
*
*	Spaces and control characters are treated as normal characters.
*
* Examples
*	The following patterns in the left column will match the filenames in
*	the middle column and will not match filenames in the right column:
*
*	    Pattern	Will Match			Will Not Match
*	    -------	----------			--------------
*	    a		a (only)			(anything else)
*	    a.		a. (only)			(anything else)
*	    a?c		abc, acc, arc, a.c		a, ac, abbc
*	    a*c		ac, abc, abbc, acc, a.c		a, ab, acb, bac
*	    a*		a, ab, abb, a., a.b		b, ba
*	    *		a, ab, abb, a., .foo, a.foo	(nothing)
*	    *.		a., ab., abb., a.foo.		a, ab, a.foo, .foo
*	    *.*		a., a.b, ah.bc.foo		a
*	    ^Z		a, ab, abb			a., .foo, a.foo
*	    ^Z.		a., ab., abb.			a, .foo, a.foo
*	    ^Z.*	a, a., .foo, a.foo		ab, abb
*	    *2.c	2.c, 12.c, foo2.c, foo.12.c	2x.c
*	    a[b-z]c	abc, acc, azc (only)		(anything else)
*	    [ab0-9]x	ax, bx, 0x, 9x			zx
*	    a[-.]b	a-b, a.b (only)			(anything else)
*	    a[!a-z]b	a0b, a.b, a@b			aab, azb, aa0b
*	    a[!-b]x	a0x, a+x, acx			a-x, abx, axxx
*	    a[-!b]x	a-x, a!x, abx (only)		(anything else)
*	    a[`]]x	a]x (only)			(anything else)
*	    a``x	a`x (only)			(anything else)
*	    oh`!	oh! (only)			(anything else)
*	    is`?it	is?it (only)			(anything else)
*	    !a?c	a, ac, ab, abb, acb, a.foo      abc, a.c, azc
*
* History
*	1.00 1997-01-03 David Tribble.
*		First cut.
*	1.01 1997-01-03 David Tribble.
*		Added '^Z' pattern character.
*		Added fpattern_matchn().
*	1.02 1997-01-26 David Tribble.
*		Changed range negation character from '^' to '!', ala Unix.
*	1.03 1997-08-02 David Tribble.
*		Added 'FPAT_XXX' macro constants.
*
* Limitations
*	This code is copyrighted by the author, but permission is hereby
*	granted for its unlimited use provided that the original copyright
*	and authorship notices are retained intact.
*
*	Other queries can be sent to:
*	    dtribble@technologist.com
*	    david.tribble@beasys.com
*	    dtribble@flash.net
*
* Copyright �1997 by David R. Tribble, all rights reserved.
*/

#ifndef ___FNPATTERN_H___
#define ___FNPATTERN_H___

#include "rhcommon.h"

#define FPAT_QUOTE    '\\'   /* Quotes a special char */
#define FPAT_QUOTE2   '`'    /* Quotes a special char */
#define FPAT_DEL      '/'    /* Path delimiter */
#define FPAT_DEL2     '\\'   /* Path delimiter */
#define FPAT_DOT      '.'    /* Dot char */
#define FPAT_NOT      '!'    /* Exclusion */
#define FPAT_ANY      '?'    /* Any one char */
#define FPAT_CLOS     '*'    /* Zero or more chars */
#define FPAT_CLOSP    '\x1A' /* Zero or more nondelimiters */
#define FPAT_SET_L    '['    /* Set/range open bracket */
#define FPAT_SET_R    ']'    /* Set/range close bracket */
#define FPAT_SET_NOT  '!'    /* Set exclusion */
#define FPAT_SET_THRU '-'    /* Set range of chars */

bool fpattern_isvalid(const char *pat);
bool fpattern_match(const char *pat, const char *fname);

bool _fpattern_submatch(const char *pat, const char *fname);

#endif // ___FNPATTERN_H___
