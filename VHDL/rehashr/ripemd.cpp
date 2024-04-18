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

// This free implementation is based on Tom St Denis free implementation, which
// is based on the free implementation of Antoon Bosselaers, ESAT-COSIC.

#include "ripemd.h"

#define RMD128_F(x, y, z) ((x) ^ (y) ^ (z))
#define RMD128_G(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define RMD128_H(x, y, z) (((x) | ~(y)) ^ (z))
#define RMD128_I(x, y, z) (((x) & (z)) | ((y) & ~(z)))

#define RMD128_FF(a, b, c, d, x, s) \
	(a) += RMD128_F((b), (c), (d)) + (x); \
	(a) = ROL32((a), (s));

#define RMD128_GG(a, b, c, d, x, s) \
	(a) += RMD128_G((b), (c), (d)) + (x) + CONST32(0x5a827999); \
	(a) = ROL32((a), (s));

#define RMD128_HH(a, b, c, d, x, s) \
	(a) += RMD128_H((b), (c), (d)) + (x) + CONST32(0x6ed9eba1); \
	(a) = ROL32((a), (s));

#define RMD128_II(a, b, c, d, x, s) \
	(a) += RMD128_I((b), (c), (d)) + (x) + CONST32(0x8f1bbcdc); \
	(a) = ROL32((a), (s));

#define RMD128_FFF(a, b, c, d, x, s) \
	(a) += RMD128_F((b), (c), (d)) + (x); \
	(a) = ROL32((a), (s));

#define RMD128_GGG(a, b, c, d, x, s) \
	(a) += RMD128_G((b), (c), (d)) + (x) + CONST32(0x6d703ef3); \
	(a) = ROL32((a), (s));

#define RMD128_HHH(a, b, c, d, x, s) \
	(a) += RMD128_H((b), (c), (d)) + (x) + CONST32(0x5c4dd124); \
	(a) = ROL32((a), (s));

#define RMD128_III(a, b, c, d, x, s) \
	(a) += RMD128_I((b), (c), (d)) + (x) + CONST32(0x50a28be6); \
	(a) = ROL32((a), (s));

/////////////////////////////////////////////////////////////////////////////

#define RMD160_F(x, y, z) ((x) ^ (y) ^ (z))
#define RMD160_G(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define RMD160_H(x, y, z) (((x) | ~(y)) ^ (z))
#define RMD160_I(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define RMD160_J(x, y, z) ((x) ^ ((y) | ~(z)))

#define RMD160_FF(a, b, c, d, e, x, s) \
      (a) += RMD160_F((b), (c), (d)) + (x);\
      (a) = ROL32((a), (s)) + (e); \
      (c) = ROL32((c), 10);

#define RMD160_GG(a, b, c, d, e, x, s) \
      (a) += RMD160_G((b), (c), (d)) + (x) + CONST32(0x5a827999);\
      (a) = ROL32((a), (s)) + (e); \
      (c) = ROL32((c), 10);

#define RMD160_HH(a, b, c, d, e, x, s) \
      (a) += RMD160_H((b), (c), (d)) + (x) + CONST32(0x6ed9eba1);\
      (a) = ROL32((a), (s)) + (e); \
      (c) = ROL32((c), 10);

#define RMD160_II(a, b, c, d, e, x, s) \
      (a) += RMD160_I((b), (c), (d)) + (x) + CONST32(0x8f1bbcdc);\
      (a) = ROL32((a), (s)) + (e); \
      (c) = ROL32((c), 10);

#define RMD160_JJ(a, b, c, d, e, x, s) \
      (a) += RMD160_J((b), (c), (d)) + (x) + CONST32(0xa953fd4e);\
      (a) = ROL32((a), (s)) + (e); \
      (c) = ROL32((c), 10);

#define RMD160_FFF(a, b, c, d, e, x, s) \
      (a) += RMD160_F((b), (c), (d)) + (x);\
      (a) = ROL32((a), (s)) + (e); \
      (c) = ROL32((c), 10);

#define RMD160_GGG(a, b, c, d, e, x, s) \
      (a) += RMD160_G((b), (c), (d)) + (x) + CONST32(0x7a6d76e9);\
      (a) = ROL32((a), (s)) + (e); \
      (c) = ROL32((c), 10);

#define RMD160_HHH(a, b, c, d, e, x, s) \
      (a) += RMD160_H((b), (c), (d)) + (x) + CONST32(0x6d703ef3);\
      (a) = ROL32((a), (s)) + (e); \
      (c) = ROL32((c), 10);

#define RMD160_III(a, b, c, d, e, x, s) \
      (a) += RMD160_I((b), (c), (d)) + (x) + CONST32(0x5c4dd124);\
      (a) = ROL32((a), (s)) + (e); \
      (c) = ROL32((c), 10);

#define RMD160_JJJ(a, b, c, d, e, x, s) \
      (a) += RMD160_J((b), (c), (d)) + (x) + CONST32(0x50a28be6);\
      (a) = ROL32((a), (s)) + (e); \
      (c) = ROL32((c), 10);

/////////////////////////////////////////////////////////////////////////////

CRMD128Hash::CRMD128Hash()
{
}

CRMD128Hash::~CRMD128Hash()
{
}

void CRMD128Hash::_Compress()
{
	UWORD32 aa, bb, cc, dd, aaa, bbb, ccc, ddd, X[16];
	INTPREF i;

	for(i = 0; i < 16; i++)
	{
		LOAD32L(X[i], m_buf + (i << 2));
	}

	aa = aaa = m_state[0];
	bb = bbb = m_state[1];
	cc = ccc = m_state[2];
	dd = ddd = m_state[3];

	RMD128_FF(aa, bb, cc, dd, X[ 0], 11);
	RMD128_FF(dd, aa, bb, cc, X[ 1], 14);
	RMD128_FF(cc, dd, aa, bb, X[ 2], 15);
	RMD128_FF(bb, cc, dd, aa, X[ 3], 12);
	RMD128_FF(aa, bb, cc, dd, X[ 4],  5);
	RMD128_FF(dd, aa, bb, cc, X[ 5],  8);
	RMD128_FF(cc, dd, aa, bb, X[ 6],  7);
	RMD128_FF(bb, cc, dd, aa, X[ 7],  9);
	RMD128_FF(aa, bb, cc, dd, X[ 8], 11);
	RMD128_FF(dd, aa, bb, cc, X[ 9], 13);
	RMD128_FF(cc, dd, aa, bb, X[10], 14);
	RMD128_FF(bb, cc, dd, aa, X[11], 15);
	RMD128_FF(aa, bb, cc, dd, X[12],  6);
	RMD128_FF(dd, aa, bb, cc, X[13],  7);
	RMD128_FF(cc, dd, aa, bb, X[14],  9);
	RMD128_FF(bb, cc, dd, aa, X[15],  8);

	RMD128_GG(aa, bb, cc, dd, X[ 7],  7);
	RMD128_GG(dd, aa, bb, cc, X[ 4],  6);
	RMD128_GG(cc, dd, aa, bb, X[13],  8);
	RMD128_GG(bb, cc, dd, aa, X[ 1], 13);
	RMD128_GG(aa, bb, cc, dd, X[10], 11);
	RMD128_GG(dd, aa, bb, cc, X[ 6],  9);
	RMD128_GG(cc, dd, aa, bb, X[15],  7);
	RMD128_GG(bb, cc, dd, aa, X[ 3], 15);
	RMD128_GG(aa, bb, cc, dd, X[12],  7);
	RMD128_GG(dd, aa, bb, cc, X[ 0], 12);
	RMD128_GG(cc, dd, aa, bb, X[ 9], 15);
	RMD128_GG(bb, cc, dd, aa, X[ 5],  9);
	RMD128_GG(aa, bb, cc, dd, X[ 2], 11);
	RMD128_GG(dd, aa, bb, cc, X[14],  7);
	RMD128_GG(cc, dd, aa, bb, X[11], 13);
	RMD128_GG(bb, cc, dd, aa, X[ 8], 12);

	RMD128_HH(aa, bb, cc, dd, X[ 3], 11);
	RMD128_HH(dd, aa, bb, cc, X[10], 13);
	RMD128_HH(cc, dd, aa, bb, X[14],  6);
	RMD128_HH(bb, cc, dd, aa, X[ 4],  7);
	RMD128_HH(aa, bb, cc, dd, X[ 9], 14);
	RMD128_HH(dd, aa, bb, cc, X[15],  9);
	RMD128_HH(cc, dd, aa, bb, X[ 8], 13);
	RMD128_HH(bb, cc, dd, aa, X[ 1], 15);
	RMD128_HH(aa, bb, cc, dd, X[ 2], 14);
	RMD128_HH(dd, aa, bb, cc, X[ 7],  8);
	RMD128_HH(cc, dd, aa, bb, X[ 0], 13);
	RMD128_HH(bb, cc, dd, aa, X[ 6],  6);
	RMD128_HH(aa, bb, cc, dd, X[13],  5);
	RMD128_HH(dd, aa, bb, cc, X[11], 12);
	RMD128_HH(cc, dd, aa, bb, X[ 5],  7);
	RMD128_HH(bb, cc, dd, aa, X[12],  5);

	RMD128_II(aa, bb, cc, dd, X[ 1], 11);
	RMD128_II(dd, aa, bb, cc, X[ 9], 12);
	RMD128_II(cc, dd, aa, bb, X[11], 14);
	RMD128_II(bb, cc, dd, aa, X[10], 15);
	RMD128_II(aa, bb, cc, dd, X[ 0], 14);
	RMD128_II(dd, aa, bb, cc, X[ 8], 15);
	RMD128_II(cc, dd, aa, bb, X[12],  9);
	RMD128_II(bb, cc, dd, aa, X[ 4],  8);
	RMD128_II(aa, bb, cc, dd, X[13],  9);
	RMD128_II(dd, aa, bb, cc, X[ 3], 14);
	RMD128_II(cc, dd, aa, bb, X[ 7],  5);
	RMD128_II(bb, cc, dd, aa, X[15],  6);
	RMD128_II(aa, bb, cc, dd, X[14],  8);
	RMD128_II(dd, aa, bb, cc, X[ 5],  6);
	RMD128_II(cc, dd, aa, bb, X[ 6],  5);
	RMD128_II(bb, cc, dd, aa, X[ 2], 12);

	RMD128_III(aaa, bbb, ccc, ddd, X[ 5],  8);
	RMD128_III(ddd, aaa, bbb, ccc, X[14],  9);
	RMD128_III(ccc, ddd, aaa, bbb, X[ 7],  9);
	RMD128_III(bbb, ccc, ddd, aaa, X[ 0], 11);
	RMD128_III(aaa, bbb, ccc, ddd, X[ 9], 13);
	RMD128_III(ddd, aaa, bbb, ccc, X[ 2], 15);
	RMD128_III(ccc, ddd, aaa, bbb, X[11], 15);
	RMD128_III(bbb, ccc, ddd, aaa, X[ 4],  5);
	RMD128_III(aaa, bbb, ccc, ddd, X[13],  7);
	RMD128_III(ddd, aaa, bbb, ccc, X[ 6],  7);
	RMD128_III(ccc, ddd, aaa, bbb, X[15],  8);
	RMD128_III(bbb, ccc, ddd, aaa, X[ 8], 11);
	RMD128_III(aaa, bbb, ccc, ddd, X[ 1], 14);
	RMD128_III(ddd, aaa, bbb, ccc, X[10], 14);
	RMD128_III(ccc, ddd, aaa, bbb, X[ 3], 12);
	RMD128_III(bbb, ccc, ddd, aaa, X[12],  6);

	RMD128_HHH(aaa, bbb, ccc, ddd, X[ 6],  9);
	RMD128_HHH(ddd, aaa, bbb, ccc, X[11], 13);
	RMD128_HHH(ccc, ddd, aaa, bbb, X[ 3], 15);
	RMD128_HHH(bbb, ccc, ddd, aaa, X[ 7],  7);
	RMD128_HHH(aaa, bbb, ccc, ddd, X[ 0], 12);
	RMD128_HHH(ddd, aaa, bbb, ccc, X[13],  8);
	RMD128_HHH(ccc, ddd, aaa, bbb, X[ 5],  9);
	RMD128_HHH(bbb, ccc, ddd, aaa, X[10], 11);
	RMD128_HHH(aaa, bbb, ccc, ddd, X[14],  7);
	RMD128_HHH(ddd, aaa, bbb, ccc, X[15],  7);
	RMD128_HHH(ccc, ddd, aaa, bbb, X[ 8], 12);
	RMD128_HHH(bbb, ccc, ddd, aaa, X[12],  7);
	RMD128_HHH(aaa, bbb, ccc, ddd, X[ 4],  6);
	RMD128_HHH(ddd, aaa, bbb, ccc, X[ 9], 15);
	RMD128_HHH(ccc, ddd, aaa, bbb, X[ 1], 13);
	RMD128_HHH(bbb, ccc, ddd, aaa, X[ 2], 11);

	RMD128_GGG(aaa, bbb, ccc, ddd, X[15],  9);
	RMD128_GGG(ddd, aaa, bbb, ccc, X[ 5],  7);
	RMD128_GGG(ccc, ddd, aaa, bbb, X[ 1], 15);
	RMD128_GGG(bbb, ccc, ddd, aaa, X[ 3], 11);
	RMD128_GGG(aaa, bbb, ccc, ddd, X[ 7],  8);
	RMD128_GGG(ddd, aaa, bbb, ccc, X[14],  6);
	RMD128_GGG(ccc, ddd, aaa, bbb, X[ 6],  6);
	RMD128_GGG(bbb, ccc, ddd, aaa, X[ 9], 14);
	RMD128_GGG(aaa, bbb, ccc, ddd, X[11], 12);
	RMD128_GGG(ddd, aaa, bbb, ccc, X[ 8], 13);
	RMD128_GGG(ccc, ddd, aaa, bbb, X[12],  5);
	RMD128_GGG(bbb, ccc, ddd, aaa, X[ 2], 14);
	RMD128_GGG(aaa, bbb, ccc, ddd, X[10], 13);
	RMD128_GGG(ddd, aaa, bbb, ccc, X[ 0], 13);
	RMD128_GGG(ccc, ddd, aaa, bbb, X[ 4],  7);
	RMD128_GGG(bbb, ccc, ddd, aaa, X[13],  5);

	RMD128_FFF(aaa, bbb, ccc, ddd, X[ 8], 15);
	RMD128_FFF(ddd, aaa, bbb, ccc, X[ 6],  5);
	RMD128_FFF(ccc, ddd, aaa, bbb, X[ 4],  8);
	RMD128_FFF(bbb, ccc, ddd, aaa, X[ 1], 11);
	RMD128_FFF(aaa, bbb, ccc, ddd, X[ 3], 14);
	RMD128_FFF(ddd, aaa, bbb, ccc, X[11], 14);
	RMD128_FFF(ccc, ddd, aaa, bbb, X[15],  6);
	RMD128_FFF(bbb, ccc, ddd, aaa, X[ 0], 14);
	RMD128_FFF(aaa, bbb, ccc, ddd, X[ 5],  6);
	RMD128_FFF(ddd, aaa, bbb, ccc, X[12],  9);
	RMD128_FFF(ccc, ddd, aaa, bbb, X[ 2], 12);
	RMD128_FFF(bbb, ccc, ddd, aaa, X[13],  9);
	RMD128_FFF(aaa, bbb, ccc, ddd, X[ 9], 12);
	RMD128_FFF(ddd, aaa, bbb, ccc, X[ 7],  5);
	RMD128_FFF(ccc, ddd, aaa, bbb, X[10], 15);
	RMD128_FFF(bbb, ccc, ddd, aaa, X[14],  8);

	ddd += cc + m_state[1];
	m_state[1] = m_state[2] + dd + aaa;
	m_state[2] = m_state[3] + aa + bbb;
	m_state[3] = m_state[0] + bb + ccc;
	m_state[0] = ddd;
}

void CRMD128Hash::Init(RH_DATA_INFO *pInfo)
{
	m_state[0] = CONST32(0x67452301);
	m_state[1] = CONST32(0xefcdab89);
	m_state[2] = CONST32(0x98badcfe);
	m_state[3] = CONST32(0x10325476);
	m_curlen = 0;
	m_length = 0;
}

void CRMD128Hash::Update(const UWORD8 *pBuf, UINTPREF uLen)
{
	UINTPREF n;

	while(uLen > 0)
	{
		n = min(uLen, (64 - m_curlen));
		memcpy(m_buf + m_curlen, pBuf, n);
		m_curlen += n;
		pBuf += n;
		uLen -= n;

		if(m_curlen == 64)
		{
			_Compress();
			m_length += 512;
			m_curlen = 0;
		}
	}
}

void CRMD128Hash::Final()
{
	INTPREF i;

	m_length += m_curlen * 8;

	m_buf[m_curlen++] = 0x80;

	if(m_curlen > 56)
	{
		while(m_curlen < 64)
		{
			m_buf[m_curlen++] = 0;
		}

		_Compress();
		m_curlen = 0;
	}

	while(m_curlen < 56)
	{
		m_buf[m_curlen++] = 0;
	}

	STORE64L(m_length, m_buf + 56);
	_Compress();

	for(i = 0; i < 4; i++)
	{
		STORE32L(m_state[i], m_final + (i << 2));
	}
}

/////////////////////////////////////////////////////////////////////////////

CRMD160Hash::CRMD160Hash()
{
}

CRMD160Hash::~CRMD160Hash()
{
}

void CRMD160Hash::_Compress()
{
	UWORD32 aa, bb, cc, dd, ee, aaa, bbb, ccc, ddd, eee, X[16];
	INTPREF i;

	for(i = 0; i < 16; i++)
	{
		LOAD32L(X[i], m_buf + (i << 2));
	}

	aa = aaa = m_state[0];
	bb = bbb = m_state[1];
	cc = ccc = m_state[2];
	dd = ddd = m_state[3];
	ee = eee = m_state[4];

	RMD160_FF(aa, bb, cc, dd, ee, X[ 0], 11);
	RMD160_FF(ee, aa, bb, cc, dd, X[ 1], 14);
	RMD160_FF(dd, ee, aa, bb, cc, X[ 2], 15);
	RMD160_FF(cc, dd, ee, aa, bb, X[ 3], 12);
	RMD160_FF(bb, cc, dd, ee, aa, X[ 4],  5);
	RMD160_FF(aa, bb, cc, dd, ee, X[ 5],  8);
	RMD160_FF(ee, aa, bb, cc, dd, X[ 6],  7);
	RMD160_FF(dd, ee, aa, bb, cc, X[ 7],  9);
	RMD160_FF(cc, dd, ee, aa, bb, X[ 8], 11);
	RMD160_FF(bb, cc, dd, ee, aa, X[ 9], 13);
	RMD160_FF(aa, bb, cc, dd, ee, X[10], 14);
	RMD160_FF(ee, aa, bb, cc, dd, X[11], 15);
	RMD160_FF(dd, ee, aa, bb, cc, X[12],  6);
	RMD160_FF(cc, dd, ee, aa, bb, X[13],  7);
	RMD160_FF(bb, cc, dd, ee, aa, X[14],  9);
	RMD160_FF(aa, bb, cc, dd, ee, X[15],  8);

	RMD160_GG(ee, aa, bb, cc, dd, X[ 7],  7);
	RMD160_GG(dd, ee, aa, bb, cc, X[ 4],  6);
	RMD160_GG(cc, dd, ee, aa, bb, X[13],  8);
	RMD160_GG(bb, cc, dd, ee, aa, X[ 1], 13);
	RMD160_GG(aa, bb, cc, dd, ee, X[10], 11);
	RMD160_GG(ee, aa, bb, cc, dd, X[ 6],  9);
	RMD160_GG(dd, ee, aa, bb, cc, X[15],  7);
	RMD160_GG(cc, dd, ee, aa, bb, X[ 3], 15);
	RMD160_GG(bb, cc, dd, ee, aa, X[12],  7);
	RMD160_GG(aa, bb, cc, dd, ee, X[ 0], 12);
	RMD160_GG(ee, aa, bb, cc, dd, X[ 9], 15);
	RMD160_GG(dd, ee, aa, bb, cc, X[ 5],  9);
	RMD160_GG(cc, dd, ee, aa, bb, X[ 2], 11);
	RMD160_GG(bb, cc, dd, ee, aa, X[14],  7);
	RMD160_GG(aa, bb, cc, dd, ee, X[11], 13);
	RMD160_GG(ee, aa, bb, cc, dd, X[ 8], 12);

	RMD160_HH(dd, ee, aa, bb, cc, X[ 3], 11);
	RMD160_HH(cc, dd, ee, aa, bb, X[10], 13);
	RMD160_HH(bb, cc, dd, ee, aa, X[14],  6);
	RMD160_HH(aa, bb, cc, dd, ee, X[ 4],  7);
	RMD160_HH(ee, aa, bb, cc, dd, X[ 9], 14);
	RMD160_HH(dd, ee, aa, bb, cc, X[15],  9);
	RMD160_HH(cc, dd, ee, aa, bb, X[ 8], 13);
	RMD160_HH(bb, cc, dd, ee, aa, X[ 1], 15);
	RMD160_HH(aa, bb, cc, dd, ee, X[ 2], 14);
	RMD160_HH(ee, aa, bb, cc, dd, X[ 7],  8);
	RMD160_HH(dd, ee, aa, bb, cc, X[ 0], 13);
	RMD160_HH(cc, dd, ee, aa, bb, X[ 6],  6);
	RMD160_HH(bb, cc, dd, ee, aa, X[13],  5);
	RMD160_HH(aa, bb, cc, dd, ee, X[11], 12);
	RMD160_HH(ee, aa, bb, cc, dd, X[ 5],  7);
	RMD160_HH(dd, ee, aa, bb, cc, X[12],  5);

	RMD160_II(cc, dd, ee, aa, bb, X[ 1], 11);
	RMD160_II(bb, cc, dd, ee, aa, X[ 9], 12);
	RMD160_II(aa, bb, cc, dd, ee, X[11], 14);
	RMD160_II(ee, aa, bb, cc, dd, X[10], 15);
	RMD160_II(dd, ee, aa, bb, cc, X[ 0], 14);
	RMD160_II(cc, dd, ee, aa, bb, X[ 8], 15);
	RMD160_II(bb, cc, dd, ee, aa, X[12],  9);
	RMD160_II(aa, bb, cc, dd, ee, X[ 4],  8);
	RMD160_II(ee, aa, bb, cc, dd, X[13],  9);
	RMD160_II(dd, ee, aa, bb, cc, X[ 3], 14);
	RMD160_II(cc, dd, ee, aa, bb, X[ 7],  5);
	RMD160_II(bb, cc, dd, ee, aa, X[15],  6);
	RMD160_II(aa, bb, cc, dd, ee, X[14],  8);
	RMD160_II(ee, aa, bb, cc, dd, X[ 5],  6);
	RMD160_II(dd, ee, aa, bb, cc, X[ 6],  5);
	RMD160_II(cc, dd, ee, aa, bb, X[ 2], 12);

	RMD160_JJ(bb, cc, dd, ee, aa, X[ 4],  9);
	RMD160_JJ(aa, bb, cc, dd, ee, X[ 0], 15);
	RMD160_JJ(ee, aa, bb, cc, dd, X[ 5],  5);
	RMD160_JJ(dd, ee, aa, bb, cc, X[ 9], 11);
	RMD160_JJ(cc, dd, ee, aa, bb, X[ 7],  6);
	RMD160_JJ(bb, cc, dd, ee, aa, X[12],  8);
	RMD160_JJ(aa, bb, cc, dd, ee, X[ 2], 13);
	RMD160_JJ(ee, aa, bb, cc, dd, X[10], 12);
	RMD160_JJ(dd, ee, aa, bb, cc, X[14],  5);
	RMD160_JJ(cc, dd, ee, aa, bb, X[ 1], 12);
	RMD160_JJ(bb, cc, dd, ee, aa, X[ 3], 13);
	RMD160_JJ(aa, bb, cc, dd, ee, X[ 8], 14);
	RMD160_JJ(ee, aa, bb, cc, dd, X[11], 11);
	RMD160_JJ(dd, ee, aa, bb, cc, X[ 6],  8);
	RMD160_JJ(cc, dd, ee, aa, bb, X[15],  5);
	RMD160_JJ(bb, cc, dd, ee, aa, X[13],  6);

	RMD160_JJJ(aaa, bbb, ccc, ddd, eee, X[ 5],  8);
	RMD160_JJJ(eee, aaa, bbb, ccc, ddd, X[14],  9);
	RMD160_JJJ(ddd, eee, aaa, bbb, ccc, X[ 7],  9);
	RMD160_JJJ(ccc, ddd, eee, aaa, bbb, X[ 0], 11);
	RMD160_JJJ(bbb, ccc, ddd, eee, aaa, X[ 9], 13);
	RMD160_JJJ(aaa, bbb, ccc, ddd, eee, X[ 2], 15);
	RMD160_JJJ(eee, aaa, bbb, ccc, ddd, X[11], 15);
	RMD160_JJJ(ddd, eee, aaa, bbb, ccc, X[ 4],  5);
	RMD160_JJJ(ccc, ddd, eee, aaa, bbb, X[13],  7);
	RMD160_JJJ(bbb, ccc, ddd, eee, aaa, X[ 6],  7);
	RMD160_JJJ(aaa, bbb, ccc, ddd, eee, X[15],  8);
	RMD160_JJJ(eee, aaa, bbb, ccc, ddd, X[ 8], 11);
	RMD160_JJJ(ddd, eee, aaa, bbb, ccc, X[ 1], 14);
	RMD160_JJJ(ccc, ddd, eee, aaa, bbb, X[10], 14);
	RMD160_JJJ(bbb, ccc, ddd, eee, aaa, X[ 3], 12);
	RMD160_JJJ(aaa, bbb, ccc, ddd, eee, X[12],  6);

	RMD160_III(eee, aaa, bbb, ccc, ddd, X[ 6],  9);
	RMD160_III(ddd, eee, aaa, bbb, ccc, X[11], 13);
	RMD160_III(ccc, ddd, eee, aaa, bbb, X[ 3], 15);
	RMD160_III(bbb, ccc, ddd, eee, aaa, X[ 7],  7);
	RMD160_III(aaa, bbb, ccc, ddd, eee, X[ 0], 12);
	RMD160_III(eee, aaa, bbb, ccc, ddd, X[13],  8);
	RMD160_III(ddd, eee, aaa, bbb, ccc, X[ 5],  9);
	RMD160_III(ccc, ddd, eee, aaa, bbb, X[10], 11);
	RMD160_III(bbb, ccc, ddd, eee, aaa, X[14],  7);
	RMD160_III(aaa, bbb, ccc, ddd, eee, X[15],  7);
	RMD160_III(eee, aaa, bbb, ccc, ddd, X[ 8], 12);
	RMD160_III(ddd, eee, aaa, bbb, ccc, X[12],  7);
	RMD160_III(ccc, ddd, eee, aaa, bbb, X[ 4],  6);
	RMD160_III(bbb, ccc, ddd, eee, aaa, X[ 9], 15);
	RMD160_III(aaa, bbb, ccc, ddd, eee, X[ 1], 13);
	RMD160_III(eee, aaa, bbb, ccc, ddd, X[ 2], 11);

	RMD160_HHH(ddd, eee, aaa, bbb, ccc, X[15],  9);
	RMD160_HHH(ccc, ddd, eee, aaa, bbb, X[ 5],  7);
	RMD160_HHH(bbb, ccc, ddd, eee, aaa, X[ 1], 15);
	RMD160_HHH(aaa, bbb, ccc, ddd, eee, X[ 3], 11);
	RMD160_HHH(eee, aaa, bbb, ccc, ddd, X[ 7],  8);
	RMD160_HHH(ddd, eee, aaa, bbb, ccc, X[14],  6);
	RMD160_HHH(ccc, ddd, eee, aaa, bbb, X[ 6],  6);
	RMD160_HHH(bbb, ccc, ddd, eee, aaa, X[ 9], 14);
	RMD160_HHH(aaa, bbb, ccc, ddd, eee, X[11], 12);
	RMD160_HHH(eee, aaa, bbb, ccc, ddd, X[ 8], 13);
	RMD160_HHH(ddd, eee, aaa, bbb, ccc, X[12],  5);
	RMD160_HHH(ccc, ddd, eee, aaa, bbb, X[ 2], 14);
	RMD160_HHH(bbb, ccc, ddd, eee, aaa, X[10], 13);
	RMD160_HHH(aaa, bbb, ccc, ddd, eee, X[ 0], 13);
	RMD160_HHH(eee, aaa, bbb, ccc, ddd, X[ 4],  7);
	RMD160_HHH(ddd, eee, aaa, bbb, ccc, X[13],  5);

	RMD160_GGG(ccc, ddd, eee, aaa, bbb, X[ 8], 15);
	RMD160_GGG(bbb, ccc, ddd, eee, aaa, X[ 6],  5);
	RMD160_GGG(aaa, bbb, ccc, ddd, eee, X[ 4],  8);
	RMD160_GGG(eee, aaa, bbb, ccc, ddd, X[ 1], 11);
	RMD160_GGG(ddd, eee, aaa, bbb, ccc, X[ 3], 14);
	RMD160_GGG(ccc, ddd, eee, aaa, bbb, X[11], 14);
	RMD160_GGG(bbb, ccc, ddd, eee, aaa, X[15],  6);
	RMD160_GGG(aaa, bbb, ccc, ddd, eee, X[ 0], 14);
	RMD160_GGG(eee, aaa, bbb, ccc, ddd, X[ 5],  6);
	RMD160_GGG(ddd, eee, aaa, bbb, ccc, X[12],  9);
	RMD160_GGG(ccc, ddd, eee, aaa, bbb, X[ 2], 12);
	RMD160_GGG(bbb, ccc, ddd, eee, aaa, X[13],  9);
	RMD160_GGG(aaa, bbb, ccc, ddd, eee, X[ 9], 12);
	RMD160_GGG(eee, aaa, bbb, ccc, ddd, X[ 7],  5);
	RMD160_GGG(ddd, eee, aaa, bbb, ccc, X[10], 15);
	RMD160_GGG(ccc, ddd, eee, aaa, bbb, X[14],  8);

	RMD160_FFF(bbb, ccc, ddd, eee, aaa, X[12],  8);
	RMD160_FFF(aaa, bbb, ccc, ddd, eee, X[15],  5);
	RMD160_FFF(eee, aaa, bbb, ccc, ddd, X[10], 12);
	RMD160_FFF(ddd, eee, aaa, bbb, ccc, X[ 4],  9);
	RMD160_FFF(ccc, ddd, eee, aaa, bbb, X[ 1], 12);
	RMD160_FFF(bbb, ccc, ddd, eee, aaa, X[ 5],  5);
	RMD160_FFF(aaa, bbb, ccc, ddd, eee, X[ 8], 14);
	RMD160_FFF(eee, aaa, bbb, ccc, ddd, X[ 7],  6);
	RMD160_FFF(ddd, eee, aaa, bbb, ccc, X[ 6],  8);
	RMD160_FFF(ccc, ddd, eee, aaa, bbb, X[ 2], 13);
	RMD160_FFF(bbb, ccc, ddd, eee, aaa, X[13],  6);
	RMD160_FFF(aaa, bbb, ccc, ddd, eee, X[14],  5);
	RMD160_FFF(eee, aaa, bbb, ccc, ddd, X[ 0], 15);
	RMD160_FFF(ddd, eee, aaa, bbb, ccc, X[ 3], 13);
	RMD160_FFF(ccc, ddd, eee, aaa, bbb, X[ 9], 11);
	RMD160_FFF(bbb, ccc, ddd, eee, aaa, X[11], 11);

	ddd += cc + m_state[1];
	m_state[1] = m_state[2] + dd + eee;
	m_state[2] = m_state[3] + ee + aaa;
	m_state[3] = m_state[4] + aa + bbb;
	m_state[4] = m_state[0] + bb + ccc;
	m_state[0] = ddd;
}

void CRMD160Hash::Init(RH_DATA_INFO *pInfo)
{
	m_state[0] = 0x67452301UL;
	m_state[1] = 0xefcdab89UL;
	m_state[2] = 0x98badcfeUL;
	m_state[3] = 0x10325476UL;
	m_state[4] = 0xc3d2e1f0UL;
	m_curlen   = 0;
	m_length   = 0;
}

void CRMD160Hash::Update(const UWORD8 *pBuf, UINTPREF uLen)
{
	UINTPREF n;

	while(uLen > 0)
	{
		n = min(uLen, (64 - m_curlen));
		memcpy(m_buf + m_curlen, pBuf, n);
		m_curlen += n;
		pBuf += n;
		uLen -= n;

		if(m_curlen == 64)
		{
			_Compress();
			m_length += 512;
			m_curlen = 0;
		}
	}
}

void CRMD160Hash::Final()
{
	INTPREF i;

	m_length += m_curlen * 8;

	m_buf[m_curlen++] = 0x80;

	if(m_curlen > 56)
	{
		while(m_curlen < 64)
		{
			m_buf[m_curlen++] = 0;
		}

		_Compress();
		m_curlen = 0;
	}

	while(m_curlen < 56)
	{
		m_buf[m_curlen++] = 0;
	}

	STORE64L(m_length, m_buf+56);
	_Compress();

	for(i = 0; i < 5; i++)
	{
		STORE32L(m_state[i], m_final + (i << 2));
	}
}
