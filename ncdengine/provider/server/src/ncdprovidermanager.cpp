/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of CNcdProviderManager
*
*/


#include "ncdprovidermanager.h"

#include <bautils.h>

#include "ncdproviderimpl.h"
#include "catalogsconstants.h"
#include "catalogstransportimpl.h"
#include "ncdproviderutils.h"
#include "ncdstoragemanagerimpl.h"
#include "catalogsutils.h"

#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========

CNcdProviderManager* CNcdProviderManager::NewL()
    {
    CNcdProviderManager* self = new (ELeave) CNcdProviderManager;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
CNcdProviderManager::CNcdProviderManager()        
    {
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
CNcdProviderManager::~CNcdProviderManager()
    {
    // Providers are not owned by the array
    iProviders.Close();
    delete iEngineRootPath;
    delete iTransport;
    delete iStorageManager;    
    delete iProviderUtils;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CNcdProviderManager::ConstructL()
    {
    DLTRACEIN((""));
    // create provider utils, fs is used as the global fs returned by 
    // CNcdProviderUtils::FileSession so it must not be closed here
    iProviderUtils = CNcdProviderUtils::NewL(
        KCatalogsConfigFile );        
    
    iEngineRootPath = WritableEnginePathL( CNcdProviderUtils::FileSession() );                        
    
    // Create transport
    iTransport = CCatalogsTransport::NewL();
    
    iStorageManager = CNcdStorageManager::NewL( 
        CNcdProviderUtils::FileSession(), *iEngineRootPath );
    DLTRACE(("iStorageManager ok"));
        
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
CNcdProvider& CNcdProviderManager::ProviderL( 
    const MCatalogsContext& aContext,
    TBool& aCreated )
    {
    DLTRACEIN((""));
    TInt index = FindFamily( aContext );
    aCreated = EFalse;
    
    if ( index == KErrNotFound ) 
        {
        DLTRACE(("Creating new provider"));
        CNcdProvider* provider = CNcdProvider::NewLC( 
            aContext.FamilyId(), 
            *this,
            *iEngineRootPath,
            *iTransport,
            *iStorageManager );
        
        iProviders.AppendL( provider );
        
        CleanupStack::Pop( provider );        
        index = iProviders.Count() - 1;
        aCreated = ETrue;
        }
    
    return *iProviders[ index ];
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
CNcdProvider* CNcdProviderManager::Provider( 
    const MCatalogsContext& aContext ) const
    {
    TInt index = FindFamily( aContext );
    CNcdProvider* provider = NULL;
    if ( index != KErrNotFound )
        {
        provider = iProviders[ index ];
        }
    return provider;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CNcdProviderManager::ProviderClosed( CNcdProvider& aProvider )
    {
    DLTRACEIN((""));
    
    TInt index = iProviders.Find( &aProvider );
    if ( index != KErrNotFound )
        {
        DLTRACE(("Removing provider: %x", &aProvider));
        iProviders.Remove( index );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CNcdProviderManager::FindFamily( const MCatalogsContext& aContext ) const
    {
    TInt count = iProviders.Count();
    TInt index = KErrNotFound;
    while( count-- ) 
        {
        if ( aContext.FamilyId() == iProviders[ count ]->FamilyId() ) 
            {
            index = count;
            break;
            }
        }
    return index;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
HBufC* CNcdProviderManager::WritableEnginePathL( RFs& aFs )
    {
    DLTRACEIN( ( "" ) );    
    // Determine the installation path from the location of the 
    // engine's config file
    HBufC* fullPath = FindEngineFileL( aFs, KCatalogsConfigFile );    
    TParsePtrC parse( *fullPath );
    TPath path( parse.DriveAndPath() );
    delete fullPath;
    
    if ( path.Length() < 1 )
        {    
        DLERROR(("No path"));
        User::Leave( KErrArgument );
        }
    
    // If it is in ROM, default to C.        
    if ( IsRomDriveL( aFs, path[0] ) )
        {
        DLTRACE(("Config on ROM, use C:"));
        path[0] = DriveToCharL( EDriveC );        
        }
        
    TDriveName drive;
    drive.Append( path[0] );
    drive.Append( KDriveDelimiter );
    
    CreatePrivatePathL( aFs, drive, path );
    BaflUtils::EnsurePathExistsL( aFs, path );
    
    DLTRACEOUT( ( _L("Path: %S"), &path ) );
    return path.AllocL();
    }
