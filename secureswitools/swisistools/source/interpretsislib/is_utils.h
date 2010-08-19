/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @file IS_UTILS.H
 @internalComponent
 @released
*/

#ifndef	IS_UTILS_H
#define	IS_UTILS_H

#pragma warning (disable: 4786)

#include <string>
#include <list>
#include "sislanguage.h"

#include "symbiantypes.h"
// Seclib.h special handling!
enum TCapability
	{
	ECapability_HardLimit			= 255
	};

// Constants for seclib
const TInt KCapabilitySetMaxSize	= (((TInt)ECapability_HardLimit + 7)>>3);
const int maxFileSize = 1024;
#include <seclib.h>

struct stat;

#ifdef _MSC_VER
	#include "utils_win32.h"
#else 
	#include <dirent.h>
#endif 


	std::string&  Ucs2ToUtf8(const std::wstring& aStr, std::string& aOut);
	std::wstring& Utf8ToUcs2(const std::string& aStr, std::wstring& aOut);

	std::string Ucs2ToUtf8(const std::wstring& aStr);
	std::wstring Utf8ToUcs2(const std::string& aStr);

	bool FileExists(const std::wstring& aFile);
	bool RemoveFile(const std::wstring& aFile);
	void RemoveHashForFile(const std::wstring& aFile, const int aDriveLetter, const std::wstring& aPath);
	bool MakeDir(const std::wstring& aDir);
	bool OpenFile(const std::wstring& aFile, std::fstream& aStream, std::ios_base::open_mode mode);

	bool IsDirectory(std::wstring& path);

	void GetDirContents(const std::wstring& path, 
						std::list<std::wstring>& contents);

	int GetAugmentationsNumber(const std::wstring& aDrivePath);

	bool CheckSisRegistryDirPresent(const std::wstring& aDrivePath, TUint32 aUid);

	int GetStat(const std::wstring& aFile, struct stat*);

	void ConvertToPlatformSpecificPath( TUint16* aIn, TUint32 len );
	#ifdef __TOOLS2_LINUX__
	void ConvertToForwardSlash( std::wstring& aIn);
	#endif
	void ConvertToLocalPath(
		std::wstring& aIn,
		const std::wstring& aDrivePath);

	int FoldedCompare(const std::wstring& aLeft, const std::wstring& aRight);

	int ReadSecurityInfo( SBinarySecurityInfo& aInfo, const std::wstring aFileName );

namespace Utils
	{
	 const TInt KMaxDowngradeLanguages = 16;
	 typedef CSISLanguage::TLanguage TLanguagePath[KMaxDowngradeLanguages + 1];
	 const CSISLanguage::TLanguage dp0[] = { CSISLanguage::ELangCanadianEnglish, CSISLanguage::ELangAmerican,CSISLanguage::ELangEnglish, CSISLanguage::ELangEnglish_Apac,CSISLanguage::ELangEnglish_Taiwan,CSISLanguage::ELangEnglish_HongKong,CSISLanguage::ELangEnglish_Prc,CSISLanguage::ELangEnglish_Japan,CSISLanguage::ELangEnglish_Thailand,CSISLanguage::ELangEnglish_India,CSISLanguage::ELangAustralian,CSISLanguage::ELangNewZealand,CSISLanguage::ELangInternationalEnglish,CSISLanguage::ELangSouthAfricanEnglish,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp1[] = { CSISLanguage::ELangAmerican, CSISLanguage::ELangEnglish,CSISLanguage::ELangCanadianEnglish, CSISLanguage::ELangEnglish_Apac,CSISLanguage::ELangEnglish_Taiwan,CSISLanguage::ELangEnglish_HongKong,CSISLanguage::ELangEnglish_Prc,CSISLanguage::ELangEnglish_Japan,CSISLanguage::ELangEnglish_Thailand,CSISLanguage::ELangEnglish_India,CSISLanguage::ELangAustralian,CSISLanguage::ELangNewZealand,CSISLanguage::ELangInternationalEnglish,CSISLanguage::ELangSouthAfricanEnglish, CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp2[] = { CSISLanguage::ELangAustralian, CSISLanguage::ELangEnglish, CSISLanguage::ELangAmerican, CSISLanguage::ELangEnglish_Apac,CSISLanguage::ELangEnglish_Taiwan,CSISLanguage::ELangEnglish_HongKong,CSISLanguage::ELangEnglish_Prc,CSISLanguage::ELangEnglish_Japan,CSISLanguage::ELangEnglish_Thailand,CSISLanguage::ELangEnglish_India,CSISLanguage::ELangNewZealand,CSISLanguage::ELangInternationalEnglish,CSISLanguage::ELangSouthAfricanEnglish, CSISLanguage::ELangCanadianEnglish,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp3[] = { CSISLanguage::ELangSouthAfricanEnglish, CSISLanguage::ELangEnglish, CSISLanguage::ELangAustralian, CSISLanguage::ELangAmerican, CSISLanguage::ELangEnglish_Apac,CSISLanguage::ELangEnglish_Taiwan,CSISLanguage::ELangEnglish_HongKong,CSISLanguage::ELangEnglish_Prc,CSISLanguage::ELangEnglish_Japan,CSISLanguage::ELangEnglish_Thailand,CSISLanguage::ELangEnglish_India,CSISLanguage::ELangNewZealand,CSISLanguage::ELangInternationalEnglish,CSISLanguage::ELangCanadianEnglish,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp4[] = { CSISLanguage::ELangInternationalEnglish, CSISLanguage::ELangEnglish, CSISLanguage::ELangAustralian, CSISLanguage::ELangAmerican, CSISLanguage::ELangEnglish_Apac,CSISLanguage::ELangEnglish_Taiwan,CSISLanguage::ELangEnglish_HongKong,CSISLanguage::ELangEnglish_Prc,CSISLanguage::ELangEnglish_Japan,CSISLanguage::ELangEnglish_Thailand,CSISLanguage::ELangEnglish_India,CSISLanguage::ELangNewZealand,CSISLanguage::ELangSouthAfricanEnglish,CSISLanguage::ELangCanadianEnglish,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp5[] = { CSISLanguage::ELangEnglish_Apac, CSISLanguage::ELangEnglish, CSISLanguage::ELangAustralian, CSISLanguage::ELangAmerican,CSISLanguage::ELangInternationalEnglish,CSISLanguage::ELangEnglish_Taiwan,CSISLanguage::ELangEnglish_HongKong,CSISLanguage::ELangEnglish_Prc,CSISLanguage::ELangEnglish_Japan,CSISLanguage::ELangEnglish_Thailand,CSISLanguage::ELangEnglish_India,CSISLanguage::ELangNewZealand,CSISLanguage::ELangSouthAfricanEnglish,CSISLanguage::ELangCanadianEnglish,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp6[] = { CSISLanguage::ELangEnglish_Taiwan, CSISLanguage::ELangEnglish, CSISLanguage::ELangAustralian, CSISLanguage::ELangAmerican, CSISLanguage::ELangEnglish_Apac,CSISLanguage::ELangInternationalEnglish,CSISLanguage::ELangEnglish_HongKong,CSISLanguage::ELangEnglish_Prc,CSISLanguage::ELangEnglish_Japan,CSISLanguage::ELangEnglish_Thailand,CSISLanguage::ELangEnglish_India,CSISLanguage::ELangNewZealand,CSISLanguage::ELangSouthAfricanEnglish,CSISLanguage::ELangCanadianEnglish,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp7[] = { CSISLanguage::ELangEnglish_HongKong, CSISLanguage::ELangEnglish, CSISLanguage::ELangAustralian, CSISLanguage::ELangAmerican, CSISLanguage::ELangEnglish_Apac,CSISLanguage::ELangEnglish_Taiwan,CSISLanguage::ELangInternationalEnglish,CSISLanguage::ELangEnglish_Prc,CSISLanguage::ELangEnglish_Japan,CSISLanguage::ELangEnglish_Thailand,CSISLanguage::ELangEnglish_India,CSISLanguage::ELangNewZealand,CSISLanguage::ELangSouthAfricanEnglish,CSISLanguage::ELangCanadianEnglish,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp8[] = { CSISLanguage::ELangEnglish_Prc, CSISLanguage::ELangEnglish, CSISLanguage::ELangAustralian, CSISLanguage::ELangAmerican, CSISLanguage::ELangEnglish_Apac,CSISLanguage::ELangEnglish_Taiwan,CSISLanguage::ELangEnglish_HongKong,CSISLanguage::ELangInternationalEnglish,CSISLanguage::ELangEnglish_Japan,CSISLanguage::ELangEnglish_Thailand,CSISLanguage::ELangEnglish_India,CSISLanguage::ELangNewZealand,CSISLanguage::ELangSouthAfricanEnglish,CSISLanguage::ELangCanadianEnglish,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp9[] = { CSISLanguage::ELangEnglish_Japan, CSISLanguage::ELangEnglish, CSISLanguage::ELangAustralian, CSISLanguage::ELangAmerican, CSISLanguage::ELangEnglish_Apac,CSISLanguage::ELangEnglish_Taiwan,CSISLanguage::ELangEnglish_HongKong,CSISLanguage::ELangEnglish_Prc,CSISLanguage::ELangInternationalEnglish,CSISLanguage::ELangEnglish_Thailand,CSISLanguage::ELangEnglish_India,CSISLanguage::ELangNewZealand,CSISLanguage::ELangSouthAfricanEnglish,CSISLanguage::ELangCanadianEnglish,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp10[] = { CSISLanguage::ELangEnglish_Thailand, CSISLanguage::ELangEnglish, CSISLanguage::ELangAustralian, CSISLanguage::ELangAmerican, CSISLanguage::ELangEnglish_Apac,CSISLanguage::ELangEnglish_Taiwan,CSISLanguage::ELangEnglish_HongKong,CSISLanguage::ELangEnglish_Prc,CSISLanguage::ELangEnglish_Japan,CSISLanguage::ELangInternationalEnglish,CSISLanguage::ELangEnglish_India,CSISLanguage::ELangNewZealand,CSISLanguage::ELangSouthAfricanEnglish,CSISLanguage::ELangCanadianEnglish,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp11[] = { CSISLanguage::ELangEnglish_India, CSISLanguage::ELangEnglish, CSISLanguage::ELangAustralian, CSISLanguage::ELangAmerican, CSISLanguage::ELangEnglish_Apac,CSISLanguage::ELangEnglish_Taiwan,CSISLanguage::ELangEnglish_HongKong,CSISLanguage::ELangEnglish_Prc,CSISLanguage::ELangEnglish_Japan,CSISLanguage::ELangEnglish_Thailand,CSISLanguage::ELangInternationalEnglish,CSISLanguage::ELangNewZealand,CSISLanguage::ELangSouthAfricanEnglish,CSISLanguage::ELangCanadianEnglish,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp12[] = { CSISLanguage::ELangNewZealand, CSISLanguage::ELangEnglish, CSISLanguage::ELangAmerican, CSISLanguage::ELangEnglish_Apac,CSISLanguage::ELangEnglish_Taiwan,CSISLanguage::ELangEnglish_HongKong,CSISLanguage::ELangEnglish_Prc,CSISLanguage::ELangEnglish_Japan,CSISLanguage::ELangEnglish_Thailand,CSISLanguage::ELangEnglish_India,CSISLanguage::ELangAustralian,CSISLanguage::ELangInternationalEnglish,CSISLanguage::ELangSouthAfricanEnglish, CSISLanguage::ELangCanadianEnglish,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp13[] = { CSISLanguage::ELangInternationalFrench,CSISLanguage::ELangFrench,CSISLanguage::ELangSwissFrench,CSISLanguage::ELangBelgianFrench,CSISLanguage::ELangCanadianFrench,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp14[] = { CSISLanguage::ELangBelgianFrench, CSISLanguage::ELangFrench,CSISLanguage::ELangInternationalFrench,CSISLanguage::ELangSwissFrench,CSISLanguage::ELangCanadianFrench,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp15[] = { CSISLanguage::ELangCanadianFrench, CSISLanguage::ELangFrench,CSISLanguage::ELangInternationalFrench,CSISLanguage::ELangSwissFrench,CSISLanguage::ELangBelgianFrench,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp16[] = { CSISLanguage::ELangFrench,CSISLanguage::ELangInternationalFrench,CSISLanguage::ELangSwissFrench,CSISLanguage::ELangBelgianFrench,CSISLanguage::ELangCanadianFrench,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp17[] = { CSISLanguage::ELangSwissFrench,CSISLanguage::ELangFrench,CSISLanguage::ELangInternationalFrench,CSISLanguage::ELangBelgianFrench,CSISLanguage::ELangCanadianFrench,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp18[] = { CSISLanguage::ELangSwissGerman,CSISLanguage::ELangGerman,CSISLanguage::ELangAustrian,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp19[] = { CSISLanguage::ELangAustrian,CSISLanguage::ELangGerman,CSISLanguage::ELangSwissGerman,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp20[] = { CSISLanguage::ELangGerman,CSISLanguage::ELangSwissGerman,CSISLanguage::ELangAustrian,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp21[] = { CSISLanguage::ELangSerbian,CSISLanguage::ELangCroatian,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp22[] = { CSISLanguage::ELangCroatian,CSISLanguage::ELangSerbian,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp23[] = { CSISLanguage::ELangRomanian,CSISLanguage::ELangMoldavian,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp24[] = { CSISLanguage::ELangMoldavian,CSISLanguage::ELangRomanian,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp25[] = { CSISLanguage::ELangBelgianFlemish,CSISLanguage::ELangDutch,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp26[] = { CSISLanguage::ELangDutch,CSISLanguage::ELangBelgianFlemish,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp27[] = { CSISLanguage::ELangAfrikaans,CSISLanguage::ELangDutch,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp28[] = { CSISLanguage::ELangMalay_Apac,CSISLanguage::ELangMalay,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp29[] = { CSISLanguage::ELangIndonesian_Apac,CSISLanguage::ELangIndonesian,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp30[] = { CSISLanguage::ELangSpanish,CSISLanguage::ELangInternationalSpanish,CSISLanguage::ELangLatinAmericanSpanish,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp31[] = { CSISLanguage::ELangLatinAmericanSpanish,CSISLanguage::ELangSpanish,CSISLanguage::ELangInternationalSpanish,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp32[] = { CSISLanguage::ELangInternationalSpanish,CSISLanguage::ELangSpanish,CSISLanguage::ELangLatinAmericanSpanish,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp33[] = { CSISLanguage::ELangCyprusGreek,CSISLanguage::ELangGreek,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp34[] = { CSISLanguage::ELangGreek,CSISLanguage::ELangCyprusGreek,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp35[] = { CSISLanguage::ELangSwissItalian,CSISLanguage::ELangItalian,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp36[] = { CSISLanguage::ELangItalian,CSISLanguage::ELangSwissItalian,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp37[] = { CSISLanguage::ELangBrazilianPortuguese,CSISLanguage::ELangPortuguese,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp38[] = { CSISLanguage::ELangPortuguese,CSISLanguage::ELangBrazilianPortuguese,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp39[] = { CSISLanguage::ELangFinlandSwedish,CSISLanguage::ELangSwedish,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp40[] = { CSISLanguage::ELangSwedish,CSISLanguage::ELangFinlandSwedish,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp41[] = { CSISLanguage::ELangCyprusTurkish,CSISLanguage::ELangTurkish,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp42[] = { CSISLanguage::ELangTurkish,CSISLanguage::ELangCyprusTurkish,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp43[] = { CSISLanguage::ELangHongKongChinese, CSISLanguage::ELangTaiwanChinese, CSISLanguage::ELangPrcChinese,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp44[] = { CSISLanguage::ELangTaiwanChinese, CSISLanguage::ELangHongKongChinese,CSISLanguage::ELangPrcChinese,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage dp45[] = { CSISLanguage::ELangPrcChinese, CSISLanguage::ELangHongKongChinese, CSISLanguage::ELangTaiwanChinese,CSISLanguage::ELangNone };
	 const CSISLanguage::TLanguage * const KEquivalentLists[] = { dp0,  dp1,  dp2,  dp3,  dp4,  dp5,  dp6,  
			dp7,  dp8,  dp9,  dp10,  dp11,  dp12,  dp13,  dp14,  dp15,  dp16,  dp17,
			dp18,  dp19,  dp20,  dp21,  dp22,  dp23,  dp24,  dp25,  dp26,  dp27,  
			dp28,  dp29,  dp30,  dp31,  dp32,  dp33,  dp34,  dp35,  dp36,  dp37,  
			dp38,  dp39,  dp40,  dp41,  dp42,  dp43,  dp44,  dp45};
	 
	void GetEquivalentLanguageList(CSISLanguage::TLanguage aLang, TLanguagePath& aEquivalents); 

	const std::wstring IntegerToWideString(int aInt);

	std::wstring Int64ToWideString(TInt64 aInt);

	int WideStringToInteger(const std::wstring& aWideString);

	std::string Base64Encode( std::string aBinaryData );
	};

#endif	/* IS_UTILS_H */
