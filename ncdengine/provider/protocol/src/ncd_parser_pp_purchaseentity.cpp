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
* Description:   CNcdPreminetProtocolPurchaseEntityParser implementation
*
*/


#include "ncd_parser_pp_purchaseentity.h"
#include "ncd_parser_pp_payment.h"
#include "ncd_parser_pp_download.h"
#include "ncd_pp_purchaseentityimpl.h"
#include "ncd_pp_download.h"
#include "ncd_pp_purchase.h"
#include "ncdparserobserverbundleimpl.h"
#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncdprotocoltypes.h"

#include "catalogsdebug.h"


CNcdPreminetProtocolPurchaseEntityParser* 
CNcdPreminetProtocolPurchaseEntityParser::NewL( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth,
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& aParentId )
    {
    CNcdPreminetProtocolPurchaseEntityParser* self 
        = new(ELeave) CNcdPreminetProtocolPurchaseEntityParser( 
            aObservers, aSubParserObserver, aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes, aParentId );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolPurchaseEntityParser::CNcdPreminetProtocolPurchaseEntityParser( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {

    }

CNcdPreminetProtocolPurchaseEntityParser::~CNcdPreminetProtocolPurchaseEntityParser()
    {
    delete iEntity;
    }

void CNcdPreminetProtocolPurchaseEntityParser::ConstructL( 
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& /*aParentId*/ )
    {
    CNcdSubParser::ConstructL( aElement );

    DLTRACEIN(("depth=%d tag=%S",iDepth,iTag));
    //DLINFO((_L("parent=%S"),&aParentId));

    if (!iEntity)
        {
        iEntity = CNcdPreminetProtocolPurchaseEntityImpl::NewL();
        }

    // read attributes here
    TPtrC8 ticket = AttributeValue( KAttrTicket, aAttributes );
    NcdProtocolUtils::AssignDesL(iEntity->iTicket, ticket);

    TPtrC8 id = AttributeValue( KAttrId, aAttributes );
    NcdProtocolUtils::AssignDesL(iEntity->iId, id);
    }


void CNcdPreminetProtocolPurchaseEntityParser::OnStartElementL( 
    const Xml::RTagInfo& aElement, 
    const Xml::RAttributeArray& aAttributes, 
    TInt aErrorCode) 
    {
    CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
    
    TPtrC8 tag( aElement.LocalName().DesC() );
    
    if( iSubParser == 0 )
        {
        DLTRACEIN(("purchaseEntity start tag=%S error=%d depth=%d",
            &aElement.LocalName().DesC(),aErrorCode,iDepth));
        if( tag == KTagDelivery )
            {
            TPtrC8 method = AttributeValue( KAttrMethod, aAttributes );
            if (method == KValueDirect)
                iEntity->iDeliveryMethods.Append(EDeliveryDirect);
            if (method == KValueSms)
                iEntity->iDeliveryMethods.Append(EDeliverySms);
            if (method == KValueWapPush)
                iEntity->iDeliveryMethods.Append(EDeliveryWapPush);
            if (method == KValueNone)
                iEntity->iDeliveryMethods.Append(EDeliveryNone);
            }
        else if( tag == KTagPrice )
            {
            TPtrC8 currency = AttributeValue( KAttrCurrency, aAttributes );
            NcdProtocolUtils::AssignDesL(iEntity->iPriceCurrency, currency);
            // price value saved in OnEndElementL
            }
        else if( tag == KTagPriceText )
            {
            // price text saved in OnEndElementL
            }
        else if( tag == KTagDownloadDetails )
            {
            iSubParser = CNcdPreminetProtocolDownloadParser::NewL( 
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



void CNcdPreminetProtocolPurchaseEntityParser::OnEndElementL(
    const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));

    TPtrC8 tag( aElement.LocalName().DesC() );
    if (iBuffer) 
        {
        if (tag == KTagPrice)
            {
            NcdProtocolUtils::AssignDesL(iEntity->iPrice, *iBuffer);
            }
        else if (tag == KTagPriceText)
            {
            NcdProtocolUtils::AssignDesL(iEntity->iPriceText, *iBuffer);
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


void CNcdPreminetProtocolPurchaseEntityParser::SubParserFinishedL( 
    const TDesC8& aTag, TInt aErrorCode )
    {
    DLTRACEIN(("tag=%S subparser=%X error=%d",&aTag,iSubParser,aErrorCode));
    (void) aTag; // suppresses compiler warning
    (void) aErrorCode; // suppresses compiler warning

    // : child entity has been finished. store it as a 
    // a childen to the node this parser is parsing.

    // Needs nodemanager.
    if( iSubParser->Tag() == KTagDownloadDetails )
        {
        // A subentity is now ready. Report it
        MNcdPreminetProtocolDownload* download = 
            static_cast<CNcdPreminetProtocolDownloadParser*>
            (iSubParser)->Download();
        DLINFO(("subparser download=%X",download));
        if( download && iEntity )
            {
            iEntity->iDownloadDetails.AppendL( download );
            }
        }

    delete iSubParser;
    iSubParser = 0;
    }

MNcdPreminetProtocolPurchaseEntity*
CNcdPreminetProtocolPurchaseEntityParser::Entity()
    {
    MNcdPreminetProtocolPurchaseEntity* entity = iEntity;
    iEntity = 0;
    return entity;
    }

