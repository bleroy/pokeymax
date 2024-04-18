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

#include "hashmgr.h"
#include "clutil.h"
#include "fnpattern.h"

CHashManager::CHashManager()
{
	INTPREF i;

	// Load all algorithms
	FillAlgorithmList(m_pAlgorithms);
	RH_ASSERT(m_pAlgorithms[0] != NULL);

	for(i = 0; i < RH_MAX_ALGORITHMS; i++)
	{
		m_bUseAlgorithm[i] = true;

		m_pHMACs[i] = NULL;
	}

	m_bRecursiveScanning = true;
	m_bTimings = true;
	m_bShortNames = true;
	m_bHMAC = false;

	m_output.SetOutputStyleN(HASHOUTPUT_TEXT);
}

CHashManager::~CHashManager()
{
	INTPREF i = 0;

	while(1)
	{
		if(m_pAlgorithms[i] == NULL) break;

		if(m_bHMAC == true)
		{
			if(m_pHMACs[i] != NULL) delete m_pHMACs[i];
			m_pHMACs[i] = NULL;
		}

		delete m_pAlgorithms[i];
		m_pAlgorithms[i] = NULL;
		i++;
	}
}

INTPREF CHashManager::FindAlgorithm(const char *pszName, bool bShortName)
{
	INTPREF i = 0;

	if(bShortName)
	{
		while(1)
		{
			if(m_pAlgorithms[i] == NULL) return -1;

			if(stricmp(pszName, m_pAlgorithms[i]->GetShortName()) == 0)
				return i;

			i++;
		}
	}
	else
	{
		while(1)
		{
			if(m_pAlgorithms[i] == NULL) return -1;

			if(stricmp(pszName, m_pAlgorithms[i]->GetName()) == 0)
				return i;

			i++;
		}
	}

	RH_ASSERT(false); // We should never get here
	return -1;
}

bool CHashManager::SelectAlgorithm(INTPREF nAlgorithm, bool bSelect)
{
	if((nAlgorithm < 0) || (nAlgorithm >= RH_MAX_ALGORITHMS))
		return false;

	if(m_pAlgorithms[nAlgorithm] == NULL) return false;

	m_bUseAlgorithm[nAlgorithm] = bSelect;
	return true;
}

void CHashManager::SelectAllAlgorithms(bool bSelect)
{
	UINTPREF i = 0;

	while(1)
	{
		if(m_pAlgorithms[i] == NULL) break;

		m_bUseAlgorithm[i] = bSelect;
		i++;
	}
}

void CHashManager::SetOutputStyle(const char *pszOutputSchemeName)
{
	RH_ASSERT(pszOutputSchemeName != NULL);

	m_output.SetOutputStyleA(pszOutputSchemeName);
}

void CHashManager::SetHMAC(bool bHMAC, UWORD8 *pKey, UINTPREF uKeySize)
{
	UINTPREF i = 0;

	RH_ASSERT((bHMAC == true) || (bHMAC == false)); // Only accept real bool

	if(bHMAC == false) return;

	if(m_bHMAC == false)
		FillHMACList(m_pHMACs, m_pAlgorithms);

	while(1)
	{
		if(m_pAlgorithms[i] == NULL) break;

		RH_ASSERT(m_pHMACs[i] != NULL);

		m_pHMACs[i]->SetKey(pKey, uKeySize);

		i++;
	}

	m_bHMAC = bHMAC;
	rhstrcpy(m_szHMACKey, (const char *)pKey, RH_MAX_STD_BUFFER);
}

void CHashManager::SetOption(INTPREF nOption, bool bFlag)
{
	RH_ASSERT((nOption > HM_NULL) && (nOption < HM_LAST));

	if(nOption == HM_RECURSIVE) m_bRecursiveScanning = bFlag;
	if(nOption == HM_SHORTNAMES) m_bShortNames = bFlag;
}

void CHashManager::PrepareHashing()
{
	m_output.InitOutput(this);
}

void CHashManager::EndHashing()
{
	m_output.CloseOutput();
}

bool CHashManager::HashPath(const char *pszPath, const char *pszMask)
{
	RH_TRACE2("Hashing path: %s, mask: %s", pszPath, pszMask);

#if (RH_TARGET_SYSTEM == RH_TARGET_SYSTEM_WINDOWS)

	HANDLE hFind;
	WIN32_FIND_DATA fd;
	char szBasePath[RH_MAX_PATH];
	char szFileDesc[RH_MAX_PATH];
	bool bFileFound = true;

	// Construct the full path
	rhfullpathex(pszPath, szBasePath);

	ZeroMemory(&fd, sizeof(WIN32_FIND_DATA));

	rhstrcpy(szFileDesc, szBasePath, RH_MAX_PATH);
	catdirsep(szFileDesc);
	rhstrcat(szFileDesc, "*", RH_MAX_PATH);

	RH_TRACE1("FindFile: %s", szFileDesc);

	hFind = FindFirstFile(szFileDesc, &fd);
	if(hFind == INVALID_HANDLE_VALUE) return true; // Nothing to do

	bFileFound = true;
	while(1)
	{
		if(bFileFound == false) break;

		rhstrcpy(szFileDesc, fd.cFileName, RH_MAX_PATH);
		fileonly(szFileDesc);

		if(ispathnav(szFileDesc) == false)
		{
			if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				if(fpattern_match(pszMask, szFileDesc) == true)
				{
					rhstrcpy(szFileDesc, szBasePath, RH_MAX_PATH);
					catdirsep(szFileDesc);
					rhstrcat(szFileDesc, fd.cFileName, RH_MAX_PATH);

					HashFile(szFileDesc);
				}
			}
			else // It's a directory
			{
				if(m_bRecursiveScanning == true)
				{
					rhstrcpy(szFileDesc, szBasePath, RH_MAX_PATH);
					catdirsep(szFileDesc);
					rhstrcat(szFileDesc, fd.cFileName, RH_MAX_PATH);

					HashPath(szFileDesc, pszMask);
				}
			}
		}

		if(FindNextFile(hFind, &fd) == FALSE) bFileFound = false;
	}

	FindClose(hFind);

	return true;

	//////////////////////////////////////////////////////////////////////////

#else // RH_TARGET_SYSTEM != RH_TARGET_SYSTEM_WINDOWS

	RH_TRACE1("New scan called with pszPath = %s", pszPath);

	if(chdir(pszPath) != 0) return RH_FIND_DENIED; // Permission denied?
	DIR *pDir = opendir("."); // Find all files in that path
	if(pDir == NULL) return true; // No access
	struct dirent *pEntry;
	struct stat fileStats;
	char szPath[RH_MAX_PATH];

	while((pEntry = readdir(pDir)))
	{
		if(lstat(pEntry->d_name, &fileStats) == 0)
		{
			rhfullpathex(pEntry->d_name, szPath);

			RH_TRACE2("Entry: %s, fullpath: %s", pEntry->d_name, szPath);

			if(ispathnav(pEntry->d_name) == false)
			{
				if(S_ISDIR(fileStats.st_mode))
				{
					if(m_bRecursiveScanning == true)
					{
						RH_TRACE1("New path hashing: %s", szPath);
						catdirsep(szPath);
						if(HashPath(szPath, pszMask) != RH_FIND_DENIED)
							chdir("..");
					}
				}
				else
				{
					if(fpattern_match(pszMask, pEntry->d_name) == true)
						HashFile(szPath);
				}
			}
			else
			{
				RH_TRACE0("Entry skipped");
			}
		}
	}

	closedir(pDir);
	return true;

#endif
}

INTPREF CHashManager::HashFile(const char *pszFile)
{
	FILE *fp;
	UINTPREF uFileSize, uRead;
	UWORD8 pBuf[HM_BUFFER_SIZE];
	INTPREF nAlgorithm;
	RH_DATA_INFO rhDataInfo;

	RH_ASSERT(HM_BUFFER_SIZE >= RH_MAX_PATH);

	RH_ASSERT(pszFile != NULL);
	if(pszFile == NULL) return RH_NULLPOINTER;

	RH_TRACE1("Hashing file: %s", pszFile);

	fp = fopen(pszFile, "rb");
	if(fp == NULL) return RH_FILE_NOT_FOUND;

	fseek(fp, 0, SEEK_END);
	uFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	rhDataInfo.pszFileName = pszFile;
	rhDataInfo.uDataLen = uFileSize;

	nAlgorithm = 0;
	while(1)
	{
		if(m_pAlgorithms[nAlgorithm] == NULL) break;

		if(m_bUseAlgorithm[nAlgorithm] == true)
		{
			if(m_bHMAC == false)
				m_pAlgorithms[nAlgorithm]->Init(&rhDataInfo);
			else
				m_pHMACs[nAlgorithm]->Init(&rhDataInfo);
		}

		nAlgorithm++;
	}

	while(1)
	{
		uRead = fread(pBuf, 1, HM_BUFFER_SIZE, fp);
		if(uRead == 0) break;

		nAlgorithm = 0;
		while(1)
		{
			if(m_pAlgorithms[nAlgorithm] == NULL) break;

			if(m_bUseAlgorithm[nAlgorithm] == true)
			{
				if(m_bHMAC == false)
					m_pAlgorithms[nAlgorithm]->Update(pBuf, uRead);
				else
					m_pHMACs[nAlgorithm]->Update(pBuf, uRead);
			}

			nAlgorithm++;
		}

		if(uRead < HM_BUFFER_SIZE) break;
	}

	fclose(fp);

	if(m_bShortNames == true)
	{
		rhstrcpy((char *)pBuf, pszFile, HM_BUFFER_SIZE);
		fileonly((char *)pBuf);
		m_output.NewDataSource((char *)pBuf, m_bHMAC, m_szHMACKey);
	}
	else
	{
		m_output.NewDataSource(pszFile, m_bHMAC, m_szHMACKey);
	}

	CHashAlgorithm *pAlgo;
	CHMAC *pHMAC;
	nAlgorithm = 0;
	while(1)
	{
		if(m_pAlgorithms[nAlgorithm] == NULL) break;

		if(m_bUseAlgorithm[nAlgorithm] == true)
		{
			pAlgo = m_pAlgorithms[nAlgorithm]; // Select algorithm

			if(m_bHMAC == false)
			{
				pAlgo->Final();
				pAlgo->GetHash(pBuf); // Reuse the file buffer

				m_output.Output(pAlgo->GetName(), pBuf, pAlgo->GetLength());
			}
			else
			{
				pHMAC = m_pHMACs[nAlgorithm]; // Select HMAC

				pHMAC->Final();
				pHMAC->GetHash(pBuf); // Reuse the file buffer

				m_output.Output(pAlgo->GetName(), pBuf, pAlgo->GetLength());
			}
		}

		nAlgorithm++;
	}

	m_output.CloseDataSource();

	return RH_SUCCESS;
}
