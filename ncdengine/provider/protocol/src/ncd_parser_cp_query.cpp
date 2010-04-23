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
* Description:   CNcdConfigurationProtocolQueryParser implementation
*
*/


#include "ncd_parser_cp_query.h"
#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "catalogsdebug.h"
#include "ncd_cp_queryimpl.h"
#include "ncd_cp_queryelement.h"
#include "ncd_cp_queryelementimpl.h"
#include "ncd_parser_cp_queryelement.h"
#include "ncdstring.h"

CNcdConfigurationProtocolQueryParser* 
CNcdConfigurationProtocolQueryParser::NewL( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth,
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes )
    {
    CNcdConfigurationProtocolQueryParser* self 
        = new(ELeave) CNcdConfigurationProtocolQueryParser( 
            aObservers, aSubParserObserver, aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes );
    CleanupStack::Pop( self );
    return self;
    }

CNcdConfigurationProtocolQueryParser::CNcdConfigurationProtocolQueryParser( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {
    }

CNcdConfigurationProtocolQueryParser::~CNcdConfigurationProtocolQueryParser()
    {
    delete iQuery;
    iQuery = 0;
    }

void CNcdConfigurationProtocolQueryParser::ConstructL( 
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes )
    {
    CNcdSubParser::ConstructL( aElement );
    
    if (!iQuery)
        iQuery = CNcdConfigurationProtocolQueryImpl::NewL();

    // id
    NcdProtocolUtils::AssignDesL( 
        iQuery->iId, AttributeValue( KAttrId, aAttributes ) );

    // schemantics
    TPtrC8 semantics = AttributeValue( KAttrSemantics, aAttributes );
    if( semantics == KNcdQuerySemanticsOperator )
        {
        iQuery->iSemantics = MNcdQuery::ESemanticsOperatorQuery;
        }
    else if ( semantics == KNcdQuerySemanticsLanguage )
        {
        iQuery->iSemantics = MNcdQuery::ESemanticsLanguageQuery;
        }
    else if ( semantics == KNcdQuerySemanticsRegistration )
        {
        iQuery->iSemantics = MNcdQuery::ESemanticsRegistrationQuery;
        }
    else if ( semantics == KNcdQuerySemanticsAuthentication )
        {
        iQuery->iSemantics = MNcdQuery::ESemanticsAuthenticationQuery;
        }
    else if ( semantics == KNcdQuerySemanticsReview )
        {
        iQuery->iSemantics = MNcdQuery::ESemanticsReviewQuery;
        }
    else if ( semantics == KNcdQuerySemanticsInfo )
        {
        iQuery->iSemantics = MNcdQuery::ESemanticsInfoMessage;
        }
    else if ( semantics == KNcdQuerySemanticsError )
        {
        iQuery->iSemantics = MNcdQuery::ESemanticsErrorMessage;
        }
    else if ( semantics == KNcdQuerySemanticsDisclaimer )
        {
        iQuery->iSemantics = MNcdQuery::ESemanticsDisclaimer;
        }
    else if ( semantics == KNcdQuerySemanticsAdvertisment )
        {
        iQuery->iSemantics = MNcdQuery::ESemanticsAdvertisementMessage;
        }
    else if ( semantics == KNcdQuerySemanticsConfirmation )
        {
        iQuery->iSemantics = MNcdQuery::ESemanticsConfirmationQuery;
        }
    else if ( semantics == KNcdQuerySemanticsPayment )
        {
        iQuery->iSemantics = MNcdQuery::ESemanticsPaymentQuery;
        }
    else
        {
        iQuery->iSemantics = MNcdQuery::ESemanticsNone;
        }

    
    // trigger
    TPtrC8 trigger = AttributeValue( KAttrTrigger, aAttributes );
    if( trigger == KNcdPreminetProtocolTriggerAutomatic )
        {
        iQuery->iTrigger = MNcdConfigurationProtocolQuery::ETriggerAutomatic;
        }
    else if( trigger == KNcdPreminetProtocolTriggerActivation )
        {
        iQuery->iTrigger = MNcdConfigurationProtocolQuery::ETriggerActivation;
        }
     
    // force
    TPtrC8 force = AttributeValue( KAttrForce, aAttributes );
    NcdProtocolUtils::DesToBool( iQuery->iForce, force );

    // responseUri
    NcdProtocolUtils::AssignDesL( iQuery->iResponseUri, 
        AttributeValue( KAttrResponseUri, aAttributes ) );
    
    // optional
    TPtrC8 optional = AttributeValue( KAttrOptional, aAttributes );
    NcdProtocolUtils::DesToBool( iQuery->iOptional, optional );
    }

void CNcdConfigurationProtocolQueryParser::OnStartElementL( 
    const Xml::RTagInfo& aElement, 
    const Xml::RAttributeArray& aAttributes, 
    TInt aErrorCode ) 
    {
    CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
    TPtrC8 tag( aElement.LocalName().DesC() );

    if ( iSubParser == 0 )
        {
        DLTRACE(("query start tag=%S error=%d depth=%d",&aElement.LocalName().DesC(),aErrorCode,iDepth));
        if( tag == KTagBodyText )
            {
            // read in OnContextL
            iQuery->iBodyText->SetKeyL( AttributeValue( KAttrKey, aAttributes ) );
            }
        else if( tag == KTagTitle )
            {
            // read in OnContextL
            iQuery->iTitle->SetKeyL( AttributeValue( KAttrKey, aAttributes ) );
            }
        else if( tag == KTagQueryElement )
            {
            iSubParser = CNcdConfigurationProtocolQueryElementParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        else 
            {
            iSubParser = CNcdUnknownParser::NewL( 
                *iObservers, *this, iDepth + 1, aElement, aAttributes );
            }
        }
    else
        {
        // Use existing subparser.
        iSubParser->OnStartElementL( aElement, aAttributes, aErrorCode );
        }
    }

void CNcdConfigurationProtocolQueryParser::OnEndElementL(
    const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));
    
    TPtrC8 tag( aElement.LocalName().DesC() );
    DLTRACE(("end iTag=%S tag=%S", iTag, &aElement.LocalName().DesC()));

    if ( iBuffer )
        {
        if ( tag == KTagBodyText ) 
            {
            iQuery->iBodyText->SetDataL( *iBuffer );
            }
        else if ( tag == KTagTitle ) 
            {
            iQuery->iTitle->SetDataL( *iBuffer );
            }
        delete iBuffer;
        iBuffer = 0;
        }

    if( iSubParser == 0 && iTag && *iTag == aElement.LocalName().DesC() )
        {
        DLTRACE(("end tag=%S",&aElement.LocalName().DesC()));
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

void CNcdConfigurationProtocolQueryParser::SubParserFinishedL( 
    const TDesC8& aTag, TInt /*aErrorCode*/ )
    {
    DLTRACE(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning

    if( iSubParser->Tag() == KTagQueryElement )
        {
        CNcdConfigurationProtocolQueryElementImpl* queryElement = 
            static_cast<CNcdConfigurationProtocolQueryElementParser*>
            ( iSubParser )->QueryElement();
        CleanupDeletePushL( queryElement );
        iQuery->iQueryElements.AppendL( queryElement );
        CleanupStack::Pop( queryElement );
        }

    delete iSubParser;
    iSubParser = 0;
    }

MNcdConfigurationProtocolQuery* 
CNcdConfigurationProtocolQueryParser::Query()
{
    MNcdConfigurationProtocolQuery* tmpQueryPtr = iQuery;
    iQuery = 0;
    return tmpQueryPtr;
}


