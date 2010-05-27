/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifdef _WIN32
#pragma warning (disable: 4786)
#endif //_WIN32

#include "sisinstallblock.h"
#include "sisdata.h"

CSISInstallBlock::CSISInstallBlock ()
	{
	InsertMembers (); 
	}


CSISInstallBlock::CSISInstallBlock (const CSISInstallBlock& aInitialiser) :
		CStructure <CSISFieldRoot::ESISInstallBlock> (aInitialiser),
		iFiles (aInitialiser.iFiles),
		iEmbeddedFiles (aInitialiser.iEmbeddedFiles),
		iIfs (aInitialiser.iIfs)
	{
	InsertMembers (); 
	}


void CSISInstallBlock::InsertMembers ()
	{
	InsertMember (iFiles);
	InsertMember (iEmbeddedFiles);
	InsertMember (iIfs);
	}


void CSISInstallBlock::Embed (class CSISController& aController, const TUint32 aIndex)
	{
	iEmbeddedFiles.Push (aController); 
	}


void CSISInstallBlock::AddIf ()
	{
		iIfs.Push (CSISIf ());
	}


CSISIf& CSISInstallBlock::If ()
	{ 
	return iIfs.Last ();
	}

const CSISIf& CSISInstallBlock::If () const
	{
	return iIfs.Last (); 
	}


CSISInstallBlock& CSISInstallBlock::IfInstall ()
	{
	return iIfs.Last ().InstallBlock (); 
	}


const CSISInstallBlock& CSISInstallBlock::IfInstall () const
	{
	return iIfs.Last ().InstallBlock (); 
	}

void CSISInstallBlock::SetHash (const CSISHash& aHash)
	{
	iFiles.Last ().SetHash (aHash);
	}

void CSISInstallBlock::ExtractCapabilities(const std::wstring& aFileName)
	{
	iFiles.Last().ExtractCapabilities(aFileName);
	}

void CSISInstallBlock::GetFileList(TFileDescList& aFileList, bool aRecursive) const
	{
	for(int i = 0; i < iFiles.size(); ++i)
		{
		aFileList.push_back(&iFiles[i]);
		}
	
	if(!aRecursive)
		{
		return;
		}
	
	for(i = 0; i < iIfs.size(); ++i)
		{
		iIfs[i].GetFileList(aFileList);
		}
	}
void CSISInstallBlock::GetEmbeddedControllers(TControllerMap& aControllers, bool aRecursive) const
	{
	for(int i = 0; i < iEmbeddedFiles.size(); ++i)
		{
		aControllers[iEmbeddedFiles[i].DataIndex()] = &iEmbeddedFiles[i];
		}
	
	if(!aRecursive)
		{
		return;
		}
	
	for(i = 0; i < iIfs.size(); ++i)
		{
		iIfs[i].GetEmbeddedControllers(aControllers);
		}
	}

void CSISInstallBlock::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	TUint32 descriptionsCount = iFiles.size();
	for (TUint32 i = 0; i < descriptionsCount; ++i)
		{
		iFiles[i].AddPackageEntry(aStream, aVerbose);
		}
	TUint32 embeddedCount = iEmbeddedFiles.size();
	if (embeddedCount)
		{
		if (aVerbose)
			{
			aStream << std::endl << L"; " << embeddedCount << L" embedded files" << std::endl;
			}
		for (int i = 0; i < embeddedCount; ++i)
			{
			wchar_t sisFileName[30];
#ifdef _MSC_VER
			swprintf(sisFileName, L"sis%d.sis", iEmbeddedFiles[i].GetControllerID());
#else
			swprintf(sisFileName, 30, L"sis%d.sis", iEmbeddedFiles[i].GetControllerID());
#endif //_MSC_VER
			aStream << L"@\"" << sisFileName << L"\",(0x" << std::hex << iEmbeddedFiles[i].UID1() << std::dec << L")" << std::endl;
			}
		}
	iIfs.AddPackageEntry(aStream, aVerbose);
	}
