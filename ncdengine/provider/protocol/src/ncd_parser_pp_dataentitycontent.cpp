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


#include "ncd_parser_pp_dataentitycontent.h"
#include "ncd_parser_pp_entitydependency.h"
#include "ncd_pp_dataentitycontent.h"
#include "ncd_pp_dataentitycontentimpl.h"
#include "ncd_pp_entitydependency.h"
#include "ncd_pp_entitydependencyimpl.h"
#include "ncdparserobserverbundleimpl.h"
#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncdprotocoltypes.h"

#include "catalogsdebug.h"


CNcdPreminetProtocolDataEntityContentParser* 
CNcdPreminetProtocolDataEntityContentParser::NewL( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth,
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& aNamespace )
    {
    CNcdPreminetProtocolDataEntityContentParser* self 
        = new(ELeave) CNcdPreminetProtocolDataEntityContentParser( 
            aObservers, aSubParserObserver, aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes, aNamespace );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolDataEntityContentParser::CNcdPreminetProtocolDataEntityContentParser( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {

    }

CNcdPreminetProtocolDataEntityContentParser::~CNcdPreminetProtocolDataEntityContentParser()
    {
    delete iContent;
    }

void CNcdPreminetProtocolDataEntityContentParser::ConstructL( 
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& /*aNamespace*/ )
    {
    CNcdSubParser::ConstructL( aElement );

    DLTRACEIN(("depth=%d tag=%S",iDepth,iTag));

    if (!iContent)
        {
        iContent = CNcdPreminetProtocolDataEntityContentImpl::NewL();
        }

    // read attributes here
    TPtrC8 timestamp = AttributeValue( KAttrTimestamp, aAttributes );
    TPtrC8 size = AttributeValue( KAttrSize, aAttributes );
    TPtrC8 mime = AttributeValue( KAttrMime, aAttributes );
    TPtrC8 id = AttributeValue( KAttrId, aAttributes );
    TPtrC8 version = AttributeValue( KAttrVersion, aAttributes );

    NcdProtocolUtils::AssignDesL(iContent->iTimestamp, timestamp);
    if( size != KNullDesC8 )
        {
        iContent->iSize = NcdProtocolUtils::DesDecToIntL(size);
        }
    NcdProtocolUtils::AssignDesL(iContent->iMime, mime);
    NcdProtocolUtils::AssignDesL(iContent->iId, id);
    NcdProtocolUtils::AssignDesL(iContent->iVersion, version);
    
    // attribute set only in subscribableContent
    TPtrC8 type = AttributeValue( KAttrType, aAttributes );
    if ( type == KValuePeriodic ) 
        iContent->iSubscriptionType =  EPeriodic;
    else if ( type == KValueAutomaticContinous ) 
        iContent->iSubscriptionType =  EAutomaticContinous;
    else    
        iContent->iSubscriptionType =  ENotSubscribable;
    }


void CNcdPreminetProtocolDataEntityContentParser::OnStartElementL( 
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

        if( tag == KTagName || tag == KTagDescription )
            {
            // handled in OnContentL and OnEndElementL
            }
        else if( tag == KTagContentPurposes || tag == KTagDependencies )
            {
            // skipped
            }
        else if( tag == KTagContentPurpose )
            {
            // read in OnContentL and OnEndElementL
            }
        else if( tag == KTagPreview || tag == KTagReview )
            {
            //  ignored for now
            }
        else if( tag == KTagValidUntil )
            {
            TPtrC8 delta = AttributeValue( KAttrDelta, aAttributes );
            iContent->iValidUntilDelta = NcdProtocolUtils::DesDecToIntL(delta);

            TPtrC8 autoUpdate = AttributeValue( KAttrAutoUpdate, aAttributes );
            NcdProtocolUtils::DesToBool(iContent->iValidUntilAutoUpdate, autoUpdate);
            }
        else if( tag == KTagDependency )
            {
            iSubParser = CNcdPreminetProtocolEntityDependencyParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes,
                KNullDesC /*no parent id*/ );
            }
        else if( tag == KTagChildAccessibility )
            {
            TPtrC8 viewable = AttributeValue( KAttrViewable, aAttributes );
            NcdProtocolUtils::DesToBool(iContent->iChildViewable, viewable);

            TPtrC8 separatelyPurchasable = AttributeValue( KAttrViewable, aAttributes );
            NcdProtocolUtils::DesToBool(iContent->iChildSeparatelyPurchasable, separatelyPurchasable);
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



void CNcdPreminetProtocolDataEntityContentParser::OnEndElementL(
    const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {

    TPtrC8 tag( aElement.LocalName().DesC() );
    DLTRACEIN(("%X end iTag=%S tag=%S", this, iTag, &tag));

    if ( iBuffer )
        {
        if ( tag == KTagContentPurpose ) 
            {
            HBufC* purpose = 0;
            NcdProtocolUtils::AssignDesL(purpose, *iBuffer);
            iContent->iContentPurposes.Append(purpose);
            DLINFO((_L("contentPurpose=%S"),purpose));
            }
        delete iBuffer;
        iBuffer = 0;
        }

    if ( iSubParser == 0 && iTag && *iTag == aElement.LocalName().DesC() )
        {
        // dataEntity end
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
void CNcdPreminetProtocolDataEntityContentParser::SubParserFinishedL( 
    const TDesC8& aTag, TInt /*aErrorCode*/ )
    {
    DLTRACEIN(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning

    if( iSubParser->Tag() == KTagDependency )
        {
        MNcdPreminetProtocolEntityDependency* dep =
            static_cast<CNcdPreminetProtocolEntityDependencyParser*>
            ( iSubParser )->Dependency();
        DLINFO(("subparser entityDependency=%X",dep));
        if( dep && iContent )
            {
            iContent->iEntityDependencies.AppendL(dep);
            }
        }
    else
        {
        DLWARNING(("Child entity %S ignored", &aTag));
        }

    delete iSubParser;
    iSubParser = 0;
    }

MNcdPreminetProtocolDataEntityContent* 
CNcdPreminetProtocolDataEntityContentParser::Content()
    {
    DLTRACEIN(("%X",iContent));
    MNcdPreminetProtocolDataEntityContent* content= iContent;
    iContent = 0;
    return content;
    }
