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
* Description:   CNcdPreminetProtocolResponseParser implementation
*
*/


#include "ncdprotocolwords.h"
#include "ncdunknownparser.h"
#include "ncdparserobserver.h"
#include "ncdprotocolutils.h"
#include "ncd_parser_pp_response.h"
#include "ncd_parser_pp_entityref.h"
#include "ncd_parser_pp_embeddeddata.h"
#include "ncd_parser_pp_purchase.h"
#include "ncd_parser_pp_subscriptionreport.h"
#include "ncd_parser_pp_information.h"
#include "ncd_parser_cp_error.h"
#include "ncdprotocolutils.h"
#include "ncd_pp_folderref.h"
#include "ncd_pp_itemref.h"
#include "ncd_pp_informationimpl.h"
#include "catalogsdebug.h"


CNcdPreminetProtocolResponseParser* CNcdPreminetProtocolResponseParser::NewL( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth,
    const Xml::RTagInfo& aElement, 
    const Xml::RAttributeArray& aAttributes )
    {
    CNcdPreminetProtocolResponseParser* self 
        = new(ELeave) CNcdPreminetProtocolResponseParser( 
            aObservers, aSubParserObserver, aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolResponseParser::CNcdPreminetProtocolResponseParser( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {

    }

CNcdPreminetProtocolResponseParser::~CNcdPreminetProtocolResponseParser()
    {
    delete iResponseNamespace;
    }


void CNcdPreminetProtocolResponseParser::ConstructL( 
    const Xml::RTagInfo& aElement, 
    const Xml::RAttributeArray& aAttributes )
    {
    DLTRACEIN((""));
    CNcdSubParser::ConstructL( aElement );

    // handle attributes
    TPtrC8 responseNamespace = AttributeValue( KAttrNamespace, aAttributes );
    NcdProtocolUtils::AssignDesL( iResponseNamespace, responseNamespace );

    DLINFO(("namespace=%S",&responseNamespace));
    TPtrC8 id = AttributeValue( KAttrId, aAttributes );
    DLINFO(("id=%S",&id));
    TPtrC8 version = AttributeValue( KAttrVersion, aAttributes );
    DLINFO(("version=%S",&version));

    TPtrC8 session = AttributeValue( KAttrSession, aAttributes );
    if( session != KNullDesC8 )
        {
        if( iObservers->SessionObserver() )
            {
            HBufC* data = NcdProtocolUtils::ConvertUtf8ToUnicodeLC( session );
            iObservers->SessionObserver()->SessionL( 
                *data, iObservers->SessionOrigin(), *iResponseNamespace );
            CleanupStack::PopAndDestroy( data );
            }
        }
    
    }


void CNcdPreminetProtocolResponseParser::OnStartElementL( 
    const Xml::RTagInfo& aElement, 
    const Xml::RAttributeArray& aAttributes, 
    TInt aErrorCode) 
    {

    TPtrC8 tag( aElement.LocalName().DesC() );
    
    if( iSubParser == 0 )
        {
        DLTRACEIN(("preminet start tag=%S error=%d depth=%d",&aElement.LocalName().DesC(),aErrorCode,iDepth));
        if( tag == KTagEntity )
            {
            iSubParser = CNcdPreminetProtocolEntityRefParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes,
                KNullDesC /*no parent id*/, *iResponseNamespace );
            }
        else if( tag == KTagEmbeddedData )
            {
            iSubParser = CNcdPreminetProtocolEmbeddedDataParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes,
                *iResponseNamespace );
            // no handling needed in SubParserFinishedL
            }
        else if( tag == KTagPurchase )
            {
            iSubParser = CNcdPreminetProtocolPurchaseParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes,
                KNullDesC /*no parent id*/ );
            // no handling needed in SubParserFinishedL
            }
        else if( tag == KTagSubscriptionReport )
            {
            iSubParser = CNcdPreminetProtocolSubscriptionReportParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes,
                KNullDesC /*no parent id*/, *iResponseNamespace );
            // no handling needed in SubParserFinishedL
            }
        else if( tag == KTagInformation )
            {
            iSubParser = CNcdPreminetProtocolInformationParser::NewL( 
                    *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        else if( tag == KTagError )
            {
            iSubParser = CNcdConfigurationProtocolErrorParser::NewL( 
                    *iObservers, *this, iDepth+1, aElement, aAttributes );
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



void CNcdPreminetProtocolResponseParser::OnEndElementL(
    const Xml::RTagInfo& aElement, TInt aErrorCode)
    {
    DLTRACEIN((""));
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


void CNcdPreminetProtocolResponseParser::OnContentL(
    const TDesC8& aBytes, TInt aErrorCode)
    { 
    DLTRACEIN((""));
    if( iSubParser )
        {
        iSubParser->OnContentL( aBytes, aErrorCode );
        }
    }

void CNcdPreminetProtocolResponseParser::SubParserFinishedL( 
    const TDesC8& aTag, TInt /*aErrorCode*/ )
    {
    DLTRACEIN(("tag=%S subparser=%X",&aTag,iSubParser));

    if( iSubParser->Type() == EParserEntityRef &&
        iObservers->EntityObserver() &&
        aTag == KTagEntity )
        {
        MNcdPreminetProtocolItemRef* itemRef =
            static_cast<CNcdPreminetProtocolEntityRefParser*>
            ( iSubParser )->ItemRef();
        MNcdPreminetProtocolFolderRef* folderRef = 
            static_cast<CNcdPreminetProtocolEntityRefParser*>
            ( iSubParser )->FolderRef();
        DLINFO(("subparser itemref=%X folderref=%X",itemRef,folderRef));
        if( itemRef )
            {
            iObservers->EntityObserver()->ItemRefL( itemRef );
            }
        if( folderRef )
            {
            DLINFO((_L("folder id=%S"),&folderRef->Id()));
            iObservers->EntityObserver()->FolderRefL( folderRef );
            }
        }
    else if( aTag == KTagInformation )
        {
        MNcdParserInformationObserver* observer = iObservers->InformationObserver();
        
        if( observer )
            {
            MNcdPreminetProtocolInformation* information =
                static_cast<CNcdPreminetProtocolInformationParser*>( iSubParser )->Information();
            CNcdPreminetProtocolInformationImpl* info = 
                static_cast<CNcdPreminetProtocolInformationImpl*>(information);
            NcdProtocolUtils::AssignDesL( info->iNamespace, *iResponseNamespace );
            observer->InformationL( information );
            }
        }

    else if( aTag == KTagError )
        {
        MNcdParserErrorObserver* observer = iObservers->ErrorObserver();
        
        if( observer )
            {
            MNcdPreminetProtocolError* error =
                static_cast<CNcdConfigurationProtocolErrorParser*>( iSubParser )->PreminetProtocolError();
            observer->ErrorL( error );
            }
        }
    
    delete iSubParser;
    iSubParser = 0; 
    }
