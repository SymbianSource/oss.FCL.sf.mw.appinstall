/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CNcdProviderUtils implementation
*
*/


#include <s32file.h>
#include <bautils.h>
#include "ncdproviderutils.h"
#include "catalogsdebug.h"
#include "ncdengineconfigurationimpl.h"
#include "ncddeviceinteractionfactory.h"
#include "ncddeviceserviceimpl.h"
#include "ncdinstallationserviceimpl.h"
#include "catalogsutils.h"
#include "ncdproviderdefines.h"
#include "catalogsstringmanager.h"
#include "ncdhttputils.h"

#ifdef CATALOGS_BUILD_CONFIG_DEBUG
    #include "ncdtestconfig.h"
    _LIT( KTestConfigFile, "c:\\data\\others\\ncdtestnetwork.cfg" );
#endif    


RFs CNcdProviderUtils::iFs;
CNcdEngineConfiguration* CNcdProviderUtils::iConfig = NULL;
MNcdDeviceService* CNcdProviderUtils::iDeviceService = NULL;
MNcdInstallationService* CNcdProviderUtils::iInstallationService = NULL;

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdProviderUtils* CNcdProviderUtils::NewL( const TDesC& aFilename )    
    {
    CNcdProviderUtils* self = new( ELeave ) CNcdProviderUtils();
    CleanupStack::PushL( self );
    self->ConstructL( aFilename );
    CleanupStack::Pop( self );
    return self;
    }
    
    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdProviderUtils::~CNcdProviderUtils()
    {    
    delete iConfig;
    delete iDeviceService;
    delete iInstallationService;    
    iFs.Close();
    CCatalogsStringManager::Delete();
    }


// ---------------------------------------------------------------------------
// File session getter
// ---------------------------------------------------------------------------
//
RFs& CNcdProviderUtils::FileSession()
    {
    return iFs;
    }


// ---------------------------------------------------------------------------
// Engine configuration getter
// ---------------------------------------------------------------------------
//
MNcdEngineConfiguration& CNcdProviderUtils::EngineConfig()    
    {
    DASSERT( iConfig );
    return *iConfig;
    }


// ---------------------------------------------------------------------------
// Installation service getter
// ---------------------------------------------------------------------------
//
MNcdInstallationService& CNcdProviderUtils::InstallationServiceL()    
    {
    if ( !iInstallationService )
        {
        iInstallationService = 
            NcdDeviceInteractionFactory::CreateInstallationServiceL();            
        }
    return *iInstallationService;
    }


// ---------------------------------------------------------------------------
// Device service getter
// ---------------------------------------------------------------------------
//
MNcdDeviceService& CNcdProviderUtils::DeviceService()    
    {
    DASSERT( iDeviceService );
    return *iDeviceService;
    }



// ---------------------------------------------------------------------------
// Temp path getter
// ---------------------------------------------------------------------------
//
HBufC* CNcdProviderUtils::TempPathLC( const TDesC& aClientId )
    {
    DASSERT( iConfig );
    return iConfig->ClientDataPathLC( aClientId, ETrue );
    }



// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CNcdProviderUtils::ReadDatabaseVersionsL( 
    const TDesC& aRootPath,
    TUint32& aGeneralVersion, 
    TUint32& aPurchaseHistoryVersion )
    {
    DLTRACEIN((""));        
    
    RBuf path;
    AppendPathsLC( 
        path, 
        aRootPath, 
        NcdProviderDefines::KNcdDatabaseVersionFile );
    
    
    RFileReadStream stream;
    CleanupClosePushL( stream );
    User::LeaveIfError( stream.Open( FileSession(), 
        path, EFileRead ) );
    aGeneralVersion = stream.ReadUint32L();
    aPurchaseHistoryVersion = stream.ReadUint32L();
    CleanupStack::PopAndDestroy( 2, &path ); // stream, path
    DLTRACEOUT(("Versions, general: %d, purchase history: %d",
        aGeneralVersion, aPurchaseHistoryVersion ));
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CNcdProviderUtils::WriteDatabaseVersionsL(
    const TDesC& aRootPath,
    TUint32 aGeneralVersion,
    TUint32 aPurchaseHistoryVersion )
    {
    DLTRACEIN(("Writing versions, general: %d, purchase history: %d",
        aGeneralVersion, aPurchaseHistoryVersion ));

    RBuf path;
    
    AppendPathsLC( 
        path, 
        aRootPath, 
        NcdProviderDefines::KNcdDatabaseVersionFile );
        
    RFileWriteStream stream;
    CleanupClosePushL( stream );
    User::LeaveIfError( stream.Replace( FileSession(),
        path, EFileWrite ) );
    stream.WriteUint32L( aGeneralVersion );
    stream.WriteUint32L( aPurchaseHistoryVersion );
    CleanupStack::PopAndDestroy( 2, &path );
    DLTRACEOUT(("Versions successfully written"));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TInt CNcdProviderUtils::UpdateShutdownFileL( 
    const TDesC& aRootPath )
    {
    DLTRACEIN((""));        
    
    RBuf path;       
    
    AppendPathsLC( 
        path, 
        aRootPath,
        NcdProviderDefines::KNcdProviderShutdownFile );
        
    BaflUtils::EnsurePathExistsL( FileSession(), aRootPath );
    
    // Try to read the value from the file if it exists, otherwise create a
    // new file.
    RFile file;
    CleanupClosePushL( file );
    TInt previousStartups = 0;
    
    TInt err = file.Open( FileSession(), path, EFileWrite );
    if ( err == KErrNone ) 
        {
        HBufC8* data = ReadFileL( file );
        if ( data->Length() ) 
            {            
            previousStartups = Des8ToInt( *data );
            DLTRACE(("Previous startups without good shutdown: %d", 
                previousStartups ));           
            }
        delete data;
        data = NULL;        
        User::LeaveIfError( file.Seek( ESeekStart, err ) );
        }
    else
        {
        DLTRACE(("No shutdown file, creating"));        
        User::LeaveIfError( file.Replace( FileSession(), path, EFileWrite ) );
        }
        
    // Update new count to the file
    previousStartups++;
    HBufC8* newData = IntToDes8LC( previousStartups );
    User::LeaveIfError( file.Write( *newData ) );     

    CleanupStack::PopAndDestroy( 3, &path ); // newData, file, path
    DLTRACEOUT(("Wrote previousStartups as: %d", previousStartups));    
    return previousStartups;
    }


// ---------------------------------------------------------------------------
// Removes the shutdown file
// ---------------------------------------------------------------------------
//
void CNcdProviderUtils::RemoveShutdownFileL( const TDesC& aRootPath )
    {
    DLTRACEIN((""));        
    
    RBuf path;
    AppendPathsLC( 
        path, 
        aRootPath, 
        NcdProviderDefines::KNcdProviderShutdownFile() );    

    User::LeaveIfError( BaflUtils::DeleteFile( FileSession(), path ) );
    CleanupStack::PopAndDestroy( &path );
    }
    


// ---------------------------------------------------------------------------
// Checks if the application with the given uid is installed
// ---------------------------------------------------------------------------
//
TNcdApplicationStatus CNcdProviderUtils::IsApplicationInstalledL( 
    const TUid& aUid, const TDesC& aVersion )
    {
    DLTRACEIN(( _L("Uid: %x, version: %S"), aUid.iUid, &aVersion ));
    
    TCatalogsVersion version;
    TRAPD( err, TCatalogsVersion::ConvertL( version, aVersion ) );
    LeaveIfNotErrorL( err, KErrArgument, KErrGeneral );
    
    return InstallationServiceL().IsApplicationInstalledL( aUid, version );
    }

// ---------------------------------------------------------------------------
// Checks if the widget with the given identifier is installed
// call CNcdInstallationService in deviceinteraction dll
// ---------------------------------------------------------------------------
//
TNcdApplicationStatus CNcdProviderUtils::IsWidgetInstalledL( 
    const TDesC& aIdentifier, const TDesC& aVersion )
    {
    TCatalogsVersion version;
    TRAPD( err, TCatalogsVersion::ConvertL( version, aVersion ) );
    LeaveIfNotErrorL( err, KErrArgument, KErrGeneral );
                             
    return InstallationServiceL().IsWidgetInstalledL( aIdentifier, version );
    }

// ---------------------------------------------------------------------------
// Calling widget registry API to return the Uid of the widget with given ident
// ---------------------------------------------------------------------------
//
TUid CNcdProviderUtils::WidgetUidL( const TDesC& aIdentifier)
    {
    return InstallationServiceL().WidgetUidL( aIdentifier );
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CNcdProviderUtils::CompareVersionsL( 
    const TDesC& aLeft, 
    const TDesC& aRight )
    {
    DLTRACEIN(( _L("Left: %S, right: %S"), &aLeft, &aRight ));
    
    TCatalogsVersion leftVersion;
    TCatalogsVersion::ConvertL( leftVersion, aLeft );

    TCatalogsVersion rightVersion;
    TCatalogsVersion::ConvertL( rightVersion, aRight );
    
    if ( leftVersion > rightVersion ) 
        {
        return 1;
        }
    else if ( leftVersion == rightVersion ) 
        {
        return 0;
        }
    return -1;    
    }
    


 
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdProviderUtils::CNcdProviderUtils()
    {
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CNcdProviderUtils::ConstructL( const TDesC& aFilename )
    {    
    DLTRACEIN((""));
    User::LeaveIfError( iFs.Connect() );
    
    // This creates the string manager to its own static variable
    CCatalogsStringManager::SetStringManager( 
        CCatalogsStringManager::NewL() );
        
    iDeviceService = NcdDeviceInteractionFactory::CreateDeviceServiceL();
    
    // Read test configuration for device service 
    #ifdef CATALOGS_BUILD_CONFIG_DEBUG
    
    TRAP_IGNORE( 
        {
        // Leaves if the parsing fails for any reason
        CNcdTestConfig* testConfig = CNcdTestConfig::NewL( 
            FileSession(), KTestConfigFile() );
            
        DLINFO(("Setting test network config"));
        // Ownership is transferred
        iDeviceService->SetTestConfig( testConfig );
        });
    
    #endif // CATALOGS_BUILD_CONFIG_DEBUG

    
    iConfig = CNcdEngineConfiguration::NewL( *iDeviceService );
    
    TRAPD( err, 
        iConfig->ReadConfigurationL( aFilename ) ); 
    if ( err != KErrNone && err != KErrNotFound ) 
        {
        User::Leave( err );
        }
    }
    
    

