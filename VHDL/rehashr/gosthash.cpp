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

#include "gosthash.h"

static bool g_bTablesInitialized = false;

static UWORD32 g_pSBox1[256];
static UWORD32 g_pSBox2[256];
static UWORD32 g_pSBox3[256];
static UWORD32 g_pSBox4[256];

#define GOST_ENCRYPT_ROUND(k1, k2) \
	t = (k1) + r; \
	l ^= g_pSBox1[t & 0xFF] ^ g_pSBox2[(t >> 8) & 0xFF] ^ \
	g_pSBox3[(t >> 16) & 0xFF] ^ g_pSBox4[t >> 24]; \
	t = (k2) + l; \
	r ^= g_pSBox1[t & 0xFF] ^ g_pSBox2[(t >> 8) & 0xFF] ^ \
	g_pSBox3[(t >> 16) & 0xFF] ^ g_pSBox4[t >> 24];

#define GOST_ENCRYPT(k0) \
	GOST_ENCRYPT_ROUND(k0[0], k0[1]) \
	GOST_ENCRYPT_ROUND(k0[2], k0[3]) \
	GOST_ENCRYPT_ROUND(k0[4], k0[5]) \
	GOST_ENCRYPT_ROUND(k0[6], k0[7]) \
	GOST_ENCRYPT_ROUND(k0[0], k0[1]) \
	GOST_ENCRYPT_ROUND(k0[2], k0[3]) \
	GOST_ENCRYPT_ROUND(k0[4], k0[5]) \
	GOST_ENCRYPT_ROUND(k0[6], k0[7]) \
	GOST_ENCRYPT_ROUND(k0[0], k0[1]) \
	GOST_ENCRYPT_ROUND(k0[2], k0[3]) \
	GOST_ENCRYPT_ROUND(k0[4], k0[5]) \
	GOST_ENCRYPT_ROUND(k0[6], k0[7]) \
	GOST_ENCRYPT_ROUND(k0[7], k0[6]) \
	GOST_ENCRYPT_ROUND(k0[5], k0[4]) \
	GOST_ENCRYPT_ROUND(k0[3], k0[2]) \
	GOST_ENCRYPT_ROUND(k0[1], k0[0]) \
	t = r; \
	r = l; \
	l = t;

void _gostMakeTables()
{
	INTPREF a, b, i;
	UWORD32 ax, bx, cx, dx;

	const UWORD32 uStdSBox[8][16] = // 4-bit SBox
	{
		{  4, 10,  9,  2, 13,  8,  0, 14,  6, 11,  1, 12,  7, 15,  5,  3 },
		{ 14, 11,  4, 12,  6, 13, 15, 10,  2,  3,  8,  1,  0,  7,  5,  9 },
		{  5,  8,  1, 13, 10,  3,  4,  2, 14, 15, 12,  7,  6,  0,  9, 11 },
		{  7, 13, 10,  1,  0,  8,  9, 15, 14,  4,  6, 12, 11,  2,  5,  3 },
		{  6, 12,  7,  1,  5, 15, 13,  8,  4, 10,  9, 14,  0,  3, 11,  2 },
		{  4, 11, 10,  0,  7,  2,  1, 13,  3,  6,  8,  5,  9, 12, 15, 14 },
		{ 13, 11,  4,  1,  3, 15,  5,  9,  0, 10, 14,  7,  6,  8,  2, 12 },
		{  1, 15, 13,  0,  5,  7, 10,  4,  9,  2,  3, 14,  6, 11,  8, 12 }
	};

	i = 0;
	for(a = 0; a < 16; a++)
	{
		ax = uStdSBox[1][a] << 15;
		bx = uStdSBox[3][a] << 23;
		cx = uStdSBox[5][a];
		cx = (cx >> 1) | (cx << 31);
		dx = uStdSBox[7][a] << 7;

		for(b = 0; b < 16; b++)
		{
			g_pSBox1[i  ] = ax | (uStdSBox[0][b] << 11);
			g_pSBox2[i  ] = bx | (uStdSBox[2][b] << 19);
			g_pSBox3[i  ] = cx | (uStdSBox[4][b] << 27);
			g_pSBox4[i++] = dx | (uStdSBox[6][b] <<  3);
		}
	}
}

CGOSTHash::CGOSTHash()
{
}

CGOSTHash::~CGOSTHash()
{
}

void CGOSTHash::Init(RH_DATA_INFO *pInfo)
{
	if(g_bTablesInitialized == false)
	{
		_gostMakeTables();
		g_bTablesInitialized = true;
	}

	memset(m_sum, 0, 32);
	memset(m_hash, 0, 32);
	memset(m_len, 0, 32);
	memset(m_partial, 0, 32);
	m_partial_bytes = 0;
}

void CGOSTHash::_Compress(UWORD32 *h, UWORD32 *m)
{
	INTPREF i;
	UWORD32 l, r, t, k0[8], u[8], v[8], w[8], s[8];

	memcpy(u, h, 32);
	memcpy(v, m, 32);

	for(i = 0; i < 8; i += 2)
	{
		w[0] = u[0] ^ v[0]; // w = u XOR v
		w[1] = u[1] ^ v[1];
		w[2] = u[2] ^ v[2];
		w[3] = u[3] ^ v[3];
		w[4] = u[4] ^ v[4];
		w[5] = u[5] ^ v[5];
		w[6] = u[6] ^ v[6];
		w[7] = u[7] ^ v[7];

		// P transformation

		k0[0] = (w[0]  & 0x000000FF) | ((w[2] & 0x000000FF) << 8) |
			((w[4] & 0x000000FF) << 16) | ((w[6] & 0x000000FF) << 24);
		k0[1] = ((w[0] & 0x0000FF00) >> 8)  | (w[2]  & 0x0000FF00) |
			((w[4] & 0x0000FF00) << 8) | ((w[6] & 0x0000FF00) << 16);
		k0[2] = ((w[0] & 0x00FF0000) >> 16) | ((w[2] & 0x00FF0000) >> 8) |
			(w[4] & 0x00FF0000) | ((w[6] & 0x00FF0000) << 8);
		k0[3] = ((w[0] & 0xFF000000) >> 24) | ((w[2] & 0xFF000000) >> 16) |
			((w[4] & 0xFF000000) >> 8) | (w[6] & 0xFF000000);
		k0[4] = (w[1] & 0x000000FF) | ((w[3] & 0x000000FF) << 8) |
			((w[5] & 0x000000FF) << 16) | ((w[7] & 0x000000FF) << 24);
		k0[5] = ((w[1] & 0x0000FF00) >> 8) | (w[3]  & 0x0000FF00) |
			((w[5] & 0x0000FF00) << 8) | ((w[7] & 0x0000FF00) << 16);
		k0[6] = ((w[1] & 0x00FF0000) >> 16) | ((w[3] & 0x00FF0000) >> 8) |
			(w[5] & 0x00FF0000) | ((w[7] & 0x00FF0000) << 8);
		k0[7] = ((w[1] & 0xFF000000) >> 24) | ((w[3] & 0xFF000000) >> 16) |
			((w[5] & 0xFF000000) >> 8) | (w[7] & 0xFF000000);

		r = h[i];
		l = h[i + 1];
		GOST_ENCRYPT(k0);

		s[i] = r;
		s[i + 1] = l;

		if(i == 6) break;

		l = u[0] ^ u[2]; // U = A(U)
		r = u[1] ^ u[3];
		u[0] = u[2];
		u[1] = u[3];
		u[2] = u[4];
		u[3] = u[5];
		u[4] = u[6];
		u[5] = u[7];
		u[6] = l;
		u[7] = r;

		if(i == 2) // Constant C_3
		{
			u[0] ^= 0xFF00FF00;
			u[1] ^= 0xFF00FF00;
			u[2] ^= 0x00FF00FF;
			u[3] ^= 0x00FF00FF;
			u[4] ^= 0x00FFFF00;
			u[5] ^= 0xFF0000FF;
			u[6] ^= 0x000000FF;
			u[7] ^= 0xFF00FFFF;
		}

		l = v[0]; // V = A(A(V))
		r = v[2];
		v[0] = v[4];
		v[2] = v[6];
		v[4] = l ^ r;
		v[6] = v[0] ^ r;
		l = v[1];
		r = v[3];
		v[1] = v[5];
		v[3] = v[7];
		v[5] = l ^ r;
		v[7] = v[1] ^ r;
	}

	// 12 rounds of the LFSR (computed from a product matrix) and XOR in M
	u[0] = m[0] ^ s[6];
	u[1] = m[1] ^ s[7];
	u[2] = m[2] ^ (s[0] << 16) ^ (s[0] >> 16) ^ (s[0] & 0xFFFF) ^
		(s[1] & 0xFFFF) ^ (s[1] >> 16) ^ (s[2] << 16) ^ s[6] ^ (s[6] << 16) ^
		(s[7] & 0xFFFF0000) ^ (s[7] >> 16);
	u[3] = m[3] ^ (s[0] & 0xFFFF) ^ (s[0] << 16) ^ (s[1] & 0xFFFF) ^
		(s[1] << 16) ^ (s[1] >> 16) ^ (s[2] << 16) ^ (s[2] >> 16) ^
		(s[3] << 16) ^ s[6] ^ (s[6] << 16) ^ (s[6] >> 16) ^ (s[7] & 0xFFFF) ^
		(s[7] << 16) ^ (s[7] >> 16);
	u[4] = m[4] ^
		(s[0] & 0xFFFF0000) ^ (s[0] << 16) ^ (s[0] >> 16) ^
		(s[1] & 0xFFFF0000) ^ (s[1] >> 16) ^ (s[2] << 16) ^ (s[2] >> 16) ^
		(s[3] << 16) ^ (s[3] >> 16) ^ (s[4] << 16) ^ (s[6] << 16) ^
		(s[6] >> 16) ^(s[7] & 0xFFFF) ^ (s[7] << 16) ^ (s[7] >> 16);
	u[5] = m[5] ^ (s[0] << 16) ^ (s[0] >> 16) ^ (s[0] & 0xFFFF0000) ^
		(s[1] & 0xFFFF) ^ s[2] ^ (s[2] >> 16) ^ (s[3] << 16) ^ (s[3] >> 16) ^
		(s[4] << 16) ^ (s[4] >> 16) ^ (s[5] << 16) ^  (s[6] << 16) ^
		(s[6] >> 16) ^ (s[7] & 0xFFFF0000) ^ (s[7] << 16) ^ (s[7] >> 16);
	u[6] = m[6] ^ s[0] ^ (s[1] >> 16) ^ (s[2] << 16) ^ s[3] ^ (s[3] >> 16) ^
		(s[4] << 16) ^ (s[4] >> 16) ^ (s[5] << 16) ^ (s[5] >> 16) ^ s[6] ^
		(s[6] << 16) ^ (s[6] >> 16) ^ (s[7] << 16);
	u[7] = m[7] ^ (s[0] & 0xFFFF0000) ^ (s[0] << 16) ^ (s[1] & 0xFFFF) ^
		(s[1] << 16) ^ (s[2] >> 16) ^ (s[3] << 16) ^ s[4] ^ (s[4] >> 16) ^
		(s[5] << 16) ^ (s[5] >> 16) ^ (s[6] >> 16) ^ (s[7] & 0xFFFF) ^
		(s[7] << 16) ^ (s[7] >> 16);

	// 16 * 1 round of the LFSR and XOR in H
	v[0] = h[0] ^ (u[1] << 16) ^ (u[0] >> 16);
	v[1] = h[1] ^ (u[2] << 16) ^ (u[1] >> 16);
	v[2] = h[2] ^ (u[3] << 16) ^ (u[2] >> 16);
	v[3] = h[3] ^ (u[4] << 16) ^ (u[3] >> 16);
	v[4] = h[4] ^ (u[5] << 16) ^ (u[4] >> 16);
	v[5] = h[5] ^ (u[6] << 16) ^ (u[5] >> 16);
	v[6] = h[6] ^ (u[7] << 16) ^ (u[6] >> 16);
	v[7] = h[7] ^ (u[0] & 0xFFFF0000) ^ (u[0] << 16) ^ (u[7] >> 16) ^
		(u[1] & 0xFFFF0000) ^ (u[1] << 16) ^ (u[6] << 16) ^ (u[7] & 0xFFFF0000);

	// 61 rounds of LFSR, mixing up h (computed from a product matrix)
	h[0] = (v[0] & 0xFFFF0000) ^ (v[0] << 16) ^ (v[0] >> 16) ^ (v[1] >> 16) ^
		(v[1] & 0xFFFF0000) ^ (v[2] << 16) ^ (v[3] >> 16) ^ (v[4] << 16) ^
		(v[5] >> 16) ^ v[5] ^ (v[6] >> 16) ^ (v[7] << 16) ^ (v[7] >> 16) ^
		(v[7] & 0xFFFF);
	h[1] = (v[0] << 16) ^ (v[0] >> 16) ^ (v[0] & 0xFFFF0000) ^ (v[1] & 0xFFFF) ^
		v[2] ^ (v[2] >> 16) ^ (v[3] << 16) ^ (v[4] >> 16) ^ (v[5] << 16) ^
		(v[6] << 16) ^ v[6] ^ (v[7] & 0xFFFF0000) ^ (v[7] >> 16);
	h[2] = (v[0] & 0xFFFF) ^ (v[0] << 16) ^ (v[1] << 16) ^ (v[1] >> 16) ^
		(v[1] & 0xFFFF0000) ^ (v[2] << 16) ^ (v[3] >> 16) ^ v[3] ^ (v[4] << 16) ^
		(v[5] >> 16) ^ v[6] ^ (v[6] >> 16) ^ (v[7] & 0xFFFF) ^ (v[7] << 16) ^
		(v[7] >> 16);
	h[3] = (v[0] << 16) ^ (v[0] >> 16) ^ (v[0] & 0xFFFF0000) ^
		(v[1] & 0xFFFF0000) ^ (v[1] >> 16) ^ (v[2] << 16) ^ (v[2] >> 16) ^ v[2] ^
		(v[3] << 16) ^ (v[4] >> 16) ^ v[4] ^ (v[5] << 16) ^ (v[6] << 16) ^
		(v[7] & 0xFFFF) ^ (v[7] >> 16);
	h[4] = (v[0] >> 16) ^ (v[1] << 16) ^ v[1] ^ (v[2] >> 16) ^ v[2] ^
		(v[3] << 16) ^ (v[3] >> 16) ^ v[3] ^ (v[4] << 16) ^ (v[5] >> 16) ^
		v[5] ^ (v[6] << 16) ^ (v[6] >> 16) ^ (v[7] << 16);
	h[5] = (v[0] << 16) ^ (v[0] & 0xFFFF0000) ^ (v[1] << 16) ^ (v[1] >> 16) ^
		(v[1] & 0xFFFF0000) ^ (v[2] << 16) ^ v[2] ^ (v[3] >> 16) ^ v[3] ^
		(v[4] << 16) ^ (v[4] >> 16) ^ v[4] ^ (v[5] << 16) ^ (v[6] << 16) ^
		(v[6] >> 16) ^ v[6] ^ (v[7] << 16) ^ (v[7] >> 16) ^ (v[7] & 0xFFFF0000);
	h[6] = v[0] ^ v[2] ^ (v[2] >> 16) ^ v[3] ^ (v[3] << 16) ^ v[4] ^
		(v[4] >> 16) ^ (v[5] << 16) ^ (v[5] >> 16) ^ v[5] ^ (v[6] << 16) ^
		(v[6] >> 16) ^ v[6] ^ (v[7] << 16) ^ v[7];
	h[7] = v[0] ^ (v[0] >> 16) ^ (v[1] << 16) ^ (v[1] >> 16) ^ (v[2] << 16) ^
		(v[3] >> 16) ^ v[3] ^ (v[4] << 16) ^ v[4] ^ (v[5] >> 16) ^ v[5] ^
		(v[6] << 16) ^ (v[6] >> 16) ^ (v[7] << 16) ^ v[7];
}

void CGOSTHash::_Bytes(const UWORD8 *pBuf, UWORD32 uBits)
{
	INTPREF i, j = 0;
	UWORD32 a, c = 0, m[8];

	// Convert bytes to 32-bit words and compute the sum
	for(i = 0; i < 8; i++)
	{
		a = ((UWORD32)pBuf[j]) |
			(((UWORD32)pBuf[j + 1]) << 8) |
			(((UWORD32)pBuf[j + 2]) << 16) |
			(((UWORD32)pBuf[j + 3]) << 24);

		j += 4;
		m[i] = a;

		// Bugfix July 23, 2002 mjos@iki.fi. Thanks to Kaluzhinsky Anatoly.
		if(c)
		{
			c = a + m_sum[i] + 1;
			m_sum[i] = c;
			c = c <= a;
		}
		else
		{
			c = a + m_sum[i];
			m_sum[i] = c;
			c = c < a;
		}
	}

	_Compress(m_hash, m);

	// A 64-bit counter should be sufficient
	m_len[0] += uBits;
	if(m_len[0] < uBits) // Overflowed
		m_len[1]++;
}

void CGOSTHash::Update(const UWORD8 *pBuf, UINTPREF uLen)
{
	UINTPREF i, j = 0;

	i = m_partial_bytes;

	while((i < 32) && (j < uLen))
		m_partial[i++] = pBuf[j++];

	if(i < 32)
	{
		m_partial_bytes = i;
		return;
	}
	_Bytes(m_partial, 256);

	while((j + 32) < uLen)
	{
		_Bytes(&pBuf[j], 256);
		j += 32;
	}

	i = 0;
	while(j < uLen)
		m_partial[i++] = pBuf[j++];

	m_partial_bytes = i;
}

void CGOSTHash::Final()
{
	INTPREF i, j = 0;
	UWORD32 a;

	if(m_partial_bytes > 0)
	{
		memset(&m_partial[m_partial_bytes], 0, 32 - m_partial_bytes);
		_Bytes(m_partial, m_partial_bytes << 3);
	}

	_Compress(m_hash, m_len);
	_Compress(m_hash, m_sum);

	for(i = 0; i < 8; i++)
	{
		a = m_hash[i];
		m_digest[j] = (UWORD8) a;
		m_digest[j + 1] = (UWORD8)(a >> 8);
		m_digest[j + 2] = (UWORD8)(a >> 16);
		m_digest[j + 3] = (UWORD8)(a >> 24);
		j += 4;
	}
}
