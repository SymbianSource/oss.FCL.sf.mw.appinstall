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


#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncd_cp_detailimpl.h"
#include "ncd_parser_cp_details.h"
#include "catalogsdebug.h"
#include "ncd_parser_cp_detail.h"

CNcdConfigurationProtocolDetailsParser* 
CNcdConfigurationProtocolDetailsParser::NewL( MNcdParserObserverBundle& aObservers,
                                              MNcdSubParserObserver& aSubParserObserver,
                                              TInt aDepth,
                                              const Xml::RTagInfo& aElement,
                                              const Xml::RAttributeArray& aAttributes )
    {
    CNcdConfigurationProtocolDetailsParser* self 
        = new(ELeave) CNcdConfigurationProtocolDetailsParser( aObservers,
                                                              aSubParserObserver,
                                                              aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes );
    CleanupStack::Pop( self );
    return self;
    }

CNcdConfigurationProtocolDetailsParser::CNcdConfigurationProtocolDetailsParser( MNcdParserObserverBundle& aObservers,
                                                                                MNcdSubParserObserver& aSubParserObserver,
                                                                                TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {
    }

CNcdConfigurationProtocolDetailsParser::~CNcdConfigurationProtocolDetailsParser()
    {
    delete iDetails;
    iDetails = 0;
    }

void CNcdConfigurationProtocolDetailsParser::ConstructL( 
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& /*aAttributes*/ )
    {
    CNcdSubParser::ConstructL( aElement );
    iDetails = new(ELeave) CArrayPtrFlat<MNcdConfigurationProtocolDetail>( 4 );
    }

void CNcdConfigurationProtocolDetailsParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                                              const Xml::RAttributeArray& aAttributes, 
                                                              TInt aErrorCode ) 
    {
    if( iSubParser == 0 )
        {
        CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );

        TPtrC8 tag( aElement.LocalName().DesC() );
        DLTRACE(("details start tag=%S error=%d depth=%d",&aElement.LocalName().DesC(),aErrorCode,iDepth));
        
        if( tag == KTagDetail )
            {
            iSubParser = CNcdConfigurationProtocolDetailParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
            }

        else
            {
            iSubParser = CNcdUnknownParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        }
    else
        {
        iSubParser->OnStartElementL( aElement, aAttributes, aErrorCode );
        }    
    }


void CNcdConfigurationProtocolDetailsParser::OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode) 
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

void CNcdConfigurationProtocolDetailsParser::SubParserFinishedL( const TDesC8& aTag, TInt /*aErrorCode*/ )                                                                  
    {
    DLTRACE(("tag=%S subparser=%X",&aTag,iSubParser));

    if( aTag == KTagDetail )
        {
        MNcdConfigurationProtocolDetail* detail =
            static_cast<CNcdConfigurationProtocolDetailParser*>( iSubParser )->Detail();
        CleanupDeletePushL( detail );
        iDetails->AppendL( detail );
        CleanupStack::Pop( detail );
        }
    
    delete iSubParser;
    iSubParser = 0;
    }

CArrayPtr<MNcdConfigurationProtocolDetail>* CNcdConfigurationProtocolDetailsParser::Details()
{
    CArrayPtr<MNcdConfigurationProtocolDetail>* returnPtr = iDetails;
    iDetails = 0;
    return returnPtr;
}

