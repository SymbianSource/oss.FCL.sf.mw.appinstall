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
    CActiveScheduler::Add( this );
    }


CNcdSilentInstallActiveObserver::~CNcdSilentInstallActiveObserver()
    {
    // It is always good and safe to do cancel in CActive objects.
    // If silent installation is going on, this operation will
    // cancel install operation and closes the silent installer. 
    // If no operation is going on, the installer is already closed. 
    // So, no need to close the silent launcher here separately.
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
    SilentLauncher().CancelAsyncRequest( iCancelCode );
    }


void CNcdSilentInstallActiveObserver::StartToObserveL( const TDesC& aFileName,
                                                       const SwiUI::TInstallOptionsPckg& aSilentInstallOptionsPckg )
    {
    // For silent installation
    // Notice that if the user does not have TrustedUI capabilities
    // then this will given KErrPermissionDenied.
    // Connect to the launcher here just before it is needed, 
    // because the launcher will be shown in the application list.
    // So, it would not be nice to connect in the ConstructL and to show
    // the icon in the list all the time.
    User::LeaveIfError( SilentLauncher().Connect() );
    
    iCancelCode = SwiUI::ERequestSilentInstall;
    SilentLauncher().SilentInstall( iStatus,
                                    aFileName,
                                    aSilentInstallOptionsPckg );

    SetActive();
    }


void CNcdSilentInstallActiveObserver::StartToObserveL( RFile& aFile,
                                                       const SwiUI::TInstallOptionsPckg& aSilentInstallOptionsPckg )
    {
    // For silent installation
    // Notice that if the user does not have TrustedUI capabilities
    // then this will given KErrPermissionDenied.
    // Connect to the launcher here just before it is needed, 
    // because the launcher will be shown in the application list.
    // So, it would not be nice to connect in the ConstructL and to show
    // the icon in the list all the time.
    User::LeaveIfError( SilentLauncher().Connect() );
    
    iCancelCode = SwiUI::ERequestSilentInstallHandle;
    SilentLauncher().SilentInstall( iStatus,
                                    aFile,
                                    aSilentInstallOptionsPckg );

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
    SilentLauncher().Close();

    TInt errorCode( ConvertErrorCode( iStatus.Int() ) );
    
    // Just forward the information to the observer.
    AsyncObserver().AsyncSilentInstallComplete( errorCode );
    }


MNcdAsyncSilentInstallObserver& CNcdSilentInstallActiveObserver::AsyncObserver() const
    {
    return iObserver;
    }


SwiUI::RSWInstSilentLauncher& CNcdSilentInstallActiveObserver::SilentLauncher()
    {
    return iSilentLauncher;
    }


TInt CNcdSilentInstallActiveObserver::ConvertErrorCode( TInt aErrorCode )
    {
    switch ( aErrorCode )
        {
        case SwiUI::KSWInstErrUserCancel:
            // To simplify the cancel response and cancel handling,
            // convert the error code to normal cancel error code.
            aErrorCode = KErrCancel;
            break;

        case SwiUI::KSWInstErrInsufficientMemory:
            // To simplify the insufficient memory response and its handling,
            // convert the error code to normal no memory code.
            aErrorCode = KErrNoMemory;
            break;
                    
        default:
            // Nothing to do here.
            break;
        }

    return aErrorCode;
    }
