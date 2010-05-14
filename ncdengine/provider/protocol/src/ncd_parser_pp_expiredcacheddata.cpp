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

#include "ncd_parser_pp_expiredcacheddata.h"
#include "ncd_pp_expiredcacheddataimpl.h"

#include "catalogsdebug.h"

CNcdPreminetProtocolExpiredCachedDataParser* 
CNcdPreminetProtocolExpiredCachedDataParser::NewL( MNcdParserObserverBundle& aObservers,
                                                   MNcdSubParserObserver& aSubParserObserver,
                                                   TInt aDepth,
                                                   const Xml::RTagInfo& aElement,
                                                   const Xml::RAttributeArray& aAttributes )
    {
    CNcdPreminetProtocolExpiredCachedDataParser* self 
        = new(ELeave) CNcdPreminetProtocolExpiredCachedDataParser( aObservers,
                                                                   aSubParserObserver,
                                                                   aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolExpiredCachedDataParser::CNcdPreminetProtocolExpiredCachedDataParser( 
                                                                            MNcdParserObserverBundle& aObservers,
                                                                            MNcdSubParserObserver& aSubParserObserver,
                                                                            TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {
    }

CNcdPreminetProtocolExpiredCachedDataParser::~CNcdPreminetProtocolExpiredCachedDataParser()
    {
    delete iExpiredCachedData;
    iExpiredCachedData = 0;
    }

void CNcdPreminetProtocolExpiredCachedDataParser::ConstructL( 
    const Xml::RTagInfo& aElement, const Xml::RAttributeArray& /*aAttributes*/ )
    {
    CNcdSubParser::ConstructL( aElement );

    DLTRACEIN(("depth=%d tag=%S",iDepth,iTag));

    iExpiredCachedData = new(ELeave) CNcdPreminetProtocolExpiredCachedDataImpl();
    iExpiredCachedData->ConstructL();

    }

void CNcdPreminetProtocolExpiredCachedDataParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                                                   const Xml::RAttributeArray& aAttributes, 
                                                                   TInt aErrorCode) 
    {
    DLTRACE(("start tag=%S",&aElement.LocalName().DesC()));
    if( iSubParser )
        {
        // Use existing subparser.
        iSubParser->OnStartElementL( aElement, aAttributes, aErrorCode );
        }
    else
        {
        CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
        
        TPtrC8 tag( aElement.LocalName().DesC() );
        
        if( tag == KTagExpiredEntity )
            {
            CNcdPreminetProtocolExpiredEntityImpl* expiredEntity =
                new(ELeave) CNcdPreminetProtocolExpiredEntityImpl();
            ( iExpiredCachedData->iExpiredEntityCounts ).AppendL( expiredEntity );
            expiredEntity->ConstructL();

            // expired entity attributes

            // entity id
            NcdProtocolUtils::AssignDesL( expiredEntity->iEntityId, AttributeValue( KAttrEntityId, aAttributes ));

            // recursive
            TPtrC8 recursive = AttributeValue( KAttrRecursive, aAttributes );
            NcdProtocolUtils::DesToBool( expiredEntity->iRecursive, recursive );

            // force update
            TPtrC8 forceUpdate = AttributeValue( KAttrForceUpdate, aAttributes );
            NcdProtocolUtils::DesToBool( expiredEntity->iForceUpdate, forceUpdate );

            iSubParser = CNcdUnknownParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        
        else if( tag == KTagClearOldEntities )
            {
            CNcdPreminetProtocolClearOldEntitiesImpl* clearOldEntities =
                new(ELeave) CNcdPreminetProtocolClearOldEntitiesImpl();
            iExpiredCachedData->iClearOldEntities = clearOldEntities;
            clearOldEntities->ConstructL();

            // clear old entities attributes

            // start timestamp
            NcdProtocolUtils::AssignDesL( clearOldEntities->iStartTimestamp, AttributeValue( KAttrStartTimestamp, aAttributes ));

            // end timestamp
            NcdProtocolUtils::AssignDesL( clearOldEntities->iEndTimestamp, AttributeValue( KAttrEndTimestamp, aAttributes ));
            
            iSubParser = CNcdUnknownParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        
        else
            {
            iSubParser = CNcdUnknownParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        }
    }


void CNcdPreminetProtocolExpiredCachedDataParser::OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode) 
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

void CNcdPreminetProtocolExpiredCachedDataParser::SubParserFinishedL( 
    const TDesC8& aTag, TInt /*aErrorCode*/ )
    {
    DLTRACE(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning

    delete iSubParser;
    iSubParser = 0;
    }

MNcdPreminetProtocolExpiredCachedData* CNcdPreminetProtocolExpiredCachedDataParser::ExpiredCachedData()
{
    MNcdPreminetProtocolExpiredCachedData* returnPtr = iExpiredCachedData;
    iExpiredCachedData = 0;
    return returnPtr;
}

