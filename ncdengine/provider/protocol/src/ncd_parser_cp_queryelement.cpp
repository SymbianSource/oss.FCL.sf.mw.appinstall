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


#include "ncd_parser_cp_queryelement.h"
#include "ncd_parser_cp_detail.h"
#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "catalogsdebug.h"
#include "ncd_cp_queryelementimpl.h"
#include "ncd_cp_queryoptionimpl.h"
#include "ncd_cp_detailimpl.h"
#include "ncdstring.h"

CNcdConfigurationProtocolQueryElementParser* 
CNcdConfigurationProtocolQueryElementParser::NewL( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth,
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes )
    {
    CNcdConfigurationProtocolQueryElementParser* self 
        = new(ELeave) CNcdConfigurationProtocolQueryElementParser( 
            aObservers, aSubParserObserver, aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes );
    CleanupStack::Pop( self );
    return self;
    }

CNcdConfigurationProtocolQueryElementParser::CNcdConfigurationProtocolQueryElementParser( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth )
    
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {
    }

CNcdConfigurationProtocolQueryElementParser::~CNcdConfigurationProtocolQueryElementParser()
    {
    delete iQueryElement;
    iQueryElement = 0;
    }

void CNcdConfigurationProtocolQueryElementParser::ConstructL( const Xml::RTagInfo& aElement,
                                                         const Xml::RAttributeArray& aAttributes )
    {
    CNcdSubParser::ConstructL( aElement );
    
    iQueryElement = new(ELeave) CNcdConfigurationProtocolQueryElementImpl();
    iQueryElement->ConstructL();

    // id
    NcdProtocolUtils::AssignDesL( iQueryElement->iId, AttributeValue( KAttrId, aAttributes ) );
    
    // semantics
    TPtrC8 semantics = AttributeValue( KAttrSemantics, aAttributes );

    if( semantics == KNcdQueryElementSemanticsMsisdn ) 
        iQueryElement->iSemantics = MNcdQueryItem::ESemanticsMsisdn;
    
    else if( semantics == KNcdQueryElementSemanticsEmailAddress )
        iQueryElement->iSemantics = MNcdQueryItem::ESemanticsEmailAddress;
        
    else if( semantics == KNcdQueryElementSemanticsCreditCardNumber )
        iQueryElement->iSemantics = MNcdQueryItem::ESemanticsCreditCardNumber;    
    
    else if( semantics == KNcdQueryElementSemanticsCreditCardExpirationYear )
        iQueryElement->iSemantics = MNcdQueryItem::ESemanticsCreditCardExpirationYear;    
    
    else if( semantics == KNcdQueryElementSemanticsCreditCardExpirationMonth )
            iQueryElement->iSemantics = MNcdQueryItem::ESemanticsCreditCardExpirationMonth;    

    else if( semantics == KNcdQueryElementSemanticsCreditCardOwner )
            iQueryElement->iSemantics = MNcdQueryItem::ESemanticsCreditCardOwner;
   
    else if( semantics == KNcdQueryElementSemanticsCreditCardVerificationCode )
            iQueryElement->iSemantics = MNcdQueryItem::ESemanticsCreditCardVerificationCode;

    else if( semantics == KNcdQueryElementSemanticsCreditCardType )
            iQueryElement->iSemantics = MNcdQueryItem::ESemanticsCreditCardType;

    else if( semantics == KNcdQueryElementSemanticsAddressStreet )
            iQueryElement->iSemantics = MNcdQueryItem::ESemanticsAddressStreet;

    else if( semantics == KNcdQueryElementSemanticsAddressZipCode )
        iQueryElement->iSemantics = MNcdQueryItem::ESemanticsAddressZipCode;

    else if( semantics == KNcdQueryElementSemanticsAddressCity )
        iQueryElement->iSemantics = MNcdQueryItem::ESemanticsAddressCity;

    else if( semantics == KNcdQueryElementSemanticsAddressCountry )
        iQueryElement->iSemantics = MNcdQueryItem::ESemanticsAddressCountry;

    else if( semantics == KNcdQueryElementSemanticsUserName )
        iQueryElement->iSemantics = MNcdQueryItem::ESemanticsUserName;

    else if( semantics == KNcdQueryElementSemanticsPassword )
        iQueryElement->iSemantics = MNcdQueryItem::ESemanticsPassword;

    else if( semantics == KNcdQueryElementSemanticsPinCode )
        iQueryElement->iSemantics = MNcdQueryItem::ESemanticsPinCode;

    else if( semantics == KNcdQueryElementSemanticsImei )
        iQueryElement->iSemantics = MNcdQueryItem::ESemanticsImei;

    else
        iQueryElement->iSemantics = MNcdQueryItem::ESemanticsUnknown;
    
    // type
    TPtrC8 type = AttributeValue( KAttrType, aAttributes );
    if( type == KNcdQueryElementTypeString )
        iQueryElement->iType = MNcdConfigurationProtocolQueryElement::EFreeText;

    else if( type == KNcdQueryElementTypeNumeric )
        iQueryElement->iType = MNcdConfigurationProtocolQueryElement::ENumeric;

    else if( type == KNcdQueryElementTypeSingleSelect )
        iQueryElement->iType = MNcdConfigurationProtocolQueryElement::ESingleSelect;

    else if( type == KNcdQueryElementTypeMultiSelect )
        iQueryElement->iType = MNcdConfigurationProtocolQueryElement::EMultiSelect;

    else if( type == KNcdQueryElementTypeGpsLocation )
        iQueryElement->iType = MNcdConfigurationProtocolQueryElement::EGpsLocation;

    else if( type == KNcdQueryElementTypeConfiguration )
        iQueryElement->iType = MNcdConfigurationProtocolQueryElement::EConfiguration;

    else if( type == KNcdQueryElementTypeFile )
        iQueryElement->iType = MNcdConfigurationProtocolQueryElement::EFile;

    else if( type == KNcdQueryElementTypeSms )
        iQueryElement->iType = MNcdConfigurationProtocolQueryElement::ESms;

    else if( type == KNcdQueryElementTypePurchaseHistory )
        iQueryElement->iType = MNcdConfigurationProtocolQueryElement::EPurchaseHistory;

    else
        iQueryElement->iType = MNcdConfigurationProtocolQueryElement::EUnknown;
    
    // optional
    TPtrC8 optional = AttributeValue( KAttrOptional, aAttributes );
    NcdProtocolUtils::DesToBool( iQueryElement->iOptional, optional );

    }

void CNcdConfigurationProtocolQueryElementParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                                       const Xml::RAttributeArray& aAttributes, 
                                                       TInt aErrorCode ) 
    {
    CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
    TPtrC8 tag( aElement.LocalName().DesC() );

    if ( iSubParser == 0 ) 
        {


        DLTRACE(("query element start tag=%S error=%d depth=%d",&aElement.LocalName().DesC(),aErrorCode,iDepth));
        if( tag == KTagOption )
            {
            CNcdConfigurationProtocolQueryOptionImpl* queryOption = 
                new(ELeave) CNcdConfigurationProtocolQueryOptionImpl();
            CleanupStack::PushL( queryOption );
            queryOption->ConstructL();
            
            // Queryoption value
            NcdProtocolUtils::AssignDesL( queryOption->iValue, AttributeValue( KAttrValue, aAttributes ) );
            // Queryoption name
            queryOption->iName->SetDataL( AttributeValue( KAttrName, aAttributes ) );
            // queryoption key
            queryOption->iName->SetKeyL( AttributeValue( KAttrKey, aAttributes ) );
            
            iQueryElement->iOptions.AppendL( queryOption );
            CleanupStack::Pop( queryOption );
            // subquery missing
            }
        else if( tag == KTagLabel )
            {
            iQueryElement->iLabel->SetKeyL( AttributeValue( KAttrKey, aAttributes ) );
            }
        else if( tag == KTagMessage )
            {
            iQueryElement->iMessage->SetKeyL( AttributeValue( KAttrKey, aAttributes ) );
            }
        else if( tag == KTagDescription )
            {
            iQueryElement->iDescription->SetKeyL( AttributeValue( KAttrKey, aAttributes ) );
            }
        else if( tag == KTagDetails )
            {
            iSubParser = CNcdConfigurationProtocolDetailParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        else 
            {
            iSubParser = CNcdUnknownParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        // sub-queryElements not yet implemented
        }
    else 
        {
        // Use existing subparser.
        iSubParser->OnStartElementL( aElement, aAttributes, aErrorCode );
        }
    }

void CNcdConfigurationProtocolQueryElementParser::OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));
    
    TPtrC8 tag( aElement.LocalName().DesC() );
    DLTRACE(("end tag=%S",&aElement.LocalName().DesC()));

    if( iBuffer )
        {
        if( tag == KTagLabel )
            {
            iQueryElement->iLabel->SetDataL( *iBuffer );
            }
        else if( tag == KTagMessage )
            {
            iQueryElement->iMessage->SetDataL( *iBuffer );
            }
        else if( tag == KTagDescription )
            {
            iQueryElement->iDescription->SetDataL( *iBuffer );
            }
        delete iBuffer;
        iBuffer = 0;
        }

    if( iSubParser == 0 && iTag && *iTag == aElement.LocalName().DesC() )
        {
        DLTRACE(("end tag=%S",&aElement.LocalName().DesC()));
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

void CNcdConfigurationProtocolQueryElementParser::SubParserFinishedL( 
    const TDesC8& aTag, TInt /*aErrorCode*/ )
    {
    DLTRACE(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning

    if ( iSubParser->Tag() == KTagDetails ) 
        {
        MNcdConfigurationProtocolDetail* detail =
            static_cast<CNcdConfigurationProtocolDetailParser*>( iSubParser )->Detail();
        if ( detail && iQueryElement )
            {
            iQueryElement->iDetails = detail;
            }
        }
        
    delete iSubParser;
    iSubParser = 0;
    }

CNcdConfigurationProtocolQueryElementImpl* CNcdConfigurationProtocolQueryElementParser::QueryElement()
{
    CNcdConfigurationProtocolQueryElementImpl* tmpQueryElementPtr = iQueryElement;
    iQueryElement = 0;
    return tmpQueryElementPtr;
}

