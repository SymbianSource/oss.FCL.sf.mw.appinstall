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


#include "ncd_parser_pp_purchaseoption.h"
#include "ncd_parser_pp_download.h"
#include "ncd_pp_purchaseoptionimpl.h"
#include "ncd_pp_subscriptiondetailsimpl.h"
#include "ncd_pp_download.h"
#include "ncdparserobserverbundleimpl.h"
#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncdprotocoltypes.h"

#include "catalogsdebug.h"


CNcdPreminetProtocolPurchaseOptionParser* 
CNcdPreminetProtocolPurchaseOptionParser::NewL( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth,
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& aParentId )
    {
    CNcdPreminetProtocolPurchaseOptionParser* self 
        = new(ELeave) CNcdPreminetProtocolPurchaseOptionParser( 
            aObservers, aSubParserObserver, aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes, aParentId );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolPurchaseOptionParser::CNcdPreminetProtocolPurchaseOptionParser( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {

    }

CNcdPreminetProtocolPurchaseOptionParser::~CNcdPreminetProtocolPurchaseOptionParser()
    {
    delete iPurchaseOption;
    }

void CNcdPreminetProtocolPurchaseOptionParser::ConstructL( 
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& /*aParentId*/ )
    {
    CNcdSubParser::ConstructL( aElement );

    DLTRACEIN(("depth=%d tag=%S",iDepth,iTag));
    //DLINFO((_L("parent=%S"),&aParentId));

    if (!iPurchaseOption)
        {
        iPurchaseOption = CNcdPreminetProtocolPurchaseOptionImpl::NewL();
        }

    // read attributes here
    TPtrC8 id = AttributeValue( KAttrId, aAttributes );
    TPtrC8 dependencyId = AttributeValue( KAttrDependencyId, aAttributes );
    TPtrC8 name = AttributeValue( KAttrName, aAttributes );
    TPtrC8 free = AttributeValue( KAttrFree, aAttributes );
    TPtrC8 canBeGift = AttributeValue( KAttrCanBeGift, aAttributes );
    TPtrC8 requirePurchaseProcess = 
        AttributeValue( KAttrRequirePurchaseProcess, aAttributes );

    NcdProtocolUtils::AssignDesL(iPurchaseOption->iId, id);
    NcdProtocolUtils::AssignDesL(iPurchaseOption->iDependencyId, dependencyId);
    NcdProtocolUtils::AssignDesL(iPurchaseOption->iName, name);
    NcdProtocolUtils::DesToBool(iPurchaseOption->iFree, free);    
    NcdProtocolUtils::DesToBool(iPurchaseOption->iCanBeGift, canBeGift);
    NcdProtocolUtils::DesToBool(iPurchaseOption->iRequirePurchaseProcess, 
                                 requirePurchaseProcess);    

    TPtrC8 purchase = AttributeValue( KAttrPurchase, aAttributes );
    if ( purchase == KValueContent ) 
        {
        iPurchaseOption->iPurchase = EContent;
        }
    else if ( purchase == KValueSubscription ) 
        {
        iPurchaseOption->iPurchase = ESubscription;
        }
    else if ( purchase == KValuePartOfSubscription ) 
        {
        iPurchaseOption->iPurchase = EPartOfSubscription;
        }
    else if ( purchase == KValueUpgrade ) 
        {
        iPurchaseOption->iPurchase = EUpgrade;
        }
    else 
        {
        iPurchaseOption->iPurchase = ETypeNotSet;
        }
    
    //NcdProtocolUtils::AssignEmptyDesL(iParentTag);
    }


void CNcdPreminetProtocolPurchaseOptionParser::OnStartElementL( 
    const Xml::RTagInfo& aElement, 
    const Xml::RAttributeArray& aAttributes, 
    TInt aErrorCode) 
    {
    CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
    
    TPtrC8 tag( aElement.LocalName().DesC() );
    
    if( iSubParser == 0 )
        {
        DLTRACEIN(("purchaseOption start tag=%S error=%d depth=%d",
            &aElement.LocalName().DesC(),aErrorCode,iDepth));

        if( tag == KTagDownloadDetails )
            {
            iSubParser = CNcdPreminetProtocolDownloadParser::NewL(
                *iObservers, *this, iDepth+1, aElement, aAttributes,
                KNullDesC /*no parent id*/ );
            }
        else if( tag == KTagSubscriptionDetails )
            {
            // skip this tag
            //*iParentTag = tag;
            }
        else if( tag == KTagParentSubscription )
            {
            TPtrC8 entityId = AttributeValue( KAttrEntityId, aAttributes );
            NcdProtocolUtils::AssignDesL(
                iPurchaseOption->iParentSubscriptionEntityId, entityId);
            TPtrC8 purchaseOptionId = AttributeValue( KAttrPurchaseOptionId, aAttributes );
            NcdProtocolUtils::AssignDesL(
                iPurchaseOption->iParentSubscriptionPurchaseOptionId, purchaseOptionId);
            }
        else if( tag == KTagAmountOfCredits /*&& *iParentTag == KTagSubscriptionDetails*/ )
            {
            TPtrC8 currency = AttributeValue( KAttrCurrency, aAttributes );
            NcdProtocolUtils::AssignDesL(
                iPurchaseOption->iSubscriptionDetails->iAmountOfCreditsCurrency, currency);
            // value saved in OnEndElementL
            }
        else if( tag == KTagValidity /*&& *iParentTag == KTagSubscriptionDetails*/ )
            {
            TPtrC8 delta = AttributeValue( KAttrDelta, aAttributes );
            iPurchaseOption->iSubscriptionDetails->iValidityDelta = 
                NcdProtocolUtils::DesDecToIntL(delta);
            TPtrC8 autoUpdate = AttributeValue( KAttrAutoUpdate, aAttributes );
            NcdProtocolUtils::DesToBool(
                iPurchaseOption->iSubscriptionDetails->iValidityAutoUpdate, autoUpdate);
            }
        else if( tag == KTagNumberOfDownloads /*&& *iParentTag == KTagSubscriptionDetails*/ )
            {
            // value saved in OnEndElementL
            }
        else if( tag == KTagPriceText || tag == KTagCreditPrice )
            {
            // price text is saved in OnEndElementL
            }
        else if ( tag == KTagPrice )
            {
            TPtrC8 currency = AttributeValue( KAttrCurrency, aAttributes );
            NcdProtocolUtils::AssignDesL(iPurchaseOption->iPriceCurrency, currency);
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



void CNcdPreminetProtocolPurchaseOptionParser::OnEndElementL(
    const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));

    TPtrC8 tag( aElement.LocalName().DesC() );

    if ( iBuffer )
        {
        if ( tag == KTagPriceText ) 
            {
            NcdProtocolUtils::AssignDesL(iPurchaseOption->iPriceText, *iBuffer);
            }
        else if ( tag == KTagPrice )
            {
            iPurchaseOption->iPrice = NcdProtocolUtils::DesDecToRealL(*iBuffer);
            }
        else if ( tag == KTagAmountOfCredits )
            {
            iPurchaseOption->iSubscriptionDetails->iAmountOfCredits = 
                NcdProtocolUtils::DesDecToRealL(*iBuffer);
            }
        else if ( tag == KTagCreditPrice )
            {
            iPurchaseOption->iCreditPrice = NcdProtocolUtils::DesDecToRealL(*iBuffer);
            }
        else if ( tag == KTagNumberOfDownloads && iPurchaseOption->iSubscriptionDetails )
            {
            iPurchaseOption->iSubscriptionDetails->iNumberOfDownloads = 
                NcdProtocolUtils::DesDecToIntL(*iBuffer);
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


void CNcdPreminetProtocolPurchaseOptionParser::SubParserFinishedL( const TDesC8& aTag, 
                                                              TInt /*aErrorCode*/ )
    {
    DLTRACEIN(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning

    // : child entity has been finished. store it as a 
    // a childen to the node this parser is parsing.
    // Needs nodemanager.
    if( iSubParser->Tag() == KTagDownloadDetails )
        {
        // A subentity is now ready. Report it.
        MNcdPreminetProtocolDownload* download =
            static_cast<CNcdPreminetProtocolDownloadParser*>( iSubParser )->Download();
        DLINFO(("subparser download=%X",download));
        if( download && iPurchaseOption )
            {
            iPurchaseOption->iDownloadDetails.AppendL(download);
            }
        }

    delete iSubParser;
    iSubParser = 0;
    }

MNcdPreminetProtocolPurchaseOption*
CNcdPreminetProtocolPurchaseOptionParser::PurchaseOption()
    {
    MNcdPreminetProtocolPurchaseOption* opt = iPurchaseOption;
    iPurchaseOption = 0;
    return opt;
    }

