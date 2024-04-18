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

#include "haval256_5.h"

#define HAVAL_COMMON_SOURCE 0
#define HAVAL_PASS          5
#define HAVAL_FPTLEN      256
#define HAVAL_VERSION       1

#if (HAVAL_COMMON_SOURCE == 0)

static const UWORD8 HAVAL_PADDING[128] =
{
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define HAVAL_F_1(x6, x5, x4, x3, x2, x1, x0) \
	((x1) & ((x0) ^ (x4)) ^ (x2) & (x5) ^ \
	(x3) & (x6) ^ (x0))

#define HAVAL_F_2(x6, x5, x4, x3, x2, x1, x0) \
	((x2) & ((x1) & ~(x3) ^ (x4) & (x5) ^ (x6) ^ (x0)) ^ \
	(x4) & ((x1) ^ (x5)) ^ (x3) & (x5) ^ (x0))

#define HAVAL_F_3(x6, x5, x4, x3, x2, x1, x0) \
	((x3) & ((x1) & (x2) ^ (x6) ^ (x0)) ^ \
	(x1) & (x4) ^ (x2) & (x5) ^ (x0))

#define HAVAL_F_4(x6, x5, x4, x3, x2, x1, x0) \
	((x4) & ((x5) & ~(x2) ^ (x3) & ~(x6) ^ (x1) ^ (x6) ^ (x0)) ^ \
	(x3) & ((x1) & (x2) ^ (x5) ^ (x6)) ^ \
	(x2) & (x6) ^ (x0))

#define HAVAL_F_5(x6, x5, x4, x3, x2, x1, x0) \
	((x0) & ((x1) & (x2) & (x3) ^ ~(x5)) ^ \
	(x1) & (x4) ^ (x2) & (x5) ^ (x3) & (x6))

#if (HAVAL_PASS == 3)
#define HAVAL_FPHI_1(x6, x5, x4, x3, x2, x1, x0) \
	HAVAL_F_1(x1, x0, x3, x5, x6, x2, x4)
#elif (HAVAL_PASS == 4)
#define HAVAL_FPHI_1(x6, x5, x4, x3, x2, x1, x0) \
	HAVAL_F_1(x2, x6, x1, x4, x5, x3, x0)
#else
#define HAVAL_FPHI_1(x6, x5, x4, x3, x2, x1, x0) \
	HAVAL_F_1(x3, x4, x1, x0, x5, x2, x6)
#endif

#if (HAVAL_PASS == 3)
#define HAVAL_FPHI_2(x6, x5, x4, x3, x2, x1, x0) \
	HAVAL_F_2(x4, x2, x1, x0, x5, x3, x6)
#elif (HAVAL_PASS == 4)
#define HAVAL_FPHI_2(x6, x5, x4, x3, x2, x1, x0) \
	HAVAL_F_2(x3, x5, x2, x0, x1, x6, x4)
#else
#define HAVAL_FPHI_2(x6, x5, x4, x3, x2, x1, x0) \
	HAVAL_F_2(x6, x2, x1, x0, x3, x4, x5)
#endif

#if (HAVAL_PASS == 3)
#define HAVAL_FPHI_3(x6, x5, x4, x3, x2, x1, x0) \
	HAVAL_F_3(x6, x1, x2, x3, x4, x5, x0)
#elif (HAVAL_PASS == 4)
#define HAVAL_FPHI_3(x6, x5, x4, x3, x2, x1, x0) \
	HAVAL_F_3(x1, x4, x3, x6, x0, x2, x5)
#else
#define HAVAL_FPHI_3(x6, x5, x4, x3, x2, x1, x0) \
	HAVAL_F_3(x2, x6, x0, x4, x3, x1, x5)
#endif

#if (HAVAL_PASS == 4)
#define HAVAL_FPHI_4(x6, x5, x4, x3, x2, x1, x0) \
	HAVAL_F_4(x6, x4, x0, x5, x2, x1, x3)
#else
#define HAVAL_FPHI_4(x6, x5, x4, x3, x2, x1, x0) \
	HAVAL_F_4(x1, x5, x3, x2, x0, x4, x6)
#endif

#define HAVAL_FPHI_5(x6, x5, x4, x3, x2, x1, x0) \
	HAVAL_F_5(x2, x5, x0, x6, x4, x3, x1)

#define HAVAL_FF_1(x7, x6, x5, x4, x3, x2, x1, x0, w) { \
	register UWORD32 utemp = HAVAL_FPHI_1(x6, x5, x4, x3, x2, x1, x0); \
	(x7) = ROR32(utemp, 7) + ROR32((x7), 11) + (w); }

#define HAVAL_FF_2(x7, x6, x5, x4, x3, x2, x1, x0, w, c) { \
	register UWORD32 utemp = HAVAL_FPHI_2(x6, x5, x4, x3, x2, x1, x0); \
	(x7) = ROR32(utemp, 7) + ROR32((x7), 11) + (w) + (c); }

#define HAVAL_FF_3(x7, x6, x5, x4, x3, x2, x1, x0, w, c) { \
	register UWORD32 utemp = HAVAL_FPHI_3(x6, x5, x4, x3, x2, x1, x0); \
	(x7) = ROR32(utemp, 7) + ROR32((x7), 11) + (w) + (c); }

#define HAVAL_FF_4(x7, x6, x5, x4, x3, x2, x1, x0, w, c) { \
	register UWORD32 utemp = HAVAL_FPHI_4(x6, x5, x4, x3, x2, x1, x0); \
	(x7) = ROR32(utemp, 7) + ROR32((x7), 11) + (w) + (c); }

#define HAVAL_FF_5(x7, x6, x5, x4, x3, x2, x1, x0, w, c) { \
	register UWORD32 utemp = HAVAL_FPHI_5(x6, x5, x4, x3, x2, x1, x0); \
	(x7) = ROR32(utemp, 7) + ROR32((x7), 11) + (w) + (c); }

#define HAVAL_CH2UINT(pString, pWord, uSLen) \
{ \
	UWORD8 *sp = (UWORD8 *)pString; \
	UWORD32 *wp = (UWORD32 *)pWord; \
	while(sp < ((pString) + (uSLen))) \
	{ \
		*wp++ = (UWORD32)*sp | \
				((UWORD32)*(sp + 1) <<  8) | \
				((UWORD32)*(sp + 2) << 16) | \
				((UWORD32)*(sp + 3) << 24); \
		sp += 4; \
	} \
}

#define HAVAL_UINT2CH(pWord, pString, uWLen) \
{ \
	UWORD32 *wp = (UWORD32 *)pWord; \
	UWORD8 *sp = (UWORD8 *)pString; \
	while(wp < (pWord) + (uWLen)) \
	{ \
		*(sp++) = (UWORD8)( *wp        & 0xFF); \
		*(sp++) = (UWORD8)((*wp >>  8) & 0xFF); \
		*(sp++) = (UWORD8)((*wp >> 16) & 0xFF); \
		*(sp++) = (UWORD8)((*wp >> 24) & 0xFF); \
		wp++; \
	} \
}

CHaval256_5::CHaval256_5()
{
}

CHaval256_5::~CHaval256_5()
{
}

void CHaval256_5::Init(RH_DATA_INFO *pInfo)
{
	m_count[0] = m_count[1] = 0;
	m_fingerprint[0] = CONST32(0x243F6A88);
	m_fingerprint[1] = CONST32(0x85A308D3);
	m_fingerprint[2] = CONST32(0x13198A2E);
	m_fingerprint[3] = CONST32(0x03707344);
	m_fingerprint[4] = CONST32(0xA4093822);
	m_fingerprint[5] = CONST32(0x299F31D0);
	m_fingerprint[6] = CONST32(0x082EFA98);
	m_fingerprint[7] = CONST32(0xEC4E6C89);
}

void CHaval256_5::Update(const UWORD8 *pBuf, UINTPREF uLen)
{
	UWORD32 i, rmd_len, fill_len;

	rmd_len  = (UWORD32)((m_count[0] >> 3) & 0x7F);
	fill_len = 128 - rmd_len;

	if((m_count[0] += (UWORD32)uLen << 3) < ((UWORD32)uLen << 3))
		m_count[1]++;
	m_count[1] += (UWORD32)uLen >> 29;

#ifdef RH_LITTLE_ENDIAN

	if((rmd_len + uLen) >= 128)
	{
		memcpy(((UWORD8 *)m_block) + rmd_len, pBuf, fill_len);
		_Compile();
		for(i = fill_len; (i + 127) < uLen; i += 128)
		{
			memcpy((UWORD8 *)m_block, pBuf + i, 128);
			_Compile();
		}
		rmd_len = 0;
	}
	else
	{
		i = 0;
	}

	memcpy(((UWORD8 *)m_block) + rmd_len, pBuf + i, uLen - i);

#else // !RH_LITTLE_ENDIAN

	if((rmd_len + uLen) >= 128)
	{
		memcpy(&m_remainder[rmd_len], pBuf, fill_len);
		HAVAL_CH2UINT(m_remainder, m_block, 128);
		_Compile();
		for(i = fill_len; (i + 127) < uLen; i += 128)
		{
			memcpy(m_remainder, pBuf + i, 128);
			HAVAL_CH2UINT(m_remainder, m_block, 128);
			_Compile();
		}
		rmd_len = 0;
	}
	else
	{
		i = 0;
	}

	memcpy (&m_remainder[rmd_len], pBuf + i, uLen - i);

#endif
}

void CHaval256_5::Final()
{
	UWORD8 pTail[10];
	UWORD32 rmd_len, pad_len;

	pTail[0] = (UWORD8)(((HAVAL_FPTLEN  & 0x03) << 6) |
		((HAVAL_PASS    & 0x07) << 3) |
		(HAVAL_VERSION & 0x07));
	pTail[1] = (UWORD8)((HAVAL_FPTLEN >> 2) & 0xFF);
	HAVAL_UINT2CH (m_count, &pTail[2], 2);

	rmd_len = (UWORD32)((m_count[0] >> 3) & 0x7F);
	pad_len = (rmd_len < 118) ? (118 - rmd_len) : (246 - rmd_len);
	Update(HAVAL_PADDING, pad_len);

	Update(pTail, 10);

	_Tailor();

	HAVAL_UINT2CH (m_fingerprint, m_final, (HAVAL_FPTLEN >> 5));
}

void CHaval256_5::_Compile()
{
	register UWORD32 t0 = m_fingerprint[0], t1 = m_fingerprint[1],
		t2 = m_fingerprint[2], t3 = m_fingerprint[3],
		t4 = m_fingerprint[4], t5 = m_fingerprint[5],
		t6 = m_fingerprint[6], t7 = m_fingerprint[7],
		*w = m_block;

	HAVAL_FF_1(t7, t6, t5, t4, t3, t2, t1, t0, *(w   ));
	HAVAL_FF_1(t6, t5, t4, t3, t2, t1, t0, t7, *(w+ 1));
	HAVAL_FF_1(t5, t4, t3, t2, t1, t0, t7, t6, *(w+ 2));
	HAVAL_FF_1(t4, t3, t2, t1, t0, t7, t6, t5, *(w+ 3));
	HAVAL_FF_1(t3, t2, t1, t0, t7, t6, t5, t4, *(w+ 4));
	HAVAL_FF_1(t2, t1, t0, t7, t6, t5, t4, t3, *(w+ 5));
	HAVAL_FF_1(t1, t0, t7, t6, t5, t4, t3, t2, *(w+ 6));
	HAVAL_FF_1(t0, t7, t6, t5, t4, t3, t2, t1, *(w+ 7));

	HAVAL_FF_1(t7, t6, t5, t4, t3, t2, t1, t0, *(w+ 8));
	HAVAL_FF_1(t6, t5, t4, t3, t2, t1, t0, t7, *(w+ 9));
	HAVAL_FF_1(t5, t4, t3, t2, t1, t0, t7, t6, *(w+10));
	HAVAL_FF_1(t4, t3, t2, t1, t0, t7, t6, t5, *(w+11));
	HAVAL_FF_1(t3, t2, t1, t0, t7, t6, t5, t4, *(w+12));
	HAVAL_FF_1(t2, t1, t0, t7, t6, t5, t4, t3, *(w+13));
	HAVAL_FF_1(t1, t0, t7, t6, t5, t4, t3, t2, *(w+14));
	HAVAL_FF_1(t0, t7, t6, t5, t4, t3, t2, t1, *(w+15));

	HAVAL_FF_1(t7, t6, t5, t4, t3, t2, t1, t0, *(w+16));
	HAVAL_FF_1(t6, t5, t4, t3, t2, t1, t0, t7, *(w+17));
	HAVAL_FF_1(t5, t4, t3, t2, t1, t0, t7, t6, *(w+18));
	HAVAL_FF_1(t4, t3, t2, t1, t0, t7, t6, t5, *(w+19));
	HAVAL_FF_1(t3, t2, t1, t0, t7, t6, t5, t4, *(w+20));
	HAVAL_FF_1(t2, t1, t0, t7, t6, t5, t4, t3, *(w+21));
	HAVAL_FF_1(t1, t0, t7, t6, t5, t4, t3, t2, *(w+22));
	HAVAL_FF_1(t0, t7, t6, t5, t4, t3, t2, t1, *(w+23));

	HAVAL_FF_1(t7, t6, t5, t4, t3, t2, t1, t0, *(w+24));
	HAVAL_FF_1(t6, t5, t4, t3, t2, t1, t0, t7, *(w+25));
	HAVAL_FF_1(t5, t4, t3, t2, t1, t0, t7, t6, *(w+26));
	HAVAL_FF_1(t4, t3, t2, t1, t0, t7, t6, t5, *(w+27));
	HAVAL_FF_1(t3, t2, t1, t0, t7, t6, t5, t4, *(w+28));
	HAVAL_FF_1(t2, t1, t0, t7, t6, t5, t4, t3, *(w+29));
	HAVAL_FF_1(t1, t0, t7, t6, t5, t4, t3, t2, *(w+30));
	HAVAL_FF_1(t0, t7, t6, t5, t4, t3, t2, t1, *(w+31));

	HAVAL_FF_2(t7, t6, t5, t4, t3, t2, t1, t0, *(w+ 5), CONST32(0x452821E6));
	HAVAL_FF_2(t6, t5, t4, t3, t2, t1, t0, t7, *(w+14), CONST32(0x38D01377));
	HAVAL_FF_2(t5, t4, t3, t2, t1, t0, t7, t6, *(w+26), CONST32(0xBE5466CF));
	HAVAL_FF_2(t4, t3, t2, t1, t0, t7, t6, t5, *(w+18), CONST32(0x34E90C6C));
	HAVAL_FF_2(t3, t2, t1, t0, t7, t6, t5, t4, *(w+11), CONST32(0xC0AC29B7));
	HAVAL_FF_2(t2, t1, t0, t7, t6, t5, t4, t3, *(w+28), CONST32(0xC97C50DD));
	HAVAL_FF_2(t1, t0, t7, t6, t5, t4, t3, t2, *(w+ 7), CONST32(0x3F84D5B5));
	HAVAL_FF_2(t0, t7, t6, t5, t4, t3, t2, t1, *(w+16), CONST32(0xB5470917));

	HAVAL_FF_2(t7, t6, t5, t4, t3, t2, t1, t0, *(w   ), CONST32(0x9216D5D9));
	HAVAL_FF_2(t6, t5, t4, t3, t2, t1, t0, t7, *(w+23), CONST32(0x8979FB1B));
	HAVAL_FF_2(t5, t4, t3, t2, t1, t0, t7, t6, *(w+20), CONST32(0xD1310BA6));
	HAVAL_FF_2(t4, t3, t2, t1, t0, t7, t6, t5, *(w+22), CONST32(0x98DFB5AC));
	HAVAL_FF_2(t3, t2, t1, t0, t7, t6, t5, t4, *(w+ 1), CONST32(0x2FFD72DB));
	HAVAL_FF_2(t2, t1, t0, t7, t6, t5, t4, t3, *(w+10), CONST32(0xD01ADFB7));
	HAVAL_FF_2(t1, t0, t7, t6, t5, t4, t3, t2, *(w+ 4), CONST32(0xB8E1AFED));
	HAVAL_FF_2(t0, t7, t6, t5, t4, t3, t2, t1, *(w+ 8), CONST32(0x6A267E96));

	HAVAL_FF_2(t7, t6, t5, t4, t3, t2, t1, t0, *(w+30), CONST32(0xBA7C9045));
	HAVAL_FF_2(t6, t5, t4, t3, t2, t1, t0, t7, *(w+ 3), CONST32(0xF12C7F99));
	HAVAL_FF_2(t5, t4, t3, t2, t1, t0, t7, t6, *(w+21), CONST32(0x24A19947));
	HAVAL_FF_2(t4, t3, t2, t1, t0, t7, t6, t5, *(w+ 9), CONST32(0xB3916CF7));
	HAVAL_FF_2(t3, t2, t1, t0, t7, t6, t5, t4, *(w+17), CONST32(0x0801F2E2));
	HAVAL_FF_2(t2, t1, t0, t7, t6, t5, t4, t3, *(w+24), CONST32(0x858EFC16));
	HAVAL_FF_2(t1, t0, t7, t6, t5, t4, t3, t2, *(w+29), CONST32(0x636920D8));
	HAVAL_FF_2(t0, t7, t6, t5, t4, t3, t2, t1, *(w+ 6), CONST32(0x71574E69));

	HAVAL_FF_2(t7, t6, t5, t4, t3, t2, t1, t0, *(w+19), CONST32(0xA458FEA3));
	HAVAL_FF_2(t6, t5, t4, t3, t2, t1, t0, t7, *(w+12), CONST32(0xF4933D7E));
	HAVAL_FF_2(t5, t4, t3, t2, t1, t0, t7, t6, *(w+15), CONST32(0x0D95748F));
	HAVAL_FF_2(t4, t3, t2, t1, t0, t7, t6, t5, *(w+13), CONST32(0x728EB658));
	HAVAL_FF_2(t3, t2, t1, t0, t7, t6, t5, t4, *(w+ 2), CONST32(0x718BCD58));

	HAVAL_FF_2(t2, t1, t0, t7, t6, t5, t4, t3, *(w+25), CONST32(0x82154AEE));
	HAVAL_FF_2(t1, t0, t7, t6, t5, t4, t3, t2, *(w+31), CONST32(0x7B54A41D));
	HAVAL_FF_2(t0, t7, t6, t5, t4, t3, t2, t1, *(w+27), CONST32(0xC25A59B5));

	HAVAL_FF_3(t7, t6, t5, t4, t3, t2, t1, t0, *(w+19), CONST32(0x9C30D539));
	HAVAL_FF_3(t6, t5, t4, t3, t2, t1, t0, t7, *(w+ 9), CONST32(0x2AF26013));
	HAVAL_FF_3(t5, t4, t3, t2, t1, t0, t7, t6, *(w+ 4), CONST32(0xC5D1B023));
	HAVAL_FF_3(t4, t3, t2, t1, t0, t7, t6, t5, *(w+20), CONST32(0x286085F0));
	HAVAL_FF_3(t3, t2, t1, t0, t7, t6, t5, t4, *(w+28), CONST32(0xCA417918));
	HAVAL_FF_3(t2, t1, t0, t7, t6, t5, t4, t3, *(w+17), CONST32(0xB8DB38EF));
	HAVAL_FF_3(t1, t0, t7, t6, t5, t4, t3, t2, *(w+ 8), CONST32(0x8E79DCB0));
	HAVAL_FF_3(t0, t7, t6, t5, t4, t3, t2, t1, *(w+22), CONST32(0x603A180E));

	HAVAL_FF_3(t7, t6, t5, t4, t3, t2, t1, t0, *(w+29), CONST32(0x6C9E0E8B));
	HAVAL_FF_3(t6, t5, t4, t3, t2, t1, t0, t7, *(w+14), CONST32(0xB01E8A3E));
	HAVAL_FF_3(t5, t4, t3, t2, t1, t0, t7, t6, *(w+25), CONST32(0xD71577C1));
	HAVAL_FF_3(t4, t3, t2, t1, t0, t7, t6, t5, *(w+12), CONST32(0xBD314B27));
	HAVAL_FF_3(t3, t2, t1, t0, t7, t6, t5, t4, *(w+24), CONST32(0x78AF2FDA));
	HAVAL_FF_3(t2, t1, t0, t7, t6, t5, t4, t3, *(w+30), CONST32(0x55605C60));
	HAVAL_FF_3(t1, t0, t7, t6, t5, t4, t3, t2, *(w+16), CONST32(0xE65525F3));
	HAVAL_FF_3(t0, t7, t6, t5, t4, t3, t2, t1, *(w+26), CONST32(0xAA55AB94));

	HAVAL_FF_3(t7, t6, t5, t4, t3, t2, t1, t0, *(w+31), CONST32(0x57489862));
	HAVAL_FF_3(t6, t5, t4, t3, t2, t1, t0, t7, *(w+15), CONST32(0x63E81440));
	HAVAL_FF_3(t5, t4, t3, t2, t1, t0, t7, t6, *(w+ 7), CONST32(0x55CA396A));
	HAVAL_FF_3(t4, t3, t2, t1, t0, t7, t6, t5, *(w+ 3), CONST32(0x2AAB10B6));
	HAVAL_FF_3(t3, t2, t1, t0, t7, t6, t5, t4, *(w+ 1), CONST32(0xB4CC5C34));
	HAVAL_FF_3(t2, t1, t0, t7, t6, t5, t4, t3, *(w   ), CONST32(0x1141E8CE));
	HAVAL_FF_3(t1, t0, t7, t6, t5, t4, t3, t2, *(w+18), CONST32(0xA15486AF));
	HAVAL_FF_3(t0, t7, t6, t5, t4, t3, t2, t1, *(w+27), CONST32(0x7C72E993));

	HAVAL_FF_3(t7, t6, t5, t4, t3, t2, t1, t0, *(w+13), CONST32(0xB3EE1411));
	HAVAL_FF_3(t6, t5, t4, t3, t2, t1, t0, t7, *(w+ 6), CONST32(0x636FBC2A));
	HAVAL_FF_3(t5, t4, t3, t2, t1, t0, t7, t6, *(w+21), CONST32(0x2BA9C55D));
	HAVAL_FF_3(t4, t3, t2, t1, t0, t7, t6, t5, *(w+10), CONST32(0x741831F6));
	HAVAL_FF_3(t3, t2, t1, t0, t7, t6, t5, t4, *(w+23), CONST32(0xCE5C3E16));
	HAVAL_FF_3(t2, t1, t0, t7, t6, t5, t4, t3, *(w+11), CONST32(0x9B87931E));
	HAVAL_FF_3(t1, t0, t7, t6, t5, t4, t3, t2, *(w+ 5), CONST32(0xAFD6BA33));
	HAVAL_FF_3(t0, t7, t6, t5, t4, t3, t2, t1, *(w+ 2), CONST32(0x6C24CF5C));

#if (HAVAL_PASS >= 4)
	HAVAL_FF_4(t7, t6, t5, t4, t3, t2, t1, t0, *(w+24), CONST32(0x7A325381));
	HAVAL_FF_4(t6, t5, t4, t3, t2, t1, t0, t7, *(w+ 4), CONST32(0x28958677));
	HAVAL_FF_4(t5, t4, t3, t2, t1, t0, t7, t6, *(w   ), CONST32(0x3B8F4898));
	HAVAL_FF_4(t4, t3, t2, t1, t0, t7, t6, t5, *(w+14), CONST32(0x6B4BB9AF));
	HAVAL_FF_4(t3, t2, t1, t0, t7, t6, t5, t4, *(w+ 2), CONST32(0xC4BFE81B));
	HAVAL_FF_4(t2, t1, t0, t7, t6, t5, t4, t3, *(w+ 7), CONST32(0x66282193));
	HAVAL_FF_4(t1, t0, t7, t6, t5, t4, t3, t2, *(w+28), CONST32(0x61D809CC));
	HAVAL_FF_4(t0, t7, t6, t5, t4, t3, t2, t1, *(w+23), CONST32(0xFB21A991));

	HAVAL_FF_4(t7, t6, t5, t4, t3, t2, t1, t0, *(w+26), CONST32(0x487CAC60));
	HAVAL_FF_4(t6, t5, t4, t3, t2, t1, t0, t7, *(w+ 6), CONST32(0x5DEC8032));
	HAVAL_FF_4(t5, t4, t3, t2, t1, t0, t7, t6, *(w+30), CONST32(0xEF845D5D));
	HAVAL_FF_4(t4, t3, t2, t1, t0, t7, t6, t5, *(w+20), CONST32(0xE98575B1));
	HAVAL_FF_4(t3, t2, t1, t0, t7, t6, t5, t4, *(w+18), CONST32(0xDC262302));
	HAVAL_FF_4(t2, t1, t0, t7, t6, t5, t4, t3, *(w+25), CONST32(0xEB651B88));
	HAVAL_FF_4(t1, t0, t7, t6, t5, t4, t3, t2, *(w+19), CONST32(0x23893E81));
	HAVAL_FF_4(t0, t7, t6, t5, t4, t3, t2, t1, *(w+ 3), CONST32(0xD396ACC5));

	HAVAL_FF_4(t7, t6, t5, t4, t3, t2, t1, t0, *(w+22), CONST32(0x0F6D6FF3));
	HAVAL_FF_4(t6, t5, t4, t3, t2, t1, t0, t7, *(w+11), CONST32(0x83F44239));
	HAVAL_FF_4(t5, t4, t3, t2, t1, t0, t7, t6, *(w+31), CONST32(0x2E0B4482));
	HAVAL_FF_4(t4, t3, t2, t1, t0, t7, t6, t5, *(w+21), CONST32(0xA4842004));
	HAVAL_FF_4(t3, t2, t1, t0, t7, t6, t5, t4, *(w+ 8), CONST32(0x69C8F04A));
	HAVAL_FF_4(t2, t1, t0, t7, t6, t5, t4, t3, *(w+27), CONST32(0x9E1F9B5E));
	HAVAL_FF_4(t1, t0, t7, t6, t5, t4, t3, t2, *(w+12), CONST32(0x21C66842));
	HAVAL_FF_4(t0, t7, t6, t5, t4, t3, t2, t1, *(w+ 9), CONST32(0xF6E96C9A));

	HAVAL_FF_4(t7, t6, t5, t4, t3, t2, t1, t0, *(w+ 1), CONST32(0x670C9C61));
	HAVAL_FF_4(t6, t5, t4, t3, t2, t1, t0, t7, *(w+29), CONST32(0xABD388F0));
	HAVAL_FF_4(t5, t4, t3, t2, t1, t0, t7, t6, *(w+ 5), CONST32(0x6A51A0D2));
	HAVAL_FF_4(t4, t3, t2, t1, t0, t7, t6, t5, *(w+15), CONST32(0xD8542F68));
	HAVAL_FF_4(t3, t2, t1, t0, t7, t6, t5, t4, *(w+17), CONST32(0x960FA728));
	HAVAL_FF_4(t2, t1, t0, t7, t6, t5, t4, t3, *(w+10), CONST32(0xAB5133A3));
	HAVAL_FF_4(t1, t0, t7, t6, t5, t4, t3, t2, *(w+16), CONST32(0x6EEF0B6C));
	HAVAL_FF_4(t0, t7, t6, t5, t4, t3, t2, t1, *(w+13), CONST32(0x137A3BE4));
#endif

#if (HAVAL_PASS == 5)
	HAVAL_FF_5(t7, t6, t5, t4, t3, t2, t1, t0, *(w+27), CONST32(0xBA3BF050));
	HAVAL_FF_5(t6, t5, t4, t3, t2, t1, t0, t7, *(w+ 3), CONST32(0x7EFB2A98));
	HAVAL_FF_5(t5, t4, t3, t2, t1, t0, t7, t6, *(w+21), CONST32(0xA1F1651D));
	HAVAL_FF_5(t4, t3, t2, t1, t0, t7, t6, t5, *(w+26), CONST32(0x39AF0176));
	HAVAL_FF_5(t3, t2, t1, t0, t7, t6, t5, t4, *(w+17), CONST32(0x66CA593E));
	HAVAL_FF_5(t2, t1, t0, t7, t6, t5, t4, t3, *(w+11), CONST32(0x82430E88));
	HAVAL_FF_5(t1, t0, t7, t6, t5, t4, t3, t2, *(w+20), CONST32(0x8CEE8619));
	HAVAL_FF_5(t0, t7, t6, t5, t4, t3, t2, t1, *(w+29), CONST32(0x456F9FB4));

	HAVAL_FF_5(t7, t6, t5, t4, t3, t2, t1, t0, *(w+19), CONST32(0x7D84A5C3));
	HAVAL_FF_5(t6, t5, t4, t3, t2, t1, t0, t7, *(w   ), CONST32(0x3B8B5EBE));
	HAVAL_FF_5(t5, t4, t3, t2, t1, t0, t7, t6, *(w+12), CONST32(0xE06F75D8));
	HAVAL_FF_5(t4, t3, t2, t1, t0, t7, t6, t5, *(w+ 7), CONST32(0x85C12073));
	HAVAL_FF_5(t3, t2, t1, t0, t7, t6, t5, t4, *(w+13), CONST32(0x401A449F));
	HAVAL_FF_5(t2, t1, t0, t7, t6, t5, t4, t3, *(w+ 8), CONST32(0x56C16AA6));
	HAVAL_FF_5(t1, t0, t7, t6, t5, t4, t3, t2, *(w+31), CONST32(0x4ED3AA62));
	HAVAL_FF_5(t0, t7, t6, t5, t4, t3, t2, t1, *(w+10), CONST32(0x363F7706));

	HAVAL_FF_5(t7, t6, t5, t4, t3, t2, t1, t0, *(w+ 5), CONST32(0x1BFEDF72));
	HAVAL_FF_5(t6, t5, t4, t3, t2, t1, t0, t7, *(w+ 9), CONST32(0x429B023D));
	HAVAL_FF_5(t5, t4, t3, t2, t1, t0, t7, t6, *(w+14), CONST32(0x37D0D724));
	HAVAL_FF_5(t4, t3, t2, t1, t0, t7, t6, t5, *(w+30), CONST32(0xD00A1248));
	HAVAL_FF_5(t3, t2, t1, t0, t7, t6, t5, t4, *(w+18), CONST32(0xDB0FEAD3));
	HAVAL_FF_5(t2, t1, t0, t7, t6, t5, t4, t3, *(w+ 6), CONST32(0x49F1C09B));
	HAVAL_FF_5(t1, t0, t7, t6, t5, t4, t3, t2, *(w+28), CONST32(0x075372C9));
	HAVAL_FF_5(t0, t7, t6, t5, t4, t3, t2, t1, *(w+24), CONST32(0x80991B7B));

	HAVAL_FF_5(t7, t6, t5, t4, t3, t2, t1, t0, *(w+ 2), CONST32(0x25D479D8));
	HAVAL_FF_5(t6, t5, t4, t3, t2, t1, t0, t7, *(w+23), CONST32(0xF6E8DEF7));
	HAVAL_FF_5(t5, t4, t3, t2, t1, t0, t7, t6, *(w+16), CONST32(0xE3FE501A));
	HAVAL_FF_5(t4, t3, t2, t1, t0, t7, t6, t5, *(w+22), CONST32(0xB6794C3B));
	HAVAL_FF_5(t3, t2, t1, t0, t7, t6, t5, t4, *(w+ 4), CONST32(0x976CE0BD));
	HAVAL_FF_5(t2, t1, t0, t7, t6, t5, t4, t3, *(w+ 1), CONST32(0x04C006BA));
	HAVAL_FF_5(t1, t0, t7, t6, t5, t4, t3, t2, *(w+25), CONST32(0xC1A94FB6));
	HAVAL_FF_5(t0, t7, t6, t5, t4, t3, t2, t1, *(w+15), CONST32(0x409F60C4));
#endif

	m_fingerprint[0] += t0; m_fingerprint[1] += t1;
	m_fingerprint[2] += t2; m_fingerprint[3] += t3;
	m_fingerprint[4] += t4; m_fingerprint[5] += t5;
	m_fingerprint[6] += t6; m_fingerprint[7] += t7;
}

void CHaval256_5::_Tailor()
{
	UWORD32 utemp;

#if (HAVAL_FPTLEN == 128)

	utemp = (m_fingerprint[7] & CONST32(0x000000FF) |
		(m_fingerprint[6] & CONST32(0xFF000000) |
		(m_fingerprint[5] & CONST32(0x00FF0000) |
		(m_fingerprint[4] & CONST32(0x0000FF00));
	m_fingerprint[0] += ROR32(utemp,  8);

	utemp = (m_fingerprint[7] & CONST32(0x0000FF00) |
		(m_fingerprint[6] & CONST32(0x000000FF) |
		(m_fingerprint[5] & CONST32(0xFF000000) |
		(m_fingerprint[4] & CONST32(0x00FF0000));
	m_fingerprint[1] += ROR32(utemp, 16);

	utemp = (m_fingerprint[7] & CONST32(0x00FF0000) |
		(m_fingerprint[6] & CONST32(0x0000FF00) |
		(m_fingerprint[5] & CONST32(0x000000FF) |
		(m_fingerprint[4] & CONST32(0xFF000000));
	m_fingerprint[2] += ROR32(utemp, 24);

	utemp = (m_fingerprint[7] & CONST32(0xFF000000) |
		(m_fingerprint[6] & CONST32(0x00FF0000) |
		(m_fingerprint[5] & CONST32(0x0000FF00) |
		(m_fingerprint[4] & CONST32(0x000000FF));
	m_fingerprint[3] += utemp;

#elif (HAVAL_FPTLEN == 160)

	utemp = (m_fingerprint[7] &  (UWORD32)0x3F) |
		(m_fingerprint[6] & ((UWORD32)0x7F << 25)) |
		(m_fingerprint[5] & ((UWORD32)0x3F << 19));
	m_fingerprint[0] += ROR32(utemp, 19);

	utemp = (m_fingerprint[7] & ((UWORD32)0x3F <<  6)) |
		(m_fingerprint[6] &  (UWORD32)0x3F) |
		(m_fingerprint[5] & ((UWORD32)0x7F << 25));
	m_fingerprint[1] += ROR32(utemp, 25);

	utemp = (m_fingerprint[7] & ((UWORD32)0x7F << 12)) |
		(m_fingerprint[6] & ((UWORD32)0x3F <<  6)) |
		(m_fingerprint[5] &  (UWORD32)0x3F);
	m_fingerprint[2] += utemp;

	utemp = (m_fingerprint[7] & ((UWORD32)0x3F << 19)) |
		(m_fingerprint[6] & ((UWORD32)0x7F << 12)) |
		(m_fingerprint[5] & ((UWORD32)0x3F <<  6));
	m_fingerprint[3] += utemp >> 6;

	utemp = (m_fingerprint[7] & ((UWORD32)0x7F << 25)) |
		(m_fingerprint[6] & ((UWORD32)0x3F << 19)) |
		(m_fingerprint[5] & ((UWORD32)0x7F << 12));
	m_fingerprint[4] += utemp >> 12;

#elif (HAVAL_FPTLEN == 192)

	utemp = (m_fingerprint[7] &  (UWORD32)0x1F) |
		(m_fingerprint[6] & ((UWORD32)0x3F << 26));
	m_fingerprint[0] += ROR32(utemp, 26);

	utemp = (m_fingerprint[7] & ((UWORD32)0x1F <<  5)) |
		(m_fingerprint[6] &  (UWORD32)0x1F);
	m_fingerprint[1] += utemp;

	utemp = (m_fingerprint[7] & ((UWORD32)0x3F << 10)) |
		(m_fingerprint[6] & ((UWORD32)0x1F <<  5));
	m_fingerprint[2] += utemp >> 5;

	utemp = (m_fingerprint[7] & ((UWORD32)0x1F << 16)) |
		(m_fingerprint[6] & ((UWORD32)0x3F << 10));
	m_fingerprint[3] += utemp >> 10;

	utemp = (m_fingerprint[7] & ((UWORD32)0x1F << 21)) |
		(m_fingerprint[6] & ((UWORD32)0x1F << 16));
	m_fingerprint[4] += utemp >> 16;

	utemp = (m_fingerprint[7] & ((UWORD32)0x3F << 26)) |
		(m_fingerprint[6] & ((UWORD32)0x1F << 21));
	m_fingerprint[5] += utemp >> 21;

#elif (HAVAL_FPTLEN == 224)

	m_fingerprint[0] += (m_fingerprint[7] >> 27) & 0x1F;
	m_fingerprint[1] += (m_fingerprint[7] >> 22) & 0x1F;
	m_fingerprint[2] += (m_fingerprint[7] >> 18) & 0x0F;
	m_fingerprint[3] += (m_fingerprint[7] >> 13) & 0x1F;
	m_fingerprint[4] += (m_fingerprint[7] >>  9) & 0x0F;
	m_fingerprint[5] += (m_fingerprint[7] >>  4) & 0x1F;
	m_fingerprint[6] +=  m_fingerprint[7]        & 0x0F;

#else // (HAVAL_FPTLEN == 256) // No tailoring needed

	utemp = 0; // Avoid unreferenced parameter

#endif
}

#endif
