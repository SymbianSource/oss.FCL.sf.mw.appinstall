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


#include "ncd_parser_pp_download.h"
#include "ncd_parser_pp_rights.h"
#include "ncd_parser_cp_query.h"
#include "ncd_pp_download.h"
#include "ncd_pp_downloadimpl.h"
#include "ncd_pp_descriptor.h"
#include "ncd_pp_descriptorimpl.h"
#include "ncd_cp_query.h"
#include "ncd_cp_queryimpl.h"
#include "ncdparserobserverbundleimpl.h"
#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncdprotocoltypes.h"

#include "catalogsdebug.h"


CNcdPreminetProtocolDownloadParser* CNcdPreminetProtocolDownloadParser::NewL( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth,
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& aParentId )
    {
    CNcdPreminetProtocolDownloadParser* self 
        = new(ELeave) CNcdPreminetProtocolDownloadParser( 
            aObservers, aSubParserObserver, aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes, aParentId );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolDownloadParser::CNcdPreminetProtocolDownloadParser( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {

    }

CNcdPreminetProtocolDownloadParser::~CNcdPreminetProtocolDownloadParser()
    {
    delete iDownload;
    }

void CNcdPreminetProtocolDownloadParser::ConstructL( 
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& /*aParentId*/ )
    {
    CNcdSubParser::ConstructL( aElement );

    DLTRACEIN(("depth=%d tag=%S",iDepth,iTag));
    //DLINFO((_L("parent=%S"),&aParentId));

    if (!iDownload)
        {
        iDownload = CNcdPreminetProtocolDownloadImpl::NewL();
        }

    DLTRACE(("Download created"));
    // read attributes here
    TPtrC8 uri = AttributeValue( KAttrUri, aAttributes );
    NcdProtocolUtils::AssignDesL(iDownload->iUri, uri);

    DLTRACE((""));
    TPtrC8 autoDownload = AttributeValue( KAttrAutoDownload, aAttributes );
    if ( autoDownload != KNullDesC8 )
        NcdProtocolUtils::DesToBool(iDownload->iAutoDownload, autoDownload);
    
    DLTRACE((""));

    TPtrC8 delayDelta = AttributeValue( KAttrDelayDelta, aAttributes );
    if ( delayDelta != KNullDesC8 )
        iDownload->iDelayDelta = NcdProtocolUtils::DesDecToIntL(delayDelta);
    DLTRACE((""));

    TPtrC8 validityDelta = AttributeValue( KAttrValidityDelta, aAttributes );
    if ( validityDelta != KNullDesC8 )
        iDownload->iValidityDelta = NcdProtocolUtils::DesDecToIntL(validityDelta);
    DLTRACE((""));

    TPtrC8 chunkSize = AttributeValue( KAttrChunkSize, aAttributes );
    if ( chunkSize != KNullDesC8 )
        iDownload->iChunkSize = NcdProtocolUtils::DesDecToIntL(chunkSize);
    DLTRACE((""));

    TPtrC8 launchable = AttributeValue( KAttrLaunchable, aAttributes );
    if ( launchable != KNullDesC8 )
        NcdProtocolUtils::DesToBool(iDownload->iLaunchable, launchable);
    DLTRACE((""));

    TPtrC8 installNotificationUri = AttributeValue( KAttrInstallNotificationUri, aAttributes );
    NcdProtocolUtils::AssignDesL(iDownload->iInstallNotificationUri, installNotificationUri);

    DLTRACE((""));
    TPtrC8 size = AttributeValue( KAttrSize, aAttributes );
    if ( size != KNullDesC8 )
        iDownload->iSize = NcdProtocolUtils::DesDecToIntL(size);
    DLTRACE((""));

    TPtrC8 mime = AttributeValue( KAttrMime, aAttributes );
    NcdProtocolUtils::AssignDesL(iDownload->iMime, mime);
    DLTRACE((""));
    TPtrC8 reDownloadable = AttributeValue( KAttrReDownloadable, aAttributes );
    NcdProtocolUtils::DesToBool(iDownload->iReDownloadable, reDownloadable);
    DLTRACE((""));
    TPtrC8 previewType = AttributeValue( KAttrType, aAttributes );
    NcdProtocolUtils::AssignDesL(iDownload->iPreviewType, previewType);

    DLTRACE((""));
    TPtrC8 target = AttributeValue( KAttrTarget, aAttributes );
    if ( target == KValueConsumable ) 
        {
        iDownload->iTarget = EDownloadTargetConsumable;
        }
    else if ( target == KValueDownloadable ) 
        {
        iDownload->iTarget = EDownloadTargetDownloadable;
        }
    DLTRACE((""));
    TPtrC8 deliveryMethod = AttributeValue( KAttrDeliveryMethod, aAttributes );
    if ( deliveryMethod == KValueWlan ) 
        {
        iDownload->iDeliveryMethod = EUriDeliveryWlan;
        }
    else if ( deliveryMethod == KValueGprs ) 
        {
        iDownload->iDeliveryMethod = EUriDeliveryGprs;
        }
    else if ( deliveryMethod == KValueBroadband ) 
        {
        iDownload->iDeliveryMethod = EUriDeliveryBroadband;
        }
    DLTRACEOUT((""));
    }


void CNcdPreminetProtocolDownloadParser::OnStartElementL( 
    const Xml::RTagInfo& aElement, 
    const Xml::RAttributeArray& aAttributes, 
    TInt aErrorCode) 
    {
    CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );

    DLTRACEIN(("download start tag=%S error=%d depth=%d",
        &aElement.LocalName().DesC(),aErrorCode,iDepth));
    
    TPtrC8 tag( aElement.LocalName().DesC() );
    
    if( iSubParser == 0 )
        {
        if ( tag == KTagMessage )
            {
            iSubParser = CNcdConfigurationProtocolQueryParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        else if ( tag == KTagRightsDetails )
            {
            iSubParser = CNcdPreminetProtocolRightsParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes, KNullDesC );
            }
        else if ( tag == KTagDescriptor )
            {
            CNcdPreminetProtocolDescriptorImpl* descriptor = 
                CNcdPreminetProtocolDescriptorImpl::NewL();
            TPtrC8 type = AttributeValue( KAttrType, aAttributes );
            NcdProtocolUtils::AssignDesL(descriptor->iType, type);
            TPtrC8 name = AttributeValue( KAttrName, aAttributes );
            NcdProtocolUtils::AssignDesL(descriptor->iName, name);
            TPtrC8 uri = AttributeValue( KAttrUri, aAttributes );
            NcdProtocolUtils::AssignDesL(descriptor->iUri, uri);
            iDownload->iDescriptor = descriptor;
            // data saved in OnEndElementL
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



void CNcdPreminetProtocolDownloadParser::OnEndElementL(
    const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));
    TPtrC8 tag( aElement.LocalName().DesC() );

    if ( iBuffer )
        {
        if ( tag == KTagDescriptor && iDownload && iDownload->iDescriptor ) 
            {
            NcdProtocolUtils::AssignDesL(iDownload->iDescriptor->iData, 
                *NcdProtocolUtils::DecodeBase64L(*iBuffer));
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


void CNcdPreminetProtocolDownloadParser::SubParserFinishedL( const TDesC8& aTag, 
                                                              TInt /*aErrorCode*/ )
    {
    DLTRACEIN(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning

    // : child entity has been finished. store it as a 
    // a childen to the node this parser is parsing.
    // Needs nodemanager.
    if( iSubParser->Tag() == KTagMessage )
        {
        // A subentity is now ready. Report it.
        MNcdConfigurationProtocolQuery* message =
            static_cast<CNcdConfigurationProtocolQueryParser*>( iSubParser )->Query();
        DLINFO(("subparser message=%X",message));
        if( message && iDownload )
            {
            iDownload->iMessage = message;
            }
        }
    else if( iSubParser->Tag() == KTagRightsDetails )
        {
        // A subentity is now ready. Report it.
        MNcdPreminetProtocolRights* rights =
            static_cast<CNcdPreminetProtocolRightsParser*>( iSubParser )->Rights();
        DLINFO(("subparser rights=%X",rights));
        if( rights && iDownload )
            {
            iDownload->iRights = rights;
            }
        }

    delete iSubParser;
    iSubParser = 0;
    }

MNcdPreminetProtocolDownload*
CNcdPreminetProtocolDownloadParser::Download()
    {
    DLTRACEIN((""));
    MNcdPreminetProtocolDownload* download = iDownload;
    iDownload = 0;
    return download;
    }

