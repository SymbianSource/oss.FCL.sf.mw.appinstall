/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* @file
* This file holds all the constants and literals required by the application
* @internalComponent
*
*/


 
 #ifndef __SWICONSOLE_CONSTANTS_H_
 #define __SWICONSOLE_CONSTANTS_H_
 
 // User includes
 #include <swiconsole.rsg>
 
 // Languages

 _LIT(KAfrikaans, "AF");
 _LIT(KAmharic, "AH");
 _LIT(KAmericanEnglish, "AM");
 _LIT(KArabic, "AR");
 _LIT(KAustrian, "AS");
 _LIT(KAustralian, "AU");
 _LIT(KBelarussian, "BE");
 _LIT(KBelgianFrench, "BF");
 _LIT(KBulgarian, "BG");
 _LIT(KBelgianFlemish, "BL");
 _LIT(KBengali, "BN");
 _LIT(KTibetan, "BO");
 _LIT(KBrazilianPortuguese, "BP");
 _LIT(KCatalan, "CA");
 _LIT(KCanadianEnglish, "CE");
 _LIT(KCanadianFrench, "CF");
 _LIT(KCyprusGreek, "CG");
 _LIT(KCzech, "CS");
 _LIT(KCyprusTurkish, "CT");
 _LIT(KWelsh, "CY");
 _LIT(KDanish, "DA");
 _LIT(KDutch, "DU");
 _LIT(KGreek, "EL");
 _LIT(KEnglish, "EN");
 _LIT(KEstonian, "ET");
 _LIT(KFarsi, "FA");
 _LIT(KFinnish, "FI");
 _LIT(KFrench, "FR");
 _LIT(KFinlandSwedish, "FS");
 _LIT(KIrish, "GA");
 _LIT(KScotsGaelic, "GD");
 _LIT(KGerman, "GE");
 _LIT(KGujarati, "GU");
 _LIT(KHebrew, "HE");
 _LIT(KHindi, "HI");
 _LIT(KHongKongChinese, "HK");
 _LIT(KCroatian, "HR");
 _LIT(KHungarian, "HU");
 _LIT(KArmenian, "HY");
 _LIT(KIcelandic, "IC");
 _LIT(KInternationalEnglish, "IE");
 _LIT(KInternationalFrench, "IF");
 _LIT(KIndonesian, "IN");
 _LIT(KItalian, "IT");
 _LIT(KJapanese, "JA");
 _LIT(KGeorgian, "KA");
 _LIT(KKazakh, "KK");
 _LIT(KKhmer, "KM");
 _LIT(KKannada, "KN");
 _LIT(KKorean, "KO");
 _LIT(KLaothian, "LO");
 _LIT(KLatinAmericanSpanish, "LS");
 _LIT(KLithuanian, "LT");
 _LIT(KLatvian, "LV");
 _LIT(KMacedonian, "MK");
 _LIT(KMalayalam, "ML");
 _LIT(KMongolian, "MN");
 _LIT(KMoldovian, "MO");
 _LIT(KMarathi, "MR");
 _LIT(KMalay, "MS");
 _LIT(KBurmese, "MY");
 _LIT(KNorwegianNynorsk, "NN");
 _LIT(KNorwegian, "NO");
 _LIT(KNewZealand, "NZ");
 _LIT(KInternationalSpanish, "OS");
 _LIT(KPunjabi, "PA");
 _LIT(KPolish, "PL");
 _LIT(KPortuguese, "PO");
 _LIT(KRomanian, "RO");
 _LIT(KRussian, "RU");
 _LIT(KSouthAfricanEnglish, "SA");
 _LIT(KSwissFrench, "SF");
 _LIT(KSwissGerman, "SG");
 _LIT(KSwahili, "SH");
 _LIT(KSinhalese, "SI");
 _LIT(KSlovak, "SK");
 _LIT(KSlovenian, "SL");
 _LIT(KSomali, "SO");
 _LIT(KSpanish, "SP");
 _LIT(KAlbanian, "SQ");
 _LIT(KSerbian, "SR");
 _LIT(KSwedish, "SW");
 _LIT(KSwissItalian, "SZ");
 _LIT(KTamil, "TA");
 _LIT(KTaiwanChinese, "TC");
 _LIT(KTelugu, "TE");
 _LIT(KThai, "TH");
 _LIT(KTigrinya, "TI");
 _LIT(KTurkmen, "TK");
 _LIT(KTagalog, "TL");
 _LIT(KTurkish, "TU");
 _LIT(KUkrainian, "UK");
 _LIT(KUrdu, "UR");
 _LIT(KVietnamese, "VI");
 _LIT(KPRCChinese, "ZH");
 _LIT(KZulu, "ZU");
 
 _LIT(KReserved, "XX");

 
const TPtrC languageArr[] = 
{
	KReserved(),
 	KEnglish(),
 	KFrench(),
 	KGerman(),
 	KSpanish(),  
 	KItalian(),  
 	KSwedish(),  
 	KDanish(),  
 	KNorwegian(),  
 	KFinnish(),  
 	KAmericanEnglish(),  
 	KSwissFrench(),  
 	KSwissGerman(),  
 	KPortuguese(),  
 	KTurkish(),  
 	KIcelandic(),  
 	KRussian(),  
 	KHungarian(),  
 	KDutch(),  
 	KBelgianFlemish(),  
 	KAustralian(),  
 	KBelgianFrench(),  
 	KAustrian(),  
 	KNewZealand(),  
 	KInternationalFrench(),  
 	KCzech(),  
 	KSlovak(),  
 	KPolish(),  
 	KSlovenian(),  
 	KTaiwanChinese(),  
 	KHongKongChinese(),  
 	KPRCChinese(),  
 	KJapanese(),  
 	KThai(),  
	KAfrikaans(),  
 	KAlbanian(),  
 	KAmharic(),  
 	KArabic(),  
 	KArmenian(),  
 	KTagalog(),  
 	KBelarussian(),  
 	KBengali(),  
 	KBulgarian(),  
 	KBurmese(),  
 	KCatalan(),  
 	KCroatian(),  
	KCanadianEnglish(),  
 	KInternationalEnglish(),  
 	KSouthAfricanEnglish(),  
 	KEstonian(),  
 	KFarsi(),  
 	KCanadianFrench(),  
 	KScotsGaelic(),  
 	KGeorgian(),  
 	KGreek(),
 	KCyprusGreek(),
 	KGujarati(),  
 	KHebrew(),  
 	KHindi(),  
 	KIndonesian(),  
 	KIrish(),  
 	KSwissItalian(),  
 	KKannada(),  
 	KKazakh(),  
 	KKhmer(),  
 	KKorean(),  
 	KLaothian(),  
 	KLatvian(),  
 	KLithuanian(),  
 	KMacedonian(),  
 	KMalay(),  
 	KMalayalam(),  
 	KMarathi(),  
 	KMoldovian(),  
 	KMongolian(),  
 	KNorwegianNynorsk(),  
 	KBrazilianPortuguese(),  
 	KPunjabi(),  
 	KRomanian(),  
 	KSerbian(),  
 	KSinhalese(),  
 	KSomali(),  
 	KInternationalSpanish(),  
 	KLatinAmericanSpanish(),  
 	KSwahili(),  
 	KFinlandSwedish(),
 	KReserved(),
 	KTamil(),  
 	KTelugu(),  
 	KTibetan(),  
 	KTigrinya(),  
 	KCyprusTurkish(),  
 	KTurkmen(),  
 	KUkrainian(),  
 	KUrdu(),  
 	KReserved(),
 	KVietnamese(),  
 	KWelsh(),  
 	KZulu()
 };
 
 const TInt langNameResourceId[] =
	{
	-1, // ELangTest
	R_LANG_ENGLISH_STR,      
	R_LANG_FRENCH_STR,      
	R_LANG_GERMAN_STR,      
	R_LANG_SPANISH_STR,      
	R_LANG_ITALIAN_STR,      
	R_LANG_SWEDISH_STR,      
	R_LANG_DANISH_STR,      
	R_LANG_NORWEGIAN_STR,      
	R_LANG_FINNISH_STR,      
	R_LANG_AMERICANENGLISH_STR,      
	R_LANG_SWISSFRENCH_STR,      
	R_LANG_SWISSGERMAN_STR,      
	R_LANG_PORTUGUESE_STR,     
	R_LANG_TURKISH_STR,     
	R_LANG_ICELANDIC_STR,     
	R_LANG_RUSSIAN_STR,     
	R_LANG_HUNGARIAN_STR,     
	R_LANG_DUTCH_STR,     
	R_LANG_BELGIANFLEMISH_STR,     
	R_LANG_AUSTRALIAN_STR,     
	R_LANG_BELGIANFRENCH_STR,     
	R_LANG_AUSTRIAN_STR,     
	R_LANG_NEWZEALAND_STR,     
	R_LANG_INTERNATIONALFRENCH_STR,     
	R_LANG_CZECH_STR,     
	R_LANG_SLOVAK_STR,     
	R_LANG_POLISH_STR,     
	R_LANG_SLOVENIAN_STR,     
	R_LANG_TAIWANCHINESE_STR,     
	R_LANG_HONGKONGCHINESE_STR,     
	R_LANG_PRCCHINESE_STR,     
	R_LANG_JAPANESE_STR,     
	R_LANG_THAI_STR,     
	R_LANG_AFRIKAANS_STR,     
	R_LANG_ALBANIAN_STR,     
	R_LANG_AMHARIC_STR,     
	R_LANG_ARABIC_STR,     
	R_LANG_ARMENIAN_STR,     
	R_LANG_TAGALOG_STR,     
	R_LANG_BELARUSSIAN_STR,     
	R_LANG_BENGALI_STR ,     
	R_LANG_BULGARIAN_STR,     
	R_LANG_BURMESE_STR,     
	R_LANG_CATALAN_STR,     
	R_LANG_CROATIAN_STR,     
	R_LANG_CANADIANENGLISH_STR,     
	R_LANG_INTERNATIONALENGLISH_STR,     
	R_LANG_SOUTHAFRICANENGLISH_STR,     
	R_LANG_ESTONIAN_STR,     
	R_LANG_FARSI_STR,     
	R_LANG_CANADIANFRENCH_STR,     
	R_LANG_SCOTSGAELIC_STR,     
	R_LANG_GEORGIAN_STR,     
	R_LANG_GREEK_STR,     
	R_LANG_CYPRUSGREEK_STR,     
	R_LANG_GUJARATI_STR,     
	R_LANG_HEBREW_STR,     
	R_LANG_HINDI_STR,     
	R_LANG_INDONESIAN_STR,     
	R_LANG_IRISH_STR,     
	R_LANG_SWISSITALIAN_STR,     
	R_LANG_KANNADA_STR,     
	R_LANG_KAZAKH_STR,     
	R_LANG_KHMER_STR,     
	R_LANG_KOREAN_STR,     
	R_LANG_LAOTHIAN_STR,     
	R_LANG_LATVIAN_STR,     
	R_LANG_LITHUANIAN_STR,     
	R_LANG_MACEDONIAN_STR,     
	R_LANG_MALAY_STR,     
	R_LANG_MALAYALAM_STR,     
	R_LANG_MARATHI_STR,     
	R_LANG_MOLDOVIAN_STR,     
	R_LANG_MONGOLIAN_STR,     
	R_LANG_NORWEGIANNYNORSK_STR,     
	R_LANG_BRAZILIANPORTUGUESE_STR,     
	R_LANG_PUNJABI_STR,     
	R_LANG_ROMANIAN_STR,     
	R_LANG_SERBIAN_STR,     
	R_LANG_SINHALESE_STR,     
	R_LANG_SOMALI_STR,     
	R_LANG_INTERNATIONALSPANISH_STR,     
	R_LANG_LATINAMERICANSPANISH_STR,     
	R_LANG_SWAHILI_STR,     
	R_LANG_FINLANDSWEDISH_STR,     
	R_LANG_RESERVED_STR,     
	R_LANG_TAMIL_STR,     
	R_LANG_TELUGU_STR,     
	R_LANG_TIBETAN_STR,     
	R_LANG_TIGRINYA_STR,     
	R_LANG_CYPRUSTURKISH_STR,     
	R_LANG_TURKMEN_STR,     
	R_LANG_UKRAINIAN_STR,     
	R_LANG_URDU_STR,     
	R_LANG_RESERVED_STR,     
	R_LANG_VIETNAMESE_STR,     
	R_LANG_WELSH_STR,     
	R_LANG_ZULU_STR
	};
 
 #endif //__SWICONSOLE_CONSTANTS_H_