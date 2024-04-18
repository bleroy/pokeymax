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

// This free implementation is based on the free original implementation of
// RSA Data Security. The source has been heavily modified to integrate
// to ReHash.

// Here's the original header:

/*
	Copyright (C) 1990-2, RSA Data Security, Inc. All rights reserved.

	License to copy and use this software is granted provided that it
	is identified as the "RSA Data Security, Inc. MD4 Message-Digest
	Algorithm" in all material mentioning or referencing this software
	or this function.

	License is also granted to make and use derivative works provided
	that such works are identified as "derived from the RSA Data
	Security, Inc. MD4 Message-Digest Algorithm" in all material
	mentioning or referencing the derived work.

	RSA Data Security, Inc. makes no representations concerning either
	the merchantability of this software or the suitability of this
	software for any particular purpose. It is provided "as is"
	without express or implied warranty of any kind.

	These notices must be retained in any copies of any part of this
	documentation and/or software.
*/

#include "md4.h"

// Constants for MD4_Transform routine
#define MD4_S11 3
#define MD4_S12 7
#define MD4_S13 11
#define MD4_S14 19
#define MD4_S21 3
#define MD4_S22 5
#define MD4_S23 9
#define MD4_S24 13
#define MD4_S31 3
#define MD4_S32 9
#define MD4_S33 11
#define MD4_S34 15

static const UWORD8 MD4_PADDING[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// MD4F, MD4G and MD4H are basic MD4 functions
#define MD4F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define MD4G(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define MD4H(x, y, z) ((x) ^ (y) ^ (z))

// MD4FF, MD4GG and MD4HH are transformations for rounds 1, 2 and 3
// Rotation is separate from addition to prevent recomputation
#define MD4FF(a, b, c, d, x, s) { \
	(a) += MD4F((b), (c), (d)) + (x); \
	(a) = ROL32((a), (s)); }
#define MD4GG(a, b, c, d, x, s) { \
	(a) += MD4G((b), (c), (d)) + (x) + 0x5a827999; \
	(a) = ROL32((a), (s)); }
#define MD4HH(a, b, c, d, x, s) { \
	(a) += MD4H((b), (c), (d)) + (x) + 0x6ed9eba1; \
	(a) = ROL32((a), (s)); }

CMD4Hash::CMD4Hash()
{
}

CMD4Hash::~CMD4Hash()
{
}

// MD4 initialization. Begins an MD4 operation, writing a new context
void CMD4Hash::Init(RH_DATA_INFO *pInfo)
{
	m_count[0] = m_count[1] = 0;

	// Load magic initialization constants
	m_state[0] = 0x67452301;
	m_state[1] = 0xefcdab89;
	m_state[2] = 0x98badcfe;
	m_state[3] = 0x10325476;
}

// MD4 block update operation. Continues an MD4 message-digest
//     operation, processing another message block, and updating the
//     context
void CMD4Hash::Update(const UWORD8 *pBuf, UINTPREF uLen)
{
	UINTPREF i = 0, ix, partLen;

	// Compute number of bytes mod 64
	ix = (UINTPREF)((m_count[0] >> 3) & 0x3F);

	// Update number of bits
	if((m_count[0] += ((UWORD32)uLen << 3)) < ((UWORD32)uLen << 3))
		m_count[1]++;
	m_count[1] += ((UWORD32)uLen >> 29);

	partLen = 64 - ix;

	// Transform as many times as possible
	if(uLen >= partLen)
	{
		memcpy(&m_buffer[ix], pBuf, partLen);
		_Transform(m_state, m_buffer);

		for(i = partLen; i + 63 < uLen; i += 64)
			_Transform(m_state, &pBuf[i]);

		ix = 0;
	}
	else i = 0;

	// Buffer remaining input
	memcpy(&m_buffer[ix], &pBuf[i], uLen - i);
}

// MD4 finalization. Ends an MD4 message-digest operation, writing the
//     the message digest and zeroizing the context
void CMD4Hash::Final()
{
	UWORD8 pBits[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	UINTPREF ix, padLen;

	// Save number of bits
	_Encode(pBits, m_count, 8);

	// Pad out to 56 mod 64.
	ix = (UINTPREF)((m_count[0] >> 3) & 0x3f);
	padLen = (ix < 56) ? (56 - ix) : (120 - ix);
	Update(MD4_PADDING, padLen);

	// Append length (before padding)
	Update(pBits, 8);

	// Store state in digest
	_Encode(m_digest, m_state, 16);
}

// MD4 basic transformation. Transforms state based on block.
void CMD4Hash::_Transform(UWORD32 *pState, const UWORD8 *pBlock)
{
	UWORD32 a = pState[0], b = pState[1], c = pState[2], d = pState[3];
	UWORD32 x[16];

	_Decode(x, pBlock, 64);

	// Round 1
	MD4FF(a, b, c, d, x[ 0], MD4_S11);
	MD4FF(d, a, b, c, x[ 1], MD4_S12);
	MD4FF(c, d, a, b, x[ 2], MD4_S13);
	MD4FF(b, c, d, a, x[ 3], MD4_S14);
	MD4FF(a, b, c, d, x[ 4], MD4_S11);
	MD4FF(d, a, b, c, x[ 5], MD4_S12);
	MD4FF(c, d, a, b, x[ 6], MD4_S13);
	MD4FF(b, c, d, a, x[ 7], MD4_S14);
	MD4FF(a, b, c, d, x[ 8], MD4_S11);
	MD4FF(d, a, b, c, x[ 9], MD4_S12);
	MD4FF(c, d, a, b, x[10], MD4_S13);
	MD4FF(b, c, d, a, x[11], MD4_S14);
	MD4FF(a, b, c, d, x[12], MD4_S11);
	MD4FF(d, a, b, c, x[13], MD4_S12);
	MD4FF(c, d, a, b, x[14], MD4_S13);
	MD4FF(b, c, d, a, x[15], MD4_S14);

	// Round 2
	MD4GG(a, b, c, d, x[ 0], MD4_S21);
	MD4GG(d, a, b, c, x[ 4], MD4_S22);
	MD4GG(c, d, a, b, x[ 8], MD4_S23);
	MD4GG(b, c, d, a, x[12], MD4_S24);
	MD4GG(a, b, c, d, x[ 1], MD4_S21);
	MD4GG(d, a, b, c, x[ 5], MD4_S22);
	MD4GG(c, d, a, b, x[ 9], MD4_S23);
	MD4GG(b, c, d, a, x[13], MD4_S24);
	MD4GG(a, b, c, d, x[ 2], MD4_S21);
	MD4GG(d, a, b, c, x[ 6], MD4_S22);
	MD4GG(c, d, a, b, x[10], MD4_S23);
	MD4GG(b, c, d, a, x[14], MD4_S24);
	MD4GG(a, b, c, d, x[ 3], MD4_S21);
	MD4GG(d, a, b, c, x[ 7], MD4_S22);
	MD4GG(c, d, a, b, x[11], MD4_S23);
	MD4GG(b, c, d, a, x[15], MD4_S24);

	// Round 3
	MD4HH(a, b, c, d, x[ 0], MD4_S31);
	MD4HH(d, a, b, c, x[ 8], MD4_S32);
	MD4HH(c, d, a, b, x[ 4], MD4_S33);
	MD4HH(b, c, d, a, x[12], MD4_S34);
	MD4HH(a, b, c, d, x[ 2], MD4_S31);
	MD4HH(d, a, b, c, x[10], MD4_S32);
	MD4HH(c, d, a, b, x[ 6], MD4_S33);
	MD4HH(b, c, d, a, x[14], MD4_S34);
	MD4HH(a, b, c, d, x[ 1], MD4_S31);
	MD4HH(d, a, b, c, x[ 9], MD4_S32);
	MD4HH(c, d, a, b, x[ 5], MD4_S33);
	MD4HH(b, c, d, a, x[13], MD4_S34);
	MD4HH(a, b, c, d, x[ 3], MD4_S31);
	MD4HH(d, a, b, c, x[11], MD4_S32);
	MD4HH(c, d, a, b, x[ 7], MD4_S33);
	MD4HH(b, c, d, a, x[15], MD4_S34);

	pState[0] += a;
	pState[1] += b;
	pState[2] += c;
	pState[3] += d;
}

void CMD4Hash::_Encode(UWORD8 *pOutput, const UWORD32 *pInput, UINTPREF uLen)
{
	UINTPREF i, j;

	for(i = 0, j = 0; j < uLen; i++, j += 4)
	{
		pOutput[j] = (UWORD8)(pInput[i] & 0xFF);
		pOutput[j+1] = (UWORD8)((pInput[i] >> 8) & 0xFF);
		pOutput[j+2] = (UWORD8)((pInput[i] >> 16) & 0xFF);
		pOutput[j+3] = (UWORD8)((pInput[i] >> 24) & 0xFF);
	}
}

void CMD4Hash::_Decode(UWORD32 *pOutput, const UWORD8 *pInput, UINTPREF uLen)
{
	UINTPREF i, j;

	for(i = 0, j = 0; j < uLen; i++, j += 4)
		pOutput[i] = ((UWORD32)pInput[j]) | (((UWORD32)pInput[j+1]) << 8) |
			(((UWORD32)pInput[j+2]) << 16) | (((UWORD32)pInput[j+3]) << 24);
}
