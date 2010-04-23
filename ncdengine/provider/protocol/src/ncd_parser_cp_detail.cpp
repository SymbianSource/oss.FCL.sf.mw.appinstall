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
* Description:   CNcdConfigurationProtocolDetailParser implementation
*
*/


#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncd_cp_detailimpl.h"
#include "ncd_parser_cp_detail.h"
#include "ncd_parser_pp_response.h" 

#include "catalogsdebug.h"

CNcdConfigurationProtocolDetailParser* 
CNcdConfigurationProtocolDetailParser::NewL( MNcdParserObserverBundle& aObservers,
                                             MNcdSubParserObserver& aSubParserObserver,
                                             TInt aDepth,
                                             const Xml::RTagInfo& aElement,
                                             const Xml::RAttributeArray& aAttributes )
    {
    CNcdConfigurationProtocolDetailParser* self 
        = new(ELeave) CNcdConfigurationProtocolDetailParser( aObservers,
                                                             aSubParserObserver,
                                                             aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes );
    CleanupStack::Pop( self );
    return self;
    }

CNcdConfigurationProtocolDetailParser::CNcdConfigurationProtocolDetailParser( MNcdParserObserverBundle& aObservers,
                                                                              MNcdSubParserObserver& aSubParserObserver,
                                                                              TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {
    }

CNcdConfigurationProtocolDetailParser::~CNcdConfigurationProtocolDetailParser()
    {
    delete iDetail;
    iDetail = 0;
    delete iContent;
    iContent = 0;
    }

void CNcdConfigurationProtocolDetailParser::ConstructL( const Xml::RTagInfo& aElement,
                                                        const Xml::RAttributeArray& aAttributes )
    {
    CNcdSubParser::ConstructL( aElement );
    
    iDetail = CNcdConfigurationProtocolDetailImpl::NewL();
    
    const TDesC8* id = &AttributeValue( KAttrId, aAttributes );
    if( *id == KNullDesC8 )
        {
        id = &AttributeValue( KAttrIdentifier, aAttributes );
        }
    NcdProtocolUtils::AssignDesL( iDetail->iId, *id );
    NcdProtocolUtils::AssignDesL( iDetail->iValue, AttributeValue( KAttrValue, aAttributes ) );
    NcdProtocolUtils::AssignDesL( iDetail->iGroupId, AttributeValue( KAttrGroupId, aAttributes ) );
    NcdProtocolUtils::AssignDesL( iDetail->iLabel, AttributeValue( KAttrLabel, aAttributes ) );
    }

void CNcdConfigurationProtocolDetailParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                                             const Xml::RAttributeArray& aAttributes, 
                                                             TInt aErrorCode ) 
    {
    DLTRACEIN(("detail start tag=%S error=%d depth=%d",
        &aElement.LocalName().DesC(),aErrorCode,iDepth));
    
    CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
    
    TPtrC8 tag( aElement.LocalName().DesC() );

    if( iSubParser )
        {
        // Use existing subparser.
        iSubParser->OnStartElementL( aElement, aAttributes, aErrorCode );
        }
    else if( tag == KTagXmlFragment )
        {
// Old Download! support case, no longer usable.
//         if( AttributeValue( KAttrId, aAttributes ) == KIdPreminetProtocol &&
//             AttributeValue( KAttrVersion, aAttributes ) == KIdPreminetProtocolVersion )     
//             {
//             DLINFO(("xmlFragment accepted, preminetResponse"));
//             // The following tag will be "preminetResponse", handled below.
//             }
//         else
//            {
            DLERROR(("Unknown xml fragment %S,%S",
                     &AttributeValue( KAttrId, aAttributes ),
                     &AttributeValue( KAttrVersion, aAttributes ) ));
            iSubParser = CNcdUnknownParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
//            }
        }
// Old Download! support case, no longer usable.
//     else if( tag == KTagPreminetResponse )
//         {
//         iSubParser = CNcdPreminetProtocolResponseParser::NewL( *iObservers, *this, iDepth+1, 
//                                                                aElement, aAttributes );
//         }
    else if( tag == KTagDetail )
        {
        iSubParser = CNcdConfigurationProtocolDetailParser::NewL( 
            *iObservers, *this, iDepth+1, aElement, aAttributes );
        }
    else if( tag == KTagContent )
        {
        iContent = CNcdConfigurationProtocolContentImpl::NewL();
        NcdProtocolUtils::AssignDesL( iContent->iKey, AttributeValue( KAttrKey, aAttributes ) );
        NcdProtocolUtils::AssignDesL( iContent->iValue, AttributeValue( KAttrValue, aAttributes ) );
        // content is saved in OnEndElementL
        }
    else
        {
        iSubParser = CNcdUnknownParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
        }
    }


void CNcdConfigurationProtocolDetailParser::OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));
    TPtrC8 tag( aElement.LocalName().DesC() );
    DLTRACE(("end tag=%S",&aElement.LocalName().DesC()));
    if (iBuffer) 
        {
        if (tag == KTagContent && iContent)
            {
            NcdProtocolUtils::AssignDesL(iContent->iContent, *iBuffer);
            }
        delete iBuffer;
        iBuffer = 0;
        }
    
    if ( tag == KTagContent && iContent ) 
        {
        User::LeaveIfError( iDetail->iContents.Append( iContent ) );
        iContent = 0;
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

MNcdConfigurationProtocolDetail* CNcdConfigurationProtocolDetailParser::Detail()
{
    MNcdConfigurationProtocolDetail* returnPtr = iDetail;
    iDetail = 0;
    return returnPtr;
}

void CNcdConfigurationProtocolDetailParser::SubParserFinishedL( const TDesC8& aTag, 
                                                                TInt /*aErrorCode*/ )
    {
    DLTRACE(("tag=%S subparser=%X",&aTag,iSubParser));

    if( aTag == KTagDetail )
        {
        CNcdConfigurationProtocolDetailParser* detailParser = 
            static_cast<CNcdConfigurationProtocolDetailParser*>( iSubParser );
        MNcdConfigurationProtocolDetail* detail = detailParser->Detail();
        User::LeaveIfError( iDetail->iDetails.Append( detail ));
        }
    
    delete iSubParser;
    iSubParser = 0;
    }


