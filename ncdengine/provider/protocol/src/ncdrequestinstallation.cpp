/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CNcdRequestInstallation implementation
*
*/


#include <e32base.h>

#include "ncdrequestinstallation.h"
#include "ncdrequestbase.h"
#include "ncdrequestconfigurationdata.h"
#include "catalogsdebug.h"
#include "ncdprotocolutils.h"
#include "ncdprotocolwords.h"


CNcdRequestInstallation* CNcdRequestInstallation::NewL()
    {
    CNcdRequestInstallation* self = CNcdRequestInstallation::NewLC();
    CleanupStack::Pop();
    return self;
    }

CNcdRequestInstallation* CNcdRequestInstallation::NewLC()
    {
    CNcdRequestInstallation* self = new (ELeave) CNcdRequestInstallation();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CNcdRequestInstallation::ConstructL()
    {
    DLTRACEIN((""));
    CNcdRequestBase::ConstructL( KTagPreminetRequest );
    iName.SetL( KTagInstallationReport );
    iNamespaceUri.SetL( KDefaultNamespaceUri );
    iPrefix = TXmlEngString();
    iVersion.SetL( KAttrPreminetVersion );
    
    }
    
CNcdRequestInstallation::CNcdRequestInstallation()
    : CNcdRequestBase()
    {
    }

CNcdRequestInstallation::~CNcdRequestInstallation()
    {
    DLTRACEIN((""));
    iName.Free();
    iNamespaceUri.Free();
    iPrefix.Free();
    
    
    DLTRACE(("Free installation details"));
    for (TInt i = 0; i < iInstallationDetails.Count(); ++i)
        {
        TNcdRequestInstallation& e = iInstallationDetails[i];
        e.iId.Free();
        //e.iTimeStamp.Free();
        //e.iPurchaseOptionId.Free();
        e.iUri.Free();
        e.iElapsedTime.Free();
        
        }
    iInstallationDetails.Close();
    DLTRACEOUT((""));
    }


void CNcdRequestInstallation::AddDownloadDetailsL( 
    const TDesC& aId, 
    const TDesC& aUri,
    TInt aStatusCode,
    const TInt64& aElapsedTime )
    {
    DLTRACEIN(( _L("Id: %S, URI: %S, Status: %d, elapsed time: %Ld"),
        &aId, &aUri, aStatusCode, aElapsedTime ));
        
    TNcdRequestInstallation install;
    
    install.iId.SetL( aId );
    install.iUri.SetL( aUri );
    install.iStatusCode = aStatusCode;
    
    // Convert TInt64 to a string
    const TInt K64BitIntAsStringLength = 21;
    
    HBufC* conversionBuffer = HBufC::NewLC( K64BitIntAsStringLength );
    conversionBuffer->Des().Num( aElapsedTime );
    
    install.iElapsedTime.SetL( *conversionBuffer );
    DLTRACE(( _L("Converted time: %S"), conversionBuffer ));
    
    CleanupStack::PopAndDestroy( conversionBuffer );
   
    iInstallationDetails.AppendL( install );
    DLTRACEOUT(("Details added successfully"));
    }


void CNcdRequestInstallation::AddInstallDetailsL( 
    const TDesC& aId, 
    TInt aStatusCode )
    {
    DLTRACEIN(( _L("Id: %S, Status: %d"), &aId, aStatusCode ));
        
    TNcdRequestInstallation install;
    
    install.iId.SetL( aId );
    install.iStatusCode = aStatusCode;

    // These are not set in install report.
    // So, set them to Null value which will be left out when
    // xml element is created.
    install.iUri.SetL( KNullDesC );
    install.iElapsedTime.SetL( KNullDesC );
    
    iInstallationDetails.AppendL( install );
    DLTRACEOUT(("Details added successfully"));
    }
    

// generates the dom nodes
HBufC8* CNcdRequestInstallation::CreateRequestL()
    {
    DLTRACEIN((""));
    // generate installation part of the request
    CNcdRequestBase::SetProtocolVersionL( KPreminetReqVersion );
    iRequestElement = NcdProtocolUtils::NewElementL( 
        iDocument, KTagInstallationReport );
        
    if ( iConfiguration ) 
        {        
        iConfiguration->SetNamespacePrefixL( KAttrCdpNamespacePrefix );
        }
        
    // Add installation/download details    
    for ( TInt i = 0; i < iInstallationDetails.Count(); ++i )
        {
        // create remote entity elements
        TNcdRequestInstallation& entity = iInstallationDetails[i];
        
        // Installation-element
        TXmlEngElement entityElement = 
            NcdProtocolUtils::NewElementL( 
                iDocument, iRequestElement, KTagInstallation );


        NcdProtocolUtils::NewAttributeL( 
            entityElement, KAttrStatusCode, entity.iStatusCode );

        if ( entity.iElapsedTime.NotNull() && entity.iElapsedTime.Length() > 0 ) 
            {            
            NcdProtocolUtils::NewAttributeL( 
                entityElement, KAttrElapsedTime, entity.iElapsedTime );
            }

        // Entity-element
        TXmlEngElement entityIdElement = 
            NcdProtocolUtils::NewElementL( 
                iDocument, entityElement, KTagEntity );
                
        NcdProtocolUtils::NewAttributeL( entityIdElement, KAttrId, entity.iId );


/* // Enable if adding installation report functionality
   // NOTE: this code is not finished.
   // These should be set when the report is created in install operation
        if ( entity.iTimestamp.NotNull() ) 
            {            
            NcdProtocolUtils::NewAttributeL( 
                entityElement, KAttrTimestamp, entity.iTimestamp );
            }

        if ( entity.iPurchaseOptionId.NotNull() )
            {
            NcdProtocolUtils::NewAttributeL( 
                entityElement, KAttrPurchaseOptionId, entity.iPurchaseOptionId);
            }
*/

        // Content URI element
        if ( entity.iUri.NotNull() && entity.iElapsedTime.Length() > 0 ) 
            {        
            TXmlEngElement uriElement = 
                NcdProtocolUtils::NewElementL( 
                    iDocument, entityElement, KTagContentUri );
                
    #ifndef RD_XML_ENGINE_API_CHANGE
            uriElement.SetTextL( entity.iUri );
    #else
            uriElement.SetTextL( entity.iUri.PtrC8() );
    #endif
            }           
        }
        
    DLTRACEOUT(("exit"));    
    // base class generates the complete request
    return CNcdRequestBase::CreateRequestL();
    }
    
    
