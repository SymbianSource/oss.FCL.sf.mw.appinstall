/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Test installer that uses Usif::RSoftwareInstall API.
*
*/

#include "activerunner_symbian.h"
#include "activerunner.h"


ActiveRunnerPrivate::ActiveRunnerPrivate( ActiveRunner *aRunner ) :
        CActive( CActive::EPriorityStandard ), q_ptr( aRunner )
    {
    CActiveScheduler::Add( this );
    }

ActiveRunnerPrivate::~ActiveRunnerPrivate()
    {
    Cancel();
    if( iUseSif )
        {
        iSoftwareInstall.Close();
        }
    else
        {
        iSWInstLauncher.Close();
        }
    iFs.Close();
    delete iFileName;
    delete iArguments;
    delete iResults;
    }

TInt ActiveRunnerPrivate::Initialize( bool aUseSif )
    {
    TRAPD( err, DoInitializeL( aUseSif ) );
    return err;
    }

TInt ActiveRunnerPrivate::Install( const QString& aFileName, bool aSilent, bool aOpenFile )
    {
    TRAPD( err, DoInstallL( aFileName, aSilent, aOpenFile ) );
    return err;
    }

TInt ActiveRunnerPrivate::Remove( const Usif::TComponentId& aComponentId, bool aSilent )
    {
    TRAPD( err, DoRemoveL( aComponentId, aSilent ) );
    return err;
    }

TInt ActiveRunnerPrivate::Remove( const TUid& aUid, const TDesC8& aMime, bool aSilent )
    {
    TRAPD( err, DoRemoveL( aUid, aMime, aSilent ) );
    return err;
    }

void ActiveRunnerPrivate::DoCancel()
    {
    if( iUseSif )
        {
        iSoftwareInstall.CancelOperation();
        }
    else
        {
        iSWInstLauncher.CancelAsyncRequest( SwiUI::ERequestInstall );
        }
    }

void ActiveRunnerPrivate::RunL()
    {
    User::LeaveIfError( iStatus.Int() );

    RDebug::Printf( "USIFTestInstaller: Installation completed" );
    if( q_ptr )
        {
        q_ptr->handleCompletion();
        }
    }

TInt ActiveRunnerPrivate::RunError( TInt aError )
    {
    RDebug::Printf( "USIFTestInstaller: Error %d", aError );
    if( q_ptr )
        {
        q_ptr->handleError( aError );
        }
    return KErrNone;
    }

void ActiveRunnerPrivate::DoInitializeL( bool aUseSif )
    {
    iUseSif = aUseSif;
    User::LeaveIfError( iFs.Connect() );
    User::LeaveIfError( iFs.ShareProtected() );
    if( iUseSif )
        {
        User::LeaveIfError( iSoftwareInstall.Connect() );
        }
    else
        {
        User::LeaveIfError( iSWInstLauncher.Connect() );
        }
    }

void ActiveRunnerPrivate::DoInstallL( const QString& aFileName, bool aSilent, bool aOpenFile )
    {
    if( iFileName )
        {
        delete iFileName;
        iFileName = NULL;
        }
    iFileName = HBufC16::NewL( aFileName.length() );
    TPtr16 fileName( iFileName->Des() );
    fileName.Copy( reinterpret_cast<const TText*>( aFileName.constData() ) );

    // Convert forward-slashes to backward-slashes
    const TChar KBackSlash = '\\';
    const TChar KSlash = '/';
    for( TInt i = 0; i < fileName.Length(); ++i )
        {
        if( fileName[i] == KSlash )
            {
            fileName[i] = KBackSlash;
            }
        }

    RFile fileHandle;
    if( aOpenFile )
        {
        User::LeaveIfError( fileHandle.Open( iFs, fileName, EFileRead ) );
        CleanupClosePushL( fileHandle );
        }

    if( iUseSif )
        {
        if( aSilent )
            {
            delete iArguments;
            iArguments = NULL;
            iArguments = Usif::COpaqueNamedParams::NewL();
            iArguments->AddIntL( Usif::KSifInParam_InstallSilently, 1 );

            delete iResults;
            iResults = NULL;
            iResults = Usif::COpaqueNamedParams::NewL();

            if( aOpenFile )
                {
                iSoftwareInstall.Install( fileHandle, *iArguments, *iResults, iStatus );
                }
            else
                {
                iSoftwareInstall.Install( fileName, *iArguments, *iResults, iStatus );
                }
            }
        else
            {
            if( aOpenFile )
                {
                iSoftwareInstall.Install( fileHandle, iStatus );
                }
            else
                {
                iSoftwareInstall.Install( fileName, iStatus );
                }
            }
        }
    else
        {
        if( aSilent )
            {
            SwiUI::TInstallOptions defaultOptions;
            SwiUI::TInstallOptionsPckg optPckg( defaultOptions );

            if( aOpenFile )
                {
                iSWInstLauncher.SilentInstall( iStatus, fileHandle, optPckg );
                }
            else
                {
                iSWInstLauncher.SilentInstall( iStatus, fileName, optPckg );
                }
            }
        else
            {
            if( aOpenFile )
                {
                iSWInstLauncher.Install( iStatus, fileHandle );
                }
            else
                {
                iSWInstLauncher.Install( iStatus, fileName );
                }
            }
        }

    if( aOpenFile )
        {
        CleanupStack::PopAndDestroy( &fileHandle );
        }

    SetActive();
    }

void ActiveRunnerPrivate::DoRemoveL( const Usif::TComponentId& aComponentId, bool aSilent )
    {
    if( aSilent )
        {
        delete iArguments;
        iArguments = NULL;
        iArguments = Usif::COpaqueNamedParams::NewL();
        iArguments->AddIntL( Usif::KSifInParam_InstallSilently, 1 );

        delete iResults;
        iResults = NULL;
        iResults = Usif::COpaqueNamedParams::NewL();

        iSoftwareInstall.Uninstall( aComponentId, *iArguments, *iResults, iStatus );
        }
    else
        {
        iSoftwareInstall.Uninstall( aComponentId, iStatus );
        }
    SetActive();
    }

void ActiveRunnerPrivate::DoRemoveL( const TUid& aUid, const TDesC8& aMime, bool aSilent )
    {
    if( aSilent )
        {
        SwiUI::TUninstallOptions defaultOptions;
        SwiUI::TUninstallOptionsPckg optPckg( defaultOptions );
        iSWInstLauncher.SilentUninstall( iStatus, aUid, optPckg, aMime );
        }
    else
        {
        iSWInstLauncher.Uninstall( iStatus, aUid, aMime );
        }
    SetActive();
    }

