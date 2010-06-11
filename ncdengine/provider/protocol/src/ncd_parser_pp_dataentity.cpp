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


#include "ncd_parser_pp_dataentity.h"
#include "ncd_parser_pp_dataentitycontent.h"
#include "ncd_parser_pp_purchaseoption.h"
#include "ncd_parser_pp_download.h"
#include "ncd_parser_cp_query.h"
#include "ncd_parser_pp_moreinfo.h"
#include "ncd_pp_dataentity.h"
#include "ncd_pp_dataentityimpl.h"
#include "ncd_pp_iconimpl.h"
#include "ncd_pp_skinimpl.h"
#include "ncd_pp_download.h"
#include "ncd_cp_query.h"
#include "ncdparserobserverbundleimpl.h"
#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncdprotocoltypes.h"
#include "ncd_parser_cp_details.h"

#include "catalogsdebug.h"


CNcdPreminetProtocolDataEntityParser* 
CNcdPreminetProtocolDataEntityParser::NewL( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth,
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& aNamespace )
    {
    CNcdPreminetProtocolDataEntityParser* self 
        = new(ELeave) CNcdPreminetProtocolDataEntityParser( 
            aObservers, aSubParserObserver, aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes, aNamespace );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolDataEntityParser::CNcdPreminetProtocolDataEntityParser( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {

    }

CNcdPreminetProtocolDataEntityParser::~CNcdPreminetProtocolDataEntityParser()
    {
    delete iDataEntity;
    delete iNamespace;
    }

void CNcdPreminetProtocolDataEntityParser::ConstructL( 
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& aNamespace )
    {
    CNcdSubParser::ConstructL( aElement );
    NcdProtocolUtils::AssignDesL( iNamespace, aNamespace );

    DLTRACEIN(("depth=%d tag=%S",iDepth,iTag));

    if (!iDataEntity)
        {
        iDataEntity = CNcdPreminetProtocolDataEntityImpl::NewL();
        }

    // read attributes here
    TPtrC8 ns = AttributeValue( KAttrNamespace, aAttributes );
    TPtrC8 id = AttributeValue( KAttrId, aAttributes );
    TPtrC8 timestamp = AttributeValue( KAttrTimestamp, aAttributes );
    TPtrC8 type = AttributeValue( KAttrXsiType, aAttributes );
    if( type == KNullDesC8 )
        {
        type.Set( AttributeValue( KAttrType, aAttributes ) );
        }

    if( ns == KNullDesC8 )
        {
        // Namespace was not set, use the response's namespace.
        NcdProtocolUtils::AssignDesL( iDataEntity->iNamespace, *iNamespace );
        DLINFO((_L("Using default namespace %S"),iNamespace));
        }
    else
        {
        NcdProtocolUtils::AssignDesL(iDataEntity->iNamespace, ns);
        DLINFO(("Metadata namespace %S",&ns));
        }
    // Set the server where this entity was received
    NcdProtocolUtils::AssignDesL( iDataEntity->iServerUri, iObservers->SessionOrigin() );
    NcdProtocolUtils::AssignDesL( iDataEntity->iId, id );
    NcdProtocolUtils::AssignDesL( iDataEntity->iTimestamp, timestamp );
    
    if ( type == KValueItemData || type == KNullDesC8 ) 
        {
        iDataEntity->iType = EItemEntity;
        DLINFO(("item entity"));
        }
    else if ( type == KValueFolderData ) 
        {
        iDataEntity->iType = EFolderEntity;
        DLINFO(("folder entity"));
        }
    }


void CNcdPreminetProtocolDataEntityParser::OnStartElementL( 
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
        else if( tag == KTagIcon )
            {
            iDataEntity->iIcon = CNcdPreminetProtocolIcon::NewL();
            TPtrC8 id = AttributeValue( KAttrId, aAttributes );
            NcdProtocolUtils::AssignDesL( iDataEntity->iIcon->iId, id );
            TPtrC8 dataBlock = AttributeValue( KAttrDataBlock, aAttributes );
            NcdProtocolUtils::AssignDesL( iDataEntity->iIcon->iDataBlock, dataBlock );
            TPtrC8 uri = AttributeValue( KAttrUri, aAttributes );
            NcdProtocolUtils::AssignDesL( iDataEntity->iIcon->iUri, uri );
            // icon data can also be received inline
            }
        else if( tag == KTagPurchaseOptions )
            {
            // skip this tag
            }
        else if( tag == KTagPurchaseOption )
            {
            iSubParser = CNcdPreminetProtocolPurchaseOptionParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes,
                KNullDesC /*no parent id*/ );
            }
        else if( tag == KTagDisclaimer )
            {
            iSubParser = CNcdConfigurationProtocolQueryParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        else if( /*tag == KTagHelpInformation ||*/
                 tag == KTagScreenshot ||
                 tag == KTagPreview )
            {
            iSubParser = CNcdPreminetProtocolDownloadParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes, KNullDesC );
            }
        else if( tag == KTagSkin )
            {
            iDataEntity->iSkin = CNcdPreminetProtocolSkin::NewL();
            NcdProtocolUtils::AssignDesL( 
                iDataEntity->iSkin->iId, AttributeValue( KAttrId, aAttributes ) );
            NcdProtocolUtils::AssignDesL( 
                iDataEntity->iSkin->iUri, AttributeValue( KAttrUri, aAttributes ) );
            NcdProtocolUtils::AssignDesL( 
                iDataEntity->iSkin->iTimestamp, AttributeValue( KAttrTimestamp, aAttributes ) );
            }
        else if( tag == KTagLayout )
            {
            TPtrC8 layoutType = AttributeValue( KAttrType, aAttributes );
            NcdProtocolUtils::AssignDesL( iDataEntity->iLayoutType, layoutType);
            }
        else if( tag == KTagDownloadableContent ||
                    tag == KTagSubscribableContent )
            {
            iSubParser = CNcdPreminetProtocolDataEntityContentParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes, KNullDesC );
            }
        else if( tag == KTagMoreInfo )
            {
            iSubParser = CNcdPreminetProtocolMoreInfoParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        else if( tag == KTagDetails )
            {
            iSubParser = CNcdConfigurationProtocolDetailsParser::NewL( 
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



void CNcdPreminetProtocolDataEntityParser::OnEndElementL(
    const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {

    TPtrC8 tag( aElement.LocalName().DesC() );
    DLTRACEIN((""));
    //DLTRACEIN(("%X end iTag=%S tag=%S", this, iTag, &tag));

    if ( iBuffer )
        {
        if ( tag == KTagName ) 
            {
            //DLINFO(("name=%S",iBuffer));
            NcdProtocolUtils::AssignDesL(iDataEntity->iName, *iBuffer);
            }
        else if ( tag == KTagDescription )
            {
            //DLINFO(("description=%S",iBuffer));
            NcdProtocolUtils::AssignDesL(iDataEntity->iDescription, *iBuffer);
            }
        else if ( tag == KTagIcon && iDataEntity->iIcon )
            {
            //DLINFO(("icon data=%S",iBuffer));
            
            NcdProtocolUtils::AssignDesL(iDataEntity->iIcon->iData, 
                *NcdProtocolUtils::DecodeBase64L(*iBuffer) );
            }
        delete iBuffer;
        iBuffer = 0;
        }

    if ( iSubParser == 0 && iTag && *iTag == aElement.LocalName().DesC() )
        {
        // dataEntity end
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
void CNcdPreminetProtocolDataEntityParser::SubParserFinishedL( const TDesC8& aTag, 
                                                              TInt /*aErrorCode*/ )
    {
    DLTRACEIN(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning

    if ( iDataEntity ) 
        {
        
        if( iSubParser->Tag() == KTagPurchaseOption )
            {
            MNcdPreminetProtocolPurchaseOption* purchaseOption =
                static_cast<CNcdPreminetProtocolPurchaseOptionParser*>( iSubParser )->PurchaseOption();
            DLINFO(("subparser purchaseoption=%X",purchaseOption));
            if( purchaseOption )
                {
                iDataEntity->iPurchaseOptions.Append(purchaseOption);
                }
            }
    //    else if( iSubParser->Tag() == KTagHelpInformation )
    //        {
    //        MNcdPreminetProtocolDownload* help =
    //            static_cast<CNcdPreminetProtocolDownloadParser*>
    //            ( iSubParser )->Download();
    //        DLINFO(("subparser helpInformation=%X",help));
    //        if( help && iDataEntity )
    //            {
    //            iDataEntity->iHelpInformation = help;
    //            }
    //        }
        else if( iSubParser->Tag() == KTagPreview )
            {
            MNcdPreminetProtocolDownload* preview =
                static_cast<CNcdPreminetProtocolDownloadParser*>
                ( iSubParser )->Download();
            //DLINFO(("subparser preview=%X",preview));
            if( preview )
                {
                iDataEntity->iPreviews.AppendL( preview );
                }
            }
        else if( iSubParser->Tag() == KTagScreenshot )
            {
            MNcdPreminetProtocolDownload* screenshot =
                static_cast<CNcdPreminetProtocolDownloadParser*>
                ( iSubParser )->Download();
            //DLINFO(("subparser screenshot=%X",screenshot));
            if( screenshot )
                {
                iDataEntity->iScreenshots.AppendL( screenshot );
                }
            }
        else if( iSubParser->Tag() == KTagDisclaimer )
            {
            MNcdConfigurationProtocolQuery* disclaimer =
                static_cast<CNcdConfigurationProtocolQueryParser*>
                ( iSubParser )->Query();
            //DLINFO(("subparser disclaimer=%X",disclaimer));
            if( disclaimer )
                {
                iDataEntity->iDisclaimer = disclaimer;
                }
            }
        else if( iSubParser->Tag() == KTagDownloadableContent )
            {
            MNcdPreminetProtocolDataEntityContent* content =
                static_cast<CNcdPreminetProtocolDataEntityContentParser*>
                ( iSubParser )->Content();
            //DLINFO(("subparser downloadableContent=%X",content));
            if( content )
                {
                DASSERT( !iDataEntity->iDownloadableContent );
                iDataEntity->iDownloadableContent = content;
                }
            }
        else if( iSubParser->Tag() == KTagSubscribableContent )
            {
            MNcdPreminetProtocolDataEntityContent* content =
                static_cast<CNcdPreminetProtocolDataEntityContentParser*>
                ( iSubParser )->Content();
            //DLINFO(("subparser subscribableContent=%X",content));
            if( content )
                {
                iDataEntity->iSubscribableContent = content;
                }
            }
        else if( iSubParser->Tag() == KTagMoreInfo )
            {
            MNcdConfigurationProtocolQuery* moreInfo =
                static_cast<CNcdPreminetProtocolMoreInfoParser*>
                ( iSubParser )->MoreInfo();
            if( moreInfo )
                {
                iDataEntity->iMoreInfo = moreInfo;
                }
            }
        else if( iSubParser->Tag() == KTagDetails )
            {
            CArrayPtr<MNcdConfigurationProtocolDetail>* details =
                static_cast<CNcdConfigurationProtocolDetailsParser*>
                ( iSubParser )->Details();
            if( details )
                {
                iDataEntity->iDetails = details;
                }
            }
        else
            {
            //DLWARNING(("Child entity %S ignored", &aTag));
            }
        }
    delete iSubParser;
    iSubParser = 0;
    }

MNcdPreminetProtocolDataEntity* 
CNcdPreminetProtocolDataEntityParser::DataEntity()
    {
    DLTRACEIN(("%X",iDataEntity));
    MNcdPreminetProtocolDataEntity* entity = iDataEntity;
    iDataEntity = 0;
    return entity;
    }
