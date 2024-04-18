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

#include "ed2khash.h"

CED2KHash::CED2KHash()
{
}

CED2KHash::~CED2KHash()
{
}

void CED2KHash::Init(RH_DATA_INFO *pInfo)
{
	m_nextPos = 0;

	m_hashTotal.Init(pInfo);
	m_hashBlock.Init(pInfo);
}

void CED2KHash::Update(const UWORD8 *pBuf, UINTPREF uLen)
{
	UINTPREF firstLen;
	UWORD8 innerDigest[16];

	if((m_nextPos > 0) && ((m_nextPos % ED2K_BLOCKSIZE) == 0))
	{
		m_hashBlock.Final();
		m_hashBlock.GetHash(innerDigest);

		// Feed it to the overall hash
		m_hashTotal.Update(innerDigest, 16);

		// Reset the current segment
		m_hashBlock.Init(0);
	}

	// Now, handle the new data
	if((m_nextPos / ED2K_BLOCKSIZE) == (m_nextPos + uLen) / ED2K_BLOCKSIZE)
	{
		// Not finishing any segments, just keep feeding segment hash
		m_hashBlock.Update(pBuf, uLen);
		m_nextPos += uLen;
		return;
	}

	// Finish the current segment
	firstLen = ED2K_BLOCKSIZE - (m_nextPos % ED2K_BLOCKSIZE);
	m_hashBlock.Update(pBuf, firstLen);
	m_nextPos += firstLen;

	// Continue with passed-in info
	Update(pBuf + firstLen, uLen - firstLen);
}

void CED2KHash::Final()
{
	UWORD8 innerDigest[16];

	m_hashBlock.Final();

	if(m_nextPos <= ED2K_BLOCKSIZE)
	{
		// There was only one segment; return its hash
		m_hashBlock.GetHash(m_final);
		return;
	}

	// Finish the segment in process
	m_hashBlock.GetHash(innerDigest);

	// Feed it to the overall hash
	m_hashTotal.Update(innerDigest, 16);

	// Finish the overall hash
	m_hashTotal.Final();
	m_hashTotal.GetHash(m_final);
}
