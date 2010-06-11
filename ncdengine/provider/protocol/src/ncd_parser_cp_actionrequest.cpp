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
* Description:   CNcdConfigurationProtocolClientConfigurationParser implementation
*
*/


#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"

#include "ncd_parser_cp_actionrequest.h"
#include "ncd_parser_cp_query.h"
#include "ncd_parser_cp_detail.h"
#include "ncd_cp_actionrequestimpl.h"
#include "ncd_cp_query.h"

#include "catalogsdebug.h"

CNcdConfigurationProtocolActionRequestParser* 
CNcdConfigurationProtocolActionRequestParser::NewL( MNcdParserObserverBundle& aObservers,
                                                    MNcdSubParserObserver& aSubParserObserver,
                                                    TInt aDepth,
                                                    const Xml::RTagInfo& aElement,
                                                    const Xml::RAttributeArray& aAttributes )
    {
    CNcdConfigurationProtocolActionRequestParser* self 
        = new(ELeave) CNcdConfigurationProtocolActionRequestParser( aObservers,
                                                                    aSubParserObserver,
                                                                    aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes );
    CleanupStack::Pop( self );
    return self;
    }

CNcdConfigurationProtocolActionRequestParser::CNcdConfigurationProtocolActionRequestParser( 
                                                                            MNcdParserObserverBundle& aObservers,
                                                                            MNcdSubParserObserver& aSubParserObserver,
                                                                            TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {
    }

CNcdConfigurationProtocolActionRequestParser::~CNcdConfigurationProtocolActionRequestParser()
    {
    DLTRACEIN((""));
    delete iActionRequest;
    iActionRequest = 0;
    DLTRACEOUT((""));
    }

void CNcdConfigurationProtocolActionRequestParser::ConstructL( const Xml::RTagInfo& aElement,
                                                                     const Xml::RAttributeArray& aAttributes )
    {
    CNcdSubParser::ConstructL( aElement );

    DLTRACEIN(("depth=%d tag=%S",iDepth,iTag));

    iActionRequest = new(ELeave) CNcdConfigurationProtocolActionRequestImpl();
    iActionRequest->ConstructL();

    // attributes

    // force
    TPtrC8 force = AttributeValue( KAttrForce, aAttributes );
    NcdProtocolUtils::DesToBool( iActionRequest->iForce, force );

    // type    
    TPtrC8 type = AttributeValue( KAttrType, aAttributes );
    if( type == KNcdPreminetProtocolTypeAdd )
        iActionRequest->iType = MNcdConfigurationProtocolActionRequest::ETypeAdd;
    else if( type == KNcdPreminetProtocolTypeRemove )
        iActionRequest->iType = MNcdConfigurationProtocolActionRequest::ETypeRemove;
    else if( type == KNcdPreminetProtocolTypeClear )
        iActionRequest->iType = MNcdConfigurationProtocolActionRequest::ETypeClear;
    else if( type == KNcdPreminetProtocolTypeUpdate )
        iActionRequest->iType = MNcdConfigurationProtocolActionRequest::ETypeUpdate;
      
    // target
    NcdProtocolUtils::AssignDesL( iActionRequest->iTarget, AttributeValue( KAttrTarget, aAttributes ) );
    
    }

void CNcdConfigurationProtocolActionRequestParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                                                          const Xml::RAttributeArray& aAttributes, 
                                                                          TInt aErrorCode) 
    {
    if( iSubParser )
        {
        // Use existing subparser.
        iSubParser->OnStartElementL( aElement, aAttributes, aErrorCode );
        }
    else
        {
        CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
        
        TPtrC8 tag( aElement.LocalName().DesC() );
        DLTRACE(("action request start tag=%S error=%d depth=%d",&aElement.LocalName().DesC(),aErrorCode,iDepth));

        if( tag == KTagMessage )
            {
            iSubParser = CNcdConfigurationProtocolQueryParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        
        else if( tag == KTagDetail )
            {
            iSubParser = CNcdConfigurationProtocolDetailParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
            }

        else if( tag == KTagUpdateDetails )
            {
            CNcdConfigurationProtocolUpdateDetailsImpl* updateDetails = new(ELeave) CNcdConfigurationProtocolUpdateDetailsImpl();
            iActionRequest->iUpdateDetails = updateDetails;
            updateDetails->ConstructL();

            // attributes
            // id
            NcdProtocolUtils::AssignDesL( updateDetails->iId, AttributeValue( KAttrId, aAttributes ) );

            // version
            NcdProtocolUtils::AssignDesL( updateDetails->iVersion, AttributeValue( KAttrVersion, aAttributes ) );
            
            // uri
            NcdProtocolUtils::AssignDesL( updateDetails->iUri, AttributeValue( KAttrUri, aAttributes ) );

// I'm sure this should not be here, so commented it out:
//             iSubParser = CNcdUnknownParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        
        else
            {
            iSubParser = CNcdUnknownParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        }
    }


void CNcdConfigurationProtocolActionRequestParser::OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));
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

void CNcdConfigurationProtocolActionRequestParser::SubParserFinishedL( const TDesC8& aTag, 
                                                                             TInt /*aErrorCode*/ )
    {
    DLTRACE(("tag=%S subparser=%X",&aTag,iSubParser));


    if( aTag == KTagMessage )
        {
        CNcdConfigurationProtocolQueryParser* queryParser = 
            static_cast<CNcdConfigurationProtocolQueryParser*>( iSubParser );
        MNcdConfigurationProtocolQuery* query = queryParser->Query();
        CleanupDeletePushL( query );
        iActionRequest->iMessages.AppendL( query );
        CleanupStack::Pop( query );
        }
    else if( aTag == KTagDetail )
        {
        CNcdConfigurationProtocolDetailParser* detailParser = 
            static_cast<CNcdConfigurationProtocolDetailParser*>( iSubParser );
        MNcdConfigurationProtocolDetail* detail = detailParser->Detail();
        CleanupDeletePushL( detail );
        iActionRequest->iDetails.AppendL( detail );
        CleanupStack::Pop( detail );
        }
    
    delete iSubParser;
    iSubParser = 0;
    }

MNcdConfigurationProtocolActionRequest* CNcdConfigurationProtocolActionRequestParser::ActionRequest()
    {
    MNcdConfigurationProtocolActionRequest* returnPtr = iActionRequest;
    iActionRequest = 0;
    return returnPtr;
    }

