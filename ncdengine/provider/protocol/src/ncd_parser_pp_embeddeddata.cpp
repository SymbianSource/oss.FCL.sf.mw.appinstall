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


#include "ncd_parser_pp_embeddeddata.h"
#include "ncd_parser_pp_dataentity.h"
#include "ncd_parser_cp_query.h"
#include "ncd_pp_dataentity.h"
#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncdparserobserver.h"
#include "catalogsdebug.h"
#include "ncd_parser_pp_datablocks.h"

CNcdPreminetProtocolEmbeddedDataParser* 
CNcdPreminetProtocolEmbeddedDataParser::NewL( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth,
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    const TDesC& aNamespace )
    {
    CNcdPreminetProtocolEmbeddedDataParser* self 
        = new(ELeave) CNcdPreminetProtocolEmbeddedDataParser( 
            aObservers, aSubParserObserver, aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes, aNamespace );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolEmbeddedDataParser::CNcdPreminetProtocolEmbeddedDataParser( 
    MNcdParserObserverBundle& aObservers,
    MNcdSubParserObserver& aSubParserObserver,
    TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {

    }

CNcdPreminetProtocolEmbeddedDataParser::~CNcdPreminetProtocolEmbeddedDataParser()
    {
    delete iNamespace;
    }

void CNcdPreminetProtocolEmbeddedDataParser::ConstructL( 
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& /*aAttributes*/,
    const TDesC& aNamespace )
    {
    CNcdSubParser::ConstructL( aElement );
    DLTRACEIN(("depth=%d tag=%S",iDepth,iTag));
    DLINFO((_L("namespace=%S"),&aNamespace));
    NcdProtocolUtils::AssignDesL( iNamespace, aNamespace );
    }


void CNcdPreminetProtocolEmbeddedDataParser::OnStartElementL( 
    const Xml::RTagInfo& aElement, 
    const Xml::RAttributeArray& aAttributes, 
    TInt aErrorCode) 
    {
    CNcdSubParser::OnStartElementL(aElement, aAttributes, aErrorCode);

    TPtrC8 tag( aElement.LocalName().DesC() );

    if( iSubParser == 0 )
        {
        DLTRACEIN(("embeddedData start tag=%S error=%d depth=%d",
                   &aElement.LocalName().DesC(), aErrorCode, iDepth));

        if( tag == KTagEntities || tag == KTagQueries || 
            tag == KTagActivities || tag == KTagEmbeddedSession )
            {
            // these are handled in this same parser
            }
        else if( tag == KTagDataEntity || tag == KTagActionEntity )
            {
            iSubParser = CNcdPreminetProtocolDataEntityParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes,
                *iNamespace );
            }
        else if( tag == KTagQuery )
            {
            iSubParser = CNcdConfigurationProtocolQueryParser::NewL( 
                *iObservers, *this, iDepth+1, aElement, aAttributes );
            }
        else if( tag == KTagDataBlocks )
            {
            iSubParser = CNcdPreminetProtocolDataBlocksParser::NewL( 
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


void CNcdPreminetProtocolEmbeddedDataParser::OnEndElementL(
    const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));

    if( iSubParser == 0 && iTag && *iTag == aElement.LocalName().DesC() )
        {
        DLTRACE(("end tag=%S",&aElement.LocalName().DesC()));
        // Should store finished entity, or let the parent do it with some function?
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


void CNcdPreminetProtocolEmbeddedDataParser::SubParserFinishedL( 
    const TDesC8& aTag, TInt /*aErrorCode*/ )
    {
    DLTRACEIN(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning

    // : child entity has been finished. store it as a 
    // a childen to the node this parser is parsing.
    // Needs nodemanager.
    if( iSubParser->Tag() == KTagDataEntity || iSubParser->Tag() == KTagActionEntity )
        {
        // A subentity is now ready. Report it.
        if( iObservers->EntityObserver())
            {
            MNcdPreminetProtocolDataEntity* dataEntity =
                static_cast<CNcdPreminetProtocolDataEntityParser*>
                ( iSubParser )->DataEntity();
            DLINFO(("subparser dataEntity=%X type=%d",dataEntity,dataEntity->Type()));
            if ( dataEntity && dataEntity->Type() == EItemEntity ) 
                {
                DLINFO(("sending itemData"));
                iObservers->EntityObserver()->ItemDataL(dataEntity);
                }
            else if (dataEntity &&  dataEntity->Type() == EFolderEntity) 
                {
                DLINFO(("sending folderData"));
                iObservers->EntityObserver()->FolderDataL(dataEntity);
                }
            }
        }
    else if( iSubParser->Tag() == KTagDataBlocks )
        {        
        if( iObservers->DataBlocksObserver())
            {
            CArrayPtr<MNcdPreminetProtocolDataBlock>* dataBlocks =
                static_cast<CNcdPreminetProtocolDataBlocksParser*>
                ( iSubParser )->DataBlocks();
            if( dataBlocks )
                {
                iObservers->DataBlocksObserver()->DataBlocksL( dataBlocks );
                }
            }
        }
    else if( iSubParser->Tag() == KTagQuery )
        {        
        if( iObservers->QueryObserver() )
            {            
            MNcdConfigurationProtocolQuery* query =
                static_cast<CNcdConfigurationProtocolQueryParser*>
                ( iSubParser )->Query();
            if( query )
                {
                iObservers->QueryObserver()->QueryL(query);
                }
            }
        }

    delete iSubParser;
    iSubParser = 0;
    }

void CNcdPreminetProtocolEmbeddedDataParser::OnContentL(
    const TDesC8& aBytes, TInt aErrorCode)
    { 
    if( iSubParser )
        {
        iSubParser->OnContentL( aBytes, aErrorCode );
        }
    }

