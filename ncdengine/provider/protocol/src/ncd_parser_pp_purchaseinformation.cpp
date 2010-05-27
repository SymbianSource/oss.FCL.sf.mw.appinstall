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
* Description:   CNcdPreminetProtocolPurchaseInformationParser implementation
*
*/


#include "ncd_parser_pp_purchaseinformation.h"
#include "ncd_parser_pp_purchaseentity.h"
#include "ncd_parser_pp_payment.h"
#include "ncd_parser_cp_query.h"
#include "ncd_cp_query.h"
#include "ncd_pp_informationimpl.h"
#include "ncd_pp_purchase.h"
#include "ncd_pp_purchaseinformationimpl.h"
#include "ncdparserobserverbundleimpl.h"
#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncdprotocoltypes.h"

#include "catalogsdebug.h"


CNcdPreminetProtocolPurchaseInformationParser* 
CNcdPreminetProtocolPurchaseInformationParser::NewL( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth,
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& aParentId )
    {
    CNcdPreminetProtocolPurchaseInformationParser* self 
        = new(ELeave) CNcdPreminetProtocolPurchaseInformationParser( 
            aObservers, aSubParserObserver, aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes, aParentId );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolPurchaseInformationParser::CNcdPreminetProtocolPurchaseInformationParser( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {

    }

CNcdPreminetProtocolPurchaseInformationParser::~CNcdPreminetProtocolPurchaseInformationParser()
    {
    delete iInformation;
    }

void CNcdPreminetProtocolPurchaseInformationParser::ConstructL( 
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& /*aParentId*/ )
    {
    CNcdSubParser::ConstructL( aElement );

    DLTRACEIN(("depth=%d tag=%S",iDepth,iTag));
    //DLINFO((_L("parent=%S"),&aParentId));

    if (!iInformation)
        {
        iInformation = CNcdPreminetProtocolPurchaseInformationImpl::NewL();
        }
    
    // read attributes here    
    TPtrC8 uri = AttributeValue( KAttrUri, aAttributes );
    NcdProtocolUtils::AssignDesL(iInformation->iUri, uri);
    
    TPtrC8 ns = AttributeValue( KAttrNamespace, aAttributes );
    NcdProtocolUtils::AssignDesL(iInformation->iNamespace, ns);
    TPtrC8 initiateSession = 
        AttributeValue( KAttrInitiateSession, aAttributes );
    NcdProtocolUtils::DesToBool(iInformation->iInitiateSession, initiateSession);
    }


void CNcdPreminetProtocolPurchaseInformationParser::OnStartElementL( 
    const Xml::RTagInfo& aElement, 
    const Xml::RAttributeArray& aAttributes, 
    TInt aErrorCode) 
    {
    CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
    
    TPtrC8 tag( aElement.LocalName().DesC() );
    
    if( iSubParser == 0 )
        {
        DLTRACEIN(("purchaseInformation start tag=%S error=%d depth=%d",
            &aElement.LocalName().DesC(),aErrorCode,iDepth));
        if( tag == KTagPayments )
            {
            // skip this tag
            }
        else if( tag == KTagPayment )
            {
            iSubParser = CNcdPreminetProtocolPaymentParser::NewL(
                *iObservers, *this, iDepth+1, aElement, aAttributes,
                KNullDesC /*no parent id*/ );
            }
        else if( tag == KTagDisclaimer )
            {
            iSubParser = CNcdConfigurationProtocolQueryParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        else if( tag == KTagTotalPrice )
            {
            TPtrC8 currency = AttributeValue( KAttrCurrency, aAttributes );
            NcdProtocolUtils::AssignDesL(
                iInformation->iTotalPriceCurrency, currency);
            // parsing myself
            }
        else if( tag == KTagEntities )
            {
            // skip this tag
            }
        else if( tag == KTagEntity )
            {
            iSubParser = CNcdPreminetProtocolPurchaseEntityParser::NewL( 
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



void CNcdPreminetProtocolPurchaseInformationParser::OnEndElementL(
    const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));

    TPtrC8 tag( aElement.LocalName().DesC() );
    if (iBuffer) 
        {
        if (tag == KTagTotalPrice)
            {
            NcdProtocolUtils::AssignDesL(iInformation->iTotalPrice, *iBuffer);
            }
        delete iBuffer;
        iBuffer = 0;
        }
    
    if( iSubParser == 0 && iTag && *iTag == aElement.LocalName().DesC() )
        {
        DLTRACE(("end tag=%S",&aElement.LocalName().DesC()));
        // Should store finished entity, or let the parent do it with some function?
        
        iSubParserObserver->SubParserFinishedL( aElement.LocalName().DesC(), aErrorCode );
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


void CNcdPreminetProtocolPurchaseInformationParser::SubParserFinishedL( 
    const TDesC8& aTag, TInt /*aErrorCode*/ )
    {
    DLTRACEIN(("tag=%S subparser=%X",&aTag,iSubParser));
    DLINFO(("subparser tag=%S",&iSubParser->Tag()));
    (void) aTag; // suppresses compiler warning

    if ( iInformation ) 
        {        
        // : child entity has been finished. store it as a 
        // a child to the node this parser is parsing.
        // Needs nodemanager.
        if( iSubParser->Tag() == KTagPayment )
            {
            // Payment data parsed, save it into info data.
            MNcdPreminetProtocolPayment* payment =
                static_cast<CNcdPreminetProtocolPaymentParser*>
                ( iSubParser )->Payment();
            DLINFO(("subparser payment=%X",payment));
            if( payment )
                {
                iInformation->iPayments.AppendL(payment);
                }
            }
        else if( iSubParser->Tag() == KTagDisclaimer )
            {
            // A subentity is now ready. Report it.
            MNcdConfigurationProtocolQuery* disclaimer =
                static_cast<CNcdConfigurationProtocolQueryParser*>
                ( iSubParser )->Query();
            DLINFO(("subparser disclaimer=%X",disclaimer));
            if( disclaimer )    
                {
                delete iInformation->iDisclaimer;
                iInformation->iDisclaimer = disclaimer;
                }
            }
        
        else if( iSubParser->Tag() == KTagEntity )
            {
            // A subentity is now ready. Report it.
            MNcdPreminetProtocolPurchaseEntity* entity =
                static_cast<CNcdPreminetProtocolPurchaseEntityParser*>
                ( iSubParser )->Entity();
            DLINFO(("subparser purchaseEntity=%X",entity));
            if( entity )
                {
                iInformation->iEntity = entity;
                }
            }
        }
    delete iSubParser;
    iSubParser = 0;
    }

MNcdPreminetProtocolPurchaseInformation*
CNcdPreminetProtocolPurchaseInformationParser::Information()
    {
    MNcdPreminetProtocolPurchaseInformation* info = iInformation;
    iInformation = 0;
    return info;
    }

