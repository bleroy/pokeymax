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

#include "benchmark.h"

CHashBenchmark::CHashBenchmark()
{
}

CHashBenchmark::~CHashBenchmark()
{
}

void CHashBenchmark::RunBenchmark()
{
	CHashAlgorithm *pAlgorithms[RH_MAX_ALGORITHMS];
	UINTPREF i;
	CHashAlgorithm *pAlgo;
	RH_DATA_INFO rhInfo;
	UWORD8 pBuf[HBENCH_BUFFER_SIZE];
	clock_t tStart, tCur;
	UINTPREF uClicks;
	UINTPREF uLoops;
	UINTPREF uUnit;

	RH_ASSERT(pAlgorithms != NULL);
	RH_ASSERT(pBuf != NULL);

	FillAlgorithmList(pAlgorithms);

	rhInfo.pszFileName = "Test";
	rhInfo.uDataLen = 0xFFFFFFFD;

	printf("Algorithm                | Hash length | Hashed bytes/second");
	rhNewLine();
	printf("============================================================");
	rhNewLine();

	// Fill the benchmark buffer with something
	for(i = 0; i < HBENCH_BUFFER_SIZE; i++)
		pBuf[i] = (UWORD8)(((i << 3) % 256) + i);

	i = 0;
	while(1)
	{
		if(pAlgorithms[i] == NULL) break;

		pAlgo = pAlgorithms[i];

		pAlgo->Init(&rhInfo);

		uLoops = 0;

		tStart = clock();
		while(1)
		{
			pAlgo->Update(pBuf, HBENCH_BUFFER_SIZE);
			uLoops++;

			tCur = clock();
			uClicks = (UWORD32)((tCur - tStart) / CLOCKS_PER_SEC);
			if(uClicks >= 2) break; // Test each algorithm for 2 seconds
		}
		pAlgo->Final();
		uLoops /= 2; // Divide by 2 to get bytes/sec

		uUnit = 0;
		uLoops *= HBENCH_BUFFER_SIZE;
		if(uLoops > (1024 << 1)) { uLoops /= 1024; uUnit++; }
		if(uLoops > (1024 << 1)) { uLoops /= 1024; uUnit++; }
		if(uLoops > (1024 << 1)) { uLoops /= 1024; uUnit++; }
		if(uLoops > (1024 << 1)) { uLoops /= 1024; uUnit++; }

		printf("%-24s | %6u bits | %16u",
			pAlgo->GetName(), pAlgo->GetLength() << 3, uLoops);

		if(uUnit == 0) printf("  B");
		if(uUnit == 1) printf(" KB");
		if(uUnit == 2) printf(" MB");
		if(uUnit == 3) printf(" GB");
		if(uUnit == 4) printf(" TB");
		rhNewLine();

		delete pAlgorithms[i];
		pAlgorithms[i] = NULL;
		i++;
	}
}
