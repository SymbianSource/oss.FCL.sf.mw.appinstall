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


#include "ncd_parser_pp_entitydependency.h"
#include "ncd_parser_pp_download.h"
#include "ncd_pp_entitydependency.h"
#include "ncd_pp_entitydependencyimpl.h"
#include "ncdparserobserverbundleimpl.h"
#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncdprotocoltypes.h"

#include "catalogsdebug.h"


CNcdPreminetProtocolEntityDependencyParser* 
CNcdPreminetProtocolEntityDependencyParser::NewL( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth,
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& aNamespace )
    {
    CNcdPreminetProtocolEntityDependencyParser* self 
        = new(ELeave) CNcdPreminetProtocolEntityDependencyParser( 
            aObservers, aSubParserObserver, aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes, aNamespace );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolEntityDependencyParser::CNcdPreminetProtocolEntityDependencyParser( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {

    }

CNcdPreminetProtocolEntityDependencyParser::~CNcdPreminetProtocolEntityDependencyParser()
    {
    delete iDependency;
    }

void CNcdPreminetProtocolEntityDependencyParser::ConstructL( 
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& /*aNamespace*/ )
    {
    CNcdSubParser::ConstructL( aElement );

    DLTRACEIN(("depth=%d tag=%S",iDepth,iTag));

    if (!iDependency)
        {
        iDependency = CNcdPreminetProtocolEntityDependencyImpl::NewL();
        }

    // read attributes here
    TPtrC8 name = AttributeValue( KAttrName, aAttributes );
    TPtrC8 contentId = AttributeValue( KAttrContentId, aAttributes );
    TPtrC8 contentVersion = AttributeValue( KAttrContentVersion, aAttributes );

    NcdProtocolUtils::AssignDesL(iDependency->iName, name);
    NcdProtocolUtils::AssignDesL(iDependency->iContentId, contentId);
    NcdProtocolUtils::AssignDesL(iDependency->iContentVersion, contentVersion);
    
    // attribute set only in subscribableContent
    TPtrC8 type = AttributeValue( KAttrType, aAttributes );
    if ( type == KValueDependency ) 
        iDependency->iType =  EDependency;
    else if ( type == KValueUpgrade) 
        iDependency->iType =  EUpgradeDependency;
    }


void CNcdPreminetProtocolEntityDependencyParser::OnStartElementL( 
    const Xml::RTagInfo& aElement, 
    const Xml::RAttributeArray& aAttributes, 
    TInt aErrorCode) 
    {
    CNcdSubParser::OnStartElementL(aElement, aAttributes, aErrorCode);
    
    TPtrC8 tag( aElement.LocalName().DesC() );
    
    if( iSubParser == 0 )
        {
        DLTRACEIN(("dataEntity start tag=%S error=%d depth=%d",
            &aElement.LocalName().DesC(),aErrorCode,iDepth));

        if( tag == KTagEntity )
            {
            TPtrC8 id = AttributeValue( KAttrId, aAttributes );
            TPtrC8 timestamp = AttributeValue( KAttrTimestamp, aAttributes );
            NcdProtocolUtils::AssignDesL(iDependency->iEntityId, id);
            NcdProtocolUtils::AssignDesL(iDependency->iEntityTimestamp, timestamp);
            }
        else if( tag == KTagDownloadDetails )
            {
            // basic download parser used for now, 
            // this doesn't support rights object
            iSubParser = CNcdPreminetProtocolDownloadParser::NewL(
                *iObservers, *this, iDepth+1, aElement, aAttributes, KNullDesC );
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



void CNcdPreminetProtocolEntityDependencyParser::OnEndElementL(
    const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {

    TPtrC8 tag( aElement.LocalName().DesC() );
    DLTRACEIN(("%X end iTag=%S tag=%S", this, iTag, &tag));

    if ( iSubParser == 0 && iTag && *iTag == aElement.LocalName().DesC() )
        {
        // entityDependency end
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
void CNcdPreminetProtocolEntityDependencyParser::SubParserFinishedL( 
    const TDesC8& aTag, TInt /*aErrorCode*/ )
    {
    DLTRACEIN(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning

    if( iDependency && iSubParser->Tag() == KTagDownloadDetails )
        {
        MNcdPreminetProtocolDownload* download =
            static_cast<CNcdPreminetProtocolDownloadParser*>
            ( iSubParser )->Download();
        DLINFO(("subparser download=%X",download));
        if( download )
            {
            iDependency->iDownloadDetails = download;
            }
        }
    else
        {
        DLWARNING(("Child entity %S ignored", &aTag));
        }

    delete iSubParser;
    iSubParser = 0;
    }

MNcdPreminetProtocolEntityDependency* 
CNcdPreminetProtocolEntityDependencyParser::Dependency()
    {
    DLTRACEIN(("%X",iDependency));
    MNcdPreminetProtocolEntityDependency* dep = iDependency;
    iDependency = 0;
    return dep;
    }
