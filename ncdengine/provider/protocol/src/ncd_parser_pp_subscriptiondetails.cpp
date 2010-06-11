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
* Description:   CNcdPreminetProtocolSubscriptionDetailsParser implementation
*
*/


#include "ncd_parser_pp_subscriptiondetails.h"
#include "ncd_pp_subscriptiondetails.h"
#include "ncd_pp_subscriptiondetailsimpl.h"
#include "ncdparserobserverbundleimpl.h"
#include "ncdparserobserver.h"
#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncdprotocoltypes.h"

#include "catalogsdebug.h"


CNcdPreminetProtocolSubscriptionDetailsParser* 
CNcdPreminetProtocolSubscriptionDetailsParser::NewL( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth,
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& aParentId )
    {
    CNcdPreminetProtocolSubscriptionDetailsParser* self 
        = new(ELeave) CNcdPreminetProtocolSubscriptionDetailsParser( 
            aObservers, aSubParserObserver, aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes, aParentId );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolSubscriptionDetailsParser::CNcdPreminetProtocolSubscriptionDetailsParser( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {
    }

CNcdPreminetProtocolSubscriptionDetailsParser::~CNcdPreminetProtocolSubscriptionDetailsParser()
    {
    delete iSubscriptionDetails;
    }

void CNcdPreminetProtocolSubscriptionDetailsParser::ConstructL( 
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& /*aAttributes*/,
    const TDesC& /*aParentId*/ )
    {
    CNcdSubParser::ConstructL( aElement );

    DLTRACEIN(("depth=%d tag=%S",iDepth,iTag));
    //DLINFO((_L("parent=%S"),&aParentId));

    iSubscriptionDetails = CNcdPreminetProtocolSubscriptionDetailsImpl::NewL();
    // no attributes to read in this element
    }


void CNcdPreminetProtocolSubscriptionDetailsParser::OnStartElementL( 
    const Xml::RTagInfo& aElement, 
    const Xml::RAttributeArray& aAttributes, 
    TInt aErrorCode) 
    {
    CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
    
    TPtrC8 tag( aElement.LocalName().DesC() );
    
    if( iSubParser == 0 )
        {
        DLTRACEIN(("subscriptionDetails start tag=%S error=%d depth=%d",
            &aElement.LocalName().DesC(),aErrorCode,iDepth));
        if ( tag == KTagValidity )
            {
            TPtrC8 delta = AttributeValue( KAttrDelta, aAttributes );
            iSubscriptionDetails->iValidityDelta = 
                NcdProtocolUtils::DesDecToIntL(delta);
                
            TPtrC8 autoUpdate = AttributeValue( KAttrAutoUpdate, aAttributes );
            NcdProtocolUtils::DesToBool(
                iSubscriptionDetails->iValidityAutoUpdate, autoUpdate );
            }
        else if ( tag == KTagAmountOfCredits ) 
            {
            TPtrC8 currency = AttributeValue( KAttrCurrency, aAttributes );
            NcdProtocolUtils::AssignDesL(
                iSubscriptionDetails->iAmountOfCreditsCurrency, currency);
            // value from OnContentL
            }
        else if ( tag == KTagNumberOfDownloads )
            {
            // value from OnContentL
            }
        else
            {
            iSubParser = CNcdUnknownParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        }
    else
        {
        iSubParser->OnStartElementL( aElement, aAttributes, aErrorCode );
        }
    }



void CNcdPreminetProtocolSubscriptionDetailsParser::OnEndElementL(
    const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));

    TPtrC8 tag( aElement.LocalName().DesC() );
    if (iBuffer) 
        {
        if (tag == KTagAmountOfCredits)
            {
            iSubscriptionDetails->iAmountOfCredits = 
                NcdProtocolUtils::DesDecToRealL(*iBuffer);
            }
        else if (tag == KTagNumberOfDownloads)
            {
            iSubscriptionDetails->iNumberOfDownloads = 
                NcdProtocolUtils::DesDecToIntL(*iBuffer);
            }
        delete iBuffer;
        iBuffer = 0;
        }

    if( iSubParser == 0 && iTag && *iTag == aElement.LocalName().DesC() )
        {
        DLTRACE(("end tag=%S",&aElement.LocalName().DesC()));
        // Should store finished entity, or let the parent do it with some function?
        iSubParserObserver->SubParserFinishedL( 
            aElement.LocalName().DesC(), aErrorCode );
        }
    else if( iSubParser )
        {
        iSubParser->OnEndElementL( aElement, aErrorCode );
        }
    else
        {
        DLWARNING(("end tag ignored, tag=%S",&aElement.LocalName().DesC()));
        }
    
    }



void CNcdPreminetProtocolSubscriptionDetailsParser::SubParserFinishedL( 
    const TDesC8& aTag, TInt /*aErrorCode*/ )
    {
    DLTRACE(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning

    delete iSubParser;
    iSubParser = 0;
    }

MNcdPreminetProtocolSubscriptionDetails*
CNcdPreminetProtocolSubscriptionDetailsParser::SubscriptionDetails() 
    {
    MNcdPreminetProtocolSubscriptionDetails* details = iSubscriptionDetails;
    iSubscriptionDetails = 0;
    return details;
    }
