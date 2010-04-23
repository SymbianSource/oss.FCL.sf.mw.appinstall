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
* Description:   CNcdPreminetProtocolRightsParser implementation
*
*/


#include "ncd_parser_pp_rights.h"
#include "ncd_pp_rights.h"
#include "ncd_pp_rightsimpl.h"
#include "ncdparserobserverbundleimpl.h"
#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncdprotocoltypes.h"

#include "catalogsdebug.h"


CNcdPreminetProtocolRightsParser* CNcdPreminetProtocolRightsParser::NewL( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth,
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& aParentId )
    {
    CNcdPreminetProtocolRightsParser* self 
        = new(ELeave) CNcdPreminetProtocolRightsParser( 
            aObservers, aSubParserObserver, aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes, aParentId );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolRightsParser::CNcdPreminetProtocolRightsParser( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {

    }

CNcdPreminetProtocolRightsParser::~CNcdPreminetProtocolRightsParser()
    {
    delete iRights;
    }

void CNcdPreminetProtocolRightsParser::ConstructL( 
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& /*aParentId*/ )
    {
    CNcdSubParser::ConstructL( aElement );

    DLTRACEIN(("depth=%d tag=%S",iDepth,iTag));
    //DLINFO((_L("parent=%S"),&aParentId));

    if (!iRights)
        {
        iRights = CNcdPreminetProtocolRightsImpl::NewL();
        }

    // read attributes here
    TPtrC8 activationKey = AttributeValue( KAttrActivationKey, aAttributes );
    NcdProtocolUtils::AssignDesL(iRights->iActivationKey, activationKey);

    TPtrC8 issuerUri = AttributeValue( KAttrIssuerUri, aAttributes );
    NcdProtocolUtils::AssignDesL(iRights->iIssuerUri, issuerUri);

    TPtrC8 uri = AttributeValue( KAttrUri, aAttributes );
    NcdProtocolUtils::AssignDesL(iRights->iUri, uri);

    TPtrC8 type = AttributeValue( KAttrType, aAttributes );
    NcdProtocolUtils::AssignDesL(iRights->iType, type);

    TPtrC8 name = AttributeValue( KAttrName, aAttributes );
    NcdProtocolUtils::AssignDesL(iRights->iName, name);

    }


void CNcdPreminetProtocolRightsParser::OnStartElementL( 
    const Xml::RTagInfo& aElement, 
    const Xml::RAttributeArray& aAttributes, 
    TInt aErrorCode) 
    {
    CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );

    DLTRACEIN(("rights start tag=%S error=%d depth=%d",
        &aElement.LocalName().DesC(),aErrorCode,iDepth));
    
    TPtrC8 tag( aElement.LocalName().DesC() );
    
    if ( iSubParser )
        {
        iSubParser->OnStartElementL( aElement, aAttributes, aErrorCode );
        }
    else if ( tag == KTagRightsObject && iRights )
        {
        TPtrC8 dataBlock = AttributeValue( KAttrDataBlock, aAttributes );
        NcdProtocolUtils::AssignDesL(iRights->iRightsObjectDataBlock, dataBlock);

        TPtrC8 mime = AttributeValue( KAttrMime, aAttributes );
        NcdProtocolUtils::AssignDesL(iRights->iRightsObjectMime, mime);
        
        // data saved in OnEndElementL
        }
    else
        {
        iSubParser = CNcdUnknownParser::NewL( 
            *iObservers, *this, iDepth+1, aElement, aAttributes );
        }
    }



void CNcdPreminetProtocolRightsParser::OnEndElementL(
    const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));

    TPtrC8 tag( aElement.LocalName().DesC() );
    if (iBuffer) 
        {
        if (tag == KTagRightsObject)
            {
            NcdProtocolUtils::AssignDesL(iRights->iRightsObjectData, 
                *NcdProtocolUtils::DecodeBase64L(*iBuffer) );
            }
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


void CNcdPreminetProtocolRightsParser::SubParserFinishedL( const TDesC8& aTag, 
                                                              TInt /*aErrorCode*/ )
    {
    DLTRACE(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning

    delete iSubParser;
    iSubParser = 0;
    }

MNcdPreminetProtocolRights*
CNcdPreminetProtocolRightsParser::Rights()
    {
    DLTRACEIN((""));
    MNcdPreminetProtocolRights* rights = iRights;
    iRights = 0;
    return rights;
    }

