/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @released
 @internalTechnology
*/

#include "integritytree.h"

#include "integrityservices.h"
#include <f32file.h>

using namespace Swi;

CIntegrityTreeLeaf* CIntegrityTreeLeaf::NewLC(const TDesC& aFileName, TIntegrityServicesEvent aType,
	const TDesC& aOwningJournal)
	{
	CIntegrityTreeLeaf* self = new (ELeave) CIntegrityTreeLeaf(aType, aOwningJournal);
	CleanupStack::PushL(self);
	self->ConstructL(aFileName);
	return self;
	}
	
CIntegrityTreeLeaf::~CIntegrityTreeLeaf()
	{
	delete iFileName;
	}

CIntegrityTreeLeaf::CIntegrityTreeLeaf(TIntegrityServicesEvent aType, const TDesC& aOwningJournal)
	: iType(aType), iOwningJournal(aOwningJournal)
	{
	}
	
void CIntegrityTreeLeaf::ConstructL(const TDesC& aFileName)	
	{
	iFileName = aFileName.AllocL();
	}

CIntegrityTreeLeaf* CIntegrityTreeLeaf::Peer()
	{
	return iPeer;
	}
	
TIntegrityServicesEvent CIntegrityTreeLeaf::Type()
	{
	return iType;
	}
	
const TDesC& CIntegrityTreeLeaf::Name()
	{
	return *iFileName;
	}
	
const TDesC& CIntegrityTreeLeaf::Journal()
	{
	return iOwningJournal;
	}
	
void CIntegrityTreeLeaf::SetPeer(CIntegrityTreeLeaf* aPeer)
	{
	iPeer = aPeer;
	} 
	
CIntegrityTreeNode* CIntegrityTreeNode::NewL(const TDesC& aDirectoryName)
	{
	CIntegrityTreeNode* self = CIntegrityTreeNode::NewLC(aDirectoryName);
	CleanupStack::Pop(self);
	return self;
	}

CIntegrityTreeNode* CIntegrityTreeNode::NewLC(const TDesC& aDirectoryName)
	{
	CIntegrityTreeNode* self = new (ELeave) CIntegrityTreeNode;
	CleanupStack::PushL(self);
	self->ConstructL(aDirectoryName);
	return self;
	}
	
	
const TDesC& CIntegrityTreeNode::Name()
	{
	return *iDirName;
	}
	
CIntegrityTreeLeaf* CIntegrityTreeNode::AddNodeL(const TDesC& aFileName, TIntegrityServicesEvent aType, const TDesC& aOwningJournal)
	{
	RBuf filename;
	filename.CreateL(aFileName, aFileName.Length());
	CleanupClosePushL(filename);
	CIntegrityTreeLeaf* leaf = DoAddNodeL(filename, aType, aOwningJournal);
	CleanupStack::PopAndDestroy(&filename);
	return leaf;
	}

TInt CIntegrityTreeNode::FindNode(const TDesC& aFileName, TIntegrityServicesEvent aType)
	{
	RBuf filename;
	TInt found = 0;
	TRAPD(err, 
			filename.CreateL(aFileName, aFileName.Length());
			CleanupClosePushL(filename);
			found = DoFindNode(filename, aType);
			CleanupStack::PopAndDestroy(&filename);
		);
	if (err != KErrNone)
		{
		return err;
		}
	return found;
	}

void CIntegrityTreeNode::WalkTreeL(TTreeWalkFunctionL aFunc, TIntegrityServicesEvent aTypeFilter,
	RFs& aFs, RLoader& aLoader, CFileMan& aFileMan,
	CIntegrityServices& aIntegrityServices, CIntegrityServices::TFailType aFailType)
	{
	RBuf filename;
	filename.CreateL(*iDirName, KMaxFileName);
	CleanupClosePushL(filename);
	DoWalkTreeL(filename, aFunc, aTypeFilter, aFs, aLoader, aFileMan, aIntegrityServices, aFailType);
	CleanupStack::PopAndDestroy(&filename);
	}
	
CIntegrityTreeLeaf* CIntegrityTreeNode::DoAddNodeL(TDes& aFileName, TIntegrityServicesEvent aType, const TDesC& aOwningJournal)
	{
	TInt pathDividerPos = aFileName.Locate(KPathDelimiter);
	User::LeaveIfError(pathDividerPos); // The must be at least /this/ directory name
	
	TPtrC dirPtr(aFileName.Left(pathDividerPos));
	if (dirPtr.CompareF(*iDirName) != 0)
		{
		User::Leave(KErrBadName); // this doesn't belong to us.
		}
	
	// remove the directory section of the path.
	aFileName.Delete(0, pathDividerPos+1);
	
	// Now, see if we need to pass it on up the chain.
	pathDividerPos = aFileName.Locate(KPathDelimiter);
	if (pathDividerPos == KErrNotFound)
		{
		// This is now just a file name, add a new file name leaf here.
		CIntegrityTreeLeaf* leaf = CIntegrityTreeLeaf::NewLC(aFileName, aType, aOwningJournal);
		iFiles.AppendL(leaf);
		CleanupStack::Pop(leaf);
		return leaf;
		}
	else
		{
		// There are more directories in the chain... See if we have an entry...
		dirPtr.Set(aFileName.Left(pathDividerPos));
	 
		TInt dirCount(iDirectories.Count());
		for (TInt i = 0; i < dirCount; ++i)
			{
			if (dirPtr.CompareF(iDirectories[i]->Name()) == 0)
				{
				return iDirectories[i]->DoAddNodeL(aFileName, aType, aOwningJournal);
				}
			}
		// doesn't yet exist. Add it to the tree.
		CIntegrityTreeNode* node = CIntegrityTreeNode::NewLC(dirPtr);
		iDirectories.AppendL(node);
		CleanupStack::Pop(node);
		
		// pass the remaining path down the tree
		return node->DoAddNodeL(aFileName, aType, aOwningJournal);
		}
	}
	
TInt CIntegrityTreeNode::DoFindNode(TDes& aFileName, TIntegrityServicesEvent aType)
	{
	TInt pathDividerPos = aFileName.Locate(KPathDelimiter);
	if (pathDividerPos < 0)
		return pathDividerPos; // The must be at least one path delimiter in the file name	
	
	TPtrC dirPtr(aFileName.Left(pathDividerPos));
	if (dirPtr.CompareF(*iDirName) != 0)
		{
		return KErrBadName; // this doesn't belong to us.
		}
	
	// remove the directory section of the path.
	aFileName.Delete(0, pathDividerPos+1);
	
	// Now, see if we need to pass it on up the chain.
	pathDividerPos = aFileName.Locate(KPathDelimiter);
	if (pathDividerPos == KErrNotFound)
		{
		TInt nameCount(iFiles.Count());
		for (TInt i = 0; i < nameCount; ++i)
			{
			if (iFiles[i]->Type() == aType &&
				iFiles[i]->Name().CompareF(aFileName) == 0)
				{
				return KErrNone;
				}
			}
		return KErrNotFound;
		}
	else
		{
		// There are more directories in the chain... See if we have an entry...
		dirPtr.Set(aFileName.Left(pathDividerPos));
	 
		TInt dirCount(iDirectories.Count());
		for (TInt i = 0; i < dirCount; ++i)
			{
			if (dirPtr.CompareF(iDirectories[i]->Name()) == 0)
				{
				return iDirectories[i]->DoFindNode(aFileName, aType);
				}
			}
		return KErrNotFound;
		}
	}
	
void CIntegrityTreeNode::DoWalkTreeL(TDes& aPath, TTreeWalkFunctionL aFunc, 
	TIntegrityServicesEvent aTypeFilter, RFs& aFs, RLoader& aLoader, CFileMan& aFileMan,
	CIntegrityServices& aIntegrityServices, CIntegrityServices::TFailType aFailType)
	{
	aPath.Append(KPathDelimiter);
	
	// Feed all the files for this node to the walk function
	
	TInt fileCount(iFiles.Count());
	TInt i;
	for (i = 0; i < fileCount; ++i)
		{
		CIntegrityTreeLeaf* leaf = iFiles[i];
		if (leaf->Type() == aTypeFilter)
			{
			aIntegrityServices.SimulatePowerFailureL(aFailType, CIntegrityServices::EBeforeAction, leaf->Name());
			aFunc(aPath, leaf, aFs, aLoader, aFileMan);
			aIntegrityServices.SimulatePowerFailureL(aFailType, CIntegrityServices::EAfterAction, leaf->Name());
			}
		}
	
	// Walk the child nodes for this tree	
	TInt len = aPath.Length();
	TInt dirCount(iDirectories.Count());
	for (i = 0; i < dirCount; ++i)
		{
		CIntegrityTreeNode* node = iDirectories[i];
		aPath.SetLength(len);
		aPath.Append(node->Name());
		node->DoWalkTreeL(aPath, aFunc, aTypeFilter, aFs, aLoader, aFileMan, aIntegrityServices, aFailType);
		}
	}
	
CIntegrityTreeNode::CIntegrityTreeNode()
	{
	}
	
void CIntegrityTreeNode::ConstructL(const TDesC& aDirectoryName)
	{
	iDirName = aDirectoryName.AllocL();
	}
	
CIntegrityTreeNode::~CIntegrityTreeNode()
	{
	delete iDirName;
	iDirectories.ResetAndDestroy();
	iFiles.ResetAndDestroy();
	}
