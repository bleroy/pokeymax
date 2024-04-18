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

#include "hashalgo.h"

#include "adler32.h"
#include "crc16.h"
#include "crc32.h"
#include "crc64.h"
#include "ed2khash.h"
#include "elfhash.h"
#include "fcs.h"
#include "fnvhash.h"
#include "ghash.h"
#include "gosthash.h"
#include "haval256_3.h"
#include "haval256_4.h"
#include "haval256_5.h"
#include "md2.h"
#include "md4.h"
#include "md5.h"
#include "ripemd.h"
#include "sha1.h"
#include "sha2.h"
#include "sizehash.h"
#include "tiger.h"
#include "xumhash.h"

void FillAlgorithmList(CHashAlgorithm **pList)
{
	UINTPREF i = 0;

	RH_ASSERT(pList != NULL);

	pList[i++] = new CAdler32Hash;
	pList[i++] = new CCRC16Hash;
	pList[i++] = new CCRC16CcittHash;
	pList[i++] = new CCRC16XModemHash;
	pList[i++] = new CCRC32Hash;
	pList[i++] = new CCRC64Hash;
	pList[i++] = new CED2KHash;
	pList[i++] = new CELF32Hash;
	pList[i++] = new CFCS16Hash;
	pList[i++] = new CFCS32Hash;
	pList[i++] = new CFNV32Hash;
	pList[i++] = new CFNV64Hash;
	pList[i++] = new CGHash32b3;
	pList[i++] = new CGHash32b5;
	pList[i++] = new CGOSTHash;
	pList[i++] = new CHaval256_3;
	pList[i++] = new CHaval256_4;
	pList[i++] = new CHaval256_5;
	pList[i++] = new CMD2Hash;
	pList[i++] = new CMD4Hash;
	pList[i++] = new CMD5Hash;
	pList[i++] = new CRMD128Hash;
	pList[i++] = new CRMD160Hash;
	pList[i++] = new CSHA1Hash;
	pList[i++] = new CSHA256Hash;
	pList[i++] = new CSHA384Hash;
	pList[i++] = new CSHA512Hash;
	pList[i++] = new CSizeHash;
	pList[i++] = new CTigerHash;
	pList[i++] = new CXUM32Hash;
	pList[i++] = NULL;

	RH_ASSERT(pList[0] != NULL);
}
