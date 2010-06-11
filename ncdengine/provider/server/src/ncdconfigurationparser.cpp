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
* Description:   CNcdConfigurationParser implementation
*
*/


#include "ncdprotocolutils.h"
#include "ncdconfigurationparser.h"
#include "ncdparserobserver.h"
#include "catalogsdebug.h"
#include "ncdprotocolutils.h"

_LIT8( KXmlType, "text/xml" );



// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdConfigurationParser* CNcdConfigurationParser::NewL( 
        MNcdConfigurationParserObserver& aObserver )
    {
    CNcdConfigurationParser* self = CNcdConfigurationParser::NewLC( 
        aObserver );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdConfigurationParser* CNcdConfigurationParser::NewLC( 
        MNcdConfigurationParserObserver& aObserver )
    {
    CNcdConfigurationParser* self = new(ELeave) CNcdConfigurationParser(
        aObserver );
    CleanupStack::PushL( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdConfigurationParser::CNcdConfigurationParser( 
    MNcdConfigurationParserObserver& aObserver )
    : iObserver( aObserver )  
    {
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdConfigurationParser::~CNcdConfigurationParser()
    {
    delete iXmlParser;
    iXmlParser = NULL;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdConfigurationParser::ParseL( const TDesC16& aData )
    {
    DLTRACEIN(("16-bit parse, length=%d",aData.Length()));
    HBufC8* utf8 = NcdProtocolUtils::ConvertUnicodeToUtf8L( aData );
    CleanupStack::PushL( utf8 );
    ParseL( *utf8 );
    CleanupStack::PopAndDestroy( utf8 );
    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdConfigurationParser::ParseL( const TDesC8& aData )
    {
    DLTRACEIN((""));
    iXmlParser = Xml::CParser::NewL( KXmlType, *this );
    iXmlParser->ParseBeginL();

    iXmlParser->ParseL( aData );
    iXmlParser->ParseEndL(); 
    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdConfigurationParser::OnStartDocumentL(
    const Xml::RDocumentParameters& /*aDocParam*/, TInt aErrorCode) 
    {
    DLTRACE(("error=%d",aErrorCode));
    (void) aErrorCode; // suppresses compiler warning
//     iDocParam.Open( aDocParam.CharacterSetName() );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdConfigurationParser::OnEndDocumentL(TInt /*aErrorCode*/)
    {
//    DLTRACE(("tag=%S error=%d",iTag,aErrorCode));
//     iDocParam.Close();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdConfigurationParser::OnStartElementL( 
    const Xml::RTagInfo& aElement, 
    const Xml::RAttributeArray& aAttributes, 
    TInt aErrorCode )
    {
    DLTRACE(("Start tag: %S, error=%d", 
        &aElement.LocalName().DesC(), aErrorCode ));
    (void) aErrorCode; // suppresses compiler warning
    
    iObserver.ConfigurationElementStartL( aElement.LocalName().DesC() );
    
    DLTRACE(("Handling %d attributes", aAttributes.Count() ));
    for ( TInt i = 0; i < aAttributes.Count(); ++i ) 
        {
        iObserver.ConfigurationAttributeL( aElement.LocalName().DesC(),
            aAttributes[i].Attribute().LocalName().DesC(),
            aAttributes[i].Value().DesC() );
        }
    
    delete iBuffer;
    iBuffer = NULL;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdConfigurationParser::OnEndElementL(
    const Xml::RTagInfo& aElement,
    TInt /* aErrorCode */ )
    {
    DLTRACEIN((""));
    if ( iBuffer ) 
        {
        DLTRACE(("Handling element data"));
        iObserver.ConfigurationElementEndL( aElement.LocalName().DesC(),
            *iBuffer );
        delete iBuffer;
        iBuffer = NULL;
        }    
    else
        {
        DLTRACE(("Handling empty element data"));
        iObserver.ConfigurationElementEndL( aElement.LocalName().DesC(),
            KNullDesC8 );

        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdConfigurationParser::OnContentL( 
    const TDesC8& aBytes, 
    TInt /*aErrorCode*/ )
    { 
    DLTRACEIN(( "content size=%d",aBytes.Length() ));
//     if( aBytes.Length() < 1000 )
//         {
//         DLINFODUMP(aBytes.Ptr(),aBytes.Length(),1024) ;
//         DLINFO(("data=%S",&aBytes));
//         }
    if ( iBuffer ) 
        {
        TInt oldLength = iBuffer->Length();
        iBuffer = iBuffer->ReAllocL( oldLength + aBytes.Length() );
        }
    else 
        {
        iBuffer = HBufC8::NewL( aBytes.Length() );
        }
        
    iBuffer->Des().Append( aBytes );
    DLTRACEOUT(( "buffer=%X size=%d", this, iBuffer->Length() ));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdConfigurationParser::OnStartPrefixMappingL( 
    const RString& /*aPrefix*/, const RString& /*aUri*/, 
    TInt aErrorCode ) 
    {
    DLTRACE(("error=%d",aErrorCode));
    (void) aErrorCode; // suppresses compiler warning
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdConfigurationParser::OnEndPrefixMappingL(
    const RString& /*aPrefix*/, TInt aErrorCode ) 
    {
    DLTRACE(("error=%d",aErrorCode));
    (void) aErrorCode; // suppresses compiler warning
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdConfigurationParser::OnIgnorableWhiteSpaceL(
    const TDesC8& /*aBytes*/, TInt aErrorCode ) 
    {
    DLTRACE(("error=%d",aErrorCode));
    (void) aErrorCode; // suppresses compiler warning
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdConfigurationParser::OnSkippedEntityL( 
    const RString& /*aName*/, TInt aErrorCode )
    {
    DLTRACE(("error=%d",aErrorCode));
    (void) aErrorCode; // suppresses compiler warning
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdConfigurationParser::OnProcessingInstructionL(
    const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt aErrorCode ) 
    {
    DLTRACE(("error=%d",aErrorCode));
    (void) aErrorCode; // suppresses compiler warning
    }


// When the CParser gives OnError(), it will not continue even if further
// data is fed to it.
void CNcdConfigurationParser::OnError(TInt aErrorCode)
    {
    DLTRACE(("error=%d",aErrorCode));
    iObserver.ConfigurationError( aErrorCode );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TAny* CNcdConfigurationParser::GetExtendedInterface(const TInt32 /*aUid*/) 
    {
    DLTRACE((""));
    return 0;
    }
