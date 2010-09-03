/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CNcdSilentInstallActiveObserver
*
*/


#include "ncdsilentinstallactiveobserver.h"
#include "ncdasyncsilentinstallobserver.h"

#include <usif/scr/scr.h>


CNcdSilentInstallActiveObserver* CNcdSilentInstallActiveObserver::NewL( MNcdAsyncSilentInstallObserver& aObserver )
    {
    CNcdSilentInstallActiveObserver* self = 
        CNcdSilentInstallActiveObserver::NewLC( aObserver ); 
    CleanupStack::Pop( self );
    return self;
    }


CNcdSilentInstallActiveObserver* CNcdSilentInstallActiveObserver::NewLC( MNcdAsyncSilentInstallObserver& aObserver )
    {
    CNcdSilentInstallActiveObserver* self = 
        new( ELeave ) CNcdSilentInstallActiveObserver( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


CNcdSilentInstallActiveObserver::CNcdSilentInstallActiveObserver( MNcdAsyncSilentInstallObserver& aObserver )
: CActive( CActive::EPriorityStandard ), 
  iObserver( aObserver )
    {
    
    }


void CNcdSilentInstallActiveObserver::ConstructL()
    {
    
    iResults = 0;
    
    CActiveScheduler::Add( this );
    }


CNcdSilentInstallActiveObserver::~CNcdSilentInstallActiveObserver()
    {
    // It is always good and safe to do cancel in CActive objects.
    // If silent installation is going on, this operation will
    // cancel install operation and closes the silent installer. 
    // If no operation is going on, the installer is already closed. 
    // So, no need to close the silent launcher here separately.
    
    delete (iResults);

    CancelAsyncOperation();
    }


void CNcdSilentInstallActiveObserver::DoCancel()
    {
    // Notice, that the active observer Cancel-function waits for the
    // silent launcher to send request complete before cancel of this
    // active object can finish. This is normal active object canel operation behaviour.
    // Notice, that we do not call the callback functions of the observer here
    // because we suppose that the observer has started the cancel operation itself
    // or the caller will inform the observer itself.
    
    SilentLauncher().CancelOperation();
    }


void CNcdSilentInstallActiveObserver::StartToObserveL( const TDesC& aFileName,
                                                       const Usif::COpaqueNamedParams* aSilentInstallOptions )
    {
    // For silent installation
    // Notice that if the user does not have TrustedUI capabilities
    // then this will given KErrPermissionDenied.
    // Connect to the launcher here just before it is needed, 
    // because the launcher will be shown in the application list.
    // So, it would not be nice to connect in the ConstructL and to show
    // the icon in the list all the time.
    // User::LeaveIfError( SilentLauncher().Connect() );
    User::LeaveIfError( SilentLauncher().Connect() );
    
    if (!iResults)
        {
        iResults = Usif::COpaqueNamedParams::NewL();
        }
    
    iInstaller.Install( aFileName, *aSilentInstallOptions, *iResults, iStatus ); 
        

    }


void CNcdSilentInstallActiveObserver::StartToObserveL( RFile& aFile,
                                                       const Usif::COpaqueNamedParams* aSilentInstallOptions )
    {
    // For silent installation
    // Notice that if the user does not have TrustedUI capabilities
    // then this will given KErrPermissionDenied.
    // Connect to the launcher here just before it is needed, 
    // because the launcher will be shown in the application list.
    // So, it would not be nice to connect in the ConstructL and to show
    // the icon in the list all the time.

    User::LeaveIfError( SilentLauncher().Connect() );
    
    if (!iResults)
        {
        iResults = Usif::COpaqueNamedParams::NewL();
        }
    
    iInstaller.Install( aFile, *aSilentInstallOptions, *iResults, iStatus ); 
    
    SetActive();
    }


TInt CNcdSilentInstallActiveObserver::CancelAsyncOperation()
    {
    // Normal active object cancel.
    // This will wait for the request complete.
    // So, close the launcher after request has been gotten.
    Cancel();

    // Release launcher, now that the launcher has cancelled its operation.
    SilentLauncher().Close();   

    TInt errorCode( ConvertErrorCode( iStatus.Int() ) );

    return errorCode; 
    }


void CNcdSilentInstallActiveObserver::RunL()
    {
    // This is called when the silent installer has done its job.
    // Close the installer. 
    // The launcher is shown in the application list.
    // So, it would not be nice to leave it there after operation is completed.
    
    /* Uncomment for testing
    TInt ErrCategory = 0;
    TInt ErrCode = 0;
    TInt ExtendedErrCode =0;
    
    ErrCategory = iResults->GetIntByNameL(Usif::KSifOutParam_ErrCategory, ErrCategory);
    ErrCode = iResults->GetIntByNameL(Usif::KSifOutParam_ErrCode, ErrCode);
    ExtendedErrCode = iResults->GetIntByNameL(Usif::KSifOutParam_ExtendedErrCode, ExtendedErrCode);
    SilentLauncher().Close();
    */
    
    TInt errorCode( ConvertErrorCode( iStatus.Int() ) );
    
    // Just forward the information to the observer.
    AsyncObserver().AsyncSilentInstallComplete( errorCode );
    }


MNcdAsyncSilentInstallObserver& CNcdSilentInstallActiveObserver::AsyncObserver() const
    {
    return iObserver;
    }

Usif::RSoftwareInstall& CNcdSilentInstallActiveObserver::SilentLauncher()
    {
    return iInstaller;
    }


TInt CNcdSilentInstallActiveObserver::ConvertErrorCode( TInt aErrorCode )
    {
    return aErrorCode;
    }

