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

// This free implementation is based on Dr Brian Gladmans free implementation.
// The source has been heavily modified to integrate to ReHash.

// Here's the original header:

/*
 ---------------------------------------------------------------------------
 Copyright (c) 2002, Dr Brian Gladman <brg@gladman.me.uk>, Worcester, UK.
 All rights reserved.

 LICENSE TERMS

 The free distribution and use of this software in both source and binary
 form is allowed (with or without changes) provided that:

   1. distributions of this source code include the above copyright
      notice, this list of conditions and the following disclaimer;

   2. distributions in binary form include the above copyright
      notice, this list of conditions and the following disclaimer
      in the documentation and/or other associated materials;

   3. the copyright holder's name is not used to endorse products
      built using this software without specific written permission.

 ALTERNATIVELY, provided that this notice is retained in full, this product
 may be distributed under the terms of the GNU General Public License (GPL),
 in which case the provisions of the GPL apply INSTEAD OF those given above.

 DISCLAIMER

 This software is provided 'as is' with no explicit or implied warranties
 in respect of its properties, including, but not limited to, correctness
 and/or fitness for purpose.
 ---------------------------------------------------------------------------
 Issue Date: 26/08/2003

 This is a byte oriented version of SHA2 that operates on arrays of bytes
 stored in memory. This code implements sha256, sha384 and sha512 but the
 latter two functions rely on efficient 64-bit integer operations that
 may not be very efficient on 32-bit machines

 My thanks to Erik Andersen <andersen@codepoet.org> for testing this code
 on big-endian systems and for his assistance with corrections
*/

#include "sha2.h"

#if !defined(BSWAP_32)
#define BSWAP_32(x) (ROR32((x), 24) & 0x00FF00FF | ROR32((x), 8) & 0xFF00FF00)
#endif
#if !defined(BSWAP_64)
#define BSWAP_64(x) (((UWORD64)(BSWAP_32((UWORD32)(x)))) << 32 | BSWAP_32((UWORD32)((x) >> 32)))
#endif

#define SHA256_MASK (SHA256_BLOCK_SIZE - 1)
#define SHA512_MASK (SHA512_BLOCK_SIZE - 1)

#if defined(RH_LITTLE_ENDIAN)
#define BSW_32(p,n) { INTPREF _i = (n); while(_i--) p[_i] = BSWAP_32(p[_i]); }
#define BSW_64(p,n) { INTPREF _i = (n); while(_i--) p[_i] = BSWAP_64(p[_i]); }
#else
#define BSW_32(p,n)
#define BSW_64(p,n)
#endif

#define SHA2_CH(x,y,z)  ((z) ^ ((x) & ((y) ^ (z))))
#define SHA2_MAJ(x,y,z) (((x) & (y)) | ((z) & ((x) ^ (y))))

#define SHA2_S256_0(x) (ROR32((x),  2) ^ ROR32((x), 13) ^ ROR32((x), 22))
#define SHA2_S256_1(x) (ROR32((x),  6) ^ ROR32((x), 11) ^ ROR32((x), 25))
#define SHA2_G256_0(x) (ROR32((x),  7) ^ ROR32((x), 18) ^ ((x) >>  3))
#define SHA2_G256_1(x) (ROR32((x), 17) ^ ROR32((x), 19) ^ ((x) >> 10))

#define SHA2_S512_0(x) (ROR64((x), 28) ^ ROR64((x), 34) ^ ROR64((x), 39))
#define SHA2_S512_1(x) (ROR64((x), 14) ^ ROR64((x), 18) ^ ROR64((x), 41))
#define SHA2_G512_0(x) (ROR64((x),  1) ^ ROR64((x),  8) ^ ((x) >>  7))
#define SHA2_G512_1(x) (ROR64((x), 19) ^ ROR64((x), 61) ^ ((x) >>  6))

#define SHA2_H2(i) p[i & 15] += \
	SHA2_G256_1(p[(i + 14) & 15]) + p[(i + 9) & 15] + SHA2_G256_0(p[(i + 1) & 15])

#define SHA2_H2_CYCLE(i,j)  \
	v[(7 - i) & 7] += (j ? SHA2_H2(i) : p[i & 15]) + SHA2_K256[i + j] \
	+ SHA2_S256_1(v[(4 - i) & 7]) + SHA2_CH(v[(4 - i) & 7], v[(5 - i) & 7], v[(6 - i) & 7]); \
	v[(3 - i) & 7] += v[(7 - i) & 7]; \
	v[(7 - i) & 7] += SHA2_S256_0(v[(0 - i) & 7]) + SHA2_MAJ(v[(0 - i) & 7], v[(1 - i) & 7], v[(2 - i) & 7])

#define SHA2_H5(i) m_wbuf[i & 15] += \
	SHA2_G512_1(m_wbuf[(i + 14) & 15]) + m_wbuf[(i + 9) & 15] + SHA2_G512_0(m_wbuf[(i + 1) & 15])

#define SHA2_H5_CYCLE(i,j)  \
	v[(7 - i) & 7] += (j ? SHA2_H5(i) : m_wbuf[i & 15]) + SHA2_K512[i + j] \
	+ SHA2_S512_1(v[(4 - i) & 7]) + SHA2_CH(v[(4 - i) & 7], v[(5 - i) & 7], v[(6 - i) & 7]); \
	v[(3 - i) & 7] += v[(7 - i) & 7]; \
	v[(7 - i) & 7] += SHA2_S512_0(v[(0 - i) & 7]) + SHA2_MAJ(v[(0 - i) & 7], v[(1 - i) & 7], v[(2 - i) & 7])

static const UWORD32 SHA2_I256[8] =
{
	0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
	0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

static const UWORD64 SHA2_I384[80] =
{
	0xcbbb9d5dc1059ed8, 0x629a292a367cd507,
	0x9159015a3070dd17, 0x152fecd8f70e5939,
	0x67332667ffc00b31, 0x8eb44a8768581511,
	0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4
};

static const UWORD64 SHA2_I512[80] =
{
	0x6a09e667f3bcc908, 0xbb67ae8584caa73b,
	0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
	0x510e527fade682d1, 0x9b05688c2b3e6c1f,
	0x1f83d9abfb41bd6b, 0x5be0cd19137e2179
};

static const UWORD32 SHA2_K256[64] =
{
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static const UWORD64 SHA2_K512[80] =
{
	0x428a2f98d728ae22, 0x7137449123ef65cd,
	0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
	0x3956c25bf348b538, 0x59f111f1b605d019,
	0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
	0xd807aa98a3030242, 0x12835b0145706fbe,
	0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
	0x72be5d74f27b896f, 0x80deb1fe3b1696b1,
	0x9bdc06a725c71235, 0xc19bf174cf692694,
	0xe49b69c19ef14ad2, 0xefbe4786384f25e3,
	0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
	0x2de92c6f592b0275, 0x4a7484aa6ea6e483,
	0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
	0x983e5152ee66dfab, 0xa831c66d2db43210,
	0xb00327c898fb213f, 0xbf597fc7beef0ee4,
	0xc6e00bf33da88fc2, 0xd5a79147930aa725,
	0x06ca6351e003826f, 0x142929670a0e6e70,
	0x27b70a8546d22ffc, 0x2e1b21385c26c926,
	0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
	0x650a73548baf63de, 0x766a0abb3c77b2a8,
	0x81c2c92e47edaee6, 0x92722c851482353b,
	0xa2bfe8a14cf10364, 0xa81a664bbc423001,
	0xc24b8b70d0f89791, 0xc76c51a30654be30,
	0xd192e819d6ef5218, 0xd69906245565a910,
	0xf40e35855771202a, 0x106aa07032bbd1b8,
	0x19a4c116b8d2d0c8, 0x1e376c085141ab53,
	0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
	0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb,
	0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
	0x748f82ee5defb2fc, 0x78a5636f43172f60,
	0x84c87814a1f0ab72, 0x8cc702081a6439ec,
	0x90befffa23631e28, 0xa4506cebde82bde9,
	0xbef9a3f7b2c67915, 0xc67178f2e372532b,
	0xca273eceea26619c, 0xd186b8c721c0c207,
	0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
	0x06f067aa72176fba, 0x0a637dc5a2c898a6,
	0x113f9804bef90dae, 0x1b710b35131c471b,
	0x28db77f523047d84, 0x32caab7b40c72493,
	0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
	0x4cc5d4becb3e42b6, 0x597f299cfc657e2a,
	0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};

static const UWORD32 SHA2_M1[4] =
{
	0x00000000, 0xff000000, 0xffff0000, 0xffffff00
};

static const UWORD32 SHA2_B1[4] =
{
	0x80000000, 0x00800000, 0x00008000, 0x00000080
};

static const UWORD64 SHA2_M2[8] =
{
	0x0000000000000000, 0xff00000000000000,
	0xffff000000000000, 0xffffff0000000000,
	0xffffffff00000000, 0xffffffffff000000,
	0xffffffffffff0000, 0xffffffffffffff00
};

static const UWORD64 SHA2_B2[8] =
{
	0x8000000000000000, 0x0080000000000000,
	0x0000800000000000, 0x0000008000000000,
	0x0000000080000000, 0x0000000000800000,
	0x0000000000008000, 0x0000000000000080
};

CSHA256Hash::CSHA256Hash()
{
}

CSHA256Hash::~CSHA256Hash()
{
}

CSHA384Hash::CSHA384Hash()
{
}

CSHA384Hash::~CSHA384Hash()
{
}

CSHA512Hash::CSHA512Hash()
{
}

CSHA512Hash::~CSHA512Hash()
{
}

void CSHA256Hash::Init(RH_DATA_INFO *pInfo)
{
	m_count[0] = m_count[1] = 0;
	memcpy(m_hash, SHA2_I256, 8 * sizeof(UWORD32));
}

void CSHA384Hash::Init(RH_DATA_INFO *pInfo)
{
	m_count[0] = m_count[1] = 0;
	memcpy(m_hash, SHA2_I384, 8 * sizeof(UWORD64));
}

void CSHA512Hash::Init(RH_DATA_INFO *pInfo)
{
	m_count[0] = m_count[1] = 0;
	memcpy(m_hash, SHA2_I512, 8 * sizeof(UWORD64));
}

void CSHA256Hash::_Compile()
{
	UWORD32 v[8], j, *p = m_wbuf;

	memcpy(v, m_hash, 8 * sizeof(UWORD32));

	for(j = 0; j < 64; j += 16)
	{
		SHA2_H2_CYCLE( 0, j); SHA2_H2_CYCLE( 1, j);
		SHA2_H2_CYCLE( 2, j); SHA2_H2_CYCLE( 3, j);
		SHA2_H2_CYCLE( 4, j); SHA2_H2_CYCLE( 5, j);
		SHA2_H2_CYCLE( 6, j); SHA2_H2_CYCLE( 7, j);
		SHA2_H2_CYCLE( 8, j); SHA2_H2_CYCLE( 9, j);
		SHA2_H2_CYCLE(10, j); SHA2_H2_CYCLE(11, j);
		SHA2_H2_CYCLE(12, j); SHA2_H2_CYCLE(13, j);
		SHA2_H2_CYCLE(14, j); SHA2_H2_CYCLE(15, j);
	}

	m_hash[0] += v[0]; m_hash[1] += v[1]; m_hash[2] += v[2]; m_hash[3] += v[3];
	m_hash[4] += v[4]; m_hash[5] += v[5]; m_hash[6] += v[6]; m_hash[7] += v[7];
}

void CSHA384Hash::_Compile()
{
	UWORD64 v[8];
	UWORD32 j;

	memcpy(v, m_hash, 8 * sizeof(UWORD64));

	for(j = 0; j < 80; j += 16)
	{
		SHA2_H5_CYCLE( 0, j); SHA2_H5_CYCLE( 1, j);
		SHA2_H5_CYCLE( 2, j); SHA2_H5_CYCLE( 3, j);
		SHA2_H5_CYCLE( 4, j); SHA2_H5_CYCLE( 5, j);
		SHA2_H5_CYCLE( 6, j); SHA2_H5_CYCLE( 7, j);
		SHA2_H5_CYCLE( 8, j); SHA2_H5_CYCLE( 9, j);
		SHA2_H5_CYCLE(10, j); SHA2_H5_CYCLE(11, j);
		SHA2_H5_CYCLE(12, j); SHA2_H5_CYCLE(13, j);
		SHA2_H5_CYCLE(14, j); SHA2_H5_CYCLE(15, j);
	}

	m_hash[0] += v[0]; m_hash[1] += v[1]; m_hash[2] += v[2]; m_hash[3] += v[3];
	m_hash[4] += v[4]; m_hash[5] += v[5]; m_hash[6] += v[6]; m_hash[7] += v[7];
}

void CSHA512Hash::_Compile()
{
	UWORD64 v[8];
	UWORD32 j;

	memcpy(v, m_hash, 8 * sizeof(UWORD64));

	for(j = 0; j < 80; j += 16)
	{
		SHA2_H5_CYCLE( 0, j); SHA2_H5_CYCLE( 1, j);
		SHA2_H5_CYCLE( 2, j); SHA2_H5_CYCLE( 3, j);
		SHA2_H5_CYCLE( 4, j); SHA2_H5_CYCLE( 5, j);
		SHA2_H5_CYCLE( 6, j); SHA2_H5_CYCLE( 7, j);
		SHA2_H5_CYCLE( 8, j); SHA2_H5_CYCLE( 9, j);
		SHA2_H5_CYCLE(10, j); SHA2_H5_CYCLE(11, j);
		SHA2_H5_CYCLE(12, j); SHA2_H5_CYCLE(13, j);
		SHA2_H5_CYCLE(14, j); SHA2_H5_CYCLE(15, j);
	}

	m_hash[0] += v[0]; m_hash[1] += v[1]; m_hash[2] += v[2]; m_hash[3] += v[3];
	m_hash[4] += v[4]; m_hash[5] += v[5]; m_hash[6] += v[6]; m_hash[7] += v[7];
}

void CSHA256Hash::Update(const UWORD8 *pBuf, UINTPREF uLen)
{
	UWORD32 pos = (UWORD32)(m_count[0] & SHA256_MASK);
	UWORD32 space = SHA256_BLOCK_SIZE - pos;
	const UWORD8 *sp = pBuf;

	if((m_count[0] += uLen) < uLen)
		++(m_count[1]);

	while(uLen >= space)
	{
		memcpy(((UWORD8 *)m_wbuf) + pos, sp, space);
		sp += space; uLen -= space; space = SHA256_BLOCK_SIZE; pos = 0;
		BSW_32(m_wbuf, SHA256_BLOCK_SIZE >> 2)
		_Compile();
	}

	memcpy(((UWORD8 *)m_wbuf) + pos, sp, uLen);
}

void CSHA384Hash::Update(const UWORD8 *pBuf, UINTPREF uLen)
{
	UWORD32 pos = (UWORD32)(m_count[0] & SHA512_MASK);
	UWORD32 space = SHA512_BLOCK_SIZE - pos;
	const UWORD8 *sp = pBuf;

	if((m_count[0] += uLen) < uLen)
		++(m_count[1]);

	while(uLen >= space)
	{
		memcpy(((UWORD8 *)m_wbuf) + pos, sp, space);
		sp += space; uLen -= space; space = SHA512_BLOCK_SIZE; pos = 0;
		BSW_64(m_wbuf, SHA512_BLOCK_SIZE >> 3);
		_Compile();
	}

	memcpy(((UWORD8 *)m_wbuf) + pos, sp, uLen);
}

void CSHA512Hash::Update(const UWORD8 *pBuf, UINTPREF uLen)
{
	UWORD32 pos = (UWORD32)(m_count[0] & SHA512_MASK);
	UWORD32 space = SHA512_BLOCK_SIZE - pos;
	const UWORD8 *sp = pBuf;

	if((m_count[0] += uLen) < uLen)
		++(m_count[1]);

	while(uLen >= space)
	{
		memcpy(((UWORD8 *)m_wbuf) + pos, sp, space);
		sp += space; uLen -= space; space = SHA512_BLOCK_SIZE; pos = 0;
		BSW_64(m_wbuf, SHA512_BLOCK_SIZE >> 3);
		_Compile();
	}

	memcpy(((UWORD8 *)m_wbuf) + pos, sp, uLen);
}

void CSHA256Hash::Final()
{
	UWORD32 i = (UWORD32)(m_count[0] & SHA256_MASK);

	BSW_32(m_wbuf, (i + 3) >> 2)

	m_wbuf[i >> 2] = (m_wbuf[i >> 2] & SHA2_M1[i & 3]) | SHA2_B1[i & 3];

	if(i > SHA256_BLOCK_SIZE - 9)
	{
		if(i < 60) m_wbuf[15] = 0;
		_Compile();
		i = 0;
	}
	else
		i = (i >> 2) + 1;

	while(i < 14)
		m_wbuf[i++] = 0;

	m_wbuf[14] = (m_count[1] << 3) | (m_count[0] >> 29);
	m_wbuf[15] = m_count[0] << 3;

	_Compile();

	for(i = 0; i < SHA256_DIGEST_SIZE; i++)
		m_final[i] = (UWORD8)(m_hash[i >> 2] >> (8 * (~i & 3)));
}

void CSHA384Hash::Final()
{
	UWORD32 i = (UWORD32)(m_count[0] & SHA512_MASK);

	BSW_64(m_wbuf, (i + 7) >> 3);

	m_wbuf[i >> 3] = (m_wbuf[i >> 3] & SHA2_M2[i & 7]) | SHA2_B2[i & 7];

	if(i > SHA512_BLOCK_SIZE - 17)
	{
		if(i < 120) m_wbuf[15] = 0;
		_Compile();
		i = 0;
	}
	else
		i = (i >> 3) + 1;

	while(i < 14)
		m_wbuf[i++] = 0;

	m_wbuf[14] = (m_count[1] << 3) | (m_count[0] >> 61);
	m_wbuf[15] = m_count[0] << 3;

	_Compile();

	for(i = 0; i < 48; ++i)
		m_final[i] = (UWORD8)(m_hash[i >> 3] >> (8 * (~i & 7)));
}

void CSHA512Hash::Final()
{
	UWORD32 i = (UWORD32)(m_count[0] & SHA512_MASK);

	BSW_64(m_wbuf, (i + 7) >> 3);

	m_wbuf[i >> 3] = (m_wbuf[i >> 3] & SHA2_M2[i & 7]) | SHA2_B2[i & 7];

	if(i > SHA512_BLOCK_SIZE - 17)
	{
		if(i < 120) m_wbuf[15] = 0;
		_Compile();
		i = 0;
	}
	else
		i = (i >> 3) + 1;

	while(i < 14)
		m_wbuf[i++] = 0;

	m_wbuf[14] = (m_count[1] << 3) | (m_count[0] >> 61);
	m_wbuf[15] = m_count[0] << 3;

	_Compile();

	for(i = 0; i < 64; ++i)
		m_final[i] = (UWORD8)(m_hash[i >> 3] >> (8 * (~i & 7)));
}
