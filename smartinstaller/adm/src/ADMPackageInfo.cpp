/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     CPackageInfo implementation
*
*
*/


#include <e32base.h>
#include <f32file.h>
#include <sysutil.h>        // SysUtil::DiskSpaceBelowCriticalLevel()

#include "ADMXmlParser.h"
#include "ADMPackageInfo.h"
#include "ADMDownloadHandler.h"
#include "ADMAppUi.h"

#include "macros.h"

//TODO: Right place?!
// The extension of the changes file
_LIT(KChangesExt,"_changes.xml");

CPackageInfo* CPackageInfo::NewLC()
	{
	CPackageInfo* object = new ( ELeave ) CPackageInfo();
	CleanupStack::PushL( object );
	object->ConstructL();
	return object;
	}

CPackageInfo* CPackageInfo::NewL()
	{
	CPackageInfo* object = CPackageInfo::NewLC();
	CleanupStack::Pop();
	return object;
	}

void CPackageInfo::ConstructL()
	{
	}

CPackageInfo::CPackageInfo() :
	iPackageStatus(EPackageStatusUnknown),
	iEdgeDlinkHdr(_FOFF(Edge, iEdgeDlink)),
	iEdgeDlinkIter(iEdgeDlinkHdr)
	{
	}

CPackageInfo::~CPackageInfo()
	{
	Edge* currentitem;

	iEdgeDlinkIter.SetToFirst();

	// Clear the memory allocated for all Edges.
	while((currentitem = iEdgeDlinkIter++) != NULL)
		{
		currentitem->iEdgeDlink.Deque();
		delete currentitem;
		};

	DELETE_IF_NONNULL( iPackageName );
	DELETE_IF_NONNULL( iVendor );
	DELETE_IF_NONNULL( iDateOfSubmission );
	DELETE_IF_NONNULL( iDateOfModification );
	DELETE_IF_NONNULL( iUrl );
	DELETE_IF_NONNULL( iDownloadUrl );
	DELETE_IF_NONNULL( iDepFileName );
	DELETE_IF_NONNULL( iChangesFileName );
	DELETE_IF_NONNULL( iSisPackageName );
	}

void CPackageInfo::AddEdgeL(CPackageInfo* aDest)
	{
	User::LeaveIfNull(aDest);

	Edge* newEdge = Edge::NewL(this, aDest);
	if (iEdgeDlinkHdr.IsEmpty())
		{
		// This is the first Edge in the list
		iEdgeDlinkHdr.AddFirst(*newEdge);
		iEdgeDlinkIter.SetToFirst();
		}
	else
		{
		// Append Edge to the list
		iEdgeDlinkHdr.AddLast(*newEdge);
		iEdgeDlinkIter.SetToLast();
		}
	}

void CPackageInfo::VisitL(CDepTree *aDepTree)
	{
	// TODO: Proper error handling
	User::LeaveIfNull(aDepTree);

	Edge *e;
	CPackageInfo *temp;

	if (iVisited == BEING_VISITED)
		{
		// Cycle is detected
		aDepTree->iCyclePresent = ETrue;
		return;
		}
	if (iVisited == DONE_VISITED)
		{
		// Node already visited added to the fetch list
		return;
		}

	iVisited = BEING_VISITED;

	// Visit all the Edges first
	while(iEdgeDlinkIter!= NULL)
		{
		e = iEdgeDlinkIter--;
		temp = e->GetVertexDst();
		temp->VisitL(aDepTree);
		}
	iVisited = DONE_VISITED;

	aDepTree->AddFetchPackageInfo(this);
	}

void CPackageInfo::SetDepFileNameL(const TDesC& aName)
	{
	__ASSERT_ALWAYS( aName.Length() > 0, User::Leave( KErrArgument) );

	DELETE_IF_NONNULL( iDepFileName );
	iDepFileName = aName.AllocL();
	}

//TODO : Clean possible?
void CPackageInfo::SetChangesFileNameL()
	{
	DELETE_IF_NONNULL( iChangesFileName );

#ifdef FEATURE_CHANGES_USE_UID
	//By default, use Packageuid as the Changes filename.
	//set the length as ( PackageUIDLength (8) + Changes_ext length )
	iChangesFileName = HBufC::NewL(8 + KChangesExt().Length());
	TPtr nameBufPtr = iChangesFileName->Des();
	nameBufPtr.Num(GetPackageUid(), EHex);
	nameBufPtr.Append(KChangesExt);
#else
	//Use Packagename as the Changes filename.
	// If package name is NULL, then Package UID is used as Changes file name.
	if( GetPackageName() != NULL )
		{
		iChangesFileName = HBufC::NewL( GetPackageName()->Length() +
										KChangesExt().Length() );
		TPtr nameBufPtr = iChangesFileName->Des();
		nameBufPtr.Copy(GetPackageName()->Des());

		const TInt nameLength = GetPackageName()->LocateReverse('.');

		if (nameLength != KErrNotFound)
			{
			//Remove the extension.
			nameBufPtr.Delete( nameLength, GetPackageName()->Length() - nameLength);
			}
		nameBufPtr.Append(KChangesExt);
		}
	else
		{
		iChangesFileName = HBufC::NewL(8 + KChangesExt().Length());
		TPtr nameBufPtr = iChangesFileName->Des();
		nameBufPtr.Num(GetPackageUid(), EHex);
		nameBufPtr.Append(KChangesExt);
		}
#endif
	}

void CPackageInfo::SetSisFileNameL(const TDesC& aName)
	{
	__ASSERT_ALWAYS( aName.Length() > 0, User::Leave( KErrArgument) );

	DELETE_IF_NONNULL( iSisPackageName );
	iSisPackageName = aName.AllocL();
	}

// -----------------------------------------------------------------------------
// Creates download URL for the package. The constructed URL is stored in
// member variable iDownloadUrl. The URL is constructed only once, the
// subsequent calls to this return iDownloadUrl.
//
//     base_url/package/version/dep_file
// i.e.
//     http://server.somewhere.net/root/qt/4.6.1/qt_dep.xml
// -----------------------------------------------------------------------------
//
HBufC8* CPackageInfo::GetDownloadUrlL()
	{
#ifdef FEATURE_CHANGES_DOWNLOAD_URL
	if (iDownloadUrl)
		{
		return iDownloadUrl;
		}

	// Url should never be null as specifying
	// the Url in changes file is mandatory
	User::LeaveIfNull(iUrl);

	TInt len = iUrl->Des().Length();
#ifdef FEATURE_CHANGES_USE_UID
	len += 8; // UID length as HEX string // iPackageName->Length();
#else
	len += iPackageName->Length();
#endif

	// TVersion: iMajor(8bit), iMinor(8bit), iBuild(16bit) = 255 255 65535
	// delimiters: /qt/4.6.1/ => //../ = (len=5)
	len += (3+3+5 + 5);

	HBufC8* url = HBufC8::NewL(len);
	TPtr8 urlPtr = url->Des();
	// Construct the download path:
	//     base_url/package/version/dep_file
	// i.e.
	//     http://server.somewhere.net/root/qt/4.6.1/qt_dep.xml
	urlPtr.Copy(*iUrl);

	// Append forward slash if that's missing
	if (urlPtr.Right(1)[0] != '/')
		{
		urlPtr.Append('/');
		}

#ifdef FEATURE_CHANGES_USE_UID
	urlPtr.AppendNum(iPackageUid, EHex);
#else
	urlPtr.Append(iPackageName->Des());
#endif
	urlPtr.Append('/');
	urlPtr.AppendNum(iVersion.iMajor);
	urlPtr.Append('.');
	urlPtr.AppendNum(iVersion.iMinor);
	urlPtr.Append('.');
	urlPtr.AppendNum(iVersion.iBuild);
	urlPtr.Append('/');

	iDownloadUrl = url;

	return url;
#else
	// Url should never be null as specifying
	// the Url in changes file is mandatory
	User::LeaveIfNull(iUrl);
	return iUrl;
#endif // FEATURE_CHANGES_DOWNLOAD_URL
	}

#ifdef USE_LOGFILE
CDepTree* CDepTree::NewLC(RFileLogger& aLogger, const TDesC& aDownloadPath)
	{
	CDepTree* object = new ( ELeave ) CDepTree(aLogger);
#else
CDepTree* CDepTree::NewLC(const TDesC& aDownloadPath)
	{
	CDepTree* object = new ( ELeave ) CDepTree();
#endif
	CleanupStack::PushL( object );
	object->ConstructL(aDownloadPath);
	return object;
	}

#ifdef USE_LOGFILE
CDepTree* CDepTree::NewL(RFileLogger& aLogger, const TDesC& aDownloadPath)
	{
	CDepTree* object = CDepTree::NewLC(aLogger, aDownloadPath);
#else
CDepTree* CDepTree::NewL(const TDesC& aDownloadPath)
	{
	CDepTree* object = CDepTree::NewLC(aDownloadPath);
#endif
	CleanupStack::Pop();
	return object;
	}

#ifdef USE_LOGFILE
CDepTree::CDepTree(RFileLogger& aLogger) :
	iLog(aLogger),
#else
CDepTree::CDepTree() :
#endif
	iCurrentPackage(-1),
	iSortedPackage(-1),
	iCyclePresent(EFalse),
	iPackageDlinkHdr(_FOFF(CPackageInfo,iPackageDlink)),
	iPackageDlinkFetchIter(iPackageDlinkHdr)
	{
	iFetchList.Reset();
	iSortedList.Reset();
	}

void CDepTree::ConstructL(const TDesC& aDownloadPath)
	{
	iDownloadPath = aDownloadPath.AllocL();
	}

CDepTree::~CDepTree()
	{
	DELETE_IF_NONNULL( iDownloadPath );

	CPackageInfo* currentPackage;
	iPackageDlinkFetchIter.SetToFirst();

	TDblQueIter<CPackageInfo> PackageIter(iPackageDlinkHdr);
	PackageIter.SetToFirst();

	// Clear memory allocated for all packages
	while((currentPackage = PackageIter++) != NULL)
		{
		currentPackage->iPackageDlink.Deque();
		delete currentPackage;
		};

	iFetchList.Close();
	iSortedList.Close();
	}

const TLinearOrder<CPackageInfo> CPackageInfo::KSortOrderByDrivePriority(CPackageInfo::CompareByDrivePriority);

TInt CPackageInfo::CompareByDrivePriority( const CPackageInfo& aPackageOne, const CPackageInfo& aPackageTwo )
{
	if ( (aPackageOne.GetDrivePriority()) > (aPackageTwo.GetDrivePriority()) )
		{
		return -1;
		}
	else
		{
		return 1;
		}
}

TBool CDepTree::ConstructFetchListL()
	{
	if(iPackageDlinkHdr.IsEmpty())
		{
		return EFalse;
		}

	CPackageInfo *rootNode = iPackageDlinkHdr.First();
	rootNode->VisitL(this);
	return ETrue;
	}

TBool CDepTree::SetDriveInfo()
	{
#ifdef FEATURE_INSTALL_DRIVE_SELECTION
	TInt driveIndex = 0;
	CPackageInfo* currentNode = NULL;
	TBool retStatus = ETrue; // Default to successful check
	RArray<TChar> driveLetters(EDriveZ-EDriveA+1);
	RArray<TInt64> driveSpaces(EDriveZ-EDriveA+1);
	RFs fs;

	if (fs.Connect() != KErrNone)
		{
		return EFalse;
		}

	CleanupClosePushL(fs);
	CleanupClosePushL(driveLetters);
	CleanupClosePushL(driveSpaces);

	// Get the drive letters and spaces
	TRAPD(err, GetDriveListL(fs, driveLetters, driveSpaces) );
	if (err != KErrNone)
		{
		return EFalse;
		}

	// The packages are sorted based on priority
	for (TInt index = 0; (index < iSortedList.Count()) && (retStatus != EFalse); index++ )
		{
		//LOG2( "Index is %d", index );

		currentNode = iSortedList[index];

		const TChar mandatoryDriveLetter = currentNode->GetMandatoryInstallDrive();

		if ( mandatoryDriveLetter )
			{
			// Reset the driveIndex for drive selection of each package.
			for ( driveIndex = 0; driveIndex < driveSpaces.Count(); driveIndex++ )
				{
				// Check whether the current drive is the mandatory drive.
				if ( driveLetters[driveIndex] == mandatoryDriveLetter )
					{
					// Check whether there is enough space in mandatory drive.
					if ( currentNode->GetInstallSize() < driveSpaces[driveIndex] )
						{
						LOG3( "PkgUid 0x%08X installation forced to %c:", currentNode->GetPackageUid(), (char)driveLetters[driveIndex] );
						currentNode->SetInstallDrive(driveLetters[driveIndex]);
						driveSpaces[driveIndex] -= currentNode->GetInstallSize();
						}
					else
						{
						retStatus = EFalse;
						}
					break;
					}
				}
			}// if ( mandatoryDriveLetter )
		else
			{
			// driveIndex is reset -> there could be a smaller package which fits in previous drive.
			for ( driveIndex = 0; driveIndex < driveSpaces.Count(); driveIndex++ )
				{
				if (currentNode->GetInstallSize() < driveSpaces[driveIndex])
					{
					currentNode->SetInstallDrive(driveLetters[driveIndex]);
					driveSpaces[driveIndex] -= currentNode->GetInstallSize();
					break;
					}
				}
			}// else of if ( mandatoryDriveLetter )

		if ( driveSpaces.Count() <= driveIndex )
			{
			//Invalid mandatory drive specified.
			retStatus = EFalse;
			}
		}// for loop of packages

	// All the dependent sis files are assigned the drives where they need to be installed.
	// Now go thru the drives to figure out which one can accommodate the download.

	// This is done as a final act so that the memory is not blocked unnecessarily by download,
	// which otherwise could be used to install a dependency sis.

	// Reset the driveIndex -> start from system drive.
	for ( driveIndex = 0; ( driveIndex < driveSpaces.Count() ) && ( retStatus != EFalse ); driveIndex++ )
		{
/*
		TInt drive;
		RFs::CharToDrive(driveLetters[driveIndex], drive);
		if (!SysUtil::DiskSpaceBelowCriticalLevelL(&fs, iMaxDownloadSize, drive))
			{
*/
		if ( driveSpaces[driveIndex] > iMaxDownloadSize )
			{
			driveSpaces[driveIndex] -= iMaxDownloadSize;
			iDownloadDrive = driveLetters[driveIndex];
			//Drive specified in download path is overriden.
			iDownloadPath->Des()[0] = iDownloadDrive;
			LOG4( "Download drive %c: free %ld (%.02f MB)", (char)iDownloadDrive, driveSpaces[driveIndex], (TReal)driveSpaces[driveIndex]/1048576.0 );
			break;
			}
		}

	// Fail, if no drive has space to max download size.
	if ( driveSpaces.Count() <= driveIndex )
		{
		// Fail: no memory.
		retStatus = EFalse;
		}

	CleanupStack::PopAndDestroy(3, &fs);

	return retStatus;

#else
	TInt64 driveSpace = iMaxDownloadSize;
	CPackageInfo* currentNode = NULL;
	const TInt installDrive = RFs::GetSystemDrive();
	TChar instDriveLetter;
	TInt err;
	TBool retStatus = ETrue; // Default to successful check
	RFs fs;

	err = fs.Connect();
	if (err != KErrNone)
		return EFalse;

	CleanupClosePushL(fs);

	RFs::DriveToChar(installDrive, instDriveLetter);

	// The install size of all dependent packages to be installed is added to the required driveSpace.
	for (TInt index = 0; index < iSortedList.Count(); index++ )
		{
		currentNode = iSortedList[index];
		driveSpace += currentNode->GetInstallSize();
		// Set the installation drive for the package
		currentNode->SetInstallDrive(instDriveLetter);
		}

	// check whether there is enough space in C to download and install all dependencies.
	TRAP( err, retStatus = !SysUtil::DiskSpaceBelowCriticalLevelL( &fs, driveSpace, installDrive ) );

	// If free space check fails, indicate not enough space
	if (err != KErrNone)
		retStatus = EFalse;

	CleanupStack::PopAndDestroy(&fs);

	return retStatus;
#endif
	}

void CDepTree::GetDriveListL(RFs& aRFs, RArray<TChar>& aDriveLetters, RArray<TInt64>& aDriveSpaces)
	{
	// This is the LFSS free space threshold
	const TInt freeSpaceAdjustment = 1024 * 384;

	// get information about drives
	TDriveList driveList;

	// List all drives in the system
	User::LeaveIfError( aRFs.DriveList(driveList) );

	TVolumeInfo volInfo;
	TInt64 volSpace = 0;
	TChar systemDriveLetter;
	const TInt systemDrive = RFs::GetSystemDrive();

	// Not sure whether the first DriveLetter in the while loop corresponds to SystemDrive.
	// Hence setting it explicitly as the first entry in Drive Array
	RFs::DriveToChar(systemDrive, systemDriveLetter);
	if (aRFs.Volume(volInfo, systemDrive) == KErrNone)
		{
		LOG4( "SysDrive %c: Free %ld (%.02f MB)", (char)systemDriveLetter, volInfo.iFree, (TReal)volInfo.iFree/1048576.0 );
		volSpace = volInfo.iFree - freeSpaceAdjustment;  // bytes
		if (volSpace < 0)
			{
			volSpace = 0;
			}
		}

	User::LeaveIfError(aDriveLetters.Append(systemDriveLetter));
	User::LeaveIfError(aDriveSpaces.Append(volSpace));

	// Check all drives
	for (TInt driveNumber = EDriveA; driveNumber <= EDriveZ; driveNumber++)
		{

		if (!driveList[driveNumber])
			{
			// Not a recognised drive
			continue;
			}

		if (aRFs.Volume(volInfo, driveNumber) != KErrNone)
			{
			// The volume is not usable (e.g. no media card inserted)
			continue;
			}

		if (driveNumber == systemDrive)
			{
			//System Drive, already added to the list
			continue;
			}

		if ( (volInfo.iDrive.iType==EMediaNotPresent) ||
			 (volInfo.iDrive.iType==EMediaRom) ||
			 (volInfo.iDrive.iType==EMediaRemote) ||
			 (volInfo.iDrive.iType==EMediaRam) ||
			 (volInfo.iDrive.iType==EMediaUnknown) )
			{
			// Exclude drives not suitable for installation
			continue;
			}

		// Do not list read only and substituted drives as an option to install to
		if (volInfo.iDrive.iDriveAtt & KDriveAttRom ||
			volInfo.iDrive.iDriveAtt & KDriveAttSubsted)
			{
			continue;
			}

		const TInt64 volSpace = volInfo.iFree - freeSpaceAdjustment;  // bytes
		if (volSpace < 0)
			{
			// Volume space below 0 => skip drive
			continue;
			}

		TChar drvLetter;
		User::LeaveIfError(RFs::DriveToChar(driveNumber, drvLetter));
		User::LeaveIfError(aDriveLetters.Append(drvLetter));
		User::LeaveIfError(aDriveSpaces.Append(volSpace));
		LOG4( "Drive %c: Free %ld (%.02f MB)", (char)drvLetter, volSpace, (TReal)volSpace/1048576.0 );
		} // for
	}

TBool CDepTree::IsDepTreeEmpty() const
	{
	return (iPackageDlinkHdr.IsEmpty());
	}

TBool CDepTree::IsCyclePresent() const
	{
	return iCyclePresent;
	}

void CDepTree::AddPackageInfo(CPackageInfo *aPackageInfo)
	{
	if(iPackageDlinkHdr.IsEmpty())
		{
		// This is the first node in the list
		iPackageDlinkHdr.AddFirst(*aPackageInfo);
		iPackageDlinkFetchIter.SetToFirst();
		}
	else
		{
		// Append node to the list
		iPackageDlinkHdr.AddLast(*aPackageInfo);
		}
	}

TPtrC CDepTree::GetDownloadPath() const
	{
	return iDownloadPath->Des();
	}

void CDepTree::AddFetchPackageInfo(CPackageInfo *aPackageInfo)
	{
	iFetchList.Append(aPackageInfo);
	//Add the download/install sizes only if the package is not already installed.
	if(aPackageInfo->iPackageStatus != EPackageInstalled)
		{
		iSortedList.InsertInOrder(aPackageInfo,CPackageInfo::KSortOrderByDrivePriority);
		iTotalDownloadSize += aPackageInfo->iDownloadSize;

		if(aPackageInfo->iDownloadSize > iMaxDownloadSize)
			{
			iMaxDownloadSize = aPackageInfo->iDownloadSize;
			}
		}
	}

CPackageInfo* CDepTree::LocatePackageInDepTree(const TUint32& aDepPackageUid)
	{
	CPackageInfo* currentItem;
	TDblQueIter<CPackageInfo> PackageIter(iPackageDlinkHdr);
	PackageIter.SetToFirst();

	while((currentItem = PackageIter++) != NULL)
		{
		if(currentItem->iPackageUid == aDepPackageUid)
			{
			return currentItem;
			}
		};
	return NULL;
	}

CPackageInfo* CDepTree::GetNextNode()
	{
	CPackageInfo* nextNode;

	if(iPackageDlinkFetchIter++ != NULL)
		{
		// This is because the first node is the root package and
		// tree is already created from it.
		nextNode = iPackageDlinkFetchIter;
		}
	else
		{
		// This case should never arise, as the api
		// should not get called.
		nextNode = NULL;
		}
	return nextNode;
	}

CPackageInfo* CDepTree::GetFetchNode( TInt nodeLocation)
	{
	CPackageInfo* node = NULL;

	if (iFetchList.Count() > nodeLocation )
		{
		node = iFetchList[nodeLocation];
		}
	return node;
	}


CPackageInfo* CDepTree::GetNextFetchNode()
	{
	CPackageInfo* nextNode = NULL;

	if (iFetchList.Count() > (iCurrentPackage+1) )
		{
		nextNode = iFetchList[++iCurrentPackage];
		}
	return nextNode;
	}

//TODO: Remove this. this is only for testing purpose.
//otherwise, there is never a need to get the sorted nodes separately.
CPackageInfo* CDepTree::GetNextSortedNode()
	{
	CPackageInfo* nextNode = NULL;

	if (iSortedList.Count() > (iSortedPackage+1) )
		{
		nextNode = iSortedList[++iSortedPackage];
		}
	return nextNode;
	}

CPackageInfo* CDepTree::GetPreviousFetchNode()
	{
	CPackageInfo* previousNode = NULL;
	if (iCurrentPackage > 0)
		{
		previousNode = iFetchList[--iCurrentPackage];
		}
	 return previousNode;
	}

void CDepTree::RemoveDownloadedFiles(RFs& aRfs)
	{
	CPackageInfo* currentItem;
	TDblQueIter<CPackageInfo> PackageIter(iPackageDlinkHdr);
	PackageIter.SetToFirst();

	// Remove all the downloaded dep and changes file
	while((currentItem = PackageIter) != NULL)
		{
		if ( currentItem->iDepFileName )
			{
			DeleteFile(aRfs, *(currentItem->iDepFileName), *iDownloadPath);
			}
		if ( currentItem->iChangesFileName )
			{
			DeleteFile(aRfs, *(currentItem->iChangesFileName), *iDownloadPath);
			}
		if ( currentItem->iSisPackageName )
			{
			DeleteFile(aRfs, *(currentItem->iSisPackageName), *iDownloadPath);
			}
		PackageIter++;
		}
	}

void CDepTree::DeleteFile(RFs& aRfs, const TDesC& aFileName, const TDesC& aFilePath )
	{
	TFileName filename;

	filename.Copy(aFilePath);
	filename.Append(aFileName);
	aRfs.Delete(filename);
	}

CPackageInfo* CDepTree::GetRootNode()
	{
	if (iPackageDlinkHdr.IsEmpty())
		{
		return NULL;
		}
	else
		{
		return iPackageDlinkHdr.First();
		}
	}

Edge* Edge::NewLC(CPackageInfo *aVtx1, CPackageInfo *aVtx2)
	{
	Edge* object = new ( ELeave ) Edge(aVtx1, aVtx2);
	CleanupStack::PushL( object );
	object->ConstructL();
	return object;
	}

Edge* Edge::NewL(CPackageInfo *aVtx1, CPackageInfo *aVtx2)
	{
	Edge* object = Edge::NewLC(aVtx1, aVtx2);
	CleanupStack::Pop();
	return object;
	}

void Edge::ConstructL()
	{
	}

Edge::Edge(CPackageInfo *aVtx1, CPackageInfo *aVtx2)
	{
	iVertexOrg = aVtx1;
	iVertexDst = aVtx2;
	}

CPackageInfo* Edge::GetVertexOrg() const
	{
	return iVertexOrg;
	}

CPackageInfo* Edge::GetVertexDst() const
	{
	return iVertexDst;
	}
