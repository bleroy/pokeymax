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

#include "ghash.h"

CGHash32b3::CGHash32b3()
{
}

CGHash32b3::~CGHash32b3()
{
}

void CGHash32b3::Init(RH_DATA_INFO *pInfo)
{
	m_ghash32 = 0x00000000;
}

void CGHash32b3::Update(const UWORD8 *pBuf, UINTPREF uLen)
{
	UINTPREF i;

	for(i = 0; i < uLen; i++)
		m_ghash32 = (m_ghash32 << 3) + m_ghash32 + pBuf[i];
}

void CGHash32b3::Final()
{
}

//////////////////////////////////////////////////////////////////////////////

CGHash32b5::CGHash32b5()
{
}

CGHash32b5::~CGHash32b5()
{
}

void CGHash32b5::Init(RH_DATA_INFO *pInfo)
{
	m_ghash32 = 0x00000000;
}

void CGHash32b5::Update(const UWORD8 *pBuf, UINTPREF uLen)
{
	UINTPREF i;

	for(i = 0; i < uLen; i++)
		m_ghash32 = (m_ghash32 << 5) + m_ghash32 + pBuf[i];
}

void CGHash32b5::Final()
{
}
