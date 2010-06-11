/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "registrytoken.h"

RegistryToken::RegistryToken()
	:iSISControllerInfo(0),
	iDrives (0),
	iCompletelyPresent (0),
	iSidCount (0),
	iSISControllerCount (0),
	iMajor (0),
	iMinor (0),
	iBuild (0),
	iLanguage (0),
	iSelectedDrive (0),
	iUnused1 (0),
	iUnused2 (0)
	{
	iLanguages[0]="Test";
	iLanguages[1]="English";
	iLanguages[2]="French";
	iLanguages[3]="German";
	iLanguages[4]="Spanish";
	iLanguages[5]="Italian";
	iLanguages[6]="Swedish";
	iLanguages[7]="Danish";
	iLanguages[8]="Norwegian";
	iLanguages[9]="Finnish";
	iLanguages[10]="American";
	iLanguages[11]="SwissFrench";
	iLanguages[12]="SwissGerman";
	iLanguages[13]="Portuguese";
	iLanguages[14]="Turkish";
	iLanguages[15]="Icelandic";
	iLanguages[16]="Russian";
	iLanguages[17]="Hungarian";
	iLanguages[18]="Dutch";
	iLanguages[19]="BelgianFlemish";
	iLanguages[20]="Australian";
	iLanguages[21]="BelgianFrench";
	iLanguages[22]="Austrian";
	iLanguages[23]="NewZealand";
	iLanguages[24]="InternationalFrench";
	iLanguages[25]="Czech";
	iLanguages[26]="Slovak";
	iLanguages[27]="Polish";
	iLanguages[28]="Slovenian";
	iLanguages[29]="TaiwanChinese";
	iLanguages[30]="HongKongChinese";
	iLanguages[31]="PrcChinese";
	iLanguages[32]="Japanese";
	iLanguages[33]="Thai";
	iLanguages[34]="Afrikaans";
	iLanguages[35]="Albanian";
	iLanguages[36]="Amharic";
	iLanguages[37]="Arabic";
	iLanguages[38]="Armenian";
	iLanguages[39]="Tagalog";
	iLanguages[40]="Belarussian";
	iLanguages[41]="Bengali";
	iLanguages[42]="Bulgarian";
	iLanguages[43]="Burmese";
	iLanguages[44]="Catalan";
	iLanguages[45]="Croatian";
	iLanguages[46]="CanadianEnglish";
	iLanguages[47]="InternationalEnglish";
	iLanguages[48]="SouthAfricanEnglish";
	iLanguages[49]="Estonian";
	iLanguages[50]="Farsi";
	iLanguages[51]="CanadianFrench";
	iLanguages[52]="ScotsGaelic";
	iLanguages[53]="Georgian";
	iLanguages[54]="Greek";
	iLanguages[55]="CyprusGreek";
	iLanguages[56]="Gujarati";
	iLanguages[57]="Hebrew";
	iLanguages[58]="Hindi";
	iLanguages[59]="Indonesian";
	iLanguages[60]="Irish";
	iLanguages[61]="SwissItalian";
	iLanguages[62]="Kannada";
	iLanguages[63]="Kazakh";
	iLanguages[64]="Khmer";
	iLanguages[65]="Korean";
	iLanguages[66]="Lao";
	iLanguages[67]="Latvian";
	iLanguages[68]="Lithuanian";
	iLanguages[69]="Macedonian";
	iLanguages[70]="Malay";
	iLanguages[71]="Malayalam";
	iLanguages[72]="Marathi";
	iLanguages[73]="Moldavian";
	iLanguages[74]="Mongolian";
	iLanguages[75]="NorwegianNynorsk";
	iLanguages[76]="BrazilianPortuguese";
	iLanguages[77]="Punjabi";
	iLanguages[78]="Romanian";
	iLanguages[79]="Serbian";
	iLanguages[80]="Sinhalese";
	iLanguages[81]="Somali";
	iLanguages[82]="InternationalSpanish";
	iLanguages[83]="LatinAmericanSpanish";
	iLanguages[84]="Swahili";
	iLanguages[85]="FinlandSwedish";
	iLanguages[86]="Reserved1";
	iLanguages[87]="Tamil";
	iLanguages[88]="Telugu";
	iLanguages[89]="Tibetan";
	iLanguages[90]="Tigrinya";
	iLanguages[91]="CyprusTurkish";
	iLanguages[92]="Turkmen";
	iLanguages[93]="Ukrainian";
	iLanguages[94]="Urdu";
	iLanguages[95]="Reserved2";
	iLanguages[96]="Vietnamese";
	iLanguages[97]="Welsh";
	iLanguages[98]="Zulu";
	iLanguages[99]="Other";
	}

void RegistryToken::ExtractRegistryToken(StreamReader& aReader , RegistryPackage& aPackage)
	{
	aPackage.ExtractPackage(aReader);
	iDrives = aReader.ReadInt32(); 
	iCompletelyPresent = aReader.ReadInt32();
	iSidCount = aReader.ReadInt32();
	for (int i=0 ; i < iSidCount ; i++)
		{
		int sid = aReader.ReadInt32();
		}

	iSISControllerCount = aReader.ReadInt32();
	iSISControllerInfo = new SISControllerInfo[iSISControllerCount];
	
	for(int j=0;j<iSISControllerCount;j++)
		{
		iSISControllerInfo[j].ExtractControllerInfo(aReader);
		}

	iMajor = aReader.ReadInt32();
	iMinor = aReader.ReadInt32();
	iBuild = aReader.ReadInt32();
	iLanguage = aReader.ReadInt32();
	iSelectedDrive = aReader.ReadInt32();
	iUnused1 = aReader.ReadInt32();
	iUnused2 = aReader.ReadInt32();
	}

void RegistryToken::DisplayRegistryToken()
	{
	cout << "Drives :" << iDrives << endl;
	cout << "No of executables :" << iSidCount << endl;
	cout << "Package Version :" << iMajor << "." << iMinor << "." << iBuild << endl;
	if(iLanguages.find(iLanguage) != iLanguages.end())
		{
		cout << "Languages :" <<iLanguages[iLanguage] << endl;
		}
	else
		{
		cout << "Languages :" << "Unknown" << endl;
		}

	if(iSelectedDrive >= 65 && iSelectedDrive <= 90)
		{
		cout << "Selected Drive :" <<(char)iSelectedDrive << endl;
		}
	else
		{
		cout << "Selected Drive :" << "No Drive Selected" << endl;
		}

	}

RegistryToken::~RegistryToken()
	{
	delete[] iSISControllerInfo;
	}