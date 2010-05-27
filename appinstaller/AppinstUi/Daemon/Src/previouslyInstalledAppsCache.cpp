/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <usif/scr/scr.h>
#include <usif/scr/scrcommon.h>
#include <usif/scr/screntries.h>

#include "sisregistrysession.h"
#include "sisregistryentry.h"
#include "sisregistrypackage.h"
#include "SWInstDebug.h"


using namespace Swi;

// @released
// @publishedPartner

#include "previouslyInstalledAppsCache.h"

_LIT(KPreInstalledApps, "c:\\private\\10202DCE\\preInstalledAppsCache.dat");



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
// ----------------------------

// -----------------------------------------------------------------------
// CPreviouslyInstalledAppsCache::NewL
// -----------------------------------------------------------------------
//  
CPreviouslyInstalledAppsCache *CPreviouslyInstalledAppsCache::NewL()
	{
	CPreviouslyInstalledAppsCache *self = new(ELeave)CPreviouslyInstalledAppsCache;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------
// CPreviouslyInstalledAppsCache::~CPreviouslyInstalledAppsCache
// -----------------------------------------------------------------------
//  
CPreviouslyInstalledAppsCache::~CPreviouslyInstalledAppsCache()
	{
	iPrevPkgUids.Reset();
	}

// -----------------------------------------------------------------------
// CPreviouslyInstalledAppsCache::UpdateAllL
// -----------------------------------------------------------------------
//  
void CPreviouslyInstalledAppsCache::UpdateAllL()
	{
    FLOG( _L("Daemon: CPreviouslyInstalledAppsCache::UpdateAllL ") );
    
	RThread ourThread; // nb. Default constructor gives handle to current thread.
	
	if( !ourThread.HasCapability(ECapabilityReadUserData, 0) )
		{
		// Have not got ReadUserData - if we try and enumerate package
		// UIDs the registry server will panic us!
		User::Leave(KErrAccessDenied);
		}
	ourThread.Close();

	// Let's use SisRegistry in this case since in SCR there is no
	// simple way to retriev sisx pacakge UIDs in one call.
    RSisRegistrySession registrySession;
    User::LeaveIfError( registrySession.Connect() );
    CleanupClosePushL( registrySession );
    
    RPointerArray<CSisRegistryPackage> packages;
    registrySession.InstalledPackagesL( packages );
    CleanupResetAndDestroy< 
           RPointerArray<CSisRegistryPackage> >::PushL( packages );
    
    for ( TInt i = 0; i < packages.Count(); ++i )
        {
        (void)iPrevPkgUids.InsertInSignedKeyOrder( packages[i]->Uid() );
        
        FLOG_1( _L("Daemon: UpdateAllL: Add UID = 0x%x"), 
                packages[i]->Uid().iUid );
        }
    
    CleanupStack::PopAndDestroy( &packages );
    CleanupStack::PopAndDestroy( &registrySession );
	}

// -----------------------------------------------------------------------
// CPreviouslyInstalledAppsCache::UpdateAddL
// -----------------------------------------------------------------------
//  
void CPreviouslyInstalledAppsCache::UpdateAddL( TUid aUid )
	{
    FLOG( _L("Daemon: CPreviouslyInstalledAppsCache::UpdateAddL ") );
 
    // Let's use SisRegistry in this case since in SCR there is no
    // simple way to open entry with given package UIDs.
    RSisRegistrySession registrySession;
    User::LeaveIfError( registrySession.Connect() );
    CleanupClosePushL( registrySession );
    
    RSisRegistryEntry registryEntry;
    TInt err = registryEntry.Open( registrySession, aUid );
    
    if( err == KErrNone )
        {
        // Ok close entry and add UID to cache.
        registryEntry.Close();
        (void)iPrevPkgUids.InsertInSignedKeyOrder( aUid );
        FLOG_1( _L("Daemon: UpdateAddL: Add UID = 0x%x"), aUid.iUid );
        }
           
    CleanupStack::PopAndDestroy( &registrySession );
	}

// -----------------------------------------------------------------------
// CPreviouslyInstalledAppsCache::HasBeenPreviouslyInstalled
// -----------------------------------------------------------------------
//  
TBool CPreviouslyInstalledAppsCache::HasBeenPreviouslyInstalled( 
    TUid aPackageUid ) const
	{
	if ( iPrevPkgUids.FindInSignedKeyOrder( aPackageUid ) == KErrNotFound )
		{
		return EFalse;
		}
	return ETrue;
	}

// -----------------------------------------------------------------------
// CPreviouslyInstalledAppsCache::CPreviouslyInstalledAppsCache
// -----------------------------------------------------------------------
//  
CPreviouslyInstalledAppsCache::CPreviouslyInstalledAppsCache()
	{
	}

// -----------------------------------------------------------------------
// CPreviouslyInstalledAppsCache::ConstructL
// -----------------------------------------------------------------------
//  
void CPreviouslyInstalledAppsCache::ConstructL()
	{
    FLOG( _L("Daemon: CPreviouslyInstalledAppsCache::ConstructL ") );
	User::LeaveIfError(iFs.Connect());
	TInt drive = 0;
    iFs.CharToDrive( 
            TParsePtrC( PathInfo::PhoneMemoryRootPath() ).Drive()[0], 
            drive );
    
    iFs.CreatePrivatePath( drive );

	// Read cache file
	TRAP_IGNORE(InitFromCacheFileL());
	
//TODO: Test if this is really needed. Let's not do updateall 2 time in boot.		
   // TRAPD( err, UpdateAllL() );
   // if(err == KErrNone)
   //     {        
   //     TRAP_IGNORE(FlushToDiskL());
   //     }	

	}

// -----------------------------------------------------------------------
// CPreviouslyInstalledAppsCache::InitFromCacheFileL
// -----------------------------------------------------------------------
//  
void CPreviouslyInstalledAppsCache::InitFromCacheFileL()
	{
	// Read in existing cache file.
	RFile cacheFile;
	TInt err = cacheFile.Open( iFs, 
                               KPreInstalledApps, 
                               EFileStream|EFileRead);
	if ( err != KErrNone )
		{
			return; // No existing cache file to read.
		}
	CleanupClosePushL( cacheFile );
	
	// Now read the cache
	RFileReadStream cacheReadStream( cacheFile );
	cacheReadStream.PushL();

	iPrevPkgUids.Reset();
	TInt32 count( cacheReadStream.ReadInt32L() );

	for ( TInt i = 0; i < count; i++ )
		{
		TUid packageId;
		packageId.iUid = cacheReadStream.ReadInt32L();
		(void)iPrevPkgUids.InsertInSignedKeyOrder( packageId );
		}	
	
	CleanupStack::PopAndDestroy(&cacheReadStream);
	CleanupStack::PopAndDestroy(&cacheFile);
}

// -----------------------------------------------------------------------
// CPreviouslyInstalledAppsCache::FlushToDiskL
// -----------------------------------------------------------------------
//  
void CPreviouslyInstalledAppsCache::FlushToDiskL()
	{
	// Write to disk
	RFile cacheFile;
	TInt err = cacheFile.Open(iFs, KPreInstalledApps, EFileStream|EFileWrite);
	if(err != KErrNone)
		{
			User::LeaveIfError(cacheFile.Create(iFs, KPreInstalledApps, EFileStream|EFileWrite));
		}
	CleanupClosePushL(cacheFile);

	// Truncate file.
	User::LeaveIfError(cacheFile.SetSize(0));
	
	// Now write the cache
	RFileWriteStream cacheWriteStream(cacheFile);
	cacheWriteStream.PushL();

	TInt32 count(iPrevPkgUids.Count());
	cacheWriteStream.WriteInt32L(count);

	for (TInt i = 0; i < count; i++)
		{
		cacheWriteStream.WriteInt32L(iPrevPkgUids[i].iUid);
		}	
	
	cacheWriteStream.CommitL();
	CleanupStack::PopAndDestroy(&cacheWriteStream);
	CleanupStack::PopAndDestroy(&cacheFile);
	}

// End of file
