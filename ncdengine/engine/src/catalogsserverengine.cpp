/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32property.h>

#include "catalogsserverengine.h"
#include "catalogssession.h"
#include "ncdproviderimpl.h"
#include "catalogsdebug.h"
#include "catalogsconstants.h"
#include "catalogsuids.h"
#include "ncdprovidermanager.h"

// ======== MEMBER FUNCTIONS ========



CCatalogsServerEngine::CCatalogsServerEngine()
    {
    }


void CCatalogsServerEngine::ConstructL()
    {
    DLTRACEIN((""));

    // Define the Catalogs Engine maintenance lock property, if not already
    // defined.
    // Don't use explicit category UID, server SID instead?
    //  - explicit category UID requires WriteDeviceData cap. Well we have it but anyway.

    // Security policy
//    TSecurityPolicy readPolicy;
//    TSecurityPolicy writePolicy;
    _LIT_SECURITY_POLICY_PASS( readPolicy );
    _LIT_SECURITY_POLICY_PASS( writePolicy );

    DLINFO(( "Defining catalogs engine maintenance lock property" ));

    TInt err = RProperty::Define( 
        KCatalogsEnginePropertyCategory,
        KCatalogsEnginePropertyKeyMaintenanceLock,
        RProperty::EInt,
        readPolicy,
        writePolicy );

    if( err != KErrNone && err != KErrAlreadyExists ) 
        {
        DLERROR(( "Maintenance lock property create failed with %d", err ));
        DLTRACEOUT(( "LEAVE %d", err ));
        User::Leave( err );
        }

    DLINFO(( "Maintenance lock property define returned %d", err ));
    
    iProviderManager = CNcdProviderManager::NewL();

    DLTRACEOUT((""));
    }

CCatalogsServerEngine* CCatalogsServerEngine::NewL()
    {
    CCatalogsServerEngine* self = CCatalogsServerEngine::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

CCatalogsServerEngine* CCatalogsServerEngine::NewLC()
    {
    CCatalogsServerEngine* self = new( ELeave ) CCatalogsServerEngine;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CCatalogsServerEngine::~CCatalogsServerEngine()
    {
    DLTRACEIN((""));
    // Engine is going to be killed and we can free our reference to
    // the provider
    delete iProviderManager;
    DLTRACEOUT((""));
    }


void CCatalogsServerEngine::CreateProviderL(
    MCatalogsSession& aSession,
    TInt aProvider,
    TInt& aHandle,
    TUint32 aOptions )
    {
    DLTRACEIN(( "aProvider=%08x", aProvider ));
    
    // Providers, who we know
    TInt providerUid( KNcdProviderUid ); // only one uid is known at the moment

    
    if ( aProvider == providerUid )
        {
        TBool created = EFalse;
        
        // if created==ETrue, provider's refcount is 1, otherwise it's > 1
        CNcdProvider& provider = iProviderManager->ProviderL(
            aSession.Context(), created );        
        
        TRAPD( err, 
            {
            provider.PrepareSessionL( aSession, aOptions );
            // AddObjectL increases provider's refcount by 1
            aHandle = aSession.AddObjectL( &provider );
            });

        
        if ( created ) 
            {
            if ( err != KErrNone ) 
                {
                provider.Close();
                }
            
            if ( provider.DatabaseClearingStatus() && err == KErrNone ) 
                {
                DLTRACE(("Databases were cleared, status: %d", 
                    provider.DatabaseClearingStatus() ));
                User::Leave( provider.DatabaseClearingStatus() );
                }
            }

        User::LeaveIfError( err );
        }
    else
        {
        DLWARNING(( "Unknown provider" ));        
        User::Leave( KErrNotFound );
        }
    
    DLTRACEOUT((""));
    }
    

void CCatalogsServerEngine::HandleSessionRemoval( 
    MCatalogsSession& aSession )
    {
    DLTRACEIN((""));
    CNcdProvider* provider = iProviderManager->Provider( aSession.Context() );
    
    if ( provider )
        {
        provider->HandleSessionRemoval( aSession );

        // Remove provider manager's reference
        if ( provider->AccessCount() == 1 ) 
            {
            DLTRACE(("Last session, deleting provider"));
            provider->Close();
            }            
        }
     
    DLTRACEOUT((""));
    }
