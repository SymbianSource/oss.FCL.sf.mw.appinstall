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
* Description:   CNcdPreminetProtocolSubscriptionReportParser implementation
*
*/


#include "ncd_parser_pp_subscriptionreport.h"
#include "ncd_parser_pp_subscription.h"
#include "ncdparserobserverbundleimpl.h"
#include "ncdparserobserver.h"
#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncdprotocoltypes.h"

#include "catalogsdebug.h"


CNcdPreminetProtocolSubscriptionReportParser* 
CNcdPreminetProtocolSubscriptionReportParser::NewL( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth,
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& aParentId,
    const TDesC& aResponseNamespace )
    {
    CNcdPreminetProtocolSubscriptionReportParser* self 
        = new(ELeave) CNcdPreminetProtocolSubscriptionReportParser( 
            aObservers, aSubParserObserver, aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes, aParentId, aResponseNamespace );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolSubscriptionReportParser::CNcdPreminetProtocolSubscriptionReportParser( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {
    }

CNcdPreminetProtocolSubscriptionReportParser::~CNcdPreminetProtocolSubscriptionReportParser()
    {
    delete iParentTag;
    delete iResponseNamespace;
    }

void CNcdPreminetProtocolSubscriptionReportParser::ConstructL( 
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& /*aAttributes*/,
    const TDesC& /*aParentId*/,
    const TDesC& aResponseNamespace )
    {
    CNcdSubParser::ConstructL( aElement );

    DLTRACEIN(("depth=%d tag=%S",iDepth,iTag));
    //DLINFO((_L("parent=%S"),&aParentId));
    NcdProtocolUtils::AssignEmptyDesL(iParentTag);
    NcdProtocolUtils::AssignDesL( iResponseNamespace, aResponseNamespace );
    // no attributes to read    
    }


void CNcdPreminetProtocolSubscriptionReportParser::OnStartElementL( 
    const Xml::RTagInfo& aElement, 
    const Xml::RAttributeArray& aAttributes, 
    TInt aErrorCode) 
    {
    CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
    
    TPtrC8 tag( aElement.LocalName().DesC() );
    
    if( iSubParser == 0 )
        {
        DLTRACEIN(("subscriptionReport start tag=%S error=%d depth=%d",
            &aElement.LocalName().DesC(),aErrorCode,iDepth));
        if( tag == KTagValidSubscriptions )
            {
            DLINFO(("validSubscription"));
            NcdProtocolUtils::AssignDesL(iParentTag, tag);
            // both subscription types use the same parser
            }
        else if( tag == KTagOldSubscriptions )
            {
            DLINFO(("oldSubscription"));
            NcdProtocolUtils::AssignDesL(iParentTag, tag);
            // both subscription types use the same parser
            }
        else if( tag == KTagSubscription )
            {
            iSubParser = CNcdPreminetProtocolSubscriptionParser::NewL(
                *iObservers, *this, iDepth+1, aElement, aAttributes,
                KNullDesC /*no parent id*/, *iResponseNamespace );
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



void CNcdPreminetProtocolSubscriptionReportParser::OnEndElementL(
    const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));

    TPtrC8 tag( aElement.LocalName().DesC() );
    if( tag == KTagValidSubscriptions )
        {
        DLINFO(("validSubscription ends"));
        NcdProtocolUtils::AssignEmptyDesL(iParentTag);
        }
    else if( tag == KTagOldSubscriptions )
        {
        DLINFO(("oldSubscription ends"));
        NcdProtocolUtils::AssignEmptyDesL(iParentTag);
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



void CNcdPreminetProtocolSubscriptionReportParser::SubParserFinishedL( 
    const TDesC8& aTag, TInt /*aErrorCode*/ )
    {
    DLTRACEIN(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning

    // : child entity has been finished. store it as a 
    // a childen to the node this parser is parsing.
    // Needs nodemanager.
    if( iSubParser->Tag() == KTagSubscription )
        {
        DLINFO(("parentTag=%S", iParentTag));
        // A subentity is now ready. Report it.

        if( iObservers->SubscriptionObserver() )
            {
            MNcdPreminetProtocolSubscription* subscription =
                static_cast<CNcdPreminetProtocolSubscriptionParser*>
                ( iSubParser )->Subscription();
            DLINFO(("subparser subscription=%X",subscription));
            
            if( subscription )
                {
                if ( *iParentTag == KTagValidSubscriptions )
                    iObservers->SubscriptionObserver()->ValidSubscriptionL( subscription );
                else if ( *iParentTag == KTagOldSubscriptions )
                    iObservers->SubscriptionObserver()->OldSubscriptionL( subscription );
                }
            }
        }

    delete iSubParser;
    iSubParser = 0;
    }
