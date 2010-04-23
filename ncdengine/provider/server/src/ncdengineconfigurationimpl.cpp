/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CNcdEngineConfiguration implementation
 *
*/


#include <bautils.h>

#include "ncdengineconfigurationimpl.h"
#include "ncdproviderutils.h"
#include "catalogsutils.h"
#include "catalogsuids.h"
#include "ncdprotocolutils.h"
#include "catalogsconstants.h"
#include "ncdproviderdefines.h"
#include "ncdutils.h"
#include "ncddeviceservice.h"

namespace NcdEngineConfiguration
    {
    _LIT8( KVersion, "version" );
    _LIT8( KType, "type" );
    _LIT8( KProvisioning, "provisioning" );
    _LIT8( KNetwork, "network" );
    _LIT8( KMcc, "mcc" );
    _LIT8( KMnc, "mnc" );
    _LIT8( KFixedAp, "fixed-access-point" );
    _LIT8( KAccessPointSettings, "access-point-settings" );
    _LIT8( KApId, "id" );
    _LIT8( KApDetail, "detail" );
    _LIT8( KApDetailId, "id" );
    _LIT8( KApDetailValue, "value" );
    }

_LIT( KEngineDefaultType, "vanilla" );
_LIT( KEngineDefaultVersion, "1.0" );


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdEngineConfiguration* CNcdEngineConfiguration::NewL( MNcdDeviceService& aDeviceService )
    {
    CNcdEngineConfiguration* self = new( ELeave ) CNcdEngineConfiguration( aDeviceService );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdEngineConfiguration::~CNcdEngineConfiguration()
    {
    DLTRACEIN((""));
    delete iType;
    delete iVersion;
    delete iProvisioning;
    delete iUid;    
    delete iMcc;
    delete iMnc;
    delete iApId;
    delete iApDetailId;
    delete iApDetailValue;
    delete iCorrectApId;
    
    iHomeMcc.Close();
    iHomeMnc.Close();
    
    iApDetails.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// Configuration reader
// ---------------------------------------------------------------------------
//
void CNcdEngineConfiguration::ReadConfigurationL( const TDesC& aFilename )
    {
    DLTRACEIN(( _L("Config file: %S"), &aFilename ));
    CNcdConfigurationParser* parser = CNcdConfigurationParser::NewLC( *this );
    DLTRACE(("Reading file"));

    // find the file from engine's private paths
    HBufC* filename = FindEngineFileL( CNcdProviderUtils::FileSession(),
        aFilename );
    CleanupStack::PushL( filename );                        

    // Read the file
    HBufC8* data = ReadFileL( CNcdProviderUtils::FileSession(), *filename );
    
    CleanupStack::PopAndDestroy( filename );
    CleanupStack::PushL( data );
    DLTRACE(("Start parsing"));
    
    // Parse the file
    parser->ParseL( *data );
    CleanupStack::PopAndDestroy( 2, parser ); // data, parser
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC& CNcdEngineConfiguration::EngineType() const
    {
    return *iType;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC& CNcdEngineConfiguration::EngineVersion() const
    {
    return *iVersion;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC& CNcdEngineConfiguration::EngineUid() const
    {
    return *iUid;
    }

// ---------------------------------------------------------------------------
// Provisioning getter
// ---------------------------------------------------------------------------
//
const TDesC& CNcdEngineConfiguration::EngineProvisioning() const
    {    
    return *iProvisioning;
    }


// ---------------------------------------------------------------------------
// Installation drive getter
// ---------------------------------------------------------------------------
//
const TDesC& CNcdEngineConfiguration::EngineInstallDrive() const
    {
    return iInstallationDrive;
    }


// ---------------------------------------------------------------------------
// Engine temp drive
// ---------------------------------------------------------------------------
//
TInt CNcdEngineConfiguration::EngineTempDrive() const
    {
    return static_cast<TInt>( iDataDrive );
    }
    

// ---------------------------------------------------------------------------
// Client data path getter
// ---------------------------------------------------------------------------
//
HBufC* CNcdEngineConfiguration::ClientDataPathLC(
    const TDesC& aClientId,
    TBool aTemp )
    {
    DLTRACEIN((""));
    TPath path;
    CreatePrivatePathL( 
        CNcdProviderUtils::FileSession(), 
        iDataDrive.Name(), 
        path );

    path.Append( aClientId );
    path.Append( KDirectorySeparator );
    if ( aTemp ) 
        {
        path.Append( NcdProviderDefines::KTempNamespace() );
        }
    else
        {
        path.Append( NcdProviderDefines::KDataNamespace() );
        }
    path.Append( KDirectorySeparator );
    BaflUtils::EnsurePathExistsL( CNcdProviderUtils::FileSession(), path );
    
    DLTRACEOUT( ( _L("Path: %S"), &path ) );
    return path.AllocLC();
    }

    
TBool CNcdEngineConfiguration::UseFixedAp() const 
    {
    DLTRACEIN((""));
    TBool retValue = iApDetails.Count() > 0;
    DLINFO(("retValue: %d", retValue ))
    return retValue;
    }
    
    
const RPointerArray<CNcdKeyValuePair>& CNcdEngineConfiguration::FixedApDetails() const 
    {
    DLTRACEIN((""));
    return iApDetails;
    }


// ---------------------------------------------------------------------------
// Client data path cleaner
// ---------------------------------------------------------------------------
//
void CNcdEngineConfiguration::ClearClientDataL( 
    const TDesC& aClientId, 
    TBool aTemp )
    {
    DLTRACEIN(( _L("aClient: %S"), &aClientId ));
    // Get the path and delete the directory
    HBufC* path = ClientDataPathLC( aClientId, aTemp );
    CFileMan* fileman = CFileMan::NewL( CNcdProviderUtils::FileSession() );
    CleanupStack::PushL( fileman );    
    User::LeaveIfError( fileman->RmDir( *path ) );
    CleanupStack::PopAndDestroy( 2, path ); // fileman, path        
    
    DLTRACEOUT(("Client data path cleaned successfully"));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdEngineConfiguration::ConfigurationElementEndL( 
    const TDesC8& aElement, 
    const TDesC8& aData )
    {
    DLTRACEIN(( "Element: %S, data: %S", &aElement, &aData ));
    
    if ( aElement == NcdEngineConfiguration::KAccessPointSettings() ) 
        {
        iParseApDetails = EFalse;
        }
    else if ( iParseApDetails && aElement == NcdEngineConfiguration::KApDetail() )
        {
        DASSERT( iApDetailId && iApDetailValue );
        CNcdKeyValuePair* detail = CNcdKeyValuePair::NewLC( *iApDetailId, *iApDetailValue );
        iApDetails.AppendL( detail );
        CleanupStack::Pop( detail );
        delete iApDetailId;
        iApDetailId = NULL;
        delete iApDetailValue;
        iApDetailValue = NULL;
        }
    else if ( !aData.Length() )
        {
        DLTRACEOUT(("No data."));
        return;
        }        
    else if ( aElement == NcdEngineConfiguration::KType() ) 
        {        
        NcdProtocolUtils::AssignDesL( iType, aData );
        }
    else if ( aElement == NcdEngineConfiguration::KVersion() ) 
        {
        NcdProtocolUtils::AssignDesL( iVersion, aData );
        }
    else if ( aElement == NcdEngineConfiguration::KProvisioning() ) 
        {
        NcdProtocolUtils::AssignDesL( iProvisioning, aData );
        }
    else 
        {
        DLINFO(("Unknown element in the engine configuration file"));
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdEngineConfiguration::ConfigurationAttributeL( 
    const TDesC8& aElement, 
    const TDesC8& aAttribute, 
    const TDesC8& aValue )
    {
    DLTRACEIN(( "Element: %S, attribute: %S, value: %S", &aElement, &aAttribute, &aValue ));
    
    if ( aElement == NcdEngineConfiguration::KNetwork() && !iApIdFound ) 
        {
        if ( aAttribute == NcdEngineConfiguration::KMcc() ) 
            {
            NcdProtocolUtils::AssignDesL( iMcc, aValue );
            }
        else if ( aAttribute == NcdEngineConfiguration::KMnc() ) 
            {
            NcdProtocolUtils::AssignDesL( iMnc, aValue );
            }
        else if ( aAttribute == NcdEngineConfiguration::KFixedAp() ) 
            {
            NcdProtocolUtils::AssignDesL( iApId, aValue );
            }
        if ( iMcc && iMnc && iApId ) 
            {
            // All the attributes read, check whether the MCC/MNC match the current ones.
            iDeviceService.HomeNetworkInfoL( iHomeMcc, iHomeMnc );
            if ( iHomeMcc == *iMcc && iHomeMnc == *iMnc ) 
                {
                // Now we know the AP id we must use.
                iApIdFound = ETrue;
                NcdProtocolUtils::AssignDesL( iCorrectApId, *iApId );
                DLINFO((("correct ap id found: %S"), iCorrectApId ));
                }
            delete iMcc;
            iMcc = NULL;
            delete iMnc;
            iMnc = NULL;
            delete iApId;
            iApId = NULL;  
            }
        }                
    
    if ( aElement == NcdEngineConfiguration::KAccessPointSettings() ) 
        {
        if ( aAttribute == NcdEngineConfiguration::KApId() ) 
            {
            if ( iApIdFound && *iCorrectApId == aValue ) 
                {
                // This is the correct AP.
                iParseApDetails = ETrue;
                }
            }
        else 
            {
            DLINFO(("Unknown attribute in the engine configuration file"));
            }
        }
        
    if ( iParseApDetails && aElement == NcdEngineConfiguration::KApDetail() ) 
        {
        if ( aAttribute == NcdEngineConfiguration::KApDetailId() ) 
            {            
            NcdProtocolUtils::AssignDesL( iApDetailId, aValue );
            }
        else if ( aAttribute == NcdEngineConfiguration::KApDetailValue() ) 
            {
            NcdProtocolUtils::AssignDesL( iApDetailValue, aValue );
            }
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdEngineConfiguration::ConfigurationError( TInt /*aError*/ )
    {
    DLERROR(("Some error occurred during parsing"));
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdEngineConfiguration::CNcdEngineConfiguration( MNcdDeviceService& aDeviceService )
    : iParseApDetails( EFalse ), iApIdFound( EFalse ), iDeviceService( aDeviceService )
    {
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdEngineConfiguration::ConstructL()
    {
    DLTRACEIN((""));
    iType = KEngineDefaultType().AllocL();
    iVersion = KEngineDefaultVersion().AllocL();
    iProvisioning = KNullDesC().AllocL();
    iUid = CleanUidName( TUid::Uid( KCatalogsServerUid ) ).AllocL();
    iHomeMcc.CreateL( MNcdDeviceService::KMccLength );
    iHomeMnc.CreateL( MNcdDeviceService::KMncLength );
    
    TFileName engineExe( RProcess().FileName() );
    TParsePtrC parseDrive( engineExe );
    iInstallationDrive = parseDrive.Drive();
        
    // Determine the drive used for temp and data
    iDataDrive = TDriveUnit( DetermineDataDriveL() );    
       
    DLTRACEOUT(( _L("Install drive: %S"), &iInstallationDrive ));
    }


// ---------------------------------------------------------------------------
// Determines the drive where data files are stored (temporarily)
// ---------------------------------------------------------------------------
//
TDriveNumber CNcdEngineConfiguration::DetermineDataDriveL() const
    {
    DLTRACEIN((""));
    TInt64 freeOnE = 0;
    TInt64 freeOnC = 0;
        
    TDriveNumber drive = EDriveC;
    TRAPD( err, freeOnE = FreeDiskSpaceL( CNcdProviderUtils::FileSession(),
        EDriveE ) );
    
    // Drive E exists and has free space
    if ( freeOnE && err == KErrNone ) 
        {
        // No trap because if this fails then we're in trouble already
        freeOnC = FreeDiskSpaceL( CNcdProviderUtils::FileSession(),
            EDriveC );
        if ( freeOnE > freeOnC ) 
            {
            DLINFO(("Using E: as temp drive"));
            drive = EDriveE;
            }
        }
    return drive;
    }

