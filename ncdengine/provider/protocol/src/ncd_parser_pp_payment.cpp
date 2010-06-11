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
* Description:   CNcdPreminetProtocolPaymentParser implementation
*
*/


#include "ncd_parser_pp_payment.h"
#include "ncd_pp_paymentimpl.h"
#include "ncd_pp_purchase.h"
#include "ncd_pp_smsdetailsimpl.h"
#include "ncdparserobserverbundleimpl.h"
#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
//#include "ncdprotocoltypes.h"
#include "ncdpaymentmethod.h"
#include "ncdstring.h"

#include "catalogsdebug.h"


CNcdPreminetProtocolPaymentParser* 
CNcdPreminetProtocolPaymentParser::NewL( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth,
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& aParentId )
    {
    CNcdPreminetProtocolPaymentParser* self 
        = new(ELeave) CNcdPreminetProtocolPaymentParser( 
            aObservers, aSubParserObserver, aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes, aParentId );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolPaymentParser::CNcdPreminetProtocolPaymentParser( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {

    }

CNcdPreminetProtocolPaymentParser::~CNcdPreminetProtocolPaymentParser()
    {
    delete iPayment;
    }

void CNcdPreminetProtocolPaymentParser::ConstructL( 
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& /*aParentId*/ )
    {
    CNcdSubParser::ConstructL( aElement );

    DLTRACEIN(("depth=%d tag=%S",iDepth,iTag));
    //DLINFO((_L("parent=%S"),&aParentId));

    if (!iPayment)
        {
        iPayment = CNcdPreminetProtocolPaymentImpl::NewL();
        }

    // read attributes here
    TPtrC8 method = AttributeValue( KAttrMethod, aAttributes );
    if (method == KValueSms)
        {
        iPayment->iMethod = MNcdPaymentMethod::EPaymentSms;
        }
    else if (method == KValueCreditCard)
        {
        iPayment->iMethod = MNcdPaymentMethod::EPaymentCreditCard;
        }
    else if (method == KValueDirect)
        {
        iPayment->iMethod = MNcdPaymentMethod::EPaymentDirect;
        }
    else if (method == KValueCustom)
        {
        iPayment->iMethod = MNcdPaymentMethod::EPaymentCustom;
        }

    TPtrC8 queryId = AttributeValue( KAttrQueryId, aAttributes );
    NcdProtocolUtils::AssignDesL(iPayment->iQueryId, queryId);
    }


void CNcdPreminetProtocolPaymentParser::OnStartElementL( 
    const Xml::RTagInfo& aElement, 
    const Xml::RAttributeArray& aAttributes, 
    TInt aErrorCode) 
    {
    CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
    
    TPtrC8 tag( aElement.LocalName().DesC() );
    
    if( iSubParser == 0 )
        {
        DLTRACEIN(("payment start tag=%S error=%d depth=%d",
            &aElement.LocalName().DesC(),aErrorCode,iDepth));
        if( tag == KTagName )
            {
            TPtrC8 key = AttributeValue( KAttrNamespace, aAttributes );
            iPayment->iName->SetKeyL( key );
            // name value saved in OnEndElementL
            }
        else if( tag == KTagSmsDetails )
            {
            // skip this element
            }
        else if( tag == KTagSmsDetail )
            {
            CNcdPreminetProtocolSmsDetailsImpl* smsDetails = 
                CNcdPreminetProtocolSmsDetailsImpl::NewLC();
            TPtrC8 address = AttributeValue( KAttrAddress, aAttributes );
            TPtrC8 message = AttributeValue( KAttrMessage, aAttributes );
            DLTRACE(( "payment address=%S message=%S", &address, &message ));
            NcdProtocolUtils::AssignDesL(smsDetails->iAddress, address);
            NcdProtocolUtils::AssignDesL(smsDetails->iMessage, message);
            iPayment->iSmsDetails.AppendL(smsDetails);
            CleanupStack::Pop( smsDetails );
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



void CNcdPreminetProtocolPaymentParser::OnEndElementL(
    const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));

//     if( iStatus == EStatusQueries &&
//         aElement.LocalName().DesC() == KTagQueries )
//         {
//         DLINFO(("queries end"));
//         iStatus = EStatusNormal;
//         return;
//         }

    TPtrC8 tag( aElement.LocalName().DesC() );
    if ( iBuffer && tag == KTagName )
        {
        iPayment->iName->SetDataL( *iBuffer );
        DLINFO(("name=%S", &iPayment->iName->Data()));
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



void CNcdPreminetProtocolPaymentParser::SubParserFinishedL( 
    const TDesC8& aTag, TInt /*aErrorCode*/ )
    {
    DLTRACE(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning

    // : child entity has been finished. store it as a 
    // a childen to the node this parser is parsing.
    // Needs nodemanager.

    delete iSubParser;
    iSubParser = 0;
    }

MNcdPreminetProtocolPayment*
CNcdPreminetProtocolPaymentParser::Payment()
    {
    MNcdPreminetProtocolPayment* payment = iPayment;
    iPayment = 0;
    return payment;
    }

