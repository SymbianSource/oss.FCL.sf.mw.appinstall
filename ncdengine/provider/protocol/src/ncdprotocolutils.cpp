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
* Description:  
*
*/


#include <utf.h>

#include "ncdprotocolutils.h"


_LIT8(KTrueString8, "true");
_LIT8(KFalseString8, "false");
_LIT16(KTrueString16, "true");
_LIT16(KFalseString16, "false");

// This circumvents an API "feature" and adds the XML prefix to the
// name itself. The API changed (AFAIK) so that a certain parameter
// set caused the prefix to be ignored.

#define XML_PREFIX_HACK
#warning XML DOM API PREFIX HACK ENABLED ON 3.2 AND 5.0 PLATFORMS



void NcdProtocolUtils::AssignEmptyDesL( HBufC16*& aDes )
    {
    delete aDes;
    aDes = 0;
    aDes = KNullDesC16().AllocL();
    }

void NcdProtocolUtils::AssignEmptyDesL( HBufC8*& aDes )
    {
    delete aDes;
    aDes = 0;
    aDes = KNullDesC8().AllocL();
    }


void NcdProtocolUtils::AssignDesL( HBufC16*& aDes, const TDesC16& aSource )
    {
    delete aDes;
    aDes = 0;
    aDes = aSource.AllocL();
    }

void NcdProtocolUtils::AssignDesL( HBufC16*& aDes, const TDesC8& aSource )
    {
    delete aDes;
    aDes = 0;
    aDes = ConvertUtf8ToUnicodeL( aSource );
    }

void NcdProtocolUtils::AssignDesL( HBufC8*& aDes, const TDesC8& aSource )
    {
    delete aDes;
    aDes = 0;
    aDes = aSource.AllocL();
    }

HBufC16* NcdProtocolUtils::ConvertUtf8ToUnicodeLC( const TDesC8& aUtfText )
    {
    HBufC16* t = ConvertUtf8ToUnicodeL( aUtfText );
    CleanupStack::PushL( t );
    return t;
    }

HBufC16* NcdProtocolUtils::ConvertUtf8ToUnicodeL( const TDesC8& aUtfText )
    {
    HBufC16* buffer = HBufC16::NewLC( aUtfText.Length() );
    TPtr ptr( buffer->Des() );
    User::LeaveIfError( 
        CnvUtfConverter::ConvertToUnicodeFromUtf8( ptr, aUtfText ) );
    CleanupStack::Pop( buffer );
    return buffer;
    }

HBufC8* NcdProtocolUtils::ConvertUnicodeToUtf8L( const TDesC16& aUnicodeText )
    {
    const TInt KConvertBufferSize = 32;

    // Place converted data here, initial size double the conversion buffer.
    HBufC8* convertedData = HBufC8::NewL( KConvertBufferSize*2 );
    CleanupStack::PushL( convertedData );
    TPtr8 destination( convertedData->Des() );

    // Create a small output buffer
    TBuf8<KConvertBufferSize> outputBuffer;
    // Create a buffer for the unconverted text - initialised with the input text
    TPtrC16 remainderOfUnicodeText( aUnicodeText );

    for ( ;; ) // conversion loop
        {
        // Start conversion. When the output buffer is full, return the 
        // number of characters that were not converted
        const TInt returnValue
            = CnvUtfConverter::ConvertFromUnicodeToUtf8( outputBuffer, 
                                                        remainderOfUnicodeText );

        // check to see that the descriptor isn’t corrupt - leave if it is
        if ( returnValue==CnvUtfConverter::EErrorIllFormedInput )
            User::Leave( KErrCorrupt );
        else if ( returnValue<0 ) // future-proof against "TError" expanding
            User::Leave( KErrGeneral );

        // Do something here to store the contents of the output buffer.
        if ( destination.Length() + outputBuffer.Length() >= destination.MaxLength() )
            {
            HBufC8* newBuffer = convertedData->ReAllocL(
                ( destination.MaxLength() + outputBuffer.Length() ) * 2 );
            CleanupStack::Pop( convertedData );
            convertedData = newBuffer;
            CleanupStack::PushL( convertedData );
            destination.Set( convertedData->Des() );
            }

        destination.Append( outputBuffer );
        outputBuffer.Zero();

        // Finish conversion if there are no unconverted characters in the remainder buffer
        if ( returnValue==0 )
            break; 

        // Remove the converted source text from the remainder buffer.
        // The remainder buffer is then fed back into loop
        remainderOfUnicodeText.Set( remainderOfUnicodeText.Right( returnValue));
        }
    CleanupStack::Pop( convertedData );
    return convertedData;
    }

TBool NcdProtocolUtils::IsWhitespace( const TDesC8& aBytes )
    {
    for( TInt i = 0; i < aBytes.Length(); i++ )
        {
        if( ! TChar( aBytes[i] ).IsSpace() )
            {
            return EFalse;
            }
        }
    return ETrue;
    }


TInt NcdProtocolUtils::DesDecToIntL( const TDesC8& aDes )
    {
    TLex8 lex( aDes );
    TInt value;
    TInt error = lex.Val( value );
    if( error != KErrNone )
        {
        DLERROR(("Conversion error %d, %S",error,&aDes));
        User::Leave( error );
        }
    return value;
    }


TInt NcdProtocolUtils::DesDecToIntL( const TDesC16& aDes )
    {
    TLex16 lex( aDes );
    TInt value;
    TInt error = lex.Val( value );
    if( error != KErrNone )
        {
        DLERROR((_L("Conversion error %d, %S"),error,&aDes));
        User::Leave( error );
        }
    return value;
    }


TInt NcdProtocolUtils::DesDecToInt( const TDesC8& aDes, TInt& aValue )
    {
    if ( aDes != KNullDesC8 ) 
        {
        aValue = 0;
        TLex8 lex( aDes );
        return lex.Val( aValue );
        }
    return KErrNone;
    }

TInt NcdProtocolUtils::DesDecToInt( const TDesC16& aDes, TInt& aValue )
    {
    if ( aDes != KNullDesC16 ) 
        {
        aValue = 0;
        TLex16 lex( aDes );
        return lex.Val( aValue );
        }
    return KErrNone;
    }

TReal32 NcdProtocolUtils::DesDecToRealL( const TDesC8& aDes )
    {
    DLTRACEIN(("des8 to real32: %S", &aDes));
    if ( aDes != KNullDesC8 ) 
        {
        TReal32 value = 0;
        TLex8 lex( aDes );
        TInt error = lex.Val(value);
        DLINFO(("real32 value=%f, error=%d", value, error));
        User::LeaveIfError(error);
        return value;
        }
    return 0;
    }




void NcdProtocolUtils::DesToNcdBool( TNcdBool& aBool, const TDesC8& aDes)
    {
    if ( aDes == KTrueString8 ) 
        {
        aBool = EValueTrue;
        }
    else if ( aDes == KFalseString8 )
        {
        aBool = EValueFalse;
        }
    else
        {
        aBool = EValueNotSet;
        }
    }

void NcdProtocolUtils::DesToNcdBool( TNcdBool& aBool, const TDesC16& aDes )
    {
    if ( aDes == KTrueString16 ) 
        {
        aBool = EValueTrue;
        }
    else if ( aDes == KFalseString16 )
        {
        aBool = EValueFalse;
        }
    else
        {
        aBool = EValueNotSet;
        }
    }

void NcdProtocolUtils::DesToBool( TBool& aBool, const TDesC8& aDes )
    {
    if ( aDes == KTrueString8 )
        {
        aBool = ETrue;
        }    
    else if ( aDes == KFalseString8 )
        {
        aBool = EFalse;
        }
    }

void NcdProtocolUtils::DesToBool( TBool& aBool, const TDesC16& aDes )
    { 
    if ( aDes == KTrueString16 )
        {
        aBool = ETrue;
        }    
    else if ( aDes == KFalseString16 )
        {
        aBool = EFalse;
        }
    }

const TDesC8& NcdProtocolUtils::BoolToDes( TBool aValue )
    {
    return aValue ? KTrueString8() : KFalseString8();
    }

const TDesC8& NcdProtocolUtils::NcdBoolToDes( TNcdBool aValue )
    {
    switch( aValue )
        {
        case EValueNotSet:
            return KNullDesC8();

        case EValueTrue:
            return KTrueString8();

        case EValueFalse:
            return KFalseString8();

        default:
            DASSERT( EFalse );
            return KNullDesC8();
        };
    }



void NcdProtocolUtils::BoolToStringL(TXmlEngString& aString, const TBool aBool) 
    {
    DLTRACEIN((""));
    aBool ? (aString.SetL(KTrueString8)) : (aString.SetL(KFalseString8));
    }

void NcdProtocolUtils::NcdBoolToStringL(TXmlEngString& aString, const TNcdBool aBool) 
    {
    DLTRACEIN((""));
    if ( aBool == EValueTrue ) 
        {
        aString.SetL(KTrueString8);
        }
    else if ( aBool == EValueFalse ) 
        {
        aString.SetL(KFalseString8);
        }
    }
    
void NcdProtocolUtils::DesToStringL(TXmlEngString& aString, const TDesC8& aValue) 
    {
    if ( aValue != KNullDesC8 ) 
        {
        aString.SetL( aValue );
        }
    else 
        {
        aString.Free();
        aString = TXmlEngString();
        }
    }

void NcdProtocolUtils::DesToStringL(TXmlEngString& aString, const TDesC& aValue) 
    {
    if ( aValue != KNullDesC ) 
        {
        aString.SetL( aValue );
        }
    else 
        {
        aString.Free();
        aString = TXmlEngString();
        }
    }

void NcdProtocolUtils::IntToStringL(TXmlEngString& aString, TInt aValue) 
    {
    TBuf8<256> buf;
    buf.AppendNum(aValue);
    aString.SetL(buf);
    }    


void NcdProtocolUtils::NewBoolAttributeL(
    TXmlEngElement& aParent, const TDesC8& aKey, const TBool aValue)
    {
    DLTRACEIN((""));
    if ( aKey != KNullDesC8 ) 
        {
        aParent.SetAttributeL( aKey, NcdProtocolUtils::BoolToDes( aValue ) );
        }
    }

void NcdProtocolUtils::NewNcdBoolAttributeL(
    TXmlEngElement& aParent, const TDesC8& aKey, const TNcdBool aValue)
    {
    DLTRACEIN((""));
    if ( aKey != KNullDesC8 && aValue != EValueNotSet ) 
        {
        aParent.SetAttributeL( aKey, NcdProtocolUtils::NcdBoolToDes( aValue ) );
        }
    }

void NcdProtocolUtils::NewAttributeL(
    TXmlEngElement& aParent, const TDesC8& aKey, const TInt aValue)
    {
    TBuf8<32> buf;
    buf.Num( aValue );
    aParent.SetAttributeL( aKey, buf );
    }

void NcdProtocolUtils::NewAttributeL(
    TXmlEngElement& aParent, const TDesC8& aKey, const TDesC8& aValue)
    {
    if ( aKey != KNullDesC8 && aValue != KNullDesC8 ) 
        {
        aParent.SetAttributeL( aKey, aValue );
        }
    }

void NcdProtocolUtils::NewAttributeL(
    TXmlEngElement& aParent, const TDesC8& aKey, const TDesC& aValue)
    {
    if ( aKey != KNullDesC8 && aValue != KNullDesC ) 
        {
        HBufC8* utf8 = NcdProtocolUtils::ConvertUnicodeToUtf8L( aValue );
        CleanupStack::PushL( utf8 );
        aParent.SetAttributeL( aKey, *utf8 );
        CleanupStack::PopAndDestroy( utf8 );
        }
    }

void NcdProtocolUtils::NewAttributeL(
    TXmlEngElement& aParent, const TDesC8& aKey, const TXmlEngString& aValue)
    {
    if ( aKey != KNullDesC8 && aValue.NotNull() ) 
        {
        aParent.SetAttributeL( aKey, aValue.PtrC8() );
        }
    }

TXmlEngElement NcdProtocolUtils::NewElementL( RXmlEngDocument& aDocument,
    TXmlEngElement& aParent, const TDesC8& aName, const TDesC8& aPrefix)
    {
    DLTRACEIN(("1 aName=%S aPrefix=%S",&aName,&aPrefix ));
#ifndef RD_XML_ENGINE_API_CHANGE
    TXmlEngString name;
    NcdProtocolUtils::DesToStringL(name, aName);
    if (aPrefix != KNullDesC8)
        {
        TXmlEngString prefix;
        NcdProtocolUtils::DesToStringL(prefix, aPrefix);
        TXmlEngElement element = aDocument.CreateElementL(name, NULL, prefix);
        aParent.AppendChildL(element);
        if ( name.NotNull() ) name.Free();
        if ( prefix.NotNull() ) prefix.Free();
        return element;
        }
    else 
        {
        TXmlEngElement element = aDocument.CreateElementL(name);
        aParent.AppendChildL(element);
        if ( name.NotNull() ) name.Free();
        return element;
        }
#else
#ifdef XML_PREFIX_HACK
    HBufC8* t = HBufC8::NewLC( aName.Length() + aPrefix.Length() + 1 );
    if( aPrefix != KNullDesC8 )
        {
        t->Des().Append( aPrefix );
        t->Des().Append( ':' );
        }
    t->Des().Append( aName );
    TXmlEngElement element = aDocument.CreateElementL( *t, KNullDesC8, KNullDesC8 );
    DLINFO(("PREFIX HACK DONE 1 %S",t));
    CleanupStack::PopAndDestroy( t );
#else
    TXmlEngElement element = aDocument.CreateElementL( aName, KNullDesC8, aPrefix );
#endif
    aParent.AppendChildL(element);
    return element;
#endif
    }

TXmlEngElement NcdProtocolUtils::NewElementL( RXmlEngDocument& aDocument,
    TXmlEngElement& aParent, const TDesC8& aName, const TXmlEngString& aPrefix )
    {
    DLTRACEIN(("2 aName=%S",&aName ));
#ifndef RD_XML_ENGINE_API_CHANGE
    TXmlEngString name;
    NcdProtocolUtils::DesToStringL(name, aName);
    if (aPrefix.NotNull())
        {
        TNamespace ns = aParent.LookupNamespaceByPrefix(aPrefix);
        TXmlEngElement element = aDocument.CreateElementL(name, NULL, aPrefix);
        aParent.AppendChildL(element);
        if ( name.NotNull() ) name.Free();
        return element;
        }
    else 
        {
        TXmlEngElement element = aDocument.CreateElementL(name);
        aParent.AppendChildL(element);
        if ( name.NotNull() ) name.Free();
        return element;
        }
#else
    return NcdProtocolUtils::NewElementL( aDocument, aParent, aName, aPrefix.PtrC8() );
#endif
    }

TXmlEngElement NcdProtocolUtils::NewElementL(
    RXmlEngDocument& aDocument, const TDesC8& aName, const TDesC8& aPrefix )
    {
    DLTRACEIN(("3 aName=%S aPrefix=%S",&aName,&aPrefix ));
#ifndef RD_XML_ENGINE_API_CHANGE
    TXmlEngString name;
    NcdProtocolUtils::DesToStringL(name, aName);
    if (aPrefix != KNullDesC8)
        {
        TXmlEngString prefix;
        NcdProtocolUtils::DesToStringL(prefix, aPrefix);
        TXmlEngElement element = aDocument.CreateElementL(name, NULL, prefix);
        if ( name.NotNull() ) name.Free();
        if ( prefix.NotNull() ) prefix.Free();
        return element;
        }
    else 
        {
        TXmlEngElement element = aDocument.CreateElementL(name);
        if ( name.NotNull() ) name.Free();
        return element;
        }
#else

#ifdef XML_PREFIX_HACK
    HBufC8* t = HBufC8::NewLC( aName.Length() + aPrefix.Length() + 1 );
    if( aPrefix != KNullDesC8 )
        {
        t->Des().Append( aPrefix );
        t->Des().Append( ':' );
        }
    t->Des().Append( aName );
    TXmlEngElement element = aDocument.CreateElementL( *t, KNullDesC8, KNullDesC8 );
    DLINFO(("PREFIX HACK DONE 2 %S",t));
    CleanupStack::PopAndDestroy( t );
    return element;
#else
    return aDocument.CreateElementL( aName, KNullDesC8, aPrefix );
#endif

#endif
    }

TXmlEngElement NcdProtocolUtils::NewElementL(
    RXmlEngDocument& aDocument, const TDesC8& aName, const TXmlEngString& aPrefix )
    {
    DLTRACEIN(("4 aName=%S",&aName ));
#ifndef RD_XML_ENGINE_API_CHANGE
    TXmlEngString name;
    NcdProtocolUtils::DesToStringL(name, aName);
    if (aPrefix.NotNull())
        {
        TXmlEngElement element = aDocument.CreateElementL(name, NULL, aPrefix);
        if ( name.NotNull() ) name.Free();
        return element;
        }
    else 
        {
        TXmlEngElement element = aDocument.CreateElementL(name);
        if ( name.NotNull() ) name.Free();
        return element;
        }
#else

#ifdef XML_PREFIX_HACK
    TPtrC8 prefix = aPrefix.PtrC8();
    HBufC8* t = HBufC8::NewLC( aName.Length() + prefix.Length() + 1 );
    if( prefix != KNullDesC8 )
        { 
        t->Des().Append( prefix );
        t->Des().Append( ':' );
        }
    t->Des().Append( aName );
    TXmlEngElement element = aDocument.CreateElementL( *t, KNullDesC8, KNullDesC8 );
    DLINFO(("PREFIX HACK DONE 3 %S",t));
    CleanupStack::PopAndDestroy( t );
    return element;
#else
    return aDocument.CreateElementL( aName, KNullDesC8, aPrefix.PtrC8() );
#endif



#endif
    }

HBufC8* NcdProtocolUtils::DecodeBase64LC( const TDesC8& aData )
    {
    TBuf8<1> temp;
    TInt decodedLength = DecodeBase64L( aData, temp, ETrue );

    HBufC8* buffer = HBufC8::NewLC( decodedLength );
    TPtr8 ptr( buffer->Des() );
    DecodeBase64L( aData, ptr, EFalse );
    return buffer;
    }

HBufC8* NcdProtocolUtils::DecodeBase64L( const TDesC8& aData )
    {
    HBufC8* b = DecodeBase64LC( aData );
    CleanupStack::Pop( b );
    return b;
    }

TInt NcdProtocolUtils::DecodeBase64L( const TDesC8& aData, 
                                      TDes8& aOutput,
                                      TBool aCalculateLength )
    {
    TInt i = 0;
    TInt len = aData.Length();
    TInt outPosition = 0;
        
    for ( ;; ) 
        {
        // Skip whitespace
        while ( i < len && aData[i] <= ' ' )
            {
            i++;
            }
            
        // End of data reached
        if ( i == len )
            {
            break;
            }

        TInt tri = 
            ( DecodeBase64L( aData[i] ) << 18 )
            + ( DecodeBase64L( aData[i + 1] ) << 12 )
            + ( DecodeBase64L( aData[i + 2] ) << 6 )
            + ( DecodeBase64L( aData[i + 3] ) );

        if ( ! aCalculateLength )
            {
            aOutput.Append( TUint8( ( tri >> 16 ) & 255 ) );
            }
        outPosition++;;

        if ( aData[i + 2] == '=' )
            {
            break;
            }

        if ( ! aCalculateLength )
            {
            aOutput.Append( TUint8( ( tri >> 8 ) & 255 ) );
            }
        outPosition++;

        if ( aData[i + 3] == '=' )
            {
            break;
            }
 
        if ( ! aCalculateLength )
            {
            aOutput.Append( TUint8( tri & 255 ) );
            }
        outPosition++;
        i += 4;
        }
    return outPosition;
    }

TInt NcdProtocolUtils::DecodeBase64L( TInt c ) 
    {
    if ( c >= 'A' && c <= 'Z' )
        {
        return c - 65;
        }
    else if ( c >= 'a' && c <= 'z' )
        {
        return c - 97 + 26;
        }
    else if ( c >= '0' && c <= '9' )
        { 
        return c - 48 + 26 + 26;
        }
    else
        {
        switch ( c ) 
            {
            case '+':
                return 62;
            case '/':
                return 63;
            case '=':
                return 0;
            default:
                DLERROR(("Conversion error %d",c));
                User::Leave( KErrCorrupt );
                return 0; // to suppress compiler warning
            }
        }
    }


TTime NcdProtocolUtils::DesToTimeL(const TDesC8& aDes) 
    {
    if (aDes == KNullDesC8)
        {
        return TTime(0);
        }

    // Date example: 2006-05-17T09:30:47.0Z
    // Month and day in TTime's date format begin from 0.

    // Read date components    
    TInt year = DesDecToIntL( aDes.Mid( 0,4 ) );
    TMonth month = TMonth(DesDecToIntL( aDes.Mid( 5,2 ) ) -1 ); // -1 needed
    TInt day = DesDecToIntL( aDes.Mid( 8,2 ) ) -1; // -1 needed
    TInt hour = DesDecToIntL( aDes.Mid( 11,2 ) );
    TInt minute = DesDecToIntL( aDes.Mid( 14,2 ) );
    TInt second = DesDecToIntL( aDes.Mid( 17,2 ) );

    // create TDateTime object from components
    // (no need to use format strings this way)
    TDateTime date;
    date.Set(year, month, day, hour, minute, second, 0);

    // create TTime object from TDateTime
    TTime time = TTime(date);
    return time;
    }
    
TTime NcdProtocolUtils::DesToTimeL(const TDesC16& aDes) 
    {
    if (aDes == KNullDesC16)
        {
        return TTime(0);
        }

    // Date example: 2006-05-17T09:30:47.0Z
    // Month and day in TTime's date format begin from 0.

    // Read date components    
    TInt year = DesDecToIntL( aDes.Mid( 0,4 ) );
    TMonth month = TMonth(DesDecToIntL( aDes.Mid( 5,2 ) ) -1 ); // -1 needed
    TInt day = DesDecToIntL( aDes.Mid( 8,2 ) ) -1; // -1 needed
    TInt hour = DesDecToIntL( aDes.Mid( 11,2 ) );
    TInt minute = DesDecToIntL( aDes.Mid( 14,2 ) );
    TInt second = DesDecToIntL( aDes.Mid( 17,2 ) );

    // create TDateTime object from components
    // (no need to use format strings this way)
    TDateTime date;
    date.Set(year, month, day, hour, minute, second, 0);

    // create TTime object from TDateTime
    TTime time = TTime(date);
    return time;
    }
    
void NcdProtocolUtils::TimeToDesL(const TTime aTime, HBufC16*& aDes)
    {
    TBuf<24> buf;
    _LIT(KFormat, "%F%Y-%M-%DT%H:%T:%S.0Y");
    aTime.FormatL(buf, KFormat);
    aDes = buf.AllocL();
    }

// void NcdProtocolUtils::DumpInfo( const Xml::RTagInfo& aElement,
//                                  const Xml::RAttributeArray& aAttributes )
//     {
//     DLINFO(("localname %S",&aElement.LocalName().DesC()));
//     DLINFO(("      uri %S",&aElement.Uri().DesC()));
//     DLINFO(("   prefix %S",&aElement.Prefix().DesC()));
//     for( TInt i = 0; i < aAttributes.Count(); i++ )
//         {
//         DLINFO(("-attribute %d",i));
//         DLINFO(("-      uri %S",  &aAttributes[i].Attribute().Uri().DesC() ));
//         DLINFO(("-localname %S",  &aAttributes[i].Attribute().LocalName().DesC() ));
//         DLINFO(("-   prefix %S",  &aAttributes[i].Attribute().Prefix().DesC() ));
//         DLINFO(("-    value %S",  &aAttributes[i].Value().DesC() ));
//         DLINFO(("-     type %d",  aAttributes[i].Type() ));
//         }
//     }

