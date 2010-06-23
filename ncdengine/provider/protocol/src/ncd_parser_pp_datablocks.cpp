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

#include "ncd_parser_pp_datablocks.h"
#include "ncd_pp_datablockimpl.h"

#include "catalogsdebug.h"

CNcdPreminetProtocolDataBlocksParser* 
CNcdPreminetProtocolDataBlocksParser::NewL( MNcdParserObserverBundle& aObservers,
                                            MNcdSubParserObserver& aSubParserObserver,
                                            TInt aDepth,
                                            const Xml::RTagInfo& aElement,
                                            const Xml::RAttributeArray& aAttributes )
    {
    CNcdPreminetProtocolDataBlocksParser* self 
        = new(ELeave) CNcdPreminetProtocolDataBlocksParser( aObservers,
                                                            aSubParserObserver,
                                                            aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolDataBlocksParser::CNcdPreminetProtocolDataBlocksParser( MNcdParserObserverBundle& aObservers,
                                                                            MNcdSubParserObserver& aSubParserObserver,
                                                                            TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth )
    {
    }

CNcdPreminetProtocolDataBlocksParser::~CNcdPreminetProtocolDataBlocksParser()
    {
    if( iDataBlocks )
        {
        iDataBlocks->ResetAndDestroy();
        }
    delete iDataBlocks;
    iDataBlocks = 0;

    delete iCurrentDataBlock;
    iCurrentDataBlock = 0;
    }

void CNcdPreminetProtocolDataBlocksParser::ConstructL( const Xml::RTagInfo& aElement,
                                                       const Xml::RAttributeArray& /*aAttributes*/ )
    {
    CNcdSubParser::ConstructL( aElement );    
    iDataBlocks = new(ELeave) CArrayPtrFlat<MNcdPreminetProtocolDataBlock>( 4 );
    }

void CNcdPreminetProtocolDataBlocksParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                                             const Xml::RAttributeArray& aAttributes, 
                                                             TInt aErrorCode ) 
    {
    CNcdSubParser::OnStartElementL( aElement, aAttributes, aErrorCode );
    
    TPtrC8 tag( aElement.LocalName().DesC() );
    DLTRACE(("dataBlocks tag=%S error=%d depth=%d",&aElement.LocalName().DesC(),aErrorCode,iDepth));

    if( iSubParser )
        {
        // Use existing subparser.
        iSubParser->OnStartElementL( aElement, aAttributes, aErrorCode );
        }
    else if( tag == KTagDataBlock )
        {
        iCurrentDataBlock = CNcdPreminetProtocolDataBlockImpl::NewL();

        // data block attributes
        // id
        NcdProtocolUtils::AssignDesL( iCurrentDataBlock->iId, AttributeValue( KAttrId, aAttributes ) );
        // name space
        NcdProtocolUtils::AssignDesL( iCurrentDataBlock->iNameSpace, AttributeValue( KAttrNamespace, aAttributes ) );

        }
    else
        {
        iSubParser = CNcdUnknownParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
        }
    }


void CNcdPreminetProtocolDataBlocksParser::OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));
    
    TPtrC8 tag( aElement.LocalName().DesC() );
    DLTRACE(("end tag=%S",&aElement.LocalName().DesC()));

    if( iSubParser == 0 && iTag && *iTag == aElement.LocalName().DesC() )
        {
        DLTRACE(("end tag=%S",&aElement.LocalName().DesC()));
        iSubParserObserver->SubParserFinishedL( aElement.LocalName().DesC(), aErrorCode );
        }
    
    else if( iSubParser )
        {
        iSubParser->OnEndElementL( aElement, aErrorCode );
        }
    
    else if( tag == KTagDataBlock )
        {
        if( iBuffer )
            {
            iCurrentDataBlock->iContent = NcdProtocolUtils::DecodeBase64L( *iBuffer );
            }
        iDataBlocks->AppendL( iCurrentDataBlock );
        iCurrentDataBlock = 0;
        }

    else
        {
        DLWARNING(("end tag ignored, tag=%S",&aElement.LocalName().DesC()));
        }
    }

CArrayPtr<MNcdPreminetProtocolDataBlock>* CNcdPreminetProtocolDataBlocksParser::DataBlocks()
{
    CArrayPtr<MNcdPreminetProtocolDataBlock>* returnPtr = iDataBlocks;
    iDataBlocks = 0;
    return returnPtr;
}

void CNcdPreminetProtocolDataBlocksParser::SubParserFinishedL( const TDesC8& aTag, 
                                                                TInt /*aErrorCode*/ )
    {
    DLTRACE(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning

    delete iSubParser;
    iSubParser = 0;
    }


