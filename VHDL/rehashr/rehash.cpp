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

#include "rhcommon.h"
#include "rehash.h"
#include "benchmark.h"
#include "selftest.h"
#include "fnpattern.h"

#pragma message("Test your creation by executing rehash -test")

int main(int argc, char **argv)
{
	INTPREF i, t, nArgLen;
	bool bOnce = true;
	char szTemp[RH_MAX_STD_BUFFER];
	char szPath[RH_MAX_PATH];
	char szMask[RH_MAX_PATH];
	CHashManager hashmgr;
	bool bBenchmark = false;
	CHashBenchmark hashbench;

	RH_ASSERT(_rhTestTypes() == 0);

	// Windows understands/requires console /n output
	rhSetNewLine(false);

	memset(szTemp, 0, RH_MAX_STD_BUFFER);
	memset(szPath, 0, RH_MAX_PATH);
	memset(szMask, 0, RH_MAX_PATH);

	if(argc <= 1)
	{
		printInfo();
		return RH_NO_ARGS;
	}

	for(i = 1; i < argc; i++)
	{
		// Test if the argument is a path descriptor
		if(isArgument(argv[i]) == false)
		{
			if(bOnce == false) rhstrcat(szPath, " ", RH_MAX_PATH);
			rhstrcat(szPath, argv[i], RH_MAX_PATH);
			bOnce = false; // Next time add a space
			continue;
		}

		// The argument is an optional argument

		if(strlen(argv[i]) >= RH_MAX_STD_BUFFER) continue; // Non-parsable option argument, ignore

		fmtArgument(argv[i], szTemp); // Format the argument, i.e. remove all special chars
		nArgLen = strlen(szTemp);

		if(strnicmpex(szTemp, "hmac:", 5) == 0)
		{
			hashmgr.SetHMAC(true, (UWORD8 *)(szTemp + 5), nArgLen - 5);
		}

		strlwr(szTemp);

		if(strcmp(szTemp, "win"       ) == 0) rhSetNewLine(false);
		if(strcmp(szTemp, "forcewin"  ) == 0) rhSetNewLine(true);
		if(strcmp(szTemp, "win9x"     ) == 0) rhSetNewLine(false);
		if(strcmp(szTemp, "winnt"     ) == 0) rhSetNewLine(false);
		if(strcmp(szTemp, "standard"  ) == 0) rhSetNewLine(false);
		if(strcmp(szTemp, "unix"      ) == 0) rhSetNewLine(false);
		if(strcmp(szTemp, "linux"     ) == 0) rhSetNewLine(false);
		if(strcmp(szTemp, "compatible") == 0) rhSetNewLine(false);

		if(strcmp(szTemp, "help"    ) == 0) printHelp();
		if(strcmp(szTemp, "h"       ) == 0) printHelp();
		if(strcmp(szTemp, "?"       ) == 0) printInfo();
		if(strcmp(szTemp, "version" ) == 0) printInfo();
		if(strcmp(szTemp, "v"       ) == 0) printInfo();
		if(strcmp(szTemp, "info"    ) == 0) printInfo();
		if(strcmp(szTemp, "alg"     ) == 0) printAlgorithms(&hashmgr);
		if(strcmp(szTemp, "alginfo" ) == 0) printAlgorithms(&hashmgr);
		if(strcmp(szTemp, "algorithms") == 0) printAlgorithms(&hashmgr);

		if(strcmp(szTemp, "bench"   ) == 0) bBenchmark = true;

		if(strcmp(szTemp, "recur") == 0) hashmgr.SetOption(HM_RECURSIVE, true);
		if(strcmp(szTemp, "rcrsv") == 0) hashmgr.SetOption(HM_RECURSIVE, true);
		if(strcmp(szTemp, "recursive") == 0) hashmgr.SetOption(HM_RECURSIVE, true);
		if(strcmp(szTemp, "recv") == 0) hashmgr.SetOption(HM_RECURSIVE, true);
		if(strcmp(szTemp, "norecur") == 0) hashmgr.SetOption(HM_RECURSIVE, false);
		if(strcmp(szTemp, "norcrsv") == 0) hashmgr.SetOption(HM_RECURSIVE, false);
		if(strcmp(szTemp, "norecursive") == 0) hashmgr.SetOption(HM_RECURSIVE, false);
		if(strcmp(szTemp, "norecv") == 0) hashmgr.SetOption(HM_RECURSIVE, false);

		if(strcmp(szTemp, "short") == 0) hashmgr.SetOption(HM_SHORTNAMES, true);
		if(strcmp(szTemp, "shortnames") == 0) hashmgr.SetOption(HM_SHORTNAMES, true);
		if(strcmp(szTemp, "snames") == 0) hashmgr.SetOption(HM_SHORTNAMES, true);
		if(strcmp(szTemp, "fileonly") == 0) hashmgr.SetOption(HM_SHORTNAMES, true);
		if(strcmp(szTemp, "filenameonly") == 0) hashmgr.SetOption(HM_SHORTNAMES, true);
		if(strcmp(szTemp, "fullpath") == 0) hashmgr.SetOption(HM_SHORTNAMES, false);
		if(strcmp(szTemp, "longpaths") == 0) hashmgr.SetOption(HM_SHORTNAMES, false);

		if(strcmp(szTemp, "all" ) == 0) hashmgr.SelectAllAlgorithms(true);
		if(strcmp(szTemp, "none") == 0) hashmgr.SelectAllAlgorithms(false);

		if(strcmp(szTemp, "test"    ) == 0) rhSelfTest();
		if(strcmp(szTemp, "selftest") == 0) rhSelfTest();

		t = hashmgr.FindAlgorithm(szTemp, true);
		if(t != -1) hashmgr.SelectAlgorithm(t, true);

		if((nArgLen > 2) && (strnicmpex(szTemp, "no", 2) == 0))
		{
			t = hashmgr.FindAlgorithm(szTemp + 2, true);
			if(t != -1) hashmgr.SelectAlgorithm(t, false);
		}
		if((nArgLen > 4) && (strnicmpex(szTemp, "out:", 4) == 0))
		{
			hashmgr.SetOutputStyle(szTemp + 4);
		}
	}

	if(bBenchmark == true) hashbench.RunBenchmark();

	// Check if a path is in the arguments
	bOnce = false;
	for(i = 1; i < argc; i++) if(isArgument(argv[i]) == 0) bOnce = true;
	if(bOnce == false) return RH_NO_PATH; // Silently exit

	fmtPath(szPath);

	rhstrcpy(szMask, szPath, RH_MAX_PATH);
	fileonly(szMask);

	if(fpattern_isvalid(szMask) == false)
	{
		printf("Filename pattern invalid.");
		rhNewLine();
		return RH_NO_PATH;
	}

	if(haspath(szPath)) pathonly(szPath);
	else getcwd(szPath, RH_MAX_PATH);

	rhfullpathex(szPath, szTemp);
	catdirsep(szTemp);
	hashmgr.PrepareHashing();
	hashmgr.HashPath(szTemp, szMask);
	hashmgr.EndHashing();

	return RH_SUCCESS;
}

void printInfo()
{
	printf(RH_PRODUCT_FULL);
	rhNewLine();

	printf("Version: ");
	printf(RH_VERSION_STR);

	// Output the endian
#if (defined(RH_LITTLE_ENDIAN) && defined(RH_BIG_ENDIAN))
	printf(", Both Endians(?)");
#elif defined(RH_LITTLE_ENDIAN)
	printf(", Little Endian");
#elif defined(RH_BIG_ENDIAN)
	printf(", Big Endian");
#else
#error Undefined endian.
#endif

	printf(", Build: ");
	printf(__DATE__);
	printf(" ");
	printf(__TIME__);

	rhNewLine();
}

void printHelp()
{
	printInfo();
	rhNewLine();

	printf("See help file.");
	rhNewLine();
}

void printAlgorithms(CHashManager *pMgr)
{
	UINTPREF i = 0;

	RH_ASSERT(pMgr != NULL);

	printf("Full name                | Short/option name | Hash length");
	rhNewLine();
	printf("----------------------------------------------------------");
	rhNewLine();

	while(1)
	{
		if(pMgr->m_pAlgorithms[i] == NULL) break;

		printf("%-24s | %-17s | %6u bits",
			pMgr->m_pAlgorithms[i]->GetName(),
			pMgr->m_pAlgorithms[i]->GetShortName(),
			pMgr->m_pAlgorithms[i]->GetLength() << 3);
		rhNewLine();

		i++;
	}
}
