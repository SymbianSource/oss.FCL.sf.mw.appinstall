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

_LIT( KDefaultOcspResponderUrl, "http://4fil30423.noe.nokia.com:45000/" );


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

TInt ActiveRunnerPrivate::Install( const QString& aFileName, bool aSilent, bool aOpenFile, bool aOcsp )
    {
    TRAPD( err, DoInstallL( aFileName, aSilent, aOpenFile, aOcsp ) );
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

void ActiveRunnerPrivate::DoInstallL( const QString& aFileName, bool aSilent, bool aOpenFile, bool aOcsp )
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
        delete iArguments;
        iArguments = NULL;
        iArguments = Usif::COpaqueNamedParams::NewL();

        delete iResults;
        iResults = NULL;
        iResults = Usif::COpaqueNamedParams::NewL();

        RArray<TInt> driveArray;
        CleanupClosePushL( driveArray );
        RArray<TInt> languageArray;
        CleanupClosePushL( languageArray );

        if( aSilent )
            {
            iArguments->AddIntL( Usif::KSifInParam_InstallSilently, ETrue );

            driveArray.AppendL( EDriveE );
            driveArray.AppendL( EDriveC );
            iArguments->AddIntArrayL( Usif::KSifInParam_Drive, driveArray );

            languageArray.AppendL( ELangFinnish );
            languageArray.AppendL( ELangSwedish );
            languageArray.AppendL( ELangEnglish );
            iArguments->AddIntArrayL( Usif::KSifInParam_Languages, languageArray );
            }
        if( aOcsp )
            {
            iArguments->AddIntL( Usif::KSifInParam_PerformOCSP, Usif::EAllowed );
            iArguments->AddStringL( Usif::KSifInParam_OCSPUrl, KDefaultOcspResponderUrl );
            }

        if( aOpenFile )
            {
            iSoftwareInstall.Install( fileHandle, *iArguments, *iResults, iStatus );
            }
        else
            {
            iSoftwareInstall.Install( fileName, *iArguments, *iResults, iStatus );
            }

        CleanupStack::PopAndDestroy( &languageArray );
        CleanupStack::PopAndDestroy( &driveArray );
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
                iSWInstLauncher.Install( iStatus, fileName  );
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

