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

// This implementation of the HAVAL hashing algorithm is based on the free
// implementation of Calyptix Security Corporation. The source has been
// heavily modified to be used in ReHash.

// Here's the original header:

/*
 *  haval.c:  specifies the routines in the HAVAL (V.1) hashing library.
 *
 *  Copyright (c) 2003 Calyptix Security Corporation
 *  All rights reserved.
 *
 *  This code is derived from software contributed to Calyptix Security
 *  Corporation by Yuliang Zheng.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *  3. Neither the name of Calyptix Security Corporation nor the
 *     names of its contributors may be used to endorse or promote
 *     products derived from this software without specific prior
 *     written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * -------------------------------------------------------------------
 *
 *      HAVAL is a one-way hashing algorithm with the following
 *      collision-resistant property:
 *             It is computationally infeasible to find two or more
 *             messages that are hashed into the same fingerprint.
 *
 *  Reference:
 *       Y. Zheng, J. Pieprzyk and J. Seberry:
 *       ``HAVAL --- a one-way hashing algorithm with variable
 *       length of output'', Advances in Cryptology --- AUSCRYPT'92,
 *       Lecture Notes in Computer Science,  Vol.718, pp.83-104,
 *       Springer-Verlag, 1993.
 *
 *  Descriptions:
 *      -  haval_string:      hash a string
 *      -  haval_file:        hash a file
 *      -  haval_stdin:       filter -- hash input from the stdin device
 *      -  haval_hash:        hash a string of specified length
 *                            (Haval_hash is used in conjunction with
 *                             haval_start & haval_end.)
 *      -  haval_hash_block:  hash a 32-word block
 *      -  haval_start:       initialization
 *      -  haval_end:         finalization
 *
 *  Authors:    Yuliang Zheng and Lawrence Teo
 *              Calyptix Security Corporation
 *              P.O. Box 561508, Charlotte, NC 28213, USA
 *              Email: info@calyptix.com
 *              URL:   http://www.calyptix.com/
 *              Voice: +1 704 806 8635
 *
 *  For a list of changes, see the ChangeLog file.
 */

// Most of the helper functions (like haval_string and haval_file) have
// been removed.

#ifndef ___HAVAL_256_5_H___
#define ___HAVAL_256_5_H___

#include "hashalgo.h"

class CHaval256_5 : public CHashAlgorithm
{
public:
	CHaval256_5();
	~CHaval256_5();

	const char *GetName() { return "HAVAL256.5"; }
	const char *GetShortName() { return "haval5"; }
	UINTPREF GetLength() { return 32; }
	UINTPREF GetInternalLength() { return 128; }

	void Init(RH_DATA_INFO *pInfo);
	void Update(const UWORD8 *pBuf, UINTPREF uLen);
	void Final();
	void GetHash(UWORD8 *pHash) { memcpy(pHash, m_final, 32); }

private:
	void _Compile();
	void _Tailor();

	UWORD32 m_count[2];
	UWORD32 m_fingerprint[8];
	UWORD32 m_block[32];
	UWORD8 m_remainder[32*4];

	UWORD8 m_final[32];
};

#endif // ___HAVAL_256_5_H___
