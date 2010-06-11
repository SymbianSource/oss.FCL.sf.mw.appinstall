/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#include "drivewatcher.h"
#include "SWInstDebug.h"

using namespace Swi;

// CDriveWatcher
_LIT(KNotificationDirectory,"mediachange\\");


// -----------------------------------------------------------------------
// CDriveWatcher::NewL
// -----------------------------------------------------------------------
//    
/*static*/ 
CDriveWatcher* CDriveWatcher::NewL( RFs& aFs, 
                                    TInt aDrive, 
                                    MDriveObserver& aObserver,
                                    TInt aPriority )
	{
	CDriveWatcher* self = NewLC( aFs, aDrive, aObserver, aPriority );
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
// 
/*static*/ 
CDriveWatcher* CDriveWatcher::NewLC( RFs& aFs, 
                                     TInt aDrive, 
                                     MDriveObserver& aObserver,
                                     TInt aPriority )
	{
	CDriveWatcher* self = new(ELeave) CDriveWatcher( aFs, 
	                                                 aDrive, 
	                                                 aObserver, 
	                                                 aPriority );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;	
	}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
//
CDriveWatcher::~CDriveWatcher()
	{
	Cancel();
	}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
//
CDriveWatcher::CDriveWatcher( RFs& aFs, 
                              TInt aDrive, 
                              MDriveObserver& aObserver,
                              TInt aPriority )
                              : CActive(aPriority), 
                                iFs(aFs), 
                                iDrive(aDrive), 
                                iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
//
void CDriveWatcher::ConstructL()
	{
	// Notify observer of media change since we're beginning 
    // from an unknown state
	NotifyMediaChange();
	
	// Start watching for changes
	WaitForChangeL();
	}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
//
void CDriveWatcher::DoCancel()
	{
	iFs.NotifyChangeCancel(iStatus);
	}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
//
TBool CDriveWatcher::IsMediaPresentL()
	{
    FLOG_1( _L("Daemon: Checking media presence for drive %d"), iDrive );
        
	TVolumeInfo volumeInfo;
	TInt err = iFs.Volume( volumeInfo, iDrive );
	
	switch ( err )
		{
		case KErrNotReady: // No Media present
			{
			return EFalse;	
			}
			
		case KErrNone: // Media Present
			{
			return ETrue;
			}
		}

	User::Leave( err );	
	return ETrue;	// Will never get here.
	}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
//
void CDriveWatcher::NotifyMediaChange()
	{
    FLOG( _L("Daemon: NotifyMediaChange") );        
	// Unsuccessful media change is not fatal, so handle here
	TRAPD( err, iObserver.MediaChangeL( iDrive, 
	                                    IsMediaPresentL() 
	                                    ? MDriveObserver::EMediaInserted : 
                                        MDriveObserver::EMediaRemoved));

	if (err != KErrNone)
		{
        FLOG_1(_L("Daemon: MediaChangeL TRAP err = %d"), err );
		}              
	}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
//	
void CDriveWatcher::RunL()
	{
	NotifyMediaChange();
			
	WaitForChangeL();
	}

// -----------------------------------------------------------------------
// 
// -----------------------------------------------------------------------
//
void CDriveWatcher::WaitForChangeL()
	{
	TChar drive;
	User::LeaveIfError( iFs.DriveToChar( iDrive, drive ) );
	TUint driveChar(drive); // Can't pass TChar to Format().
	
	TPath notificationPath;
	TPath privatePath;
	_LIT(KNotificationPathFormat,"%c:%S%S");
	User::LeaveIfError( iFs.PrivatePath( privatePath ) );

	notificationPath.Format( KNotificationPathFormat, 
	                         driveChar, 
	                         &privatePath, 
	                         &KNotificationDirectory );	
	
	iFs.NotifyChange( ENotifyEntry, iStatus, notificationPath );

	SetActive();
	}
	
//EOF
