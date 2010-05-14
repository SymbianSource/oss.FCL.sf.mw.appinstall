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


#include "ncd_parser_pp_purchase.h"
#include "ncd_parser_pp_purchaseinformation.h"
#include "ncd_parser_pp_purchaseprocessed.h"
#include "ncd_pp_purchase.h"
#include "ncd_pp_purchaseinformationimpl.h"
#include "ncd_pp_purchaseprocessedimpl.h"
#include "ncdparserobserverbundleimpl.h"
#include "ncdparserobserver.h"
#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncdprotocoltypes.h"

#include "catalogsdebug.h"


CNcdPreminetProtocolPurchaseParser* 
CNcdPreminetProtocolPurchaseParser::NewL( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth,
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& aParentId )
    {
    CNcdPreminetProtocolPurchaseParser* self 
        = new(ELeave) CNcdPreminetProtocolPurchaseParser( 
            aObservers, aSubParserObserver, aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes, aParentId );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolPurchaseParser::CNcdPreminetProtocolPurchaseParser( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {

    }

CNcdPreminetProtocolPurchaseParser::~CNcdPreminetProtocolPurchaseParser()
    {
    DLTRACEIN((""));
    delete iTransactionId;
    delete iQueryId;
    DLTRACEOUT((""));
    }

void CNcdPreminetProtocolPurchaseParser::ConstructL( 
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& /*aParentId*/ )
    {
    CNcdSubParser::ConstructL( aElement );

    DLTRACEIN(("depth=%d tag=%S",iDepth,iTag));
    //DLINFO((_L("parent=%S"),&aParentId));

    // read attributes here
    TPtrC8 transactionId = AttributeValue( KAttrTransactionId, aAttributes );
    TPtrC8 queryId = AttributeValue( KAttrQueryId, aAttributes );

    NcdProtocolUtils::AssignDesL(iTransactionId, transactionId);
    NcdProtocolUtils::AssignDesL(iQueryId, queryId);
    
    }


void CNcdPreminetProtocolPurchaseParser::OnStartElementL( 
    const Xml::RTagInfo& aElement, 
    const Xml::RAttributeArray& aAttributes, 
    TInt aErrorCode) 
    {
    CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
    
    TPtrC8 tag( aElement.LocalName().DesC() );
    
    if( iSubParser == 0 )
        {
        DLTRACEIN(("purchase start tag=%S error=%d depth=%d",
            &aElement.LocalName().DesC(),aErrorCode,iDepth));
        if( tag == KTagInformation )
            {
            iSubParser = CNcdPreminetProtocolPurchaseInformationParser::NewL(
                *iObservers, *this, iDepth+1, aElement, aAttributes,
                KNullDesC /*no parent id*/ );
            }
        else if( tag == KTagProcessed )
            {
            iSubParser = CNcdPreminetProtocolPurchaseProcessedParser::NewL( 
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



void CNcdPreminetProtocolPurchaseParser::OnEndElementL(
    const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));
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
    DLTRACEOUT((""));
    }


// void CNcdPreminetProtocolPurchaseParser::StartL( const Xml::RTagInfo& aElement, 
//                                                   const Xml::RAttributeArray& aAttributes, 
//                                                   TInt aErrorCode,
//                                                   const TDesC8& aParentId )
//     {
//     }

void CNcdPreminetProtocolPurchaseParser::SubParserFinishedL( const TDesC8& aTag, 
                                                              TInt /*aErrorCode*/ )
    {
    DLTRACEIN(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning

    // : child entity has been finished. store it as a 
    // a childen to the node this parser is parsing.
    // Needs nodemanager.
    if( iSubParser->Tag() == KTagInformation )
        {
        // A subentity is now ready. Report it.

        if( iObservers->PurchaseObserver() )
            {
            MNcdPreminetProtocolPurchaseInformation* information =
                static_cast<CNcdPreminetProtocolPurchaseInformationParser*>
                ( iSubParser )->Information();
            DLINFO(("subparser purchaseInformation=%X",information));
            
            if( information )
                {
                CNcdPreminetProtocolPurchaseInformationImpl* impl = 
                    static_cast<CNcdPreminetProtocolPurchaseInformationImpl*>( information );
                delete impl->iTransactionId;
                impl->iTransactionId = iTransactionId;
                iTransactionId = 0;

                
                delete impl->iQueryId;
                impl->iQueryId = iQueryId;
                iQueryId = 0;

                iObservers->PurchaseObserver()->InformationL( information );
                information = 0;
                }
            }
        }
    else if( iSubParser->Tag() == KTagProcessed )
        {
        // A subentity is now ready. Report it.
        
        if( iObservers->PurchaseObserver() )
            {
            MNcdPreminetProtocolPurchaseProcessed* processed =
                static_cast<CNcdPreminetProtocolPurchaseProcessedParser*>
                ( iSubParser )->Processed();
            DLINFO(("subparser purchaseProcessed=%X",processed));
            
            if( processed )
                {
                CNcdPreminetProtocolPurchaseProcessedImpl* impl =
                    static_cast<CNcdPreminetProtocolPurchaseProcessedImpl*>( processed );
                delete impl->iTransactionId;
                impl->iTransactionId = iTransactionId;
                iTransactionId = 0;
                
                delete impl->iQueryId;
                impl->iQueryId = iQueryId;
                iQueryId = 0;

                iObservers->PurchaseObserver()->ProcessedL(processed);
                processed = 0;
                }
            }
        }

    delete iSubParser;
    iSubParser = 0;
    }
