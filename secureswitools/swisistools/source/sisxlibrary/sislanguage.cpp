/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


/**
 @file 
 @internalComponent
 @released
*/

#include "sislanguage.h"
#include "utility.h"

// Language options
static const SKeyword KLanguages [] = 
	{
		{ L"EN", CSISLanguage::ELangEnglish },
		{ L"FR", CSISLanguage::ELangFrench},
		{ L"GE", CSISLanguage::ELangGerman},
		{ L"SP", CSISLanguage::ELangSpanish},
		{ L"IT", CSISLanguage::ELangItalian},
		{ L"SW", CSISLanguage::ELangSwedish},
		{ L"DA", CSISLanguage::ELangDanish},
		{ L"NO", CSISLanguage::ELangNorwegian},
		{ L"FI", CSISLanguage::ELangFinnish},
		{ L"AM", CSISLanguage::ELangAmerican},
		{ L"SF", CSISLanguage::ELangSwissFrench},
		{ L"SG", CSISLanguage::ELangSwissGerman},
		{ L"PO", CSISLanguage::ELangPortuguese},
		{ L"TU", CSISLanguage::ELangTurkish},
		{ L"IC", CSISLanguage::ELangIcelandic},
		{ L"RU", CSISLanguage::ELangRussian},
		{ L"HU", CSISLanguage::ELangHungarian},
		{ L"DU", CSISLanguage::ELangDutch},
		{ L"BL", CSISLanguage::ELangBelgianFlemish},
		{ L"AU", CSISLanguage::ELangAustralian},
		{ L"BF", CSISLanguage::ELangBelgianFrench},
		{ L"AS", CSISLanguage::ELangAustrian},
		{ L"NZ", CSISLanguage::ELangNewZealand},
		{ L"IF", CSISLanguage::ELangInternationalFrench},
		{ L"CS", CSISLanguage::ELangCzech},
		{ L"SK", CSISLanguage::ELangSlovak},
		{ L"PL", CSISLanguage::ELangPolish},
		{ L"SL", CSISLanguage::ELangSlovenian},
		{ L"TC", CSISLanguage::ELangTaiwanChinese},
		{ L"HK", CSISLanguage::ELangHongKongChinese},
		{ L"ZH", CSISLanguage::ELangPrcChinese},
		{ L"JA", CSISLanguage::ELangJapanese},
		{ L"TH", CSISLanguage::ELangThai},
		  
		{ L"AF", CSISLanguage::ELangAfrikaans },
		{ L"SQ", CSISLanguage::ELangAlbanian },
		{ L"AH", CSISLanguage::ELangAmharic },
		{ L"AR", CSISLanguage::ELangArabic },
		{ L"HY", CSISLanguage::ELangArmenian },
		{ L"TL", CSISLanguage::ELangTagalog },
		{ L"BE", CSISLanguage::ELangBelarussian },
		{ L"BN", CSISLanguage::ELangBengali },
		{ L"BG", CSISLanguage::ELangBulgarian },
		{ L"MY", CSISLanguage::ELangBurmese },
		{ L"CA", CSISLanguage::ELangCatalan },
		{ L"HR", CSISLanguage::ELangCroatian },
		{ L"CE", CSISLanguage::ELangCanadianEnglish },
		{ L"IE", CSISLanguage::ELangInternationalEnglish },
		{ L"SA", CSISLanguage::ELangSouthAfricanEnglish },
		{ L"ET", CSISLanguage::ELangEstonian },
		{ L"FA", CSISLanguage::ELangFarsi },
		{ L"CF", CSISLanguage::ELangCanadianFrench },
		{ L"GD", CSISLanguage::ELangScotsGaelic },
		{ L"KA", CSISLanguage::ELangGeorgian },
		{ L"EL", CSISLanguage::ELangGreek },
		{ L"CG", CSISLanguage::ELangCyprusGreek },
		{ L"GU", CSISLanguage::ELangGujarati },
		{ L"HE", CSISLanguage::ELangHebrew },
		{ L"HI", CSISLanguage::ELangHindi },
		{ L"IN", CSISLanguage::ELangIndonesian },
		{ L"GA", CSISLanguage::ELangIrish },
		{ L"SZ", CSISLanguage::ELangSwissItalian },
		{ L"KN", CSISLanguage::ELangKannada },
		{ L"KK", CSISLanguage::ELangKazakh },
		{ L"KM", CSISLanguage::ELangKhmer },
		{ L"KO", CSISLanguage::ELangKorean },
		{ L"LO", CSISLanguage::ELangLao },
		{ L"LV", CSISLanguage::ELangLatvian },
		{ L"LT", CSISLanguage::ELangLithuanian },
		{ L"MK", CSISLanguage::ELangMacedonian },
		{ L"MS", CSISLanguage::ELangMalay },
		{ L"ML", CSISLanguage::ELangMalayalam },
		{ L"MR", CSISLanguage::ELangMarathi },
		{ L"MO", CSISLanguage::ELangMoldavian },
		{ L"MN", CSISLanguage::ELangMongolian },
		{ L"NN", CSISLanguage::ELangNorwegianNynorsk },
		{ L"BP", CSISLanguage::ELangBrazilianPortuguese },
		{ L"PA", CSISLanguage::ELangPunjabi },
		{ L"RO", CSISLanguage::ELangRomanian },
		{ L"SR", CSISLanguage::ELangSerbian },
		{ L"SI", CSISLanguage::ELangSinhalese },
		{ L"SO", CSISLanguage::ELangSomali },
		{ L"OS", CSISLanguage::ELangInternationalSpanish },
		{ L"LS", CSISLanguage::ELangLatinAmericanSpanish },
		{ L"SH", CSISLanguage::ELangSwahili },
		{ L"FS", CSISLanguage::ELangFinlandSwedish },
		//{L"??", CSISLanguage::ELangReserved1 },
		{ L"TA", CSISLanguage::ELangTamil },
		{ L"TE", CSISLanguage::ELangTelugu },
		{ L"BO", CSISLanguage::ELangTibetan },
		{ L"TI", CSISLanguage::ELangTigrinya },
		{ L"CT", CSISLanguage::ELangCyprusTurkish },
		{ L"TK", CSISLanguage::ELangTurkmen },
		{ L"UK", CSISLanguage::ELangUkrainian },
		{ L"UR", CSISLanguage::ELangUrdu },
		//{L"??", CSISLanguage::ELangReserved2 },
		{ L"VI", CSISLanguage::ELangVietnamese },
		{ L"CY", CSISLanguage::ELangWelsh },
		{ L"ZU", CSISLanguage::ELangZulu },
		{ L"BA", CSISLanguage::ELangBasque },
		{ L"GL", CSISLanguage::ELangGalician },
		//{L"??", CSISLanguage::ELangOther },
		//{L"??", CSISLanguage::ELangNone  }

		{ NULL, CSISLanguage::ELangNone }

	};



CSISLanguage::TDialect CSISLanguage::IdentifyLanguage (const std::wstring& aIdentifier)
	{
	return IdentifyUCKeyword (KLanguages, aIdentifier, L"Unknown language: ");
	}



std::string CSISLanguage::Name () const
	{
	return "Language";
	}

const wchar_t* CSISLanguage::GetLanguageCode(TDialect& aDialect) const
	{
	aDialect = iLanguage & ~0x3FF;
	TLanguage language = (TLanguage) (iLanguage & 0x3FF);

	for (int i=0; KLanguages[i].iName != NULL; ++i)
		{
		if (KLanguages[i].iId == (TUint32)language)
			{
			return KLanguages[i].iName;
			}
		}
	return L"Unknown";
	}

void CSISLanguage::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	TDialect dialect = 0;
	aStream << GetLanguageCode(dialect);
	if (dialect != 0)
		{
		aStream << L"(" << dialect.Value() << L")";
		}	
	}

