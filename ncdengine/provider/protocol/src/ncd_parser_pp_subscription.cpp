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
* Description:   CNcdEntityRefParser implementation
*
*/


#include "ncd_parser_pp_subscription.h"
#include "ncd_parser_pp_subscriptiondetails.h"
#include "ncd_parser_pp_payment.h"
#include "ncd_parser_cp_query.h"
#include "ncd_pp_subscription.h"
#include "ncd_pp_subscriptionimpl.h"
#include "ncdparserobserverbundleimpl.h"
#include "ncdparserobserver.h"
#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncdprotocoltypes.h"

#include "catalogsdebug.h"


CNcdPreminetProtocolSubscriptionParser* 
CNcdPreminetProtocolSubscriptionParser::NewL( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth,
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& aParentId,
    const TDesC& aResponseNamespace )
    {
    CNcdPreminetProtocolSubscriptionParser* self 
        = new(ELeave) CNcdPreminetProtocolSubscriptionParser( 
            aObservers, aSubParserObserver, aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes, aParentId, aResponseNamespace );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolSubscriptionParser::CNcdPreminetProtocolSubscriptionParser( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {
    }

CNcdPreminetProtocolSubscriptionParser::~CNcdPreminetProtocolSubscriptionParser()
    {
    delete iParentTag;
    delete iSubscription;
    }

void CNcdPreminetProtocolSubscriptionParser::ConstructL( 
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& /*aParentId*/,
    const TDesC& aResponseNamespace )
    {
    CNcdSubParser::ConstructL( aElement );

    DLTRACEIN(("depth=%d tag=%S",iDepth,iTag));
    
    iSubscription = CNcdPreminetProtocolSubscriptionImpl::NewL();
    NcdProtocolUtils::AssignDesL(
        iSubscription->iNamespace, aResponseNamespace );
    NcdProtocolUtils::AssignEmptyDesL(iParentTag);
    
    TPtrC8 type = AttributeValue(KAttrType, aAttributes);
    if ( type == KValuePeriodic ) 
        iSubscription->iType = EPeriodic;
    else if ( type == KValueAutomaticContinous ) 
        iSubscription->iType = EAutomaticContinous;
    
    TPtrC8 timestamp = AttributeValue(KAttrTimestamp, aAttributes);
    NcdProtocolUtils::AssignDesL(iSubscription->iTimestamp, timestamp);
    
    TPtrC8 entityId = AttributeValue(KAttrEntityId, aAttributes);
    NcdProtocolUtils::AssignDesL(iSubscription->iEntityId, entityId);
    
    TPtrC8 purchaseOptionId = AttributeValue(KAttrPurchaseOptionId, aAttributes);
    NcdProtocolUtils::AssignDesL(
        iSubscription->iPurchaseOptionId, purchaseOptionId);
    
    TPtrC8 expiredOn = AttributeValue(KAttrExpiredOn, aAttributes);
    NcdProtocolUtils::AssignDesL(iSubscription->iExpiredOn, expiredOn);
    
    TPtrC8 cancelled = AttributeValue(KAttrCancelled, aAttributes);
    NcdProtocolUtils::DesToBool(iSubscription->iCancelled, cancelled);
    
    }


void CNcdPreminetProtocolSubscriptionParser::OnStartElementL( 
    const Xml::RTagInfo& aElement, 
    const Xml::RAttributeArray& aAttributes, 
    TInt aErrorCode) 
    {
    CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
    
    TPtrC8 tag( aElement.LocalName().DesC() );
    
    if( iSubParser == 0 )
        {
        DLTRACEIN(("subscription start tag=%S error=%d depth=%d",
            &aElement.LocalName().DesC(),aErrorCode,iDepth));
        if( tag == KTagTotalUsageRights ||
            tag == KTagRemainingUsageRights ||
            tag == KTagUsageRights )
            {
            iSubParser = CNcdPreminetProtocolSubscriptionDetailsParser::NewL(
                *iObservers, *this, iDepth+1, aElement, aAttributes,
                KNullDesC /*no parent id*/ );
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



void CNcdPreminetProtocolSubscriptionParser::OnEndElementL(
    const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));

    TPtrC8 tag( aElement.LocalName().DesC() );
    if (iBuffer) 
        {            
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



void CNcdPreminetProtocolSubscriptionParser::SubParserFinishedL( 
    const TDesC8& aTag, TInt /*aErrorCode*/ )
    {
    DLTRACEIN(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning

    // : child entity has been finished. store it as a 
    // a childen to the node this parser is parsing.
    // Needs nodemanager.
    if( iSubParser->Tag() == KTagTotalUsageRights ||
        iSubParser->Tag() == KTagUsageRights )
        {
        // A subentity is now ready. Report it.
        MNcdPreminetProtocolSubscriptionDetails* det =
            static_cast<CNcdPreminetProtocolSubscriptionDetailsParser*>
            ( iSubParser )->SubscriptionDetails();
        if ( det && iSubscription ) 
            {
            iSubscription->iTotalUsageRights = det;
            }
        DLINFO(("subparser subscriptionDetails=%X",det));
        }
    else if( iSubParser->Tag() == KTagRemainingUsageRights )
        {
        // A subentity is now ready. Report it.
        MNcdPreminetProtocolSubscriptionDetails* det =
            static_cast<CNcdPreminetProtocolSubscriptionDetailsParser*>
            ( iSubParser )->SubscriptionDetails();
        if ( det && iSubscription ) 
            {
            iSubscription->iRemainingUsageRights = det;
            }
        DLINFO(("subparser subscriptionDetails=%X",det));
        }
        
    delete iSubParser;
    iSubParser = 0;
    }

MNcdPreminetProtocolSubscription*
CNcdPreminetProtocolSubscriptionParser::Subscription() 
    {
    MNcdPreminetProtocolSubscription* sub = iSubscription;
    iSubscription = 0;
    return sub;
    }
