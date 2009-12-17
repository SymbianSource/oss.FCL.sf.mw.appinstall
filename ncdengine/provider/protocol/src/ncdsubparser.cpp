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
* Description:   CNcdSubParser implementation
*
*/


#include "ncdprotocolutils.h"
#include "ncdsubparser.h"
#include "ncdparserobserver.h"
#include "catalogsdebug.h"
#include "ncdprotocolutils.h"

CNcdSubParser::CNcdSubParser( MNcdParserObserverBundle& aObservers,
                              MNcdSubParserObserver& aSubParserObserver,
                              TInt aDepth, TType aType )
    : iType( aType ),
      iDepth( aDepth ),
      iObservers( &aObservers ),
      iSubParserObserver( &aSubParserObserver )
    {
    }

CNcdSubParser::~CNcdSubParser()
    {
    delete iSubParser;
    delete iTag;
    delete iBuffer;
    }

void CNcdSubParser::ConstructL( const Xml::RTagInfo& aElement )
    {
    NcdProtocolUtils::AssignDesL( iTag, aElement.LocalName().DesC() );
    }

const TDesC8& CNcdSubParser::Tag() const
    {
    if( iTag )
        {
        return *iTag;
        }
    else
        {
        return KNullDesC8();
        }
    }

TInt CNcdSubParser::Depth() const
    {
    return iDepth;
    }

CNcdSubParser::TType CNcdSubParser::Type() const
    {
    return iType;
    }

void CNcdSubParser::OnStartDocumentL(
    const Xml::RDocumentParameters& /*aDocParam*/, TInt aErrorCode) 
    {
    DLTRACE(("error=%d",aErrorCode));
    (void) aErrorCode; // suppresses compiler warning
//     iDocParam.Open( aDocParam.CharacterSetName() );
    }

void CNcdSubParser::OnEndDocumentL(TInt /*aErrorCode*/)
    {
//    DLTRACE(("tag=%S error=%d",iTag,aErrorCode));
//     iDocParam.Close();
    }


void CNcdSubParser::OnStartElementL( 
    const Xml::RTagInfo& /*aElement*/, 
    const Xml::RAttributeArray& /*aAttributes*/, 
    TInt aErrorCode )
    {
    DLTRACE(("error=%d",aErrorCode));
    (void) aErrorCode; // suppresses compiler warning
    delete iBuffer;
    iBuffer = 0;
    }

void CNcdSubParser::OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode)
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

void CNcdSubParser::SubParserFinishedL( const TDesC8& aTag, TInt /*aErrorCode*/ )
    {
    DLTRACE(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning
    delete iSubParser;
    iSubParser = 0;
    }

void CNcdSubParser::OnContentL(const TDesC8& aBytes, TInt aErrorCode )
    { 
    DLTRACEIN(("content size=%d",aBytes.Length()));
//     if( aBytes.Length() < 1000 )
//         {
//         DLINFODUMP(aBytes.Ptr(),aBytes.Length(),1024) ;
//         DLINFO(("data=%S",&aBytes));
//         }
    if( iSubParser )
        {
        iSubParser->OnContentL( aBytes, aErrorCode );
        }
    
    else
        {
        if (iBuffer) 
            {
            TInt oldLength = iBuffer->Length();
            iBuffer = iBuffer->ReAllocL(oldLength + aBytes.Length());
            }
        else 
            {
            iBuffer = HBufC8::NewL(aBytes.Length());
            }
        iBuffer->Des().Append(aBytes);
        DLINFO(("buffer=%X size=%d",this,iBuffer->Length()));
        }
    DLTRACEOUT((""));
    }

void CNcdSubParser::OnStartPrefixMappingL( 
    const RString& /*aPrefix*/, const RString& /*aUri*/, 
    TInt aErrorCode) 
    {
    DLTRACE(("error=%d",aErrorCode));
    (void) aErrorCode; // suppresses compiler warning
    }

void CNcdSubParser::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt aErrorCode) 
    {
    DLTRACE(("error=%d",aErrorCode));
    (void) aErrorCode; // suppresses compiler warning
    }

void CNcdSubParser::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt aErrorCode) 
    {
    DLTRACE(("error=%d",aErrorCode));
    (void) aErrorCode; // suppresses compiler warning
    }

void CNcdSubParser::OnSkippedEntityL(const RString& /*aName*/, TInt aErrorCode)
    {
    DLTRACE(("error=%d",aErrorCode));
    (void) aErrorCode; // suppresses compiler warning
    }


void CNcdSubParser::OnProcessingInstructionL(const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, 
                                             TInt aErrorCode) 
    {
    DLTRACE(("error=%d",aErrorCode));
    (void) aErrorCode; // suppresses compiler warning
    }


// When the CParser gives OnError(), it will not continue even if further
// data is fed to it.
void CNcdSubParser::OnError(TInt aErrorCode)
    {
    DLTRACE(("error=%d",aErrorCode));
    iObservers->ParserObserver()->ParseError( aErrorCode );
    }


TAny* CNcdSubParser::GetExtendedInterface(const TInt32 /*aUid*/) 
    {
    DLTRACE((""));
    return 0;
    }



const TDesC8& CNcdSubParser::AttributeValue( const TDesC8& aAttributeName,
                                             const Xml::RAttributeArray& aAttributes )
    {
    for( TInt i = 0; i < aAttributes.Count(); i++ )
        {
        if( aAttributes[i].Attribute().LocalName().DesC() == aAttributeName )
            {
            return aAttributes[i].Value().DesC();
            }
        }
    return KNullDesC8;
    }

const TDesC8& CNcdSubParser::AttributePrefix( const TDesC8& aAttributeName,
                                              const Xml::RAttributeArray& aAttributes )
    {
    for( TInt i = 0; i < aAttributes.Count(); i++ )
        {
        if( aAttributes[i].Attribute().LocalName().DesC() == aAttributeName )
            {
            return aAttributes[i].Attribute().Prefix().DesC();
            }
        }
    return KNullDesC8;
    }

