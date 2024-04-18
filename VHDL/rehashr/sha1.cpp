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

 This is a byte oriented version of SHA1 that operates on arrays of bytes
 stored in memory. It runs at 22 cycles per byte on a Pentium P4 processor
*/

#include "sha1.h"

#if defined(RH_BIG_ENDIAN)
#define SWAP_B32(x) (x)
#else
#define SWAP_B32(x) ((ROL32((x), 8) & 0x00FF00FF) | (ROL32((x), 24) & 0xFF00FF00))
#endif

#define SHA1_MASK (SHA1_BLOCK_SIZE - 1)

#if 1

#define SHA1_CH(x,y,z)       (((x) & (y)) ^ (~(x) & (z)))
#define SHA1_PARITY(x,y,z)   ((x) ^ (y) ^ (z))
#define SHA1_MAJ(x,y,z)      (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#else // Discovered Rich Schroeppel and Colin Plumb.

#define SHA1_CH(x,y,z)       ((z) ^ ((x) & ((y) ^ (z))))
#define SHA1_PARITY(x,y,z)   ((x) ^ (y) ^ (z))
#define SHA1_MAJ(x,y,z)      (((x) & (y)) | ((z) & ((x) ^ (y))))

#endif

#define SHA_RND(f,k) { \
	t = a; a = ROL32(a,5) + f(b,c,d) + e + k + w[i]; \
	e = d; d = c; c = ROL32(b, 30); b = t; }

CSHA1Hash::CSHA1Hash()
{
}

CSHA1Hash::~CSHA1Hash()
{
}

void CSHA1Hash::_Compile()
{
	UWORD32 w[80], i, a, b, c, d, e, t;

	for(i = 0; i < SHA1_BLOCK_SIZE >> 2; i++)
		w[i] = SWAP_B32(m_wbuf[i]);

	for(i = SHA1_BLOCK_SIZE >> 2; i < 80; i++)
		w[i] = ROL32(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);

	// Load hash to local variables
	a = m_hash[0]; b = m_hash[1]; c = m_hash[2];
	d = m_hash[3]; e = m_hash[4];

	for(i = 0; i < 20; i++)
		SHA_RND(SHA1_CH, 0x5a827999);

	for(i = 20; i < 40; i++)
		SHA_RND(SHA1_PARITY, 0x6ed9eba1);

	for(i = 40; i < 60; i++)
		SHA_RND(SHA1_MAJ, 0x8f1bbcdc);

	for(i = 60; i < 80; i++)
		SHA_RND(SHA1_PARITY, 0xca62c1d6);

	// Add the result back
	m_hash[0] += a; m_hash[1] += b; m_hash[2] += c;
	m_hash[3] += d; m_hash[4] += e;
}

void CSHA1Hash::Init(RH_DATA_INFO *pInfo)
{
	m_count[0] = m_count[1] = 0;

	m_hash[0] = 0x67452301;
	m_hash[1] = 0xefcdab89;
	m_hash[2] = 0x98badcfe;
	m_hash[3] = 0x10325476;
	m_hash[4] = 0xc3d2e1f0;
}

void CSHA1Hash::Update(const UWORD8 *pBuf, UINTPREF uLen)
{
	UWORD32 pos = (UWORD32)(m_count[0] & SHA1_MASK);
	UWORD32 space = SHA1_BLOCK_SIZE - pos;
	const UWORD8 *sp = pBuf;

	if((m_count[0] += uLen) < uLen)
		++(m_count[1]);

	while(uLen >= space)
	{
		memcpy(((UWORD8 *)m_wbuf) + pos, sp, space);
		sp += space; uLen -= space; space = SHA1_BLOCK_SIZE; pos = 0;
		_Compile();
	}

	memcpy(((UWORD8 *)m_wbuf) + pos, sp, uLen);
}

#if defined(RH_LITTLE_ENDIAN)
static const UWORD32 SHA1_PADMASK[4] = {
	0x00000000, 0x000000ff, 0x0000ffff, 0x00ffffff };
static const UWORD32 SHA1_PADBITS[4] = {
	0x00000080, 0x00008000, 0x00800000, 0x80000000 };
#else
static const UWORD32 SHA1_PADMASK[4] = {
	0x00000000, 0xff000000, 0xffff0000, 0xffffff00 };
static const UWORD32 SHA1_PADBITS[4] = {
	0x80000000, 0x00800000, 0x00008000, 0x00000080 };
#endif

void CSHA1Hash::Final()
{
	UWORD32 i = (UWORD32)(m_count[0] & SHA1_MASK);

	m_wbuf[i >> 2] = (m_wbuf[i >> 2] & SHA1_PADMASK[i & 3]) | SHA1_PADBITS[i & 3];

	if(i > (SHA1_BLOCK_SIZE - 9))
	{
		if(i < 60) m_wbuf[15] = 0;
		_Compile();
		i = 0;
	}
	else
		i = (i >> 2) + 1;

	while(i < 14)
		m_wbuf[i++] = 0;

	m_wbuf[14] = SWAP_B32((m_count[1] << 3) | (m_count[0] >> 29));
	m_wbuf[15] = SWAP_B32(m_count[0] << 3);

	_Compile();

	for(i = 0; i < SHA1_DIGEST_SIZE; i++)
		m_final[i] = (UWORD8)(m_hash[i >> 2] >> (8 * (~i & 3)));
}
