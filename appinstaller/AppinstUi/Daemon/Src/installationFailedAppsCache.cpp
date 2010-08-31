/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <f32file.h>
#include <s32file.h>
#include <pathinfo.h>

#include "sisregistrysession.h"
#include "sisregistryentry.h"
#include "sisregistrypackage.h"
#include "installationFailedAppsCache.h"
#include "SWInstDebug.h"

using namespace Swi;

_LIT(KInstFaildeApps, "c:\\private\\10202DCE\\instFailedAppsCache.dat");


// ----------------------------
// Borrowed from cleanuputils.h:-
/** 
 * Template class CleanupResetAndDestroy to clean up the array
 * of implementation information from the cleanup stack.
 * @released
 * @publishedPartner
 *
 */

template <class T>
class CleanupResetAndDestroy
	{
public:
	inline static void PushL(T& aRef);
private:
	static void ResetAndDestroy(TAny *aPtr);
	};
	
template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef);

template <class T>
inline void CleanupResetAndDestroy<T>::PushL(T& aRef)
	{
	CleanupStack::PushL(TCleanupItem(&ResetAndDestroy,&aRef));
	}

template <class T>
void CleanupResetAndDestroy<T>::ResetAndDestroy(TAny *aPtr)
	{
	static_cast<T*>(aPtr)->ResetAndDestroy();
	}

template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef)
	{
	CleanupResetAndDestroy<T>::PushL(aRef);
	}

// End of code from swi/inc/cleanuputils.h


// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
//    
CInstallationFailedAppsCache *CInstallationFailedAppsCache::NewL()
	{
	FLOG( _L("Daemon: CInstallationFailedAppsCache::NewL") ); 
	
	CInstallationFailedAppsCache *self = new(ELeave)CInstallationFailedAppsCache;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
//   
CInstallationFailedAppsCache::~CInstallationFailedAppsCache()
	{
	iUidsArray.Reset();
	}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
//   
void CInstallationFailedAppsCache::AddPackageUID( TUid aUid )
	{
    FLOG( _L("Daemon: CInstallationFailedAppsCache::AddPackageUID") );
    FLOG_1( _L("Daemon: AddPackageUID: Add UID = 0x%x"), aUid.iUid );
	
	(void)iUidsArray.InsertInSignedKeyOrder( aUid );
	// New UID added to array. Let's write cache to disk.
	iNewUID = ETrue;		
	}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
//   
TBool CInstallationFailedAppsCache::HasPreviousInstallationFailed( 
        TUid aUid )
	{
	FLOG( _L("Daemon: FailedAppsCache::HasPreviousInstallationFailed") );
	
	if ( iUidsArray.FindInSignedKeyOrder( aUid ) == KErrNotFound )
		{
		FLOG( _L("Daemon: UID not found. No previous install attempt") );
		return EFalse;
		}
	
	FLOG( _L("Daemon: UID found. Previous install attempt FAILED") );	
	return ETrue;
	}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
//   
CInstallationFailedAppsCache::CInstallationFailedAppsCache()
	{
	}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
//   
void CInstallationFailedAppsCache::ConstructL()
	{
	User::LeaveIfError( iFs.Connect() );
	TInt drive = 0;
    iFs.CharToDrive( TParsePtrC( 
            PathInfo::PhoneMemoryRootPath() ).Drive()[0], drive );
    
    iFs.CreatePrivatePath( drive );
    
    iNewUID = EFalse;

	// Read cache file
	TRAP_IGNORE( InitFromCacheFileL() );
	}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
//   
void CInstallationFailedAppsCache::InitFromCacheFileL()
	{
	FLOG( _L("Daemon: CInstallationFailedAppsCache::InitFromCacheFileL") );	
	
	// Read in existing cache file.
	RFile cacheFile;
	
	TInt err = cacheFile.Open( iFs, 
	                           KInstFaildeApps, 
	                           EFileStream|EFileRead );
	
	if ( err != KErrNone )
		{
		FLOG( _L("Daemon: No existing cache file to read.") );
		return; // No existing cache file to read.
		}
		
	CleanupClosePushL(cacheFile);
	
	// Now read the cache
	RFileReadStream cacheReadStream(cacheFile);
	cacheReadStream.PushL();

	iUidsArray.Reset();
	
	TInt32 count( cacheReadStream.ReadInt32L() );	

	for ( TInt i = 0; i < count; i++ )
		{		
		TUid packageId;
		packageId.iUid = cacheReadStream.ReadInt32L();
		
		(void)iUidsArray.InsertInSignedKeyOrder( packageId );
		}	
	
	CleanupStack::PopAndDestroy(&cacheReadStream);
	CleanupStack::PopAndDestroy(&cacheFile);
    }

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
//   
void CInstallationFailedAppsCache::FlushToDiskL()
	{
    FLOG( _L("Daemon: CInstallationFailedAppsCache::FlushToDiskL") );		
	
	// Write to disk if new UID is added to array. 
	if ( iNewUID )
    	{
    	FLOG( _L("Daemon: Write cache to disk") );
    	RFile cacheFile;
    	TInt err = cacheFile.Open( iFs, 
    	                           KInstFaildeApps, 
    	                           EFileStream|EFileWrite );
    	
    	// If cache was not found, create cache file.
    	if ( err != KErrNone )
    		{
    		FLOG( _L("Daemon: Create cache file.") );
    	    User::LeaveIfError( cacheFile.Create( iFs, 
    	                                          KInstFaildeApps, 
    	                                          EFileStream|EFileWrite ) );
    		}
    	
    	CleanupClosePushL(cacheFile);

    	// Truncate file.
    	User::LeaveIfError( cacheFile.SetSize(0) );
    	
    	// Now write the cache 
    	RFileWriteStream cacheWriteStream( cacheFile );
    	cacheWriteStream.PushL();

    	TInt32 count( iUidsArray.Count() );
    	cacheWriteStream.WriteInt32L( count );

    	for ( TInt i = 0; i < count; i++ )
    		{    		
    		cacheWriteStream.WriteInt32L( iUidsArray[i].iUid );
    		}	
    	
    	cacheWriteStream.CommitL();
    	FLOG( _L("Daemon: Commit cache file.") );
    	
    	CleanupStack::PopAndDestroy(&cacheWriteStream);
    	CleanupStack::PopAndDestroy(&cacheFile);
    	
    	// Ok, UIDs are written to disk, set boolen to false.
    	iNewUID = EFalse;	    
    	}
	}

// End of file
