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


#include "ncd_parser_pp_purchaseprocessed.h"
#include "ncd_parser_pp_purchaseentity.h"
#include "ncd_parser_cp_query.h"
#include "ncd_pp_purchase.h"
#include "ncd_pp_purchaseprocessedimpl.h"
#include "ncdparserobserverbundleimpl.h"
#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncdprotocoltypes.h"

#include "catalogsdebug.h"


CNcdPreminetProtocolPurchaseProcessedParser* 
CNcdPreminetProtocolPurchaseProcessedParser::NewL( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth,
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& aParentId )
    {
    CNcdPreminetProtocolPurchaseProcessedParser* self 
        = new(ELeave) CNcdPreminetProtocolPurchaseProcessedParser( 
            aObservers, aSubParserObserver, aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes, aParentId );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolPurchaseProcessedParser::CNcdPreminetProtocolPurchaseProcessedParser( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {

    }

CNcdPreminetProtocolPurchaseProcessedParser::~CNcdPreminetProtocolPurchaseProcessedParser()
    {
    DLTRACEIN((""));
    delete iProcessed;
    DLTRACEOUT((""));
    }

void CNcdPreminetProtocolPurchaseProcessedParser::ConstructL( 
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& /*aParentId*/ )
    {
    CNcdSubParser::ConstructL( aElement );

    DLTRACEIN(("depth=%d tag=%S",iDepth,iTag));
    //DLINFO((_L("parent=%S"),&aParentId));

    if (!iProcessed)
        {
        iProcessed = CNcdPreminetProtocolPurchaseProcessedImpl::NewL();
        }

    // read attributes here
    TPtrC8 resultCode = AttributeValue( KAttrResultCode, aAttributes );
    if ( resultCode != KNullDesC8 )
        NcdProtocolUtils::DesDecToInt(resultCode, iProcessed->iResultCode);
    
    }


void CNcdPreminetProtocolPurchaseProcessedParser::OnStartElementL( 
    const Xml::RTagInfo& aElement, 
    const Xml::RAttributeArray& aAttributes, 
    TInt aErrorCode) 
    {
    CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
    
    TPtrC8 tag( aElement.LocalName().DesC() );
    
    if( iSubParser == 0 )
        {
        DLTRACEIN(("purchaseProcessed start tag=%S error=%d depth=%d",
            &aElement.LocalName().DesC(),aErrorCode,iDepth));
        if( tag == KTagInformation )
            {
            iSubParser = CNcdConfigurationProtocolQueryParser::NewL(
                *iObservers, *this, iDepth+1, aElement, aAttributes );
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



void CNcdPreminetProtocolPurchaseProcessedParser::OnEndElementL(
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
    }


// void CNcdPreminetProtocolPurchaseProcessedParser::StartL( const Xml::RTagInfo& aElement, 
//                                                   const Xml::RAttributeArray& aAttributes, 
//                                                   TInt aErrorCode,
//                                                   const TDesC8& aParentId )
//     {
//     }

void CNcdPreminetProtocolPurchaseProcessedParser::SubParserFinishedL( 
    const TDesC8& aTag, TInt /*aErrorCode*/ )
    {
    DLTRACEIN(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning

    // : child entity has been finished. store it as a 
    // a childen to the node this parser is parsing.
    // Needs nodemanager.
    if( iSubParser->Tag() == KTagInformation )
        {
        // A subentity is now ready. Report it.
        MNcdConfigurationProtocolQuery* info =
            static_cast<CNcdConfigurationProtocolQueryParser*>
            ( iSubParser )->Query();
        DLINFO(("subparser information=%X",info));
        if( info && iProcessed )
            {
            iProcessed->iInformation = info;
            info = 0;
            }
        }
    else if( iSubParser->Tag() == KTagEntity )
        {
        // A subentity is now ready. Report it.
        MNcdPreminetProtocolPurchaseEntity* entity =
            static_cast<CNcdPreminetProtocolPurchaseEntityParser*>
            ( iSubParser )->Entity();
        DLINFO(("subparser purchaseEntity=%X",entity));
        if( entity && iProcessed )
            {
            iProcessed->iEntities.AppendL( entity );
            entity = 0;
            }
        }

    delete iSubParser;
    iSubParser = 0;
    }

MNcdPreminetProtocolPurchaseProcessed*
CNcdPreminetProtocolPurchaseProcessedParser::Processed()
    {
    MNcdPreminetProtocolPurchaseProcessed* proc = iProcessed;
    iProcessed = 0;
    return proc;
    }

