/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Symbian-specific part of the language selection dialog in SifUi
*
*/

#include "sifuidialogselectlanguage.h"  // SifUiDialogSelectLanguage
#include <languages.hrh>                // S60 language definitions
#include <hb/hbcore/hblocaleutil.h>     // HbLocaleUtil
#include <s32mem.h>                     // RMemReadStream

//
// String array to convert Symbian TLanguage enums to ISO 639-1 language codes.
//
struct SLangString
    {
    TInt iId;
    const TText8* iCode;
    };

const TInt KLangCodeLength = 2;
const SLangString KLangStrings[] =
    {
    { ELangEnglish,                 _S8( "en" ) },
    { ELangFrench,                  _S8( "fr" ) },
    { ELangGerman,                  _S8( "de" ) },
    { ELangSpanish,                 _S8( "es" ) },
    { ELangItalian,                 _S8( "it" ) },
    { ELangSwedish,                 _S8( "sv" ) },
    { ELangDanish,                  _S8( "da" ) },
    { ELangNorwegian,               _S8( "no" ) },
    { ELangFinnish,                 _S8( "fi" ) },
    { ELangAmerican,                _S8( "en" ) },
    { ELangSwissFrench,             _S8( "fr" ) },
    { ELangSwissGerman,             _S8( "de" ) },
    { ELangPortuguese,              _S8( "pt" ) },
    { ELangTurkish,                 _S8( "tr" ) },
    { ELangIcelandic,               _S8( "is" ) },
    { ELangRussian,                 _S8( "ru" ) },
    { ELangHungarian,               _S8( "hu" ) },
    { ELangDutch,                   _S8( "nl" ) },
    { ELangBelgianFlemish,          _S8( "nl" ) },
    { ELangAustralian,              _S8( "en" ) },
    { ELangBelgianFrench,           _S8( "fr" ) },
    { ELangAustrian,                _S8( "de" ) },
    { ELangNewZealand,              _S8( "en" ) },
    { ELangInternationalFrench,     _S8( "fr" ) },
    { ELangCzech,                   _S8( "cs" ) },
    { ELangSlovak,                  _S8( "sk" ) },
    { ELangPolish,                  _S8( "pl" ) },
    { ELangSlovenian,               _S8( "sl" ) },
    { ELangTaiwanChinese,           _S8( "zh" ) },
    { ELangHongKongChinese,         _S8( "zh" ) },
    { ELangPrcChinese,              _S8( "zh" ) },
    { ELangJapanese,                _S8( "ja" ) },
    { ELangThai,                    _S8( "th" ) },
    { ELangAfrikaans,               _S8( "af" ) },
    { ELangAlbanian,                _S8( "sq" ) },
    { ELangAmharic,                 _S8( "am" ) },
    { ELangArabic,                  _S8( "ar" ) },
    { ELangArmenian,                _S8( "hy" ) },
    { ELangTagalog,                 _S8( "tl" ) },
    { ELangBelarussian,             _S8( "be" ) },
    { ELangBengali,                 _S8( "bn" ) },
    { ELangBulgarian,               _S8( "bg" ) },
    { ELangBurmese,                 _S8( "my" ) },
    { ELangCatalan,                 _S8( "ca" ) },
    { ELangCroatian,                _S8( "hr" ) },
    { ELangCanadianEnglish,         _S8( "en" ) },
    { ELangInternationalEnglish,    _S8( "en" ) },
    { ELangSouthAfricanEnglish,     _S8( "en" ) },
    { ELangEstonian,                _S8( "et" ) },
    { ELangFarsi,                   _S8( "fa" ) },
    { ELangCanadianFrench,          _S8( "fr" ) },
    { ELangScotsGaelic,             _S8( "gd" ) },
    { ELangGeorgian,                _S8( "ka" ) },
    { ELangGreek,                   _S8( "el" ) },
    { ELangCyprusGreek,             _S8( "el" ) },
    { ELangGujarati,                _S8( "gu" ) },
    { ELangHebrew,                  _S8( "he" ) },
    { ELangHindi,                   _S8( "hi" ) },
    { ELangIndonesian,              _S8( "id" ) },
    { ELangIrish,                   _S8( "ga" ) },
    { ELangSwissItalian,            _S8( "it" ) },
    { ELangKannada,                 _S8( "kn" ) },
    { ELangKazakh,                  _S8( "kk" ) },
    { ELangKhmer,                   _S8( "km" ) },
    { ELangKorean,                  _S8( "ko" ) },
    { ELangLao,                     _S8( "lo" ) },
    { ELangLatvian,                 _S8( "lv" ) },
    { ELangLithuanian,              _S8( "lt" ) },
    { ELangMacedonian,              _S8( "mk" ) },
    { ELangMalay,                   _S8( "ms" ) },
    { ELangMalayalam,               _S8( "ml" ) },
    { ELangMarathi,                 _S8( "mr" ) },
    { ELangMoldavian,               _S8( "mo" ) },
    { ELangMongolian,               _S8( "mn" ) },
    { ELangNorwegianNynorsk,        _S8( "nn" ) },
    { ELangBrazilianPortuguese,     _S8( "pt" ) },
    { ELangPunjabi,                 _S8( "pa" ) },
    { ELangRomanian,                _S8( "ro" ) },
    { ELangSerbian,                 _S8( "sr" ) },
    { ELangSinhalese,               _S8( "si" ) },
    { ELangSomali,                  _S8( "so" ) },
    { ELangInternationalSpanish,    _S8( "es" ) },
    { ELangLatinAmericanSpanish,    _S8( "es" ) },
    { ELangSwahili,                 _S8( "sw" ) },
    { ELangFinlandSwedish,          _S8( "sv" ) },
    { ELangReserved1,               _S8( "en" ) },
    { ELangTamil,                   _S8( "ta" ) },
    { ELangTelugu,                  _S8( "te" ) },
    { ELangTibetan,                 _S8( "bo" ) },
    { ELangTigrinya,                _S8( "ti" ) },
    { ELangCyprusTurkish,           _S8( "tr" ) },
    { ELangTurkmen,                 _S8( "tk" ) },
    { ELangUkrainian,               _S8( "uk" ) },
    { ELangUrdu,                    _S8( "ur" ) },
    { ELangReserved2,               _S8( "en" ) },
    { ELangVietnamese,              _S8( "vi" ) },
    { ELangWelsh,                   _S8( "cy" ) },
    { ELangZulu,                    _S8( "zu" ) },
    { KLangTaiwanEnglish,           _S8( "en" ) },
    { KLangHongKongEnglish,         _S8( "en" ) },
    { KLangPrcEnglish,              _S8( "en" ) },
    { KLangJapaneseEnglish,         _S8( "en" ) },
    { KLangThaiEnglish,             _S8( "en" ) }
    };

#define KNUMLANGSTRINGS ( sizeof( KLangStrings ) / sizeof( SLangString ) )


// ======== LOCAL FUNCTIONS ========

// ----------------------------------------------------------------------------
// LanguageCode()
// ----------------------------------------------------------------------------
//
QString LanguageCode( const TLanguage aLanguage )
{
    for( TInt index = 0; index < KNUMLANGSTRINGS; ++index )
        {
        SLangString lang = KLangStrings[ index ];
        if( lang.iId == aLanguage )
            {
            return QString::fromAscii( reinterpret_cast<const char*>( lang.iCode ),
                KLangCodeLength );
            }
        }
    return QString("");
}

// ----------------------------------------------------------------------------
// LanguageName()
// ----------------------------------------------------------------------------
//
QString LanguageName( const TLanguage aLanguage )
{
    QString langCode = LanguageCode( aLanguage );
    if( langCode.isEmpty() ) {
        return hbTrId("txt_installer_list_unknown_language");
    }
    QString langName = HbLocaleUtil::localisedLanguageName( langCode );
    if( langName.isEmpty() ) {
        return hbTrId("txt_installer_list_unknown_language");
    }
    return langName;
}

// ----------------------------------------------------------------------------
// LanguageNamesL()
// ----------------------------------------------------------------------------
//
QStringList LanguageNamesL( const TDesC8& aBuf )
{
    RMemReadStream readStream( aBuf.Ptr(), aBuf.Size() );
    CleanupClosePushL( readStream );
    RArray<TLanguage> langArray;
    TPckg< RArray<TLanguage> > langArrayPckg( langArray );
    readStream.ReadL( langArrayPckg );
    CleanupStack::PopAndDestroy( &readStream );

    QStringList langNames;
    for( TInt index = 0; index < langArray.Count(); index++ ) {
        QT_TRYCATCH_LEAVING( langNames.append( LanguageName( langArray[index] ) ) );
    }
    return langNames;
}


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// SifUiDialogSelectLanguage::languageNames()
// ----------------------------------------------------------------------------
//
QStringList SifUiDialogSelectLanguage::languageNames(const QVariant& languages)
{
    QByteArray byteArray = languages.toByteArray();
    QStringList names;

    const TPtrC8 ptr(reinterpret_cast<const TText8*>(byteArray.constData()), byteArray.length());
    QT_TRAP_THROWING( names = LanguageNamesL( ptr ) );

    return names;
}

