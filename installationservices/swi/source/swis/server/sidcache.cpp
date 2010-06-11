/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "sidcache.h"
#include "securitycheckutil.h"
#include "cleanuputils.h"
#include <s32file.h>
#include <e32uid.h>
#include <f32file.h>
#include <e32ldr_private.h>
#include "securityinfo.h"

namespace Swi 
	{
	_LIT(KSidCacheFileName, "sidcache.cache");

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	CSidCache* CSidCache::NewL(Usif::RStsSession& aStsSession)
#else
	CSidCache* CSidCache::NewL(CIntegrityServices& aIntegrityServices)
#endif
		{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		CSidCache* self = CSidCache::NewLC(aStsSession);
#else
		CSidCache* self = CSidCache::NewLC(aIntegrityServices);
#endif
		CleanupStack::Pop(self);
		return self;
		
		}
		
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	CSidCache* CSidCache::NewLC(Usif::RStsSession& aStsSession)
#else
	CSidCache* CSidCache::NewLC(CIntegrityServices& aIntegrityServices)
#endif
		{
		
		CSidCache* self = new (ELeave) CSidCache;
		CleanupStack::PushL(self);
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		self->ConstructL(aStsSession);
#else
		self->ConstructL(aIntegrityServices);
#endif
		return self;
		
		}
		
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	void CSidCache::ConstructL(Usif::RStsSession& aStsSession)
#else
	void CSidCache::ConstructL(CIntegrityServices& aIntegrityServices)
#endif
		{
		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		
		iSystemDrive = RFs::GetSystemDrive();
		TChar driveChar;
		RFs::DriveToChar(iSystemDrive, driveChar);
		TUint systemDriveChar = driveChar;
				
		// Build the absolute file name of the SID cache
		_LIT(KSidCacheNameFormat, "%c:%S%S");		
		
		// 20 Max length ....Ex: Private\101F7295
		TBuf<20> privatePath;
		fs.PrivatePath(privatePath);
		
		//40 Max length ... Ex: C:\Private\101F7295\sidcache.cache
		TBuf<40> sidCacheFileName;
		sidCacheFileName.Format(KSidCacheNameFormat, systemDriveChar, &privatePath, &KSidCacheFileName); 
		
		RFile sidCacheFile;
		TInt err = sidCacheFile.Open(fs, sidCacheFileName, EFileStream|EFileRead);
		
		switch (err)
			{			
		case KErrNone:
			{
			// The file exists, so internalise it			
			CleanupClosePushL(sidCacheFile);
			RFileReadStream cacheReadStream(sidCacheFile);
			cacheReadStream.PushL();
			
			TRAP(err, InternalizeL(cacheReadStream));				
			CleanupStack::PopAndDestroy(2, &sidCacheFile);			
			
			if (err)
				{				
				// We failed to read the cache file. Delete it in case it is
				// corrupt
				RLoader loader;
				User::LeaveIfError(loader.Connect());
				CleanupClosePushL(loader);
		
				User::LeaveIfError(loader.Delete(sidCacheFileName));
		
				CleanupStack::PopAndDestroy(&loader);
				
				// Propogate the leave.
				User::Leave(err);				
				}
			break;
			}
			
		case KErrPathNotFound:
			// InstallServer's private directory does not exist
			User::LeaveIfError(fs.CreatePrivatePath(iSystemDrive));
			// fallthrough 
			
		case KErrNotFound:
			{
			// The cache file does not exist (This may be a fallthrough
			// from the state above)
			
			// Read the SIDs from the ROM /sys/bin directory
			ScanFileSystemL(fs);
			
			// Create the cache file and externalise the SIDs.
			// Add the file to STS, so we never
			// get a bad cache file on the device
			
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			aStsSession.RegisterNewL(sidCacheFileName);
#else
			aIntegrityServices.AddL(sidCacheFileName);
#endif
			User::LeaveIfError(sidCacheFile.Create(fs, sidCacheFileName, EFileStream|EFileWrite));
			CleanupClosePushL(sidCacheFile);
			
			RFileWriteStream cacheWriteStream(sidCacheFile);
			cacheWriteStream.PushL();
			
			ExternalizeL(cacheWriteStream);
			CleanupStack::PopAndDestroy(2, &sidCacheFile);
			break;
			}
			
			
		default:
			// Unexpected error opening cache file, leave
			User::LeaveIfError(err);
			break;
			
			}
		
		CleanupStack::PopAndDestroy(&fs);
		
		}
		
	void CSidCache::FindDirsL(RFs& aFs, RPointerArray<HBufC>& aSearchDirs)
		{								
		// Count() will increase if a subdirectory is encountered so don't 'optimise'
		// the loop invariant !				

		for (TInt i = 0; i < aSearchDirs.Count(); ++i) 
			{
			CDir* entries(0);
			CDir* subDirs(0);
			User::LeaveIfError(aFs.GetDir(*aSearchDirs[i], 
				KEntryAttMatchMask, 
				ESortByName, 
				entries,
				subDirs));
			delete entries;
			CleanupStack::PushL(subDirs);
		
			// Add all new directories to the directories to search								
			TInt subDirCount(subDirs->Count());		
			for (TInt j = 0; j < subDirCount; ++j)
				{
				const TEntry& entry = (*subDirs)[j];							
				TFileName name = *aSearchDirs[i];				
				name.Append(entry.iName);
				name.Append('\\');
				HBufC* tmpDirName = name.AllocLC();
				aSearchDirs.AppendL(tmpDirName);					
				CleanupStack::Pop(tmpDirName);
				}
			CleanupStack::PopAndDestroy(subDirs);
			}
		}
		
	void CSidCache::ScanFileSystemL(RFs& aFs)
		{		
		// Find all sub-directories of z:\sys\bin		
		RPointerArray<HBufC> searchDirs;		
		CleanupResetAndDestroyPushL(searchDirs);
		
		// Add the first (base) path to search
		_LIT(KOSExeBasePath, "z:\\sys\\bin\\");
		HBufC* osExeBasePath = KOSExeBasePath().AllocLC();
		searchDirs.AppendL(osExeBasePath);
		CleanupStack::Pop(osExeBasePath);	
							
		FindDirsL(aFs, searchDirs);
		
		TUidType exeFileFilter(KExecutableImageUid);		
		TInt numDirs(searchDirs.Count());
		for (TInt i = 0; i < numDirs; ++i)
			{			
			CDir* dirContents(0);
			User::LeaveIfError(aFs.GetDir(*searchDirs[i], exeFileFilter, ESortByName, dirContents));
			CleanupStack::PushL(dirContents);
			
			TInt numFiles(dirContents->Count());
			for (TInt j = 0; j < numFiles; ++j)
				{								
				TFileName name(*searchDirs[i]);				
				const TEntry& entry = (*dirContents)[j];
				name.Append(entry.iName);				

				// Ignore any files that aren't valid exes
				TSecurityInfo info;
				TRAPD(err, SecurityInfo::RetrieveExecutableSecurityInfoL(aFs, name, info));
				
				if (err == KErrNone)
					{			
					TUid sid;
					sid.iUid = info.iSecureId.iId;
					iSidArray.AppendL(sid);			
					}
				}
			CleanupStack::PopAndDestroy(dirContents);
			}
		CleanupStack::PopAndDestroy(&searchDirs);
		}
		
	TBool CSidCache::IsCachedL(TUid& aSid)
		{
		
		TInt err = iSidArray.Find(aSid);
		
		if (err == KErrNotFound)
			{
			return EFalse;
			}
		else if (err < 0)
			{
			User::Leave(err);
			}
			
		return ETrue;
		
		}
		
	void CSidCache::ExternalizeL(RWriteStream& aStream)
		{
		
		TInt32 count(iSidArray.Count());
		aStream.WriteInt32L(count);
		
		for (TInt i=0; i < count; i++)
			{
			aStream.WriteInt32L(iSidArray[i].iUid);
			}
			
		aStream.CommitL();
		
		}
	
	void CSidCache::InternalizeL(RReadStream& aStream)
		{
		iSidArray.Reset();
		TInt32 count(aStream.ReadInt32L());
		
		for (TInt i = 0; i < count; i++)
			{
			TUid sid;
			sid.iUid = aStream.ReadInt32L();
			iSidArray.AppendL(sid);
			}
			
		}
		
	CSidCache::~CSidCache()
		{
		iSidArray.Close();
		}
	}
