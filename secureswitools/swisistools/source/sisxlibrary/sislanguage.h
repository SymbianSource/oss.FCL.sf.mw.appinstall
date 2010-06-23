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
* Note: This file may contain code to generate corrupt files for test purposes.
* Such code is excluded from production builds by use of compiler defines;
* it is recommended that such code should be removed if this code is ever published publicly.
* As specified in SGL.GT0188.251
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __SISLANGUAGE_H__
#define __SISLANGUAGE_H__


#include "structure.h"
#include "numeric.h"

class CSISLanguage : public CStructure <CSISFieldRoot::ESISLanguage>

	{
public:

	typedef enum 
		{
		//This list is lifted directly from E32std.h
		/**
			Enumerated value used for testing - does not represent a language.
		*/
		ELangTest = 0,

		/** UK English. */
		ELangEnglish = 1,

		/** French. */
		ELangFrench = 2,

		/** German. */
		ELangGerman = 3,

		/** Spanish. */
		ELangSpanish = 4,

		/** Italian. */
		ELangItalian = 5,

		/** Swedish. */
		ELangSwedish = 6,

		/** Danish. */
		ELangDanish = 7,

		/** Norwegian. */
		ELangNorwegian = 8,

		/** Finnish. */
		ELangFinnish = 9,

		/** American. */
		ELangAmerican = 10,

		/** Swiss French. */
		ELangSwissFrench = 11,

		/** Swiss German. */
		ELangSwissGerman = 12,

		/** Portuguese. */
		ELangPortuguese = 13,

		/** Turkish. */
		ELangTurkish = 14,

		/** Icelandic. */
		ELangIcelandic = 15,

		/** Russian. */
		ELangRussian = 16,

		/** Hungarian. */
		ELangHungarian = 17,

		/** Dutch. */
		ELangDutch = 18,

		/** Belgian Flemish. */
		ELangBelgianFlemish = 19,

		/** Australian English. */
		ELangAustralian = 20,

		/** Belgian French. */
		ELangBelgianFrench = 21,

		/** Austrian German. */
		ELangAustrian = 22,

		/** New Zealand English. */
		ELangNewZealand = 23,

		/** International French. */
		ELangInternationalFrench = 24,

		/** Czech. */
		ELangCzech = 25,

		/** Slovak. */
		ELangSlovak = 26,

		/** Polish. */
		ELangPolish = 27,

		/** Slovenian. */
		ELangSlovenian = 28,

		/** Taiwanese Chinese. */
		ELangTaiwanChinese = 29,

		/** Hong Kong Chinese. */
		ELangHongKongChinese = 30,

		/** Peoples Republic of China's Chinese. */
		ELangPrcChinese = 31,

		/** Japanese. */
		ELangJapanese = 32,

		/** Thai. */
		ELangThai = 33,

		/** Afrikaans. */
		ELangAfrikaans = 34,

		/** Albanian. */
		ELangAlbanian = 35,

		/** Amharic. */
		ELangAmharic = 36,

		/** Arabic. */
		ELangArabic = 37,

		/** Armenian. */
		ELangArmenian = 38,

		/** Tagalog. */
		ELangTagalog = 39,

		/** Belarussian. */
		ELangBelarussian = 40,

		/** Bengali. */
		ELangBengali = 41,

		/** Bulgarian. */
		ELangBulgarian = 42,

		/** Burmese. */
		ELangBurmese = 43,

		/** Catalan. */
		ELangCatalan = 44,

		/** Croatian. */
		ELangCroatian = 45,

		/** Canadian English. */
		ELangCanadianEnglish = 46,

		/** International English. */
		ELangInternationalEnglish = 47,

		/** South African English. */
		ELangSouthAfricanEnglish = 48,

		/** Estonian. */
		ELangEstonian = 49,

		/** Farsi. */
		ELangFarsi = 50,

		/** Canadian French. */
		ELangCanadianFrench = 51,

		/** Gaelic. */
		ELangScotsGaelic = 52,

		/** Georgian. */
		ELangGeorgian = 53,

		/** Greek. */
		ELangGreek = 54,

		/** Cyprus Greek. */
		ELangCyprusGreek = 55,

		/** Gujarati. */
		ELangGujarati = 56,

		/** Hebrew. */
		ELangHebrew = 57,

		/** Hindi. */
		ELangHindi = 58,

		/** Indonesian. */
		ELangIndonesian = 59,

		/** Irish. */
		ELangIrish = 60,

		/** Swiss Italian. */
		ELangSwissItalian = 61,

		/** Kannada. */
		ELangKannada = 62,

		/** Kazakh. */
		ELangKazakh = 63,

		/** Khmer. */
		ELangKhmer = 64,

		/** Korean. */
		ELangKorean = 65,

		/** Lao. */
		ELangLao = 66,

		/** Latvian. */
		ELangLatvian = 67,

		/** Lithuanian. */
		ELangLithuanian = 68,

		/** Macedonian. */
		ELangMacedonian = 69,

		/** Malay. */
		ELangMalay = 70,

		/** Malayalam. */
		ELangMalayalam = 71,

		/** Marathi. */
		ELangMarathi = 72,

		/** Moldavian. */
		ELangMoldavian = 73,

		/** Mongolian. */
		ELangMongolian = 74,

		/** Norwegian Nynorsk. */
		ELangNorwegianNynorsk = 75,

		/** Brazilian Portuguese. */
		ELangBrazilianPortuguese = 76,

		/** Punjabi. */
		ELangPunjabi = 77,

		/** Romanian. */
		ELangRomanian = 78,

		/** Serbian. */
		ELangSerbian = 79,

		/** Sinhalese. */
		ELangSinhalese = 80,

		/** Somali. */
		ELangSomali = 81,

		/** International Spanish. */
		ELangInternationalSpanish = 82,

		/** American Spanish. */
		ELangLatinAmericanSpanish = 83,

		/** Swahili. */
		ELangSwahili = 84,

		/** Finland Swedish. */
		ELangFinlandSwedish = 85,

		/** Reserved, not in use. */
		ELangReserved1 = 86,		// This enum should not be used for new languages, see INC110543

		/** Tamil. */
		ELangTamil = 87,

		/** Telugu. */
		ELangTelugu = 88,

		/** Tibetan. */
		ELangTibetan = 89,

		/** Tigrinya. */
		ELangTigrinya = 90,

		/** Cyprus Turkish. */
		ELangCyprusTurkish = 91,

		/** Turkmen. */
		ELangTurkmen = 92,

		/** Ukrainian. */
		ELangUkrainian = 93,

		/** Urdu. */
		ELangUrdu = 94,

		/** Reserved, not in use. */
		ELangReserved2 = 95,		// This enum should not be used for new languages, see INC110543

		/** Vietnamese. */
		ELangVietnamese = 96,

		/** Welsh. */
		ELangWelsh = 97,

		/** Zulu. */
		ELangZulu = 98,

		/**
		  @deprecated

		  Use of this value is deprecated.
		  */
		ELangOther = 99,

		/** English with terms as used by the device manufacturer, if this needs to
		  be distinct from the English used by the UI vendor. */
		ELangManufacturerEnglish = 100,

		/** South Sotho.

		  A language of Lesotho also called Sesotho. SIL code sot. */
		ELangSouthSotho = 101,

		/** Basque. */
		ELangBasque = 102,

		/** Galician. */
		ELangGalician = 103,

		/** Javanese. */
		ELangJavanese = 104,

		/** Maithili. */
		ELangMaithili = 105,

		/** Azerbaijani(Latin alphabet). */
		ELangAzerbaijani_Latin = 106,

		/** Azerbaijani(Cyrillic alphabet). */
		ELangAzerbaijani_Cyrillic = 107,

		/** Oriya. */
		ELangOriya = 108,

		/** Bhojpuri. */
		ELangBhojpuri = 109,

		/** Sundanese. */
		ELangSundanese = 110,

		/** Kurdish(Latin alphabet). */
		ELangKurdish_Latin = 111,

		/** Kurdish(Arabic alphabet). */
		ELangKurdish_Arabic = 112,

		/** Pashto. */
		ELangPashto = 113,

		/** Hausa. */
		ELangHausa = 114,

		/** Oromo. */
		ELangOromo = 115,

		/** Uzbek(Latin alphabet). */
		ELangUzbek_Latin = 116,

		/** Uzbek(Cyrillic alphabet). */
		ELangUzbek_Cyrillic = 117,

		/** Sindhi(Arabic alphabet). */
		ELangSindhi_Arabic = 118,

		/** Sindhi(using Devanagari script). */
		ELangSindhi_Devanagari = 119,

		/** Yoruba. */
		ELangYoruba = 120,

		/** Cebuano. */
		ELangCebuano = 121,

		/** Igbo. */
		ELangIgbo = 122,

		/** Malagasy. */
		ELangMalagasy = 123,

		/** Nepali. */ 
		ELangNepali = 124,

		/** Assamese. */
		ELangAssamese = 125,

		/** Shona. */
		ELangShona = 126,

		/** Zhuang. */
		ELangZhuang = 127,

		/** Madurese. */
		ELangMadurese = 128,

		/** English as appropriate for use in Asia-Pacific regions. */
		ELangEnglish_Apac=129,

		/** English as appropriate for use in Taiwan. */
		ELangEnglish_Taiwan=157,

		/** English as appropriate for use in Hong Kong. */
		ELangEnglish_HongKong=158,

		/** English as appropriate for use in the Peoples Republic of China. */
		ELangEnglish_Prc=159,

		/** English as appropriate for use in Japan. */
		ELangEnglish_Japan=160,

		/** English as appropriate for use in Thailand. */
		ELangEnglish_Thailand=161,

		/** Fulfulde, also known as Fula */
		ELangFulfulde = 162,

		/** Tamazight. */
		ELangTamazight = 163,

		/** Bolivian Quechua. */
		ELangBolivianQuechua = 164,

		/** Peru Quechua. */
		ELangPeruQuechua = 165,

		/** Ecuador Quechua. */
		ELangEcuadorQuechua = 166,

		/** Tajik(Cyrillic alphabet). */
		ELangTajik_Cyrillic = 167,

		/** Tajik(using Perso-Arabic script). */
		ELangTajik_PersoArabic = 168,

		/** Nyanja, also known as Chichewa or Chewa. */
		ELangNyanja = 169,

		/** Haitian Creole. */
		ELangHaitianCreole = 170,

		/** Lombard. */
		ELangLombard = 171,

		/** Koongo, also known as Kongo or KiKongo. */
		ELangKoongo = 172, 

		/** Akan. */
		ELangAkan = 173,

		/** Hmong. */
		ELangHmong = 174,

		/** Yi. */
		ELangYi = 175,

		/** Tshiluba, also known as Luba-Kasai */
		ELangTshiluba = 176,

		/** Ilocano, also know as Ilokano or Iloko. */
		ELangIlocano = 177,

		/** Uyghur. */
		ELangUyghur = 178,

		/** Neapolitan. */
		ELangNeapolitan = 179,

		/** Rwanda, also known as Kinyarwanda */
		ELangRwanda = 180,

		/** Xhosa. */
		ELangXhosa = 181,

		/** Balochi, also known as Baluchi */
		ELangBalochi = 182,

		/** Hiligaynon. */
		ElangHiligaynon = 183,

		/** Minangkabau. */
		ELangMinangkabau = 184,

		/** Makhuwa. */
		ELangMakhuwa = 185,

		/** Santali. */
		ELangSantali = 186,

		/** Gikuyu, sometimes written Kikuyu. */
		ELangGikuyu = 187,

		/** Mòoré, also known as Mossi or More. */
		ELangMoore = 188,

		/** Guaraní. */
		ELangGuarani = 189, 

		/** Rundi, also known as Kirundi. */
		ELangRundi = 190,

		/** Romani(Latin alphabet). */
		ELangRomani_Latin = 191,

		/** Romani(Cyrillic alphabet). */
		ELangRomani_Cyrillic = 192,

		/** Tswana. */
		ELangTswana = 193,

		/** Kanuri. */
		ELangKanuri = 194,

		/** Kashmiri(using Devanagari script). */
		ELangKashmiri_Devanagari = 195,

		/** Kashmiri(using Perso-Arabic script). */
		ELangKashmiri_PersoArabic = 196,

		/** Umbundu. */
		ELangUmbundu = 197,

		/** Konkani. */
		ELangKonkani = 198,

		/** Balinese, a language used in Indonesia (Java and Bali). */
		ELangBalinese = 199,

		/** Northern Sotho. */
		ELangNorthernSotho = 200,

		/** Wolof. */
		ELangWolof = 201,

		/** Bemba. */
		ELangBemba = 202,

		/** Tsonga. */
		ELangTsonga = 203,

		/** Yiddish. */
		ELangYiddish = 204,

		/** Kirghiz, also known as Kyrgyz. */
		ELangKirghiz = 205,

		/** Ganda, also known as Luganda. */
		ELangGanda = 206,

		/** Soga, also known as Lusoga. */
		ELangSoga = 207,

		/** Mbundu, also known as Kimbundu. */
		ELangMbundu = 208,

		/** Bambara. */
		ELangBambara = 209,

		/** Central Aymara. */
		ELangCentralAymara = 210,

		/** Zarma. */
		ELangZarma = 211,

		/** Lingala. */
		ELangLingala = 212,

		/** Bashkir. */
		ELangBashkir = 213,

		/** Chuvash. */
		ELangChuvash = 214,

		/** Swati. */
		ELangSwati = 215,

		/** Tatar. */
		ELangTatar = 216,

		/** Southern Ndebele. */
		ELangSouthernNdebele = 217,

		/** Sardinian. */
		ELangSardinian = 218,

		/** Scots. */
		ELangScots = 219,

		/** Meitei, also known as Meithei or Manipuri */
		ELangMeitei = 220,

		/** Walloon. */
		ELangWalloon = 221,

		/** Kabardian. */
		ELangKabardian = 222,

		/** Mazanderani, also know as Mazandarani or Tabri. */
		ELangMazanderani = 223,

		/** Gilaki. */
		ELangGilaki = 224,

		/** Shan. */
		ELangShan = 225,

		/** Luyia. */
		ELangLuyia = 226,

		/** Luo, also known as Dholuo, a language of Kenya. */
		ELanguageLuo = 227,

		/** Sukuma, also known as Kisukuma. */
		ELangSukuma = 228,

		/** Aceh, also known as Achinese. */
		ELangAceh = 229,

		/** English used in India. */
		ELangEnglish_India = 230,

		/** Malay as appropriate for use in Asia-Pacific regions. */
		ELangMalay_Apac=326,

		/** Indonesian as appropriate for use in Asia-Pacific regions. */
		ELangIndonesian_Apac=327,
		
		ELangNone = 0xFFFF,
		ELangMaximum = ELangNone // This must always be equal to the last (largest) TLanguage enum.
		}
	TLanguage;

	typedef CSISUInt32 TDialect;

public:
	CSISLanguage ();
	CSISLanguage (const TDialect aLan);
	CSISLanguage (const CSISLanguage& aInitialiser);

	virtual std::string Name () const;
#ifdef GENERATE_ERRORS
	virtual void CreateDefects ();
#endif // GENERATE_ERRORS

	operator TUint32 () const;
	CSISLanguage& operator = (const TUint32 aLanguage);
	TLanguage Value () const;

	static TDialect IdentifyLanguage (const std::wstring& aIdentifier);
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;
	
private:
	const wchar_t* GetLanguageCode(TDialect& aDialect) const;

private:
	TDialect iLanguage;

	};




inline CSISLanguage::CSISLanguage ()
	{
	InsertMember (iLanguage);
	}


inline CSISLanguage::CSISLanguage (const TDialect aLan) :
		iLanguage (aLan)
	{
	InsertMember (iLanguage);
	}


inline CSISLanguage::CSISLanguage (const CSISLanguage& aInitialiser) :
		CStructure <CSISFieldRoot::ESISLanguage> (aInitialiser),
		iLanguage (aInitialiser.iLanguage)
	{
	InsertMember (iLanguage);
	}


inline CSISLanguage::operator TUint32 () const
	{
	return static_cast <TUint32> (iLanguage);
	}


inline CSISLanguage& CSISLanguage::operator = (const TUint32 aLanguage)
	{
	iLanguage = static_cast <TLanguage> (aLanguage);
	return *this;
	}


inline CSISLanguage::TDialect operator + (const CSISLanguage::TDialect aLHS, const CSISLanguage::TDialect aRHS)
	{
		return static_cast <CSISLanguage::TDialect> (static_cast <TUint32> (aLHS) + static_cast <TUint32> (aRHS));
	}

inline bool operator == (const CSISLanguage::TDialect dialect, const CSISLanguage::TLanguage language)
	{
	return dialect.Value () == static_cast <TUint32> (language);
	}

#ifdef GENERATE_ERRORS
inline void CSISLanguage::CreateDefects ()
	{
	if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugInvalidValues))
		{
		iLanguage = static_cast <TLanguage> (rand ());
		}
	}
#endif // GENERATE_ERRORS

inline CSISLanguage::TLanguage CSISLanguage::Value () const
	{
	return static_cast <CSISLanguage::TLanguage> (iLanguage.Value ());
	}

#endif // __SISLANGUAGE_H__

