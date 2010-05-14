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


#include "catalogsutils.h"

#include <utf.h>
#include <e32base.h>
#include <f32file.h>
#include <bautils.h>
#include <s32strm.h>
#include <s32mem.h>
#include <sysutil.h>
#include <pathinfo.h> 

#ifndef __S60_32__
#include <platform/mw/languages.hrh>
#endif

#include "catalogsconstants.h"
#include "catalogsbasemessage.h"

#include "catalogsdebug.h"

// Maximum length of a signed 16 bit integer when converted to a string
const TInt K16BitIntAsDescLength = 6;

// Maximum length of a signed 32 bit integer when converted to a string
const TInt K32BitIntAsDescLength = 11;

_LIT( KInitialSearchDrive, "Y:" );

void TCatalogsVersion::ConvertL( TCatalogsVersion& aTarget, const TDesC& aVersion )
    {
    TUint16 major = 0;
    TUint16 minor = 0;
    TUint32 build = 0;
    
    if ( aVersion.Length() ) 
        {        
        DesToVersionL( aVersion, major, minor, build );
        }
    
    aTarget = TCatalogsVersion( major, minor, build );        
    }
    
    
HBufC* TCatalogsVersion::ConvertLC( const TCatalogsVersion& aSource )
    {
    DLTRACEIN(("input: %u.%u.%u", 
        aSource.iMajor,
        aSource.iMinor,
        aSource.iBuild ));
        
    _LIT( KFormatString, "%u.%u.%u");
    
    HBufC* target = HBufC::NewLC( 
        // Length of major + minor
        2 * K16BitIntAsDescLength +
        // length of build
        K32BitIntAsDescLength + 
        // separators: '.'
        2 );
        
    TPtr ptr( target->Des() );
    ptr.Format( 
        KFormatString, 
        aSource.iMajor, 
        aSource.iMinor, 
        aSource.iBuild );
        
    DLTRACEOUT(( _L("Output: %S"), target));    
    return target;
    }
    

TCatalogsVersion::TCatalogsVersion()
    {
    iMajor = 0;
    iMinor = 0;
    iBuild = 0;
    }
    

TCatalogsVersion::TCatalogsVersion( TUint16 aMajor,
                                    TUint16 aMinor,
                                    TUint32 aBuild )
    {
    iMajor = aMajor;
    iMinor = aMinor;
    iBuild = aBuild;
    }

TBool TCatalogsVersion::operator==( const TCatalogsVersion& aVersion ) const
    {
    return iMajor == aVersion.iMajor && 
           iMinor == aVersion.iMinor &&
           iBuild == aVersion.iBuild;
    }

TBool TCatalogsVersion::operator>( const TCatalogsVersion& aVersion ) const
    {
    return ( iMajor > aVersion.iMajor ) || 
        ( iMajor == aVersion.iMajor && ( iMinor > aVersion.iMinor ||
        ( iMinor == aVersion.iMinor && iBuild > aVersion.iBuild ) ) );
    }
    
TBool TCatalogsVersion::operator>=( const TCatalogsVersion& aVersion ) const
    {
    return operator==( aVersion ) || operator>( aVersion );
    }




#ifndef NCD_STORAGE_TESTING

// ---------------------------------------------------------------------------
// RCatalogsMessageReader
// ---------------------------------------------------------------------------
//    

RCatalogsMessageReader::RCatalogsMessageReader()
    {
    }

void RCatalogsMessageReader::OpenL( MCatalogsBaseMessage& aMessage )
    {
    iBuf.CreateL( aMessage.InputLength() );    
    User::LeaveIfError( aMessage.ReadInput( iBuf ) );
    iStream.Open( iBuf );
    }


void RCatalogsMessageReader::OpenLC( MCatalogsBaseMessage& aMessage )
    {
    CleanupClosePushL( *this );
    OpenL( aMessage );    
    }
    

void RCatalogsMessageReader::Close()
    {
    iStream.Close();
    iBuf.Close();        
    }
    
    
RCatalogsMessageReader::~RCatalogsMessageReader()
    {
    }

#endif

// ---------------------------------------------------------------------------
// RCatalogsBufferWriter
// ---------------------------------------------------------------------------
//    

RCatalogsBufferWriter::RCatalogsBufferWriter() : iBuf( NULL )
    {
    }

void RCatalogsBufferWriter::OpenL()
    {    
    iBuf = CBufFlat::NewL( KBufExpandSize );    
    iStream.Open( *iBuf );
    }


void RCatalogsBufferWriter::OpenLC()
    {
    CleanupClosePushL( *this );
    OpenL();
    }
    

void RCatalogsBufferWriter::Close()
    {
    iStream.Close();
    delete iBuf;
    iBuf = NULL;
    }
    
    
RCatalogsBufferWriter::~RCatalogsBufferWriter()
    {
    }


// ---------------------------------------------------------------------------
// Utility functions
// ---------------------------------------------------------------------------
//    
    


void AssignDesL( HBufC8*& aDes, const TDesC8& aSource )
    {
    delete aDes;
    aDes = 0;
    aDes = aSource.AllocL();
    }


void AssignDesL( HBufC16*& aDes, const TDesC16& aSource )
    {
    delete aDes;
    aDes = 0;
    aDes = aSource.AllocL();
    }

void AssignDesL( HBufC16*& aDes, const TDesC8& aSource )
    {
    delete aDes;
    aDes = 0;
    aDes = ConvertUtf8ToUnicodeL( aSource );
    }

HBufC16* ConvertUtf8ToUnicodeL( const TDesC8& aUtfText )
    {
    HBufC16* buffer = HBufC16::NewLC( aUtfText.Length() );
    TPtr ptr( buffer->Des() );
    User::LeaveIfError( 
        CnvUtfConverter::ConvertToUnicodeFromUtf8( ptr,
                                                   aUtfText ) );
    CleanupStack::Pop( buffer );
    return buffer;
    }


HBufC8* ConvertUnicodeToUtf8L( const TDesC16& aUnicodeText )
    {
    const TInt KConvertBufferSize = 32;

    // Place converted data here, initial size double the conversion buffer.
    HBufC8* convertedData = HBufC8::NewL( KConvertBufferSize * 2 );
    CleanupStack::PushL( convertedData );
    TPtr8 destination( convertedData->Des() );

    // Create a small output buffer
    TBuf8<KConvertBufferSize> outputBuffer;
    
    // Create a buffer for the unconverted text - initialised with the 
    // input text
    TPtrC16 remainderOfUnicodeText( aUnicodeText );

    for ( ;; ) // conversion loop
        {
        // Start conversion. When the output buffer is full, return the 
        // number of characters that were not converted
        const TInt returnValue
            = CnvUtfConverter::ConvertFromUnicodeToUtf8( outputBuffer, 
            remainderOfUnicodeText );

        // check to see that the descriptor isn’t corrupt - leave if it is
        if ( returnValue == CnvUtfConverter::EErrorIllFormedInput )
            {            
            User::Leave( KErrCorrupt );
            }
        else if ( returnValue < 0 ) // future-proof against "TError" expanding
            {            
            User::Leave( KErrGeneral );
            }

        // Do something here to store the contents of the output buffer.
        if ( destination.Length() + outputBuffer.Length() >= 
             destination.MaxLength() )
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

        // Finish conversion if there are no unconverted characters 
        // in the remainder buffer
        if ( returnValue == 0 ) 
            {            
            break; 
            }

        // Remove the converted source text from the remainder buffer.
        // The remainder buffer is then fed back into loop
        remainderOfUnicodeText.Set( 
            remainderOfUnicodeText.Right( returnValue) );
        }
        
    CleanupStack::Pop( convertedData );
    return convertedData;
    }


HBufC8* Des16ToDes8LC( const TDesC16& aDes )
    {
    HBufC8* buffer = HBufC8::NewLC( aDes.Length() );
    buffer->Des().Copy( aDes );
    return buffer;
    }
    
HBufC8* Des16ToDes8L( const TDesC16& aDes )
    {
    HBufC8* t = Des16ToDes8LC( aDes );
    CleanupStack::Pop();
    return t;
    }

HBufC16* Des8ToDes16LC( const TDesC8& aDes )
    {
    HBufC16* buffer = HBufC16::NewLC( aDes.Length() );
    buffer->Des().Copy( aDes );
    return buffer;
    }

HBufC16* Des8ToDes16L( const TDesC8& aDes )
    {
    HBufC16* t = Des8ToDes16LC( aDes );
    CleanupStack::Pop();
    return t;
    }    

TInt DesDecToIntL( const TDesC& aDes )
    {
    TLex lex( aDes );
    TInt value;
    User::LeaveIfError( lex.Val( value ) );
    return value;
    }

TInt DesDecToInt( const TDesC8& aDes, TInt& aValue )
    {
    TLex8 lex( aDes );
    return lex.Val( aValue );
    }

TInt DesDecToInt( const TDesC16& aDes, TInt& aValue )
    {
    TLex16 lex( aDes );
    return lex.Val( aValue );
    }

TInt DesHexToIntL( const TDesC& aDes )
    {
    TLex lex( aDes );
    TInt position = aDes.LocateF( 'x' );
    if ( position != KErrNotFound ) 
        {
        // Hex format is of type '0xABC' or 'xABC'
        // Skip over the x-part.
        lex.Assign( aDes.Mid( position+1 ) );
        }
    TUint value;
    User::LeaveIfError( lex.Val( value, EHex ) );
    return value;
    }

TInt Des8ToInt( const TDesC8& aDes )
    {
    return *(( TInt* )aDes.Ptr());
    }

TUint Des8ToUint( const TDesC8& aDes )
    {
    return *(( TUint* )aDes.Ptr());
    }


HBufC8* IntToDes8LC( TInt aInt )
    {
    HBufC8* buffer = HBufC8::NewMaxLC( sizeof( aInt ) );
    TPtr8 ptr( buffer->Des() );
    *(( TInt* )ptr.Ptr()) = aInt;
    return buffer;
    }

HBufC8* IntToDes8L( TInt aInt )
    {
    HBufC8* t = IntToDes8LC( aInt );
    CleanupStack::Pop();
    return t;
    }

HBufC8* UintToDes8LC( TUint aUint )
    {
    HBufC8* buffer = HBufC8::NewMaxLC( sizeof( aUint ) );
    TPtr8 ptr( buffer->Des() );
    *(( TUint* )ptr.Ptr()) = aUint;
    return buffer;
    }

HBufC8* UintToDes8L( TUint aUint )
    {
    HBufC8* t = UintToDes8LC( aUint );
    CleanupStack::Pop();
    return t;
    }


// ---------------------------------------------------------------------------
// ExternalizeDesL
// ---------------------------------------------------------------------------
//    
void ExternalizeDesL( const TDesC16& aDes, RWriteStream& aStream )
    {
    aStream.WriteInt32L( aDes.Length() );
    aStream.WriteL( aDes );
    }


// ---------------------------------------------------------------------------
// InternalizeDesL
// ---------------------------------------------------------------------------
//    
TInt InternalizeDesL( HBufC16*& aDes, RReadStream& aStream )
    {
    //DLTRACEIN(("16bit"));
    TInt length = aStream.ReadInt32L();
    //DLTRACE(("length: %i", length));
    if ( length > 0 ) 
        {
        HBufC* target = HBufC::NewLC( length );
        TPtr ptr( target->Des() );
        
        aStream.ReadL( ptr, length );
        delete aDes;
        aDes = target;
        CleanupStack::Pop( target );
        }
    else
        {
        delete aDes;
        aDes = KNullDesC16().AllocL();
        }
    //DLTRACEOUT((""));
    return length;
    }


// ---------------------------------------------------------------------------
// ExternalizeDesL
// ---------------------------------------------------------------------------
//    
void ExternalizeDesL( const TDesC8& aDes, RWriteStream& aStream )
    {
    aStream.WriteInt32L( aDes.Length() );
    aStream.WriteL( aDes );
    }


// ---------------------------------------------------------------------------
// InternalizeDesL
// ---------------------------------------------------------------------------
//    
TInt InternalizeDesL( HBufC8*& aDes, RReadStream& aStream )
    {
    //DLTRACEIN(("8bit"));
    TInt length = aStream.ReadInt32L();
    //DLTRACE(("length: %i", length));
    if ( length > 0 ) 
        {
        HBufC8* target = HBufC8::NewLC( length );
        TPtr8 ptr( target->Des() );
        
        aStream.ReadL( ptr, length );
        delete aDes;
        aDes = target;
        CleanupStack::Pop( target );
        }
    else
        {
        delete aDes;
        aDes = KNullDesC8().AllocL();
        }
    //DLTRACEOUT((""));
    return length;
    }



HBufC* FindEngineFileL( RFs& aFs, const TDesC& aFilename )
    {
    DLTRACEIN(( _L("Find: %S"), &aFilename ));
    // Get engine's private path
    RBuf target;
    CleanupClosePushL( target );
    target.CreateL( KMaxPath );
    User::LeaveIfError( aFs.PrivatePath( target ) );

    // Set Y: as the first drive to be searched instead of session
    // path's drive which is C:       
    target.Insert( 0, KInitialSearchDrive );
    
    DLTRACE(( _L("Path to search from: %S"), &target ));
    
    // Find the file
    TFindFile find( aFs );

    User::LeaveIfError( find.FindByDir( aFilename, target ) );
    
    CleanupStack::PopAndDestroy( &target );    
    DLTRACEOUT(( _L("Found the file from: %S"), 
        &find.File() ));
        
    return find.File().AllocL();    
    }
    

TBool IsRomDriveL( RFs& aFs, TChar aDriveChar )
    {   
    DLTRACEIN((""));
    TDriveInfo info;
    TInt drive;
    User::LeaveIfError( RFs::CharToDrive( aDriveChar, drive ) );
    User::LeaveIfError( aFs.Drive( info, drive ) );
    return info.iDriveAtt & KDriveAttRom;
    }


TChar DriveToCharL( TInt aDrive )
    {
    TChar c;
    User::LeaveIfError( RFs::DriveToChar( aDrive, c ) );
    return c;
    }


HBufC* DriveRootPathLC( RFs& aFs, const TChar& aDriveLetter )
    {
    DLTRACEIN((""));

    HBufC* rootPath( NULL );
        
    // Get the drive num that corresponds the given letter
    TInt driveNum( EDriveA );
    User::LeaveIfError( aFs.CharToDrive( aDriveLetter, driveNum ) );

    // Get the information about the drive
    TDriveInfo info;
    User::LeaveIfError( aFs.Drive( info, driveNum ) );

    // Compare drive information to check what is the correct root path
    if ( info.iDriveAtt & KDriveAttRom )
        {
        DLINFO(("ROM drive"));
        rootPath = PathInfo::RomRootPath().AllocLC();
        }
    else if ( info.iDriveAtt & KDriveAttRemovable )
        {
        DLINFO(("Memory card"));
        // Memory card
        rootPath = PathInfo::MemoryCardRootPath().AllocLC();
        }
    else
        {
        // Phone memory
        DLINFO(("Phone memory"));
        rootPath = PathInfo::PhoneMemoryRootPath().AllocLC();
        }

    HBufC* charDes( HBufC::NewLC( 1 ) );
    charDes->Des().Append( aDriveLetter );
    // Let us make sure that the drive letter is correct
    // if there are multiple same kind of drives...
    rootPath->Des().Replace( 0, 1, *charDes );
    CleanupStack::PopAndDestroy( charDes );

    DLTRACEOUT((""));
    return rootPath;
    }


HBufC* LangCodeToDescLC( TLanguage aLang )
    {
    switch ( aLang )
        {
        case ELangEnglish:
            return _L( "en_EN" ).AllocLC();
        case ELangFrench:
            return _L( "fr_FR" ).AllocLC();
        case ELangGerman: 
            return _L( "de_DE" ).AllocLC();
        case ELangSpanish:
            return _L( "es_ES" ).AllocLC();
        case ELangItalian: 
            return _L( "it_IT" ).AllocLC();
        case ELangSwedish: 
            return _L( "sv_SE" ).AllocLC();
        case ELangDanish: 
            return _L( "da_DK" ).AllocLC();
        case ELangNorwegian:
            return _L( "no_NO" ).AllocLC();
        case ELangFinnish:
            return _L( "fi_FI" ).AllocLC();
        case ELangAmerican:
            return _L( "en_US" ).AllocLC();
        case ELangSwissFrench:
            return _L( "fr_CH" ).AllocLC();
        case ELangSwissGerman:
            return _L( "de_CH" ).AllocLC();
        case ELangPortuguese:
            return _L( "pt_PT" ).AllocLC();
        case ELangTurkish:
            return _L( "tr_TR" ).AllocLC();
        case ELangIcelandic:
            return _L( "is_IS" ).AllocLC();
        case ELangRussian:
            return _L( "ru_RU" ).AllocLC();
        case ELangHungarian:
            return _L( "hu_HU" ).AllocLC();
        case ELangDutch:
            return _L( "nl_NL" ).AllocLC();
        case ELangBelgianFlemish:
            return _L( "nl_BE" ).AllocLC();
        case ELangAustralian:
            return _L( "en_AU" ).AllocLC();
        case ELangBelgianFrench:
            return _L( "fr_BE" ).AllocLC();
        case ELangAustrian:
            return _L( "de_AT" ).AllocLC();
        case ELangNewZealand:
            return _L( "en_NZ" ).AllocLC();
        case ELangInternationalFrench:
            return _L( "fr" ).AllocLC();
        case ELangCzech:
            return _L( "cs_CZ" ).AllocLC();
        case ELangSlovak:
            return _L( "sk_SK" ).AllocLC();
        case ELangPolish:
            return _L( "pl_PL" ).AllocLC();
        case ELangSlovenian:
            return _L( "sl_SI" ).AllocLC();
        case ELangTaiwanChinese:
            return _L( "zh_TW" ).AllocLC();
        case ELangHongKongChinese:
            return _L( "zh_HK" ).AllocLC();
        case ELangPrcChinese:
            return _L( "zh_CN" ).AllocLC();
        case ELangJapanese:
            return _L( "ja_JP" ).AllocLC();
        case ELangThai:
            return _L( "th_TH" ).AllocLC();
        case ELangAfrikaans:
            return _L( "af_ZA" ).AllocLC();
        case ELangAlbanian:
            return _L( "sq_AL" ).AllocLC();
        case ELangAmharic:
            return _L( "am_ET" ).AllocLC();
        case ELangArabic:
            return _L( "ar" ).AllocLC();
        case ELangArmenian:
            return _L( "hy_AM" ).AllocLC();
        case ELangTagalog:
            return _L( "tl" ).AllocLC();
        case ELangBelarussian:
            return _L( "be_BY" ).AllocLC();
        case ELangBengali:
            return _L( "bn_IN" ).AllocLC();
        case ELangBulgarian:
            return _L( "bg_BG" ).AllocLC();
        case ELangBurmese:
            return _L( "my_MM" ).AllocLC();
        case ELangCatalan:
            return _L( "ca_ES" ).AllocLC();
        case ELangCroatian:
            return _L( "hr_HR" ).AllocLC();
        case ELangCanadianEnglish:
            return _L( "en_CA" ).AllocLC();
        case ELangInternationalEnglish:
            return _L( "en" ).AllocLC();
        case ELangSouthAfricanEnglish:
            return _L( "en_ZA" ).AllocLC();
        case ELangEstonian:
            return _L( "et_EE" ).AllocLC();
        case ELangFarsi:
            return _L( "fa_IR" ).AllocLC();
        case ELangCanadianFrench:
            return _L( "fr_CA" ).AllocLC();
        case ELangScotsGaelic:
            return _L( "gd_GB" ).AllocLC();
        case ELangGeorgian:
            return _L( "ka_GE" ).AllocLC();
        case ELangGreek:
            return _L( "el_GR" ).AllocLC();
        case ELangCyprusGreek:
            return _L( "el_CY" ).AllocLC();
        case ELangGujarati:
            return _L( "gu_IN" ).AllocLC();
        case ELangHebrew:
            return _L( "he" ).AllocLC();
        case ELangHindi:
            return _L( "hi_IN" ).AllocLC();
        case ELangIndonesian:
            return _L( "id_ID" ).AllocLC();
        case ELangIrish:
            return _L( "ga_IE" ).AllocLC();
        case ELangSwissItalian:
            return _L( "it_CH" ).AllocLC();
        case ELangKannada:
            return _L( "kn_IN" ).AllocLC();
        case ELangKazakh:
            return _L( "kk_KZ" ).AllocLC();
        case ELangKhmer:
            return _L( "kh_KH" ).AllocLC();
        case ELangKorean:
            return _L( "ko_KR" ).AllocLC();
        case ELangLao:
            return _L( "lo_LA" ).AllocLC();
        case ELangLatvian:
            return _L( "lv_LV" ).AllocLC();
        case ELangLithuanian:
            return _L( "lt_LT" ).AllocLC();
        case ELangMacedonian:
            return _L( "mk_MK" ).AllocLC();
        case ELangMalay:
            return _L( "ms_MY" ).AllocLC();
        case ELangMalayalam:
            return _L( "ml_IN" ).AllocLC();
        case ELangMarathi:
            return _L( "mr_IN" ).AllocLC();
        case ELangMoldavian:
            return _L( "mo_MD" ).AllocLC();
        case ELangMongolian:
            return _L( "mn_MN" ).AllocLC();
        case ELangNorwegianNynorsk:
            return _L( "nn_NO" ).AllocLC();
        case ELangBrazilianPortuguese:
            return _L( "pt_BR" ).AllocLC();
        case ELangPunjabi:
            return _L( "pa_IN" ).AllocLC();
        case ELangRomanian:
            return _L( "ro_RO" ).AllocLC();
        case ELangSerbian:
            return _L( "sr" ).AllocLC();
        case ELangSinhalese:
            return _L( "si_LK" ).AllocLC();
        case ELangSomali:
            return _L( "so_SO" ).AllocLC();
        case ELangInternationalSpanish:
            return _L( "es" ).AllocLC();
        case ELangLatinAmericanSpanish:
            return _L( "esa" ).AllocLC();
        case ELangSwahili:
            return _L( "sw_KE" ).AllocLC();
        case ELangFinlandSwedish:
            return _L( "sv_FI" ).AllocLC();
        case ELangTamil:
            return _L( "ta_IN" ).AllocLC();
        case ELangTelugu:
            return _L( "te_IN" ).AllocLC();
        case ELangTibetan:
            return _L( "bo" ).AllocLC();
        case ELangTigrinya:
            return _L( "ti_ET" ).AllocLC();
        case ELangCyprusTurkish:
            return _L( "tr_CY" ).AllocLC();
        case ELangTurkmen:
            return _L( "tk_TM" ).AllocLC();
        case ELangUkrainian:
            return _L( "uk_UA" ).AllocLC();
        case ELangUrdu:
            return _L( "ur_PK" ).AllocLC();
        case ELangVietnamese:
            return _L( "vi_VN" ).AllocLC();
        case ELangWelsh:
            return _L( "cy_GB" ).AllocLC();
        case ELangZulu:
            return _L( "zu_ZA" ).AllocLC();  

#ifdef __S60_32__
            
        case 129: // APAC English = 129
            return _L( "en_APAC" ).AllocLC();
        case 157: // APAC TW (a.k.a Chinese TW English) = 157
            return _L( "en_TW" ).AllocLC();
        case 158: // APAC HK = 158
            return _L( "en_HK" ).AllocLC();
        case 159: // APAC PRC = 159
            return _L( "en_CN" ).AllocLC();
        case 160: // Japanese English = 160
            return _L( "en_JP" ).AllocLC();
        case 161: // Thai English = 161
            return _L( "en_TH" ).AllocLC();
        case 326: // Bahasa Malay (Chinese) = 326
            return _L( "ms_MY" ).AllocLC();
        case 401: // Basque
            return _L( "eu" ).AllocLC();
        case 402: // Galician
            return _L( "gl_ES" ).AllocLC();

#else // __S60_32__

        case KLangApacEnglish: // APAC English = 129
            return _L( "en_APAC" ).AllocLC();
        case KLangTaiwanEnglish: // APAC TW (a.k.a Chinese TW English) = 157
            return _L( "en_TW" ).AllocLC();
        case KLangHongKongEnglish: // APAC HK = 158
            return _L( "en_HK" ).AllocLC();
        case KLangPrcEnglish: // APAC PRC = 159
            return _L( "en_CN" ).AllocLC();
        case KLangJapaneseEnglish: // Japanese English = 160
            return _L( "en_JP" ).AllocLC();
        case KLangThaiEnglish: // Thai English = 161
            return _L( "en_TH" ).AllocLC();
        case KLangApacMalay: // Bahasa Malay (Chinese) = 326
            return _L( "ms_MY" ).AllocLC();
        case KLangBasque: // Basque = 102 on 5.0 platform
            return _L( "eu" ).AllocLC();
        case KLangGalician: // Galician = 103 on 5.0 platform
            return _L( "gl_ES" ).AllocLC();

#endif // __S60_32__
            
        default:
            return _L( "undef" ).AllocLC();
        }
    }


// ---------------------------------------------------------------------------
// CleanUidName
// ---------------------------------------------------------------------------
//    
TUidName CleanUidName( const TUid& aUid )
    {
    TUidName uidName( aUid.Name() );
    DASSERT( uidName.Length() > 2 );
    // Rip '[' and ']' out of uid name
    return uidName.Mid( 1, uidName.Length() - 2 );
    }    

// ---------------------------------------------------------------------------
// Encodes a filename 
// ---------------------------------------------------------------------------
// 
HBufC* EncodeFilenameLC( const TDesC& aFileName, RFs& aFs )
    {
    DLTRACEIN(( _L("Path: %S"), &aFileName ));
    // If each character gets replaced with a number, it would quadruple the
    // length: "abc" -> "xxxxyyyyzzzz". Multiply by 8 to be safe : )
    HBufC* encoded = HBufC::NewLC( aFileName.Length() * 8 );
    TPtr ptr( encoded->Des() );

    // First some basic legality checks.
    // It seems that RFs::IsValidName() acts very strangely in some cases
    // ( returns 0x20 as badChar even though there is no such char in the desc )
    // so it's better to do as much as we can by ourselves.

    for ( TInt i = 0; i < aFileName.Length(); i++ )
        {
        TChar c = aFileName[i];
        switch ( c )
            {
            // List partly from TNameChecker
            case '"':
            case '*':
            case '/':
            case ':':
            case '<':
            case '>':
            case '?':
            case '\'':
            case '\000':
            case '\\':
            case '|':
            case '.':
                {
                TBuf<8> num;
                num.NumUC( c, EHex );
                ptr.Append( num );
                break;
                }
            default:
                {
                ptr.Append( c );
                break;
                }
            };
        }

    // Then try with system's tools.
    TText badChar = 0;
    while ( ! aFs.IsValidName( ptr, badChar ) )
        {
        TInt position = ptr.Locate( badChar );
        if ( position == KErrNotFound )
            {
            // This should not happen. 
            DLERROR(( _L("->encode filename abort! %S "),
                             &aFileName ));
            break;
            }
        // Replace evil characters with uppercase numeric codes.
        TBuf<8> num;
        num.NumUC( badChar, EHex );
        ptr.Replace( position, 1, num );
        }


    DLTRACEOUT(( _L("%S"), encoded ));
    return encoded;
    }    
    

// ---------------------------------------------------------------------------
// Creates a private directory
// ---------------------------------------------------------------------------
// 
void CreatePrivatePathL( RFs& aFs, const TDesC& aDriveAndColon, TDes& aPath ) 
    {
    DLTRACEIN((""));
    TInt driveNum;
    User::LeaveIfError( aFs.CharToDrive( aDriveAndColon[0], driveNum ) );

    DLTRACE(("Creating private path"));
    User::LeaveIfError( aFs.CreatePrivatePath( driveNum ) );
    User::LeaveIfError( aFs.PrivatePath( aPath ) );
    
    aPath.Insert( 0, aDriveAndColon );    
    }

        
// ---------------------------------------------------------------------------
// Free disk space
// ---------------------------------------------------------------------------
// 
TInt64 FreeDiskSpaceL( RFs& aFs, TInt aDriveNumber )
    {
    DLTRACEIN((""));        

    TInt64 freeSpace = 0;
    TVolumeInfo volumeInfo;
    
    if ( aFs.Volume( volumeInfo, aDriveNumber ) == KErrNone)
        {
        freeSpace = volumeInfo.iFree;
        }

    DLTRACEOUT(( "Free space: %li", freeSpace ));
    return freeSpace;
    }    
        
    
 // Specialization for 64-bit integers
template<>
void IntToDes16<TInt64>( const TInt64& aInt, TDes& aDes )
    {
    DLTRACEIN(("Int: %Li, aDes.MaxLength() = %d", aInt, aDes.MaxLength() ));
    DASSERT( aDes.MaxLength() > 3 );
    
    IntToDes16( I64HIGH( aInt ), aDes );
    aDes.SetLength( 4 );
    TPtr16 ptr( aDes.MidTPtr( 2 ) );
    TDes& des( ptr );
    IntToDes16( I64LOW( aInt ), des );
    DLTRACEOUT(( _L("High: %i, low: %i, Des: %S"), 
        I64HIGH( aInt ), I64LOW( aInt ), &aDes ));    
    }



// Specialization for 64-bit integers    
template<>
void Des16ToInt<TInt64>( const TDesC& aDes, TInt64& aInt )
    {
    DLTRACEIN(( _L("aDes: %S"), &aDes ));
    DASSERT( aDes.Length() > 3 );
    TUint32 high = 0;
    Des16ToInt( aDes, high );
    
    TUint32 low = 0;
    Des16ToInt( aDes.Mid( 2 ), low );
        
    aInt = MAKE_TINT64( high, low );        
    DLTRACEOUT(("Int: %Li, High: %i, low: %i", aInt, high, low ));
    }

// ---------------------------------------------------------------------------
// Checks if the URI is a HTTPS URI
// ---------------------------------------------------------------------------
// 
TBool IsHttpsUri( const TDesC& aUri )
    {
    _LIT(KHttps, "https");
    return aUri.Left( KHttps().Length() ) == KHttps();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt FileSystemAllocationL( RFs& aFs, const TDesC& aDir )
    {
    TInt allocation = 0;    

    CDirScan* dirScan = CDirScan::NewLC( aFs );

    dirScan->SetScanDataL( aDir,
                           KEntryAttNormal,
                           ESortByName,
                           CDirScan::EScanDownTree );
    CDir* dir = 0;
    dirScan->NextL( dir );
    while ( dir )
        {
        CleanupStack::PushL( dir );
        TInt count = dir->Count();
        
        for ( TInt i = 0; i < count; i++ )
            {
            const TEntry& entry = ( *dir )[i];
            allocation += entry.iSize;
            }
        CleanupStack::PopAndDestroy( dir );
        dirScan->NextL( dir );
        }

    CleanupStack::PopAndDestroy( dirScan );
    return allocation;
    }
    
    
// ---------------------------------------------------------------------------
// Read file
// ---------------------------------------------------------------------------
// 
HBufC8* ReadFileL( RFs& aFs, const TDesC& aFileName )
    {
    DLTRACEIN(( _L("File: %S"), &aFileName ));
    RFile file;
    User::LeaveIfError( file.Open( aFs,
                                   aFileName,
                                   EFileRead ) );
    CleanupClosePushL( file );
    HBufC8* buffer = ReadFileL( file );
    CleanupStack::PopAndDestroy(); // file
    
    return buffer;
    }

// ---------------------------------------------------------------------------
// Read file
// ---------------------------------------------------------------------------
// 
HBufC8* ReadFileL( RFile& aFile )
    {
    DLTRACEIN(( "" ));
    TInt size;
    User::LeaveIfError( aFile.Size( size ) );
    HBufC8* buffer = HBufC8::NewLC( size );
    TPtr8 ptr( buffer->Des() );
    User::LeaveIfError( aFile.Read( ptr, size ) );
    CleanupStack::Pop( buffer );
    
    return buffer;
    }


// ---------------------------------------------------------------------------
// Checks if there's enough space on the drive
// ---------------------------------------------------------------------------
// 
void WouldDiskSpaceGoBelowCriticalLevelL( const TDesC& aPath,
                                          RFs& aFs,
                                          TInt aSpaceNeeded )
    {
    DLTRACEIN(( ""));
    if ( aPath.Length() <= 0 )
        { 
        DLERROR(("Invalid path"));
        User::Leave( KErrArgument );
        }

    TInt driveNumber = 0;
    User::LeaveIfError( RFs::CharToDrive( aPath[0], driveNumber ) );
    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &aFs,
                                                aSpaceNeeded,
                                                driveNumber ) )
        {
        DLERROR(( _L( "Disk space would go below critical level, path: %S, space requested: %d " ),
                         &aPath, aSpaceNeeded ));
        User::Leave( KErrDiskFull );
        }
    }


// Accepts "x", "x.y" or "x.y.z" format version strings.
void DesToVersionL( const TDesC& aVersion, 
    TUint16& aMajor, TUint16& aMinor, TUint32& aBuild )
    {
    DLTRACEIN(( _L("Version: %S"), &aVersion ));
    // Initial values. -1 indicates that the value has not been yet parsed.
    // using double sizes to ensure that TUints fit without sign conversion
    TInt32 major = -1;
    TInt32 minor = -1;
    TInt64 build = -1;

    TLex lex( aVersion );
    lex.Mark();
    for( ;; )
        {
        if( lex.Eos() || lex.Peek() == '.' )
            {
            TPtrC data = lex.MarkedToken();
            TLex num;
            num.Assign( data );
            TInt64 value = 0;
            User::LeaveIfError( num.Val( value ) );

            if( major < 0 )
                {
                major = value;
                }
            else if( minor < 0 )
                {
                minor = value;
                }
            else if( build < 0 )
                {
                build = value;
                // all values received now, exit loop.
                break;
                }
            
            if( lex.Eos() )
                {
                break;
                }
            else
                {
                lex.SkipAndMark( 1 );
                }
            }
        else 
            {
            lex.Inc();
            }
        }

    if( major < 0 )
        {
        DLERROR(("No major value, leaving"));
        // Major required.
        User::Leave( KErrArgument );
        }
        
    if( minor < 0 )
        {
        // Assume zero for this one if not given.
        minor = 0;
        }
        
    if( build < 0 )
        {
        // Assume zero for this one if not given.
        build = 0;
        }
    
    aMajor = major;
    aMinor = minor;
    aBuild = build;
    DLTRACEOUT(("Interpreted version: %d.%d.%d", aMajor, aMinor, aBuild ));
    }

// ---------------------------------------------------------------------------
// Appends to paths
// ---------------------------------------------------------------------------
//
void AppendPathsLC( 
    RBuf& aPath, 
    const TDesC& aRoot,
    const TDesC& aAppendPath )
    {
    DLTRACEIN((""));
    CleanupClosePushL( aPath );
    aPath.CreateL( aRoot.Length() + 
        aAppendPath.Length() );
    aPath.Append( aRoot );
    aPath.Append( aAppendPath );
    DLTRACEOUT(( _L("Path: %S"), &aPath ));
    }



// ---------------------------------------------------------------------------
// Leave helpers
// ---------------------------------------------------------------------------
//
void LeaveIfNotErrorL( TInt aError, TInt aAcceptErr )
    {   
    if ( aError != KErrNone &&
         aError != aAcceptErr )
        {        
        DLERROR(("Error: %d, leaving", aError));
        User::Leave( aError );
        }
    }
    
    
void LeaveIfNotErrorL( TInt aError, TInt aAcceptErr, TInt aAcceptErr2 )
    {    
    if ( aError != KErrNone &&
         aError != aAcceptErr &&
         aError != aAcceptErr2 )
        {
        DLERROR(("Error: %d, leaving", aError));
        User::Leave( aError );
        }    
    }


void LeaveIfNotErrorL( TInt aError, TInt aAcceptErr, 
    TInt aAcceptErr2, TInt aAcceptErr3 )
    {
    if ( aError != KErrNone &&
         aError != aAcceptErr &&
         aError != aAcceptErr2 &&
         aError != aAcceptErr3 )
        {
        DLERROR(("Error: %d, leaving", aError));
        User::Leave( aError );
        }    
    }
    

void OpenOrCreateFileL( 
    RFs& aFs, RFile& aFile, const TDesC& aFilePath, TUint aFileMode )
    {
    DLTRACEIN((""));
    TInt err = aFile.Open( aFs, aFilePath, aFileMode );
    if ( err == KErrNotFound ) 
        {
        DLTRACE(("file not found, creating"));
        User::LeaveIfError( aFile.Create( aFs, aFilePath, aFileMode ) );
        }
    else if ( err != KErrNone ) 
        {
        DLERROR(("Error when opening: %d", err));
        User::Leave( err );
        }    
    }
