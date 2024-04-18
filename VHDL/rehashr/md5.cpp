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

// This implementation of the MD5 hashing algorithm is based on the free
// implementation of RSA Data Security. It has been heavily modified to
// integrate to ReHash.

// Here's the original header:

/*
 **********************************************************************
 ** Copyright (C) 1990, RSA Data Security, Inc. All rights reserved. **
 **                                                                  **
 ** License to copy and use this software is granted provided that   **
 ** it is identified as the "RSA Data Security, Inc. MD5 Message     **
 ** Digest Algorithm" in all material mentioning or referencing this **
 ** software or this function.                                       **
 **                                                                  **
 ** License is also granted to make and use derivative works         **
 ** provided that such works are identified as "derived from the RSA **
 ** Data Security, Inc. MD5 Message Digest Algorithm" in all         **
 ** material mentioning or referencing the derived work.             **
 **                                                                  **
 ** RSA Data Security, Inc. makes no representations concerning      **
 ** either the merchantability of this software or the suitability   **
 ** of this software for any particular purpose.  It is provided "as **
 ** is" without express or implied warranty of any kind.             **
 **                                                                  **
 ** These notices must be retained in any copies of any part of this **
 ** documentation and/or software.                                   **
 **********************************************************************
 */

#include "md5.h"

// Padding
static const UWORD8 MD5_PADDING[64] = {
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Selection, majority and parity
#define MD5_F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define MD5_G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define MD5_H(x, y, z) ((x) ^ (y) ^ (z))
#define MD5_I(x, y, z) ((y) ^ ((x) | (~z)))

// MD5_FF, MD5_GG, MD5_HH, and MD5_II transformations for rounds 1, 2, 3,
// and 4. Rotation is separate from addition to prevent recomputation.
#define MD5_FF(a, b, c, d, x, s, ac) {(a) += MD5_F((b), (c), (d)) + (x) + \
	(UWORD32)(ac); (a) = ROL32((a), (s)); (a) += (b); }
#define MD5_GG(a, b, c, d, x, s, ac) {(a) += MD5_G((b), (c), (d)) + (x) + \
	(UWORD32)(ac); (a) = ROL32((a), (s)); (a) += (b); }
#define MD5_HH(a, b, c, d, x, s, ac) {(a) += MD5_H((b), (c), (d)) + (x) + \
	(UWORD32)(ac); (a) = ROL32((a), (s)); (a) += (b); }
#define MD5_II(a, b, c, d, x, s, ac) {(a) += MD5_I((b), (c), (d)) + (x) + \
	(UWORD32)(ac); (a) = ROL32((a), (s)); (a) += (b); }

// Constants for transformation
#define MD5_S11 7
#define MD5_S12 12
#define MD5_S13 17
#define MD5_S14 22
#define MD5_S21 5
#define MD5_S22 9
#define MD5_S23 14
#define MD5_S24 20
#define MD5_S31 4
#define MD5_S32 11
#define MD5_S33 16
#define MD5_S34 23
#define MD5_S41 6
#define MD5_S42 10
#define MD5_S43 15
#define MD5_S44 21

CMD5Hash::CMD5Hash()
{
}

CMD5Hash::~CMD5Hash()
{
}

void CMD5Hash::_Transform(UWORD32 *pBuf, UWORD32 *pIn)
{
	UWORD32 a = pBuf[0], b = pBuf[1], c = pBuf[2], d = pBuf[3];

	MD5_FF( a, b, c, d, pIn[ 0], MD5_S11, (UWORD32) 3614090360u);
	MD5_FF( d, a, b, c, pIn[ 1], MD5_S12, (UWORD32) 3905402710u);
	MD5_FF( c, d, a, b, pIn[ 2], MD5_S13, (UWORD32)  606105819u);
	MD5_FF( b, c, d, a, pIn[ 3], MD5_S14, (UWORD32) 3250441966u);
	MD5_FF( a, b, c, d, pIn[ 4], MD5_S11, (UWORD32) 4118548399u);
	MD5_FF( d, a, b, c, pIn[ 5], MD5_S12, (UWORD32) 1200080426u);
	MD5_FF( c, d, a, b, pIn[ 6], MD5_S13, (UWORD32) 2821735955u);
	MD5_FF( b, c, d, a, pIn[ 7], MD5_S14, (UWORD32) 4249261313u);
	MD5_FF( a, b, c, d, pIn[ 8], MD5_S11, (UWORD32) 1770035416u);
	MD5_FF( d, a, b, c, pIn[ 9], MD5_S12, (UWORD32) 2336552879u);
	MD5_FF( c, d, a, b, pIn[10], MD5_S13, (UWORD32) 4294925233u);
	MD5_FF( b, c, d, a, pIn[11], MD5_S14, (UWORD32) 2304563134u);
	MD5_FF( a, b, c, d, pIn[12], MD5_S11, (UWORD32) 1804603682u);
	MD5_FF( d, a, b, c, pIn[13], MD5_S12, (UWORD32) 4254626195u);
	MD5_FF( c, d, a, b, pIn[14], MD5_S13, (UWORD32) 2792965006u);
	MD5_FF( b, c, d, a, pIn[15], MD5_S14, (UWORD32) 1236535329u);

	MD5_GG( a, b, c, d, pIn[ 1], MD5_S21, (UWORD32) 4129170786u);
	MD5_GG( d, a, b, c, pIn[ 6], MD5_S22, (UWORD32) 3225465664u);
	MD5_GG( c, d, a, b, pIn[11], MD5_S23, (UWORD32)  643717713u);
	MD5_GG( b, c, d, a, pIn[ 0], MD5_S24, (UWORD32) 3921069994u);
	MD5_GG( a, b, c, d, pIn[ 5], MD5_S21, (UWORD32) 3593408605u);
	MD5_GG( d, a, b, c, pIn[10], MD5_S22, (UWORD32)   38016083u);
	MD5_GG( c, d, a, b, pIn[15], MD5_S23, (UWORD32) 3634488961u);
	MD5_GG( b, c, d, a, pIn[ 4], MD5_S24, (UWORD32) 3889429448u);
	MD5_GG( a, b, c, d, pIn[ 9], MD5_S21, (UWORD32)  568446438u);
	MD5_GG( d, a, b, c, pIn[14], MD5_S22, (UWORD32) 3275163606u);
	MD5_GG( c, d, a, b, pIn[ 3], MD5_S23, (UWORD32) 4107603335u);
	MD5_GG( b, c, d, a, pIn[ 8], MD5_S24, (UWORD32) 1163531501u);
	MD5_GG( a, b, c, d, pIn[13], MD5_S21, (UWORD32) 2850285829u);
	MD5_GG( d, a, b, c, pIn[ 2], MD5_S22, (UWORD32) 4243563512u);
	MD5_GG( c, d, a, b, pIn[ 7], MD5_S23, (UWORD32) 1735328473u);
	MD5_GG( b, c, d, a, pIn[12], MD5_S24, (UWORD32) 2368359562u);

	MD5_HH( a, b, c, d, pIn[ 5], MD5_S31, (UWORD32) 4294588738u);
	MD5_HH( d, a, b, c, pIn[ 8], MD5_S32, (UWORD32) 2272392833u);
	MD5_HH( c, d, a, b, pIn[11], MD5_S33, (UWORD32) 1839030562u);
	MD5_HH( b, c, d, a, pIn[14], MD5_S34, (UWORD32) 4259657740u);
	MD5_HH( a, b, c, d, pIn[ 1], MD5_S31, (UWORD32) 2763975236u);
	MD5_HH( d, a, b, c, pIn[ 4], MD5_S32, (UWORD32) 1272893353u);
	MD5_HH( c, d, a, b, pIn[ 7], MD5_S33, (UWORD32) 4139469664u);
	MD5_HH( b, c, d, a, pIn[10], MD5_S34, (UWORD32) 3200236656u);
	MD5_HH( a, b, c, d, pIn[13], MD5_S31, (UWORD32)  681279174u);
	MD5_HH( d, a, b, c, pIn[ 0], MD5_S32, (UWORD32) 3936430074u);
	MD5_HH( c, d, a, b, pIn[ 3], MD5_S33, (UWORD32) 3572445317u);
	MD5_HH( b, c, d, a, pIn[ 6], MD5_S34, (UWORD32)   76029189u);
	MD5_HH( a, b, c, d, pIn[ 9], MD5_S31, (UWORD32) 3654602809u);
	MD5_HH( d, a, b, c, pIn[12], MD5_S32, (UWORD32) 3873151461u);
	MD5_HH( c, d, a, b, pIn[15], MD5_S33, (UWORD32)  530742520u);
	MD5_HH( b, c, d, a, pIn[ 2], MD5_S34, (UWORD32) 3299628645u);

	MD5_II( a, b, c, d, pIn[ 0], MD5_S41, (UWORD32) 4096336452u);
	MD5_II( d, a, b, c, pIn[ 7], MD5_S42, (UWORD32) 1126891415u);
	MD5_II( c, d, a, b, pIn[14], MD5_S43, (UWORD32) 2878612391u);
	MD5_II( b, c, d, a, pIn[ 5], MD5_S44, (UWORD32) 4237533241u);
	MD5_II( a, b, c, d, pIn[12], MD5_S41, (UWORD32) 1700485571u);
	MD5_II( d, a, b, c, pIn[ 3], MD5_S42, (UWORD32) 2399980690u);
	MD5_II( c, d, a, b, pIn[10], MD5_S43, (UWORD32) 4293915773u);
	MD5_II( b, c, d, a, pIn[ 1], MD5_S44, (UWORD32) 2240044497u);
	MD5_II( a, b, c, d, pIn[ 8], MD5_S41, (UWORD32) 1873313359u);
	MD5_II( d, a, b, c, pIn[15], MD5_S42, (UWORD32) 4264355552u);
	MD5_II( c, d, a, b, pIn[ 6], MD5_S43, (UWORD32) 2734768916u);
	MD5_II( b, c, d, a, pIn[13], MD5_S44, (UWORD32) 1309151649u);
	MD5_II( a, b, c, d, pIn[ 4], MD5_S41, (UWORD32) 4149444226u);
	MD5_II( d, a, b, c, pIn[11], MD5_S42, (UWORD32) 3174756917u);
	MD5_II( c, d, a, b, pIn[ 2], MD5_S43, (UWORD32)  718787259u);
	MD5_II( b, c, d, a, pIn[ 9], MD5_S44, (UWORD32) 3951481745u);

	pBuf[0] += a; pBuf[1] += b; pBuf[2] += c; pBuf[3] += d;
}

void CMD5Hash::Init(RH_DATA_INFO *pInfo)
{
	m_i[0] = m_i[1] = (UWORD32)0;

	// Load magic constants
	m_buf[0] = (UWORD32)0x67452301;
	m_buf[1] = (UWORD32)0xefcdab89;
	m_buf[2] = (UWORD32)0x98badcfe;
	m_buf[3] = (UWORD32)0x10325476;
}

void CMD5Hash::Update(const UWORD8 *pBuf, UINTPREF uLen)
{
	UWORD32 pIn[16];
	INTPREF mdi;
	UINTPREF i, ii;

	// Compute number of bytes mod 64
	mdi = (INTPREF)((m_i[0] >> 3) & 0x3F);

	// Update number of bits
	if((m_i[0] + ((UWORD32)uLen << 3)) < m_i[0]) m_i[1]++; // Overflow
	m_i[0] += ((UWORD32)uLen << 3);
	m_i[1] += ((UWORD32)uLen >> 29);

	while(uLen--)
	{
		// Add new character to buffer, increment mdi
		m_in[mdi++] = *pBuf++;

		// Transform if necessary
		if(mdi == 0x40)
		{
			for(i = 0, ii = 0; i < 16; i++, ii += 4)
				pIn[i] = (((UWORD32)m_in[ii+3]) << 24) |
					(((UWORD32)m_in[ii+2]) << 16) |
					(((UWORD32)m_in[ii+1]) << 8) |
					((UWORD32)m_in[ii]);

			_Transform(m_buf, pIn);
			mdi = 0; // Reset mdi
		}
	}
}

void CMD5Hash::Final()
{
	UWORD32 pIn[16];
	INTPREF mdi;
	UINTPREF i, ii, padLen;

	// Save number of bits
	pIn[14] = m_i[0];
	pIn[15] = m_i[1];

	// Compute number of bytes mod 64
	mdi = (INTPREF)((m_i[0] >> 3) & 0x3F);

	// Pad out to 56 mod 64
	padLen = (mdi < 56) ? (56 - mdi) : (120 - mdi);
	Update(MD5_PADDING, padLen);

	// Append length in bits and transform
	for(i = 0, ii = 0; i < 14; i++, ii += 4)
		pIn[i] = (((UWORD32)m_in[ii+3]) << 24) |
			(((UWORD32)m_in[ii+2]) << 16) |
			(((UWORD32)m_in[ii+1]) <<  8) |
			((UWORD32)m_in[ii]);
	_Transform(m_buf, pIn);

	// Store buffer in digest
	for(i = 0, ii = 0; i < 4; i++, ii += 4)
	{
		m_digest[ii]   = (UWORD8)( m_buf[i]        & 0xFF);
		m_digest[ii+1] = (UWORD8)((m_buf[i] >>  8) & 0xFF);
		m_digest[ii+2] = (UWORD8)((m_buf[i] >> 16) & 0xFF);
		m_digest[ii+3] = (UWORD8)((m_buf[i] >> 24) & 0xFF);
	}
}
