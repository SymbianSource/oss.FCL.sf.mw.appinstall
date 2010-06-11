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


#include <smscmds.h>
#include <smsclnt.h>
#include <smuthdr.h>
#include <mtclreg.h>

#include "catalogssmsutils.h"


// ---------------------------------------------------------------------------
// Creator
// ---------------------------------------------------------------------------
//       
CCatalogsSmsUtils* CCatalogsSmsUtils::NewL()
    {
    CCatalogsSmsUtils* self = new( ELeave ) CCatalogsSmsUtils();
    return self;
    }
    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//       
CCatalogsSmsUtils::~CCatalogsSmsUtils()
    {
    }
    
// ---------------------------------------------------------------------------
// SMS center number getter
// ---------------------------------------------------------------------------
//       
HBufC* CCatalogsSmsUtils::SmsCenterNumberLC()
    {
    // Open a session to the message server.
    CMsvSession* session;
    session = CMsvSession::OpenSyncL( *this );

    // We get a MtmClientRegistry from our session
    // this registry is used to instantiate new mtms.
    CClientMtmRegistry* mtmRegistry;
    mtmRegistry = CClientMtmRegistry::NewL( *session );

    CleanupStack::PushL( mtmRegistry );

    // Create a new message entry.
    
    // This represents an entry in the Message Server index
    TMsvEntry msvEntry;              
    
     // message type is SMS
    msvEntry.iMtm = KUidMsgTypeSMS;                        
    
    // this defines the type of the entry: message
    msvEntry.iType = KUidMsvMessageEntry;                   
    
    // ID of local service ( containing the standard folders )
    msvEntry.iServiceId = KMsvLocalServiceIndexEntryId;     
    
    // set the date of the entry to home time
    msvEntry.iDate.HomeTime();                              
    
    // a flag that this message is in preparations
    msvEntry.SetInPreparation( ETrue );                     

    // Create a new mtm to handle this message ( in case our own mtm is in use)
    CSmsClientMtm* smsMtm =
        static_cast<CSmsClientMtm*>( mtmRegistry->NewMtmL( msvEntry.iMtm ) );
    CleanupStack::PushL( smsMtm );

    smsMtm->RestoreServiceAndSettingsL();

    // no, there isn't. We assume there is at least one sc number set and use
    // the default SC number.
    CSmsSettings* serviceSettings = &( smsMtm->ServiceSettings());

    HBufC* number = NULL;
    // if number of scaddresses in the list is null
    if ( serviceSettings->ServiceCenterCount() == 0 )
        {
        // Here should be a dialog in which user can add sc number
        number = KNullDesC().AllocL();
        }
    else
        {
        // set sc address to default.
        number = serviceSettings->GetServiceCenter( 
            serviceSettings->DefaultServiceCenter() ).Address().AllocL();
        }

    CleanupStack::PopAndDestroy( smsMtm );
    CleanupStack::PopAndDestroy( mtmRegistry );
    delete session;
    CleanupStack::PushL( number );
    return number;
    }    


// ---------------------------------------------------------------------------
// Handler for CMsvSession events
// ---------------------------------------------------------------------------
//       
void CCatalogsSmsUtils::HandleSessionEventL( TMsvSessionEvent /* aEvent */, 
    TAny* /* aArg1 */, TAny* /* aArg2 */, TAny* /* aArg3 */ )
    {
    }
    

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//       
CCatalogsSmsUtils::CCatalogsSmsUtils()
    {
    }
