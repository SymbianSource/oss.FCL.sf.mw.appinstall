/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Definition of CNcdInstallationService
*
*/
 

#include "ncdinstallationserviceimpl.h"

#include <DocumentHandler.h>
#include <apmstd.h>
#include <apmrec.h>
#include <bautils.h>

#include <swi/sisregistrypackage.h>
#include <caf/caf.h>
#include <caf/supplier.h> 
#include <caf/importfile.h>
#include <Oma2Agent.h>
#include <s32file.h>
#include <e32property.h>
#include <sacls.h>

#ifdef USE_OLD_JAVA_API
    #include <mjavaregistry.h>
    #include <swi/minstalledappsregistry.h>
#else
    #include <javaregistry.h>
    #include <javaregistrypackageentry.h>

    using namespace Java;
#endif

#include "ncdinstallationserviceobserver.h"
#include "ncdactiveoperationobserver.h"
#include "ncdsilentinstallactiveobserver.h"
#include "ncderrors.h"
#include "catalogsutils.h"
#include "catalogsconstants.h"
#include "catalogsdebug.h"

_LIT( KJadFileExtension, ".jad" );
const TInt KDelayWhenAppListInvalid = 500000;

const TUint KFileOpenFlags = EFileShareReadersOrWriters;

#ifdef __SERIES60_31__

    const TInt32 KPSUidJavaLatestInstallation = KUidJmiLatestInstallation;

#else

    // Defined originally in /mw/java/inc/javauids.h
    // This should become available at some point in javadomainpskeys.h
    //const TInt32 KPSUidJavaLatestInstallation = 0x10282567;
    #include <javadomainpskeys.h>

#endif

// length taken from WidgetRegistryData.h
const TInt KWidgetBundleIdLength = KWidgetRegistryVal + 1;    

_LIT( KWidgetExtension, ".wgz" );

// ======== CALLBACK FUNCTION ========
 
static TInt InstallationCompleteCallback( TAny* aData )
    {
    DLTRACEIN((""));
    TRAPD( err,
        {
        CNcdInstallationService* service = 
            reinterpret_cast<CNcdInstallationService*>( aData );
        DASSERT( service );
        service->NotifyObserverL();
        } );
        
    DLTRACEOUT((""));
    return err;
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CNcdInstallationService* CNcdInstallationService::NewL()
    {
    CNcdInstallationService* self = NewLC();
    CleanupStack::Pop();
    return self;
    }
    
// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CNcdInstallationService* CNcdInstallationService::NewLC()
    {
    CNcdInstallationService* self =
        new (ELeave) CNcdInstallationService();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CNcdInstallationService::~CNcdInstallationService()
    {
    DLTRACEIN((""));
    delete iSilentInstallActiveObserver;
    iSilentInstallActiveObserver = NULL;
    delete iDocHandler;
    iRegistrySession.Close();
    iFs.Close();
    iAknsSrv.Close();
    delete iInstallationCompleteCallback;
    delete iJadFileName;
    delete iRecognizedMime;
#ifdef USE_OLD_JAVA_API    
    iMIDletUids.Close();
#endif    
    iApaLs.Close();
    if( iThemes )
        {
        iThemes->ResetAndDestroy();
        delete iThemes;        
        }
    
    // Deletes iInstallStatusObserver and closes iInstaller
    CancelInstall(); 
    iRomUids.Close();
    
    if ( iWidgetRegistry.Handle() )
        {
        // decreases widget server's refcount but this cannot be called
        // if Connect has not been called or we'll get a KERN-EXEC 0
        iWidgetRegistry.Disconnect();
        }
    else
        {
        iWidgetRegistry.Close();
        }
    iInstalledWidgets.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CNcdInstallationService::CNcdInstallationService()
    {
    }
    
// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::ConstructL()
    {
    DLTRACEIN((""));
    iDocHandler = CDocumentHandler::NewL();
    iDocHandler->SetExitObserver( this );
    
    User::LeaveIfError( iFs.Connect() );
    User::LeaveIfError( iFs.ShareProtected() );
    User::LeaveIfError( iRegistrySession.Connect() );
    User::LeaveIfError( iAknsSrv.Connect() ); 

    iInstallationCompleteCallback = new(ELeave) CAsyncCallBack( 
        TCallBack( InstallationCompleteCallback, this ),
        CActive::EPriorityStandard );  
              
    InitializeRomApplicationListL();
    }


// ---------------------------------------------------------------------------
// Installs all kinds of files.
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::InstallL( const TDesC& aFileName,
                                        const TDesC& aMimeType,
                                        const TNcdItemPurpose& aPurpose )
    {
    DLTRACEIN((""));
    RFile file;
    CleanupClosePushL( file );
    User::LeaveIfError( file.Open( FileServerSession(), aFileName, KFileOpenFlags ) ); 
    InstallL( file, aMimeType, aPurpose, NULL );
    CleanupStack::PopAndDestroy( &file );
    DLTRACEOUT((""));        
    }

// ---------------------------------------------------------------------------
// Installs all kinds of files.
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::InstallL( RFile& aFile,
                                        const TDesC& aMimeType,
                                        const TNcdItemPurpose& aPurpose )
    {
    DLTRACEIN((""));
    InstallL( aFile, aMimeType, aPurpose, NULL );
    DLTRACEOUT((""));        
    }


// ---------------------------------------------------------------------------
// Installs java files.
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::InstallJavaL( const TDesC& aFileName,
                                            const TDesC& aMimeType,
                                            const TDesC8& aDescriptorData )
    {
    DLTRACEIN((""));
    RFile file;
    CleanupClosePushL( file );
    User::LeaveIfError( file.Open( FileServerSession(), aFileName, KFileOpenFlags ) );     
    InstallJavaL( file, aMimeType, aDescriptorData, NULL );
    CleanupStack::PopAndDestroy( &file );
    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// Installs java files.
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::InstallJavaL( RFile& aFile,
                                            const TDesC& aMimeType,
                                            const TDesC8& aDescriptorData )
    {
    DLTRACEIN((""));    
    InstallJavaL( aFile, aMimeType, aDescriptorData, NULL );    
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Installs all kinds of files silently.
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::SilentInstallL( RFile& aFile,
                                        const TDesC& aMimeType,
                                        const TNcdItemPurpose& aPurpose,
                                        const SwiUI::TInstallOptionsPckg& aInstallOptionsPckg )
    {
    DLTRACEIN((""));

    if ( iSilentInstallActiveObserver == NULL )
        {
        DLINFO(("Create active observer for silent install"));
        iSilentInstallActiveObserver = CNcdSilentInstallActiveObserver::NewL( *this );        
        }

    InstallL( aFile, aMimeType, aPurpose, &aInstallOptionsPckg );

    DLTRACEOUT(("")); 
    }


// ---------------------------------------------------------------------------
// Installs java files silently.
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::SilentInstallJavaL( RFile& aFile,
                                            const TDesC& aMimeType,
                                            const TDesC8& aDescriptorData,
                                            const SwiUI::TInstallOptionsPckg& aInstallOptionsPckg )
    {
    DLTRACEIN((""));

    if ( iSilentInstallActiveObserver == NULL )
        {
        DLINFO(("Create active observer for silent install"));
        iSilentInstallActiveObserver = CNcdSilentInstallActiveObserver::NewL( *this );        
        }

    InstallJavaL( aFile, aMimeType, aDescriptorData, &aInstallOptionsPckg );

    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Cancell silent install.
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::CancelSilentInstall( HBufC*& aFileName,
                                                   TUid& aAppUid,
                                                   TInt& aError )
    {
    DLTRACEIN((""));
    
    // Silent install can only be cancelled if silent is allowed.
    // The capability check makes sure of this.

    // Set intial values for the reference parameters.
    delete aFileName;
    aFileName = NULL;
    aAppUid = KNullUid;
    aError = KErrCancel;
    
    if ( iBusy )
        {
        DLINFO(("Install operation was busy"));
        
        // Some operation is on. Here we expect that it is silent.
        // Inform the installer that the asynchronous silent install request 
        // needs to be cancelled. 
        // Because we use a dummy active object to observe
        // completion of installation and that dummy object uses call backs
        // to inform completion to observer, we have to cancel the operation
        // by using the dummy object here.
        aError = iSilentInstallActiveObserver->CancelAsyncOperation();

        if ( aError == KErrNone )
            {
            DLINFO(("Installation was success even if cancel was issued."))
            
            // The installation was finished after all.
            TRAPD( trapError, 
                   HandleSilentInstallSuccessAfterCancelL( aFileName,
                                                           aAppUid,
                                                           aError ) );
            if ( trapError != KErrNone )
                {
                DLERROR(("Install success handling leave."));
                aError = trapError;
                }
            }

        // Do cleaning if necessary.
        if( iJadFileName != NULL )
            {
            // Delete tmp JAD file that was created in the beginning of
            // the install operation. It will be recreated in new install operation
            // if needed.
            BaflUtils::DeleteFile( iFs, *iJadFileName ); // NOTE, error ignored
            delete iJadFileName;
            iJadFileName = NULL;
            }
        
#ifdef USE_OLD_JAVA_API        
        // Clean the array.
        iMIDletUids.Reset();
#endif        

        // Reset information flags.            
        InstallationFinishedSetup( aError );
        }
    }


// ---------------------------------------------------------------------------
// Sets observer for installation services.
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::SetObserver(
                            MNcdInstallationServiceObserver& aObserver )
    {
    iObserver = &aObserver;
    }


// ---------------------------------------------------------------------------
// Gets version of installed application by using its UID.
// ---------------------------------------------------------------------------
//
TInt CNcdInstallationService::ApplicationVersion( const TUid& aUid,
                                                  TCatalogsVersion& aVersion )
    {
    DLTRACEIN(("Uid=%X",aUid.iUid));

    // First case: see if the uid is directly in the sis registry.
    Swi::RSisRegistryEntry entry;
    
    TVersion version;
    TRAPD( err, 
        {
        SisRegistryEntryLC( entry, aUid );         
        // Get version number
        version = entry.VersionL();
        CleanupStack::PopAndDestroy( &entry );
        });

    if ( err != KErrNone ) 
        {
        DLTRACEOUT(("Error: %d", err));
        return err;
        }
        
    aVersion.iMajor = version.iMajor;
    aVersion.iMinor = version.iMinor;
    aVersion.iBuild = version.iBuild;        

    DLTRACEOUT(("err=%d",err));
    return err;
    }

// ---------------------------------------------------------------------------
// Checks whether an application is installed on the device.
// ---------------------------------------------------------------------------
//
TBool CNcdInstallationService::IsApplicationInstalledL( const TUid& aUid )
    {
    DLTRACEIN(("uid=%X",aUid.iUid));
    TBool result = EFalse;
    
    Swi::RSisRegistryEntry entry;
    
    // First case: See if the UID is directly in the SIS Registry.
    TRAPD( err, 
        {
        SisRegistryEntryLC( entry, aUid ); 
        
        DLTRACE(("Check that entry is present"));
        result = entry.IsPresentL();
        CleanupStack::PopAndDestroy( &entry );
        });
        
    LeaveIfNotErrorL( err, KErrNotFound );    

    // Third case: see if the UID is mapped to the Java Registry or is a Widget
    if ( !result )
        {
        result = ( JavaAppExistsL( aUid ) || 
                   WidgetExistsL( aUid ) );
        }
    
    
    // Check if the app is ROM only
    if ( !result ) 
        {
        result = IsRomApplication( aUid );
        }
    DLTRACEOUT(("result=%d",result));
    return result;
    }

// ---------------------------------------------------------------------------
// Opens SIS registry entry
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::SisRegistryEntryLC( 
    Swi::RSisRegistryEntry& aEntry,
    const TUid& aUid )
    {
    DLTRACEIN((""));    
    CleanupClosePushL( aEntry );
    
    if ( aEntry.Open( iRegistrySession, aUid ) == KErrNone )
        {
        DLTRACEOUT(("Entry opened"));
        return;
        }
        
    // Second case: See if the UID is mapped to a package/entry
    // in the registry ( -> APP UID that differs from SIS UID ).

    Swi::CSisRegistryPackage* sisRegistryPackage =
        iRegistrySession.SidToPackageL( aUid );
    CleanupStack::PushL( sisRegistryPackage );

    TInt err = aEntry.OpenL( iRegistrySession, *sisRegistryPackage );
    User::LeaveIfError( err );
        
    CleanupStack::PopAndDestroy( sisRegistryPackage );
    
    DLTRACEOUT(("Entry opened"));    
    }


// ---------------------------------------------------------------------------
// Open java suite entry
//
// ---------------------------------------------------------------------------
//
#ifdef USE_OLD_JAVA_API

TBool CNcdInstallationService::JavaAppExistsL( 
    const TUid& aUid )
    {
    DLTRACEIN((""));

    MJavaRegistry* javaRegistry = MJavaRegistry::CreateL();
    CleanupReleasePushL( *javaRegistry );
    
    TRAPD( err, 
        {
        // Leaves with KErrNotFound if not found
        MJavaRegistryMIDletEntry* midletEntry = javaRegistry->MIDletEntryL(
            aUid );    
        midletEntry->Release();
        });
    
    LeaveIfNotErrorL( err, KErrNotFound );
    
    CleanupStack::PopAndDestroy( javaRegistry );
    return err == KErrNone;
    }

#else

TBool CNcdInstallationService::JavaAppExistsL( 
    const TUid& aUid )
    {
    DLTRACEIN((""));

    CJavaRegistry* javaRegistry = CJavaRegistry::NewLC();    

    TBool exists = javaRegistry->RegistryEntryExistsL( aUid );
    
    CleanupStack::PopAndDestroy( javaRegistry );
    return exists;
    }

#endif
    
// ---------------------------------------------------------------------------
// Checks the application status
// ---------------------------------------------------------------------------
//
TNcdApplicationStatus CNcdInstallationService::IsApplicationInstalledL( 
    const TUid& aUid, const TCatalogsVersion& aVersion )
    {
    DLTRACEIN((""));

    TNcdApplicationStatus status( ENcdApplicationNotInstalled );
    
    if ( aUid == TUid::Null() ) 
        {
        DLTRACEOUT(("Null uid"));
        return status;
        }


    Swi::RSisRegistryEntry entry;    
    TCatalogsVersion installedVersion;
    
    // Get SIS-app version
    TRAPD( err, 
        {
        SisRegistryEntryLC( entry, aUid ); 
        
        DLTRACE(("Check that entry is present"));
        
        if ( entry.IsPresentL() ) 
            {
            DLTRACE(("Is present"));
            status = ENcdApplicationInstalled;
            }
        
        TVersion version = entry.VersionL();
        installedVersion = TCatalogsVersion(         
            version.iMajor,
            version.iMinor,
            version.iBuild );

        CleanupStack::PopAndDestroy( &entry );
        });
        
    // SIS app not found, try to get java
    if ( err == KErrNotFound &&
         ( JavaAppExistsL( aUid ) ||
           WidgetExistsL( aUid ) ) )
        {
        status = ENcdApplicationInstalled;
        }
        
    LeaveIfNotErrorL( err, KErrNotFound );  
    
    // Some app found, check version numbers if they are something else than 
    // 0.0.0
    if ( err == KErrNone && 
         status == ENcdApplicationInstalled &&
         aVersion != TCatalogsVersion() ) 
        {
        DLINFO(("Installed version: %d.%d.%d, comparing to: %d.%d.%d",
            installedVersion.iMajor,
            installedVersion.iMinor,
            installedVersion.iBuild,
            aVersion.iMajor,
            aVersion.iMinor,
            aVersion.iBuild ));
            
        if ( installedVersion > aVersion ) 
            {
            status = ENcdApplicationNewerVersionInstalled;
            }
        else if ( !( installedVersion == aVersion ) ) 
            {
            status = ENcdApplicationOlderVersionInstalled;
            }
        }
    
    // Checking if the app is ROM only
    if ( status == ENcdApplicationNotInstalled && 
         IsRomApplication( aUid ) ) 
        {
        status = ENcdApplicationInstalled;
        }
    
    DLTRACEOUT(("Status: %d", status));
    return status;
    }


// ---------------------------------------------------------------------------
// Gets the first SID from the SIS registry package
// ---------------------------------------------------------------------------
//
TUid CNcdInstallationService::SidFromSisRegistryL( const TUid& aUid )
    {
    DLTRACEIN(("uid=%X",aUid.iUid));
    Swi::RSisRegistryEntry entry;
    TUid sid( KNullUid );
    
    User::LeaveIfError( entry.Open( iRegistrySession, aUid ) );
    CleanupClosePushL( entry );

    RArray<TUid> sids;
    CleanupClosePushL( sids );
    entry.SidsL( sids );
        
    DLINFO(("sid count=%d",sids.Count()));

    if ( sids.Count() > 0 )
        {
        sid.iUid = sids[0].iUid;
        }
    else
        {
        DLERROR(("No SIDs found"));
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( &sids );
    CleanupStack::PopAndDestroy( &entry );

    DLTRACEOUT(("uid=%X",sid.iUid));
    return sid;
    }

// ---------------------------------------------------------------------------
// Appends rights into the rights database.
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::AppendRightsL(
    const TDesC8& aRightsObject,
    const TDataType& aMimeType )
    {
    DLTRACEIN(("MimeType: %S", &aMimeType.Des8() ));
    
    ContentAccess::CSupplier* supplier = 
        ContentAccess::CSupplier::NewLC();
    ContentAccess::CMetaDataArray* metaData =
        ContentAccess::CMetaDataArray::NewLC();
    
    DLTRACE(("Creating CImportFile"));
    // Indicate what type of content we are importing.
    ContentAccess::CImportFile* file = supplier->ImportFileL( aMimeType.Des8(), *metaData );
    CleanupStack::PushL( file );
    
    DLTRACE(("Importing data"));
    // Start importing data. This can also be done in parts.
    User::LeaveIfError( file->WriteData( aRightsObject ) );
    
    DLTRACE(("Finished importing"));
    // Indicate that we are finished
    User::LeaveIfError( file->WriteDataComplete() );
    
    CleanupStack::PopAndDestroy( file );
    CleanupStack::PopAndDestroy( metaData );
    CleanupStack::PopAndDestroy( supplier );
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Reads the package uid from a SISX file
// ---------------------------------------------------------------------------
//
TUid CNcdInstallationService::PackageUidFromSisL( RFile& aFile ) const
    {
    DLTRACEIN((""));
    RFileReadStream readStream( aFile );
    CleanupClosePushL( readStream );

    // Get Application UID
    //
    
    // NOTE: In 3.0 the SIS file format has changed
    // the UID is the third long word ( 32 bits ) of the file.
    TInt dummy = readStream.ReadUint32L();
    dummy = readStream.ReadUint32L();
            
    TUid uid( TUid::Uid( readStream.ReadUint32L() ) );
    
    // this closes the attached file too
    CleanupStack::PopAndDestroy( &readStream ); 
    DLTRACEOUT(( _L("Uid: %S"), &uid.Name() ));
    return uid;
    }


// ---------------------------------------------------------------------------
// IsThemeInstalled
// ---------------------------------------------------------------------------
//
TBool CNcdInstallationService::IsThemeInstalledL( const TDesC& aThemeName )
    {
    DLTRACEIN(( _L("Theme: %S"), &aThemeName ));
    TInt result = EFalse;

    if ( aThemeName != KNullDesC )
        {
        RAknsSrvSession skinSrvSession;
        User::LeaveIfError( skinSrvSession.Connect() );
        CleanupClosePushL( skinSrvSession );

        CArrayPtr<CAknsSrvSkinInformationPkg>* srvArray = NULL;
        // Look themes from C: and E: drives
        for ( TInt index = 0; index < 2; index++ )
            {
            //TUint skinLocation = 0;
            if ( index == 0 )
                {
                //skinLocation = EAknsSrvPhone;
                srvArray = skinSrvSession.EnumerateSkinPackagesL( EAknsSrvPhone );
                CleanupStack::PushL( srvArray );
                }
            else
                {
                //skinLocation = EAknsSrvMMC;
                srvArray = skinSrvSession.EnumerateSkinPackagesL( EAknsSrvMMC );
                CleanupStack::PushL( srvArray );
                }

            while ( srvArray && srvArray->Count() > 0 )
                {
                CAknsSrvSkinInformationPkg* info = srvArray->At( 0 );

                // If match is found, theme can be set
                if ( info->Name().Compare( aThemeName ) == 0 )
                    {
                    result = ETrue;
                    }
        
                delete info;
                srvArray->Delete( 0 );
                }

            CleanupStack::PopAndDestroy( srvArray );

            if ( result )
                {
                break;
                }
            }

        CleanupStack::PopAndDestroy();  // skinSrvSession
        }
    DLTRACEOUT(("Result: %d", result));
    return result;
    }


// ---------------------------------------------------------------------------
// Deletes the given file
// ---------------------------------------------------------------------------
//
TInt CNcdInstallationService::DeleteFile( const TDesC& aFilename )
    {
    DLTRACEIN(( _L("Filepath: %S"), &aFilename ));
    // Ensure that we don't accidentally delete whole directories
    // because of empty path
    if ( aFilename.Length() ) 
        {
        return BaflUtils::DeleteFile( iFs, aFilename );
        }
    return KErrArgument;
    }


// ---------------------------------------------------------------------------
// File server session getter
// ---------------------------------------------------------------------------
//
RFs& CNcdInstallationService::FileServerSession()
    {
    return iFs;
    }


// ---------------------------------------------------------------------------
// JAD writer
// ---------------------------------------------------------------------------
//
HBufC* CNcdInstallationService::WriteJadL( 
    const TDesC& aJarFileName, const TDesC8& aJad  )
    {
    DLTRACEIN(("Writing JAD"));
    TParsePtrC jarParse( aJarFileName );
    HBufC* jadFileName = HBufC::NewLC( 
            aJarFileName.Length() + KJadFileExtension().Length() );
    TPtr jadPtr( jadFileName->Des() );
    jadPtr.Append( jarParse.DriveAndPath() );
    jadPtr.Append( jarParse.Name() );
    jadPtr.Append( KJadFileExtension );

    RFile file;
    CleanupClosePushL( file );
    User::LeaveIfError( file.Replace( iFs, *jadFileName, EFileWrite | EFileShareAny ) );
    User::LeaveIfError( file.Write( aJad ) );
    DLINFO(( _L("JAD=%S"), jadFileName ));
    CleanupStack::PopAndDestroy( &file );
    CleanupStack::Pop( jadFileName );
    return jadFileName;
    }


// ---------------------------------------------------------------------------
// Documenthandler getter
// ---------------------------------------------------------------------------
//
CDocumentHandler& CNcdInstallationService::DocumentHandler()
    {
    return *iDocHandler;
    }


// ---------------------------------------------------------------------------
// Callback function of MNcdAsyncOperationObserver interface
// This is called when an async operation has finished.
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::AsyncOperationComplete( TInt aError )
    {    
    DLTRACEIN(("aError: %d", aError));
    
    iInstaller.Close();
    
    if ( aError == SwiUI::KSWInstErrUserCancel ) 
        {
        DLTRACE(("User cancelled, converting error to KErrAbort" ) );
        aError = KErrAbort;        
        }
    
    // Handle this like in normal cases.
    HandleServerAppExit( aError );
    }


// ---------------------------------------------------------------------------
// Callback function of MNcdAsyncSilentInstallObserver interface
// This is called when the silent install has finished.
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::AsyncSilentInstallComplete( TInt aError )
    {    
    DLTRACEIN(("aError: %d", aError));
    
    // Handle this like in normal cases.
    HandleServerAppExit( aError );
    }


// ---------------------------------------------------------------------------
// This function is called after application installer has finished.
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::NotifyObserverL()
    {
    DLTRACEIN(("install error: %d", iInstallError ));        
    
    if ( iInstallType == EWidgetInstall )
        {
        HandleInstalledWidgetL();
        }
    else if ( iInstallType == EJavaInstall )
        {    
        if( iJadFileName )
            {
            BaflUtils::DeleteFile( iFs, *iJadFileName ); // NOTE, error ignored
            delete iJadFileName;
            iJadFileName = 0;
            }

        if ( iInstallError != KErrNone ) 
            {        
            DLTRACE(("Notify install error"));
#ifdef USE_OLD_JAVA_API            
            iMIDletUids.Reset();
#endif            
            iObserver->InstallationCompleteL( KNullDesC, TUid(), iInstallError );       
            return; 
            }

        TUid midletUid = InstalledMidletUidL();
        
        DLINFO(("Installed midlet uid=%X", midletUid.iUid ));
        
        iObserver->InstallationCompleteL( KNullDesC, midletUid, 
                                          // Return error if nothing found.
                                          midletUid == KNullUid ? KErrNotFound : KErrNone );
        }
    
    else if ( iInstallType == ESisInstall )
        {

        if ( iInstallError != KErrNone ) 
            {        
            DLTRACE(("Notify install error"));
            iObserver->InstallationCompleteL( KNullDesC, TUid(), iInstallError );       
            return; 
            }
        
        // Get theme name if such was installed
        const TDesC& name( LatestThemeL() );
                
        // Notify about theme installation
        if ( iPackageUid.iUid == KNcdThemeSisUid || name != KNullDesC ) 
            {            
            DLTRACE(( _L("Notify observer about theme installation, theme: %S"), 
                &name ));
            // Ensure that uid is theme uid
            iPackageUid.iUid = KNcdThemeSisUid;
            
            if ( name == KNullDesC && iThemePackageUid.iUid != KNcdThemeSisUid ) 
                {
                DLTRACE(("Checking theme installation status with package uid, %x", 
                    iThemePackageUid.iUid ));
                
                TBool themeInstalled = EFalse;
                
                // Check if the theme was indeed successfully installed
                TRAP_IGNORE( themeInstalled = IsApplicationInstalledL( 
                    iThemePackageUid ) );
                    
                if ( themeInstalled ) 
                    {
                    DLINFO(("Theme is installed but we didn't get the name"));
                        iObserver->InstallationCompleteL( 
                            name, 
                            iPackageUid,                     
                            KNcdThemeReinstalled );                    
                    }
                else
                    {
                    DLINFO(("Theme was not installed"));
                    iObserver->InstallationCompleteL( 
                        name, 
                        iPackageUid,                     
                        KErrGeneral );
                    
                    }
                }
            else 
                {
                DLTRACE(("Either theme name was acquired or package uid was theme"));
                iObserver->InstallationCompleteL( 
                    name, 
                    iPackageUid, 
                    // Consider missing theme name as a theme reinstallation
                    name == KNullDesC ? KNcdThemePossiblyReinstalled : KErrNone );    
                }
            }
        else 
            {            
            
            // Get the actual app UID from sis registry
            TRAPD( err, iAppUid = SidFromSisRegistryL( iPackageUid ) );
            
            DLTRACE(("Notify observer about application installation, uid: %x", 
                iAppUid.iUid ));
            
            if ( err == KErrNotFound ) 
                {
                DLTRACE(("Didn't get SID, using package UID: %x", 
                    iPackageUid.iUid));
                iAppUid.iUid = iPackageUid.iUid;
                err = KErrNone;
                }
            
            if ( err == KErrNone ) 
                {
                DLTRACE(("Check the application is actually installed"));          
                TBool installed = EFalse;      
                TRAP( err, installed = IsApplicationInstalledL( iAppUid ) );
                if ( err == KErrNone )
                    {                    
                    if ( !installed ) 
                        {                        
                        err = KErrGeneral;
                        }
                    else
                        {
                        TCatalogsVersion version;
                        err = ApplicationVersion( iAppUid, version );
                        if ( err == KErrNone ) 
                            {
                            DLTRACE(("Converting version to string"));
                            // Put application version as the filename
                            HBufC* versionString = 
                                TCatalogsVersion::ConvertLC( version );
                            // Notify about application installation
                            iObserver->InstallationCompleteL( 
                                *versionString, 
                                iAppUid, 
                                err );
                            CleanupStack::PopAndDestroy( versionString );
                            return;
                            }
                        }
                    }
                
                }

            // Notify about application installation
            iObserver->InstallationCompleteL( KNullDesC, iAppUid, err );
            }
        }
    else 
        {
        DLTRACE(("Was not Java nor SIS install"));
        // This is executed after a content file has been moved.
        // We don't set the exit observer as NULL since it panics in debug builds
        }
    DLTRACEOUT((""));
    }

    
// ---------------------------------------------------------------------------
// This function is called after application installer has finished.
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::HandleServerAppExit( TInt aReason )
    {    
    DLTRACEIN(("aReason=%d",aReason));

    InstallationFinishedSetup( aReason );

    // If observer exists, notify it.
    iInstallationCompleteCallback->CallBack();
    }


// ---------------------------------------------------------------------------
// Update the list of installed themes
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::UpdateInstalledThemesL()
    {
    DLTRACEIN((""));
    if ( iThemes ) 
        {
        DLTRACE(("Delete old list"));
        iThemes->ResetAndDestroy();
        delete iThemes;
        iThemes = NULL;        
        }
        
    iThemes = iAknsSrv.EnumerateSkinPackagesL( EAknsSrvAll );

    DLTRACEOUT(("Got new theme list"));
    }


// ---------------------------------------------------------------------------
// Get name of the theme installed after the latest UpdateInstalledThemesL() 
// call
// ---------------------------------------------------------------------------
//
const TDesC& CNcdInstallationService::LatestThemeL()
    {  
    DLTRACEIN((""));  
    
    if ( !iThemes ) 
        {
        DLTRACEOUT(("Was not installing a theme"));
        return KNullDesC();
        }
        
    CArrayPtr<CAknsSrvSkinInformationPkg>* newThemes = 
        iAknsSrv.EnumerateSkinPackagesL( EAknsSrvAll );

#ifdef CATALOGS_BUILD_CONFIG_DEBUG
    DLTRACE(("Installed old themes, count: %d", iThemes->Count() ));
    for ( TInt i = 0; i < iThemes->Count(); ++i )
        {
        DLINFO(( _L("%S"), &iThemes->At( i )->Name() ));
        }

    DLTRACE(("Installed new themes, count: %d", newThemes->Count() ));
    for ( TInt i = 0; i < newThemes->Count(); ++i )
        {
        DLINFO(( _L("%S"), &newThemes->At( i )->Name() ));
        }            
#endif         

    // Empty previous information
    delete iThemeName;
    iThemeName = NULL;
    iPID.Set( TUid::Uid( 0 ) );
   
    // Check if new theme has been installed after last
    // call to the GetInstalledSkinPackagesL()
    if ( newThemes->Count() > iThemes->Count() )
        {
        DLTRACE(("New themes installed, count: %d", 
            newThemes->Count() - iThemes->Count() ));
            
        // New theme has been installed
        for ( TInt i = 0; i < newThemes->Count(); i++ )
            {
            TInt j = 0;
            
            for ( ; j < iThemes->Count(); j++ )
                {
                if ( newThemes->At( i )->PID() == iThemes->At( j )->PID() )
                    {
                    // PID found from old theme list. This is not the
                    // new theme.
                    break;
                    }
                }

            if ( j == iThemes->Count() )
                {
                DLTRACE(("Found new theme"));
                // PID was not found from old theme list. This is the
                // new theme.
                iPID = newThemes->At( i )->PID();
                TRAPD( err, iThemeName = newThemes->At( i )->Name().AllocL() );
                if ( err != KErrNone ) 
                    {
                    newThemes->ResetAndDestroy();
                    delete newThemes;
                    User::Leave( err );
                    }
                break;
                }
            }
        }

    newThemes->ResetAndDestroy();
    delete newThemes;
    if ( iThemeName ) 
        {
        DLTRACEOUT(( _L("Theme: %S"), iThemeName ));
        return *iThemeName;
        }
    DLTRACEOUT(("No theme"));
    return KNullDesC();
    }


// ---------------------------------------------------------------------------
// Try to recognize the data from the file
// ---------------------------------------------------------------------------
//
HBufC* CNcdInstallationService::RecognizeDataL( const TDesC& aFileName )
    {
    DLTRACEIN(( _L("Filename: %S"), &aFileName ));
    
    if ( aFileName.Length() == 0 )
        {
        DLTRACEOUT(("Empty filename"));
        return KNullDesC().AllocL();
        }

    ConnectApaLsL();

    TInt recognizeBufferLength = 0;
    User::LeaveIfError( iApaLs.GetPreferredBufSize( recognizeBufferLength ) );
    HBufC8* buffer = HBufC8::NewLC( recognizeBufferLength );
    TPtr8 ptr8( buffer->Des() );

    TDataRecognitionResult result;
    // Read necessary amount of bytes from the file
    User::LeaveIfError( iFs.ReadFileSection( 
        aFileName, 0, ptr8, recognizeBufferLength ) );
    
    
    // Try to recognize data
    User::LeaveIfError( iApaLs.RecognizeData( 
        aFileName, *buffer, result ) );
        
    HBufC* resultMime = Des8ToDes16L( result.iDataType.Des8() );
    CleanupStack::PopAndDestroy( buffer );
    iApaLs.Close();
    DLTRACEOUT(( _L("Determined MIME: %S"), resultMime ));        
    return resultMime;
    }

// ---------------------------------------------------------------------------
// Try to recognize the data from the file
// ---------------------------------------------------------------------------
//
HBufC* CNcdInstallationService::RecognizeDataL( RFile& aFile )
    {
    DLTRACEIN((""));    
    ConnectApaLsL();

    TDataRecognitionResult result;
    
    // Try to recognize data
    User::LeaveIfError( iApaLs.RecognizeData( 
        aFile, result ) );
        
    HBufC* resultMime = Des8ToDes16L( result.iDataType.Des8() );
    
    iApaLs.Close();
    DLTRACEOUT(( _L("Determined MIME: %S"), resultMime ));        
    return resultMime;
    }


void CNcdInstallationService::ConnectApaLsL()
    {
    DLTRACEIN((""));    
    User::LeaveIfError( iApaLs.Connect() );    
    DLTRACEOUT(("Connected successfully"));
    }


// ---------------------------------------------------------------------------
// Installs all kinds of files in normal or in silent way.
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::InstallL( RFile& aFile,
                                        const TDesC& aMimeType,
                                        const TNcdItemPurpose& aPurpose,
                                        const SwiUI::TInstallOptionsPckg* aSilentInstallOptionsPckg )
    {
    DLTRACEIN(( _L("iBusy=%d, MIME: %S"),iBusy, &aMimeType ));    
    DASSERT( iObserver );
    DASSERT( iDocHandler );

    
    // Check if some installation is already in progress.
    if ( iBusy )
        {
        DLERROR(("busy"));
        User::Leave( KErrInUse );
        }
    
    // Reset app uid info
    iAppUid.iUid = 0;
    iPackageUid.iUid = 0;
    iThemePackageUid.iUid = KNcdThemeSisUid;
    
    iPurpose = aPurpose;
    
    iInstallError = KErrNone;
    
    delete iRecognizedMime;
    iRecognizedMime = NULL;
        
    // Try to recognize the data if no mime given or it's DRM content
    // in which case we don't know the type of the actual content

    DLTRACE(("Recognizing mime type of the content"));
    TRAPD( err, iRecognizedMime = RecognizeDataL( aFile ) );
    if ( err != KErrNone ) 
        {
        DLERROR(("Couldn't recognize the mime type, error: %d", err));
        iRecognizedMime = KNullDesC().AllocL();
        }

    // Fail-safe in case someone tries to install a Java application
    // with this method
    if ( MatchJava( *iRecognizedMime ) )
        {
        DLTRACE(("Java"));
        InstallJavaL( 
            aFile, 
            *iRecognizedMime, 
            KNullDesC8, 
            aSilentInstallOptionsPckg );
        return;
        }
    else if ( MatchWidget( aFile, aMimeType ) )
        {
        DLTRACE(("Widget"));
        InstallWidgetL( 
            aFile,
            aSilentInstallOptionsPckg );
        return;
        }
    
    // Handle SIS(X) and DRM content which has a suitable purpose
    if ( aMimeType.MatchF( KMimeTypeMatchSymbianInstall ) != KErrNotFound
        || aMimeType.MatchF( KMimeTypeMatchSisx ) != KErrNotFound 
        || aMimeType.MatchF( KMimeTypeMatchApplicationStream ) != KErrNotFound 
        || iRecognizedMime->MatchF( KMimeTypeMatchSymbianInstall ) != KErrNotFound
        || iRecognizedMime->MatchF( KMimeTypeMatchSisx ) != KErrNotFound
        || iRecognizedMime->MatchF( KMimeTypeMatchApplicationStream ) != KErrNotFound )
        {
        DLINFO(("Installing SIS"));
        iInstallType = ESisInstall;
        // Get package UID     
        
        // PackageUidFromSisL( RFile& ) closes the filehandle so we need to make
        // a duplicate 
        RFile fileCopy;
        User::LeaveIfError( fileCopy.Duplicate( aFile ) );
        CleanupClosePushL( fileCopy );
        iPackageUid = PackageUidFromSisL( fileCopy );
        CleanupStack::PopAndDestroy( &fileCopy );
        
        // Check if the file is a theme sis
        if ( aPurpose == ENcdItemPurposeTheme ||
             iPackageUid.iUid == KNcdThemeSisUid )
            {           
            DLINFO(("This seems to be a theme"));
            iThemePackageUid = iPackageUid;
            // Ensure that package uid is a theme UID
            iPackageUid.iUid = KNcdThemeSisUid;             
            }        
    
        // Update the theme list in case it actually is a theme but purpose is wrong
        // and the package doesn't have the theme uid
        UpdateInstalledThemesL();
        
        TDataType dataType;    
        // Start application installation.
        DLINFO(( "Calling doc handler Open" ));

        if ( !aSilentInstallOptionsPckg )
            {
            DLINFO(("Normal install"));
            InitializeInstallerL();
            iCancelCode = SwiUI::ERequestInstallHandle;
            
            iInstaller.Install( iInstallStatusObserver->iStatus, aFile );
            iInstallStatusObserver->StartToObserve();            
            }
        else
            {
            DLINFO(("Silent install"));        
            // Set the observer active because it will be informed about the completion
            // of the silent install and it will forward the information for the callback
            // function of this class object.
            iSilentInstallActiveObserver->StartToObserveL( aFile,
                                                           *aSilentInstallOptionsPckg );
            }
            
        iBusy = ETrue;
 
        }
    else // Handle images etc. and DRM content that didn't have a matching purpose
        {
        DLINFO(("Installing content"));
        // File is some common format.
        iInstallType = EFileInstall;
        TDataType dataType;
    
        if ( aMimeType != KNullDesC )
            {
            // If mime type is given, it will be used in document handler.
            HBufC8* tempBuf = Des16ToDes8LC( aMimeType );
            dataType = TDataType( *tempBuf );
            CleanupStack::PopAndDestroy( tempBuf );
            DLINFO(("DataType: %S", &dataType.Des8() ));
            }
                
        TInt docHandlerError( KErrNone );
        
        DLINFO(("Normal install"));
        // Have to use CopyL since MoveL works only with filepaths
        // We can't use SilentMoveL either
        docHandlerError = 
            iDocHandler->CopyL( aFile, 
                                KNullDesC(),
                                dataType, 
                                KEntryAttNormal );            

        DLTRACE(("After move"));
        if( docHandlerError != KErrNone )
            {
            DLINFO(("error=%d",docHandlerError));
            
            // Use KErrAbort for user cancellation
            if ( docHandlerError == KUserCancel ) 
                {
                docHandlerError = KErrAbort;
                }
            iObserver->InstallationCompleteL( KNullDesC, KNullUid, docHandlerError );
            }
        else
            { 
            DLTRACE(("Installation successful"));
            
            RBuf installFileName;
            CleanupClosePushL( installFileName );
            
            installFileName.CreateL( KMaxPath );
                        
            User::LeaveIfError( iDocHandler->GetPath( installFileName ) );
            iObserver->InstallationCompleteL( installFileName, KNullUid, KErrNone );
            CleanupStack::PopAndDestroy( &installFileName );            

            }
        }
    DLTRACEOUT((""));    
    }

                   
// ---------------------------------------------------------------------------
// Installs java files in normal or in silent way.
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::InstallJavaL( RFile& aFile,
                                            const TDesC& /*aMimeType*/,
                                            const TDesC8& aDescriptorData,
                                            const SwiUI::TInstallOptionsPckg* aSilentInstallOptionsPckg )
    {
    DLTRACEIN((_L("iBusy=%d, descriptor=%d"),iBusy, aDescriptorData.Length() ));
    DASSERT( iObserver );

    // Check if some installation is already in progress.
    if ( iBusy )
        {
        DLERROR(("busy"));
        User::Leave( KErrInUse );
        }

    iInstallError = KErrNone;
    
#ifdef USE_OLD_JAVA_API
    // Store installed java app uids before installation to see
    // which one is a new java app later.
    MJavaRegistry* javaRegistry = MJavaRegistry::CreateL();
    CleanupReleasePushL( *javaRegistry );
    iMIDletUids.Reset();
    javaRegistry->InstalledMIDletUidsL( iMIDletUids );
    CleanupStack::PopAndDestroy( javaRegistry );
#endif
    
    // In platform security systems JAR and JAD has to be in same folder
    // to get the installation process work correctly.
    // First form the JAD filename from the JAR filename.

    delete iJadFileName;
    iJadFileName = 0;

    if( aDescriptorData != KNullDesC8 )
        {
        DLINFO(("Writing JAD"));
        TPath fileName;
        User::LeaveIfError( aFile.FullName( fileName ) );
        
        iJadFileName = WriteJadL( fileName, aDescriptorData );         
        }    

    
    iInstallType = EJavaInstall;
    TDataType dataType;    
            
    if ( aSilentInstallOptionsPckg == NULL )
        {
        DLINFO(("Normal install"));
        InitializeInstallerL();
        if( iJadFileName )
            {
            DLTRACE(("Installing JAD+JAR"));
            // JAD+JAR install
            iCancelCode = SwiUI::ERequestInstall;
            iInstaller.Install( iInstallStatusObserver->iStatus, *iJadFileName );
            }
        else
            {
            DLTRACE(("Installing JAR"));
            // JAR install
            iCancelCode = SwiUI::ERequestInstallHandle;
            iInstaller.Install( iInstallStatusObserver->iStatus, aFile );
            }
        
        iInstallStatusObserver->StartToObserve();
        }
    else
        {
        DLINFO(("Silent install"));            
        if( iJadFileName )
            {
            DLINFO(("Silent jad and jar"));
            // JAD+JAR install
            // Set the observer active because it will be informed about the completion
            // of the silent install and it will forward the information for the callback
            // function of this class object.
            iSilentInstallActiveObserver->StartToObserveL( *iJadFileName,
                                                           *aSilentInstallOptionsPckg );
            }
        else
            {
            DLINFO(("Silent jar"));
            // JAR install
            // Set the observer active because it will be informed about the completion
            // of the silent install and it will forward the information for the callback
            // function of this class object.
            iSilentInstallActiveObserver->StartToObserveL( aFile,
                                                           *aSilentInstallOptionsPckg );
            }        
        }

    iBusy = ETrue;

    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Installs Widgets files in normal or in silent way.
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::InstallWidgetL( 
    RFile& aFile,
    const SwiUI::TInstallOptionsPckg* aSilentInstallOptionsPckg )
    {
    DLTRACEIN((""));    
    
    iInstallType = EWidgetInstall;

    // Get the list of installed widget uids so that we can
    // get the uid of the new widget after installation
    PopulateInstalledWidgetUidsL();
    
    TDataType dataType;    
    // Start application installation.
    DLINFO(( "Calling doc handler Open" ));

    if ( !aSilentInstallOptionsPckg )
        {
        DLINFO(("Normal install"));
        InitializeInstallerL();
        iCancelCode = SwiUI::ERequestInstallHandle;
        
        iInstaller.Install( iInstallStatusObserver->iStatus, aFile );
        iInstallStatusObserver->StartToObserve();            
        }
    else
        {
        DLINFO(("Silent install"));        
        // Set the observer active because it will be informed about the completion
        // of the silent install and it will forward the information for the callback
        // function of this class object.
        iSilentInstallActiveObserver->StartToObserveL( aFile,
                                                       *aSilentInstallOptionsPckg );
        }
        
    iBusy = ETrue;

    }

// ---------------------------------------------------------------------------
// Initializes installer
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::InitializeInstallerL()
    {
    DLTRACEIN((""));
    DeletePtr( iInstallStatusObserver );
    iInstallStatusObserver = CNcdActiveOperationObserver::NewL( *this );
    
    if ( !iInstaller.Handle() ) 
        {
        User::LeaveIfError( iInstaller.Connect() );
        }
    }
    

// ---------------------------------------------------------------------------
// Cancels installation
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::CancelInstall()
    {
    DLTRACEIN((""));
    if ( iInstallStatusObserver &&
         iInstaller.Handle() ) 
        {
        DLTRACE(("Cancelling installation"));
        iInstaller.CancelAsyncRequest( iCancelCode );
        }
    
    DeletePtr( iInstallStatusObserver );
    iInstaller.Close();
    }


// ---------------------------------------------------------------------------
// Sets the flags after installation has been finished.
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::InstallationFinishedSetup( TInt aReason )
    {
    DLTRACEIN(("aReason=%d",aReason));

    // Installation has finished.
    iBusy = EFalse;    

    iInstallError = aReason;    
    }


// ---------------------------------------------------------------------------
// Handles the situation when installation was finished even if cancel request
// was issued for silent install operation
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::HandleSilentInstallSuccessAfterCancelL( HBufC*& aFileName,
                                                                      TUid& aAppUid,
                                                                      TInt& aError )
    {
    DLTRACEIN((""));
    
    // Set the error to KErrNone.
    // So, initially we think that installation was success.
    // If we later notice that installation was not finished,
    // then we suppose that cancellation was success and set the
    // value to KErrCancel
    aError = KErrNone;
    
    if ( iInstallType == EJavaInstall )
        {    
        // Set the midlet UID for the aAppUid variable.
        aAppUid = InstalledMidletUidL();
        
        if ( aAppUid == KNullUid )
            {
            DLINFO(("Installed midlet was not found"));
            // Because the new midlet was not found, we can suppose that the
            // application was not installed. So, set the error as KErrCancel.
            aError = KErrCancel;
            }
        }
    else if ( iInstallType == ESisInstall )
        {
        // Get theme name if such was installed
        const TDesC& name( LatestThemeL() );
                
        // Notify about theme installation
        if ( iPackageUid.iUid == KNcdThemeSisUid || name != KNullDesC ) 
            {            
            DLINFO(( _L("Theme installation, theme: %S"), &name ));
            // Ensure that uid is theme uid
            iPackageUid.iUid = KNcdThemeSisUid;

            // Set the uid value for the reference parameter
            aAppUid = iPackageUid;
            
            if ( name == KNullDesC && iThemePackageUid.iUid != KNcdThemeSisUid ) 
                {
                DLINFO(("Checking theme installation status with package uid, %x", 
                        iThemePackageUid.iUid ));
                
                // Check if the theme was indeed successfully installed
                TBool themeInstalled( EFalse );                
                TRAP_IGNORE( 
                    themeInstalled = IsApplicationInstalledL( iThemePackageUid ) );
                    
                if ( !themeInstalled )
                    {
                    DLINFO(("Theme was not installed"));
                    // Theme was not installed. So, let us suppose that cancel
                    // was success after all
                    aError = KErrCancel;
                    // Because name is KNullDesC, do not set the aFileName here
                    }
                }
            else 
                {
                DLTRACE(("Either theme name was acquired or package uid was theme"));
                if ( name != KNullDesC )
                    {                    
                    aFileName = name.AllocL();
                    }
                aAppUid = iPackageUid;
                }
            }
        else 
            {                    
            // Get the actual app UID from sis registry
            TRAPD( err, aAppUid = SidFromSisRegistryL( iPackageUid ) );
            
            DLTRACE(("Notify observer about application installation, uid: %x", 
                aAppUid.iUid ));
            
            if ( err == KErrNotFound ) 
                {
                DLINFO(("Didn't get SID, using package UID: %x", 
                        iPackageUid.iUid));
                aAppUid.iUid = iPackageUid.iUid;
                err = KErrNone;
                }
            
            if ( err == KErrNone ) 
                {
                DLTRACE(("Check the application is actually installed"));          
                TBool installed = IsApplicationInstalledL( aAppUid );
                if ( !installed ) 
                    {
                    aError = KErrCancel;
                    }
                }
            }
        }
    else 
        {
        DLTRACE(("Was not Java nor SIS install"));
        // This is executed after a content file has been moved.
        }
        
    DLTRACEOUT((""));
    }

 
// ---------------------------------------------------------------------------
// Creates a list of applications that are in ROM but that are not 
// in the SIS or Java registry
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::InitializeRomApplicationListL()
    {
    DLTRACEIN((""));
        
    TApaAppInfo info;
    ConnectApaLsL();    
    User::LeaveIfError( iApaLs.GetAllApps() );
    
    iRomUids.Reset();
    
    RArray<TUid> midletUids;
    CleanupClosePushL( midletUids );
    
    MidletUidsL( midletUids );
    
    DLTRACE(("%d Midlet UIDs", midletUids.Count() ));

#ifdef CATALOGS_BUILD_CONFIG_DEBUG
    TInt appCount = 0;
    iApaLs.AppCount( appCount );
    DLTRACE(("apps: %d", appCount));
#endif // CATALOGS_BUILD_CONFIG_DEBUG
    
    // If the application list is not OK after 5 tries, give up
    // Each break is 0.5 seconds
    TInt retryCount = 5;
    TInt appErr = KErrNone;    

    do
        {       
        appErr = iApaLs.GetNextApp( info );
        if ( appErr == KErrNone )
            {            
            // App is considered a ROM app if its not found either
            // in SIS registry or midlet registry
            // Note: ROM apps can be in SIS registry also but that doesn't
            // matter because we just want to get a list of apps that are
            // not found anywhere else but apparc
            if ( midletUids.Find( info.iUid ) == KErrNotFound &&
                 !iRegistrySession.IsSidPresentL( info.iUid ) ) 
                {
                
                DLTRACE(( _L("Found ROM app: %x, caption: %S, path: %S"), 
                    info.iUid.iUid,
                    &info.iCaption,
                    &info.iFullName ));
                iRomUids.AppendL( info.iUid );
                }
            }
        else if( appErr == RApaLsSession::EAppListInvalid )
            {
            // Application list is not yet fully populated
            // https://jira.bothi.fi/jira/browse/PRECLI-1364
            if ( retryCount > 0 )
                {
                DLINFO(( "Application list not yet populated, waiting" ));
                User::After( KDelayWhenAppListInvalid );
                retryCount--;
                appErr = KErrNone;
                }
            else
                {
                User::Leave( KErrNotReady );
                }
            }
        // Some unknown error. That shouldn't happen.
        else if( appErr != RApaLsSession::ENoMoreAppsInList ) 
            {
            DLERROR(("Unknown error with application list. %d", appErr));
            User::Leave( KErrGeneral );
            }
        }
    while( appErr == KErrNone && retryCount >= 0 );
        
    iApaLs.Close();
    CleanupStack::PopAndDestroy( &midletUids );    
    
    DLTRACEOUT(("ROM apps: %d", iRomUids.Count() ));
    }


// ---------------------------------------------------------------------------
// Gets a list of installed midlet UIDs
// ---------------------------------------------------------------------------
//
#ifdef USE_OLD_JAVA_API

void CNcdInstallationService::MidletUidsL( RArray<TUid>& aUids )
    {
    DLTRACEIN((""));
    MJavaRegistry* javaRegistry = MJavaRegistry::CreateL();
    CleanupReleasePushL( *javaRegistry );
    javaRegistry->InstalledMIDletUidsL( aUids );
    CleanupStack::PopAndDestroy( javaRegistry );    
    }

#else

void CNcdInstallationService::MidletUidsL( RArray<TUid>& aUids )
    {
    DLTRACEIN((""));
    CJavaRegistry* javaRegistry = CJavaRegistry::NewLC();
    javaRegistry->GetRegistryEntryUidsL( aUids );          
    CleanupStack::PopAndDestroy( javaRegistry );    
    }

#endif


// ---------------------------------------------------------------------------
// Checks if the application is in ROM
// ---------------------------------------------------------------------------
//
TBool CNcdInstallationService::IsRomApplication( const TUid& aUid ) const
    {
    DLTRACEIN(("UID: %x", aUid.iUid ));
    return iRomUids.Find( aUid ) != KErrNotFound;    
    }


// ---------------------------------------------------------------------------
// Returns the UID of the latest installed midlet, NULL UID if none have
// been installed since the last device restart
// ---------------------------------------------------------------------------
//
#ifdef USE_OLD_JAVA_API

TUid CNcdInstallationService::LatestMidletUidL( 
    MJavaRegistry& aJavaRegistry ) const
    {
    DLTRACEIN((""));
    TInt suiteUid = 0;            
    
    // Get UID for the latest installed Midlet suite
    // KPSUidJavaLatestInstallation = 0x10282567
    // Ignoring error in case the key or read policy change so that client
    // doesn't behave strangely
    RProperty::Get( KUidSystemCategory, 
        KPSUidJavaLatestInstallation, suiteUid );
    
    DLTRACE(("JMI UID: %x", suiteUid ));

    if ( !suiteUid )  
        {
        return KNullUid;
        }
    
    // Get entry for the installed suite
    MJavaRegistrySuiteEntry* suite = aJavaRegistry.SuiteEntryL( 
        TUid::Uid( suiteUid ) );
    CleanupReleasePushL( *suite );        
    RArray<TUid> suiteUids;
    CleanupClosePushL( suiteUids );

    TUid midletUid = KNullUid; 
    suite->MIDletUidsL( suiteUids );
    
    // Take first midlet UID from the suite
    if ( suiteUids.Count() ) 
        {
        midletUid = suiteUids[0];
        }
    DLTRACE(("Midlets in suite: %d", suite->NumberOfMIDletsL() ));
    CleanupStack::PopAndDestroy( &suiteUids );
    DLTRACE(("InstalledAppsEntryUid: %x", midletUid.iUid ));

    CleanupStack::PopAndDestroy( suite );    
    return midletUid;
    }

#else

TUid CNcdInstallationService::LatestMidletUidL( 
    CJavaRegistry& aJavaRegistry ) const
    {
    DLTRACEIN((""));
    TInt suiteUid = 0;            
    
    // Get UID for the latest installed Midlet suite
    // KPSUidJavaLatestInstallation = 0x10282567
    // Ignoring error in case the key or read policy change so that client
    // doesn't behave strangely
    RProperty::Get( KUidSystemCategory, 
        KPSUidJavaLatestInstallation, suiteUid );
    
    DLTRACE(("JMI UID: %x", suiteUid ));

    if ( !suiteUid )  
        {
        return KNullUid;
        }
    
    // Get entry for the installed suite
    CJavaRegistryEntry* suite = aJavaRegistry.RegistryEntryL( 
        TUid::Uid( suiteUid ) );
    
    if ( !suite )
        {
        return KNullUid;
        }
    
    CleanupStack::PushL( suite );        
    
    DASSERT( suite->Type() < EGeneralApplication && 
             suite->Type() >= EGeneralPackage );
    
    CJavaRegistryPackageEntry* entry = 
        static_cast<CJavaRegistryPackageEntry*>( suite );
    
    TUid midletUid = KNullUid;
    TInt count = entry->NumberOfEmbeddedEntries(); 
    TBool appFound = EFalse;
    TInt index = 0;
    
    // Find the first application from the suite
    while ( index < count && !appFound )
        {
        CJavaRegistryEntry* app = entry->EmbeddedEntryByNumberL( index );
        if ( app->Type() >= EGeneralApplication ) 
            {
            midletUid = app->Uid();
            appFound = ETrue;
            DLTRACE(( "Found app: %x", midletUid.iUid ));
            }
        delete app;
        ++index;
        }
    
    CleanupStack::PopAndDestroy( suite );    
    return midletUid;
    }


#endif

// ---------------------------------------------------------------------------
// Returns true if the MIME type matches a Java application or descriptor
// ---------------------------------------------------------------------------
//
TBool CNcdInstallationService::MatchJava( const TDesC& aMime )
    {
    return ( aMime.MatchF( KMimeTypeMatch1JavaApplication ) != KErrNotFound || 
             aMime.MatchF( KMimeTypeMatch2JavaApplication ) != KErrNotFound ||
             aMime.MatchF( KMimeTypeMatchJad ) != KErrNotFound );        
    }


#ifdef USE_OLD_JAVA_API

TUid CNcdInstallationService::InstalledMidletUidL()
    {
    DLTRACEIN((""));
    RArray<TUid> MIDletUids;
    CleanupClosePushL( MIDletUids );
    
    MJavaRegistry* javaRegistry = MJavaRegistry::CreateL();
    CleanupReleasePushL( *javaRegistry );
    javaRegistry->InstalledMIDletUidsL( MIDletUids );
    TUid MIDletUid = KNullUid;
    // Search for new uids in Java registry.
    for ( TInt i = 0 ; i < MIDletUids.Count() ; i++ )
        {
        if ( iMIDletUids.Find( MIDletUids[i] ) == KErrNotFound )
            {
            // A new uid found, this is the installed midlet's uid
            MIDletUid = MIDletUids[i];
            break;
            }
        }

    // We didn't get any new UID so we have to check Java installer's
    // P&S key for the installed suite UID and the get the midlet UID
    // from that. This happens when a midlet with predefined UID, 
    // eg. WidSets, is reinstalled. Midlet UIDs are predefined with
    // the attribute Nokia-MIDlet-UID-<n> in a JAD or JAR manifest
    if ( MIDletUid == KNullUid ) 
        {
        MIDletUid = LatestMidletUidL( *javaRegistry );
        }
    
    CleanupStack::PopAndDestroy( javaRegistry );
    CleanupStack::PopAndDestroy( &MIDletUids );

    iMIDletUids.Reset();
    return MIDletUid;
    }

#else // USE_OLD_JAVA_API

TUid CNcdInstallationService::InstalledMidletUidL()
    {
    DLTRACEIN((""));
    CJavaRegistry* registry = CJavaRegistry::NewLC();
    TUid midletUid = LatestMidletUidL( *registry );
    CleanupStack::PopAndDestroy( registry );
    return midletUid;
    }

#endif // USE_OLD_JAVA_API


// ---------------------------------------------------------------------------
// Populates the list of installed widgets
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::PopulateInstalledWidgetUidsL() 
    {
    DLTRACEIN((""));

    if ( !iWidgetRegistry.Handle() )
        {
        User::LeaveIfError( iWidgetRegistry.Connect() );
        }
    
    iInstalledWidgets.ResetAndDestroy();
    User::LeaveIfError( iWidgetRegistry.InstalledWidgetsL( iInstalledWidgets ) );    
    }


// ---------------------------------------------------------------------------
// Gets the name of widget that was installed last
// ---------------------------------------------------------------------------
//
HBufC* CNcdInstallationService::InstalledWidgetNameLC()
    {
    DLTRACEIN((""));
    
    TUid widgetUid = InstalledWidgetUidL();
    
    if ( widgetUid == KNullUid )
        {
        DLERROR(("No widget uid"));
        // No new UID was found, so we assume user canceled the installation.
        // Installer does not give any error code in that case.
        return NULL;
        }

    HBufC* bundleId = HBufC::NewLC( KWidgetBundleIdLength );
    TPtr des( bundleId->Des() );
    iWidgetRegistry.GetWidgetBundleId( widgetUid, des );            

    DLTRACEOUT(( _L("Widget bundle id: %S"), bundleId ));
    return bundleId;
    }
    

// ---------------------------------------------------------------------------
// Gets the UID of the widget that was just installed 
// ---------------------------------------------------------------------------
//
TUid CNcdInstallationService::InstalledWidgetUidL()
    {
    DLTRACEIN((""));
    
    RWidgetInfoArray widgets;
    CleanupResetAndDestroyPushL( widgets );
        
    User::LeaveIfError( iWidgetRegistry.InstalledWidgetsL( widgets ) );
    DLINFO(("widget count: %d", widgets.Count() ));
    
    TUid uid( KNullUid );
    
    const TInt count = widgets.Count();
    const TInt installedCount = iInstalledWidgets.Count();
    
    // Try to find a widget that was not installed earlier.
    for ( TInt i = 0; i < count; ++i )
        {
        TBool wasInstalled = EFalse;
        CWidgetInfo* widgetInfo = widgets[ i ];
        for ( TInt j = 0; j < installedCount; j++ )
            {
            if ( iInstalledWidgets[ j ]->iUid == widgetInfo->iUid )
                {
                wasInstalled = ETrue;
                break;
                }
            }
        
        if ( !wasInstalled ) 
            {
            DLTRACE(("Found installed widget"));
            uid = widgets[ i ]->iUid;
            break;
            }
        }
        
    CleanupStack::PopAndDestroy( &widgets );        

    DLTRACEOUT(("Installed uid: %x", uid.iUid ));
    return uid;
    }


// ---------------------------------------------------------------------------
// Checks from the MIME type and file extension and returns ETrue if
// either of them matches widgets
// ---------------------------------------------------------------------------
//
TBool CNcdInstallationService::MatchWidget( RFile& aFile, const TDesC& aMime )
    {
    DLTRACEIN((""));
    if ( aMime.CompareF( KMimeTypeMatchWidget ) == 0 )
        {
        DLTRACEOUT(( "Mime type matches" ));
        return ETrue;
        }
    
    TBool match = EFalse;
    TFileName filename;
    TInt err = aFile.Name( filename );
    if ( err == KErrNone )
        {
        TParsePtrC parse( filename );
        match = parse.ExtPresent() &&
                ( KWidgetExtension().CompareF( parse.Ext() ) == 0 );            
        }
    DLTRACEOUT(( "Filename match: %d", match ));
    return match;
    }


// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CNcdInstallationService::HandleInstalledWidgetL()
    {
    DLTRACEIN((""));
    TUid uid( KNullUid );
    if ( iInstallError == KErrNone )
        {
        uid = InstalledWidgetUidL();
        if ( uid == KNullUid )
            {
            iInstallError = KErrAbort;
            }        
        }
    
    iObserver->InstallationCompleteL( KNullDesC, uid, iInstallError );
    }


// ---------------------------------------------------------------------------
//   
// ---------------------------------------------------------------------------
//
TBool CNcdInstallationService::WidgetExistsL( const TUid& aUid )
    {
    DLTRACEIN((""));

    if ( !iWidgetRegistry.Handle() )
        {
        User::LeaveIfError( iWidgetRegistry.Connect() );
        }
    
    if ( iWidgetRegistry.IsWidget( aUid ) )
        {
        TBuf<KWidgetBundleIdLength> id;
        iWidgetRegistry.GetWidgetBundleId( aUid, id );
        return iWidgetRegistry.WidgetExistsL( id );
        }
    return EFalse;         
    }
