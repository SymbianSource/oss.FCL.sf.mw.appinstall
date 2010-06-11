/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* apprscreader.h 
*
*/

#ifndef __APPRSCPARSER_H__
#define __APPRSCPARSER_H__

#include <usif/scr/appregentries.h>

class RFs;
class CResourceFile;
class RResourceReader;

/**
 * @file
 * Application resource file parser for SWI
 *
 * @internalTechnology
 * @prototype
*/

const TInt KMaxAppGroupName=0x10;
const TInt KMaximumDataTypeLength=256;
typedef TBuf<KMaxAppGroupName> TAppGroupName;

/** Defines an application's attributes as a set of bit flags. */
enum TCapabilityAttribute
    {    
    EBuiltAsDll         = 0x00000001,
    EControlPanelItem   = 0x00000002,
    ENonNative          = 0x00000004
    };

class CLocalizableRsc : public CBase
    {
public:
    static CLocalizableRsc* NewL();
    static CLocalizableRsc* NewLC();
    ~CLocalizableRsc();
    
private:
    CLocalizableRsc() {}
    
public:
    TFileName iFileName;
    CResourceFile* iRscFile;
    TLanguage iLanguage;
    };

/**
 This class reads application information from a combination of registration file,
 optional localizable resource file and optional icon file.
*/
class CAppRegInfoReader : public CBase
	{
public:
    IMPORT_C static CAppRegInfoReader* NewL(RFs& aFs, const TDesC& aRegistrationFileName);
    IMPORT_C static CAppRegInfoReader* NewL(RFs& aFs, const RFile& aRegistrationFile);
    IMPORT_C Usif::CApplicationRegistrationData* ReadL(const RArray<TLanguage>& aAppLanguages);    
    IMPORT_C ~CAppRegInfoReader();

private:
    CAppRegInfoReader(RFs& aFs, const TDesC& aRegistrationFileName);
    CAppRegInfoReader(RFs& aFs, const RFile& aRegistrationFile);
	void ConstructL();
	void ReadMandatoryInfoL(RResourceReader& aResourceReader);
	void ReadNonLocalizableInfoL(RResourceReader& aResourceReader, TUint& aLocalizableResourceId, const RArray<TLanguage>& aAppLanguages);
	void ReadNonLocalizableOptionalInfoL(RResourceReader& aResourceReader, const CResourceFile* aRegistrationFile);
	void ReadMimeTypesSupportedL(RResourceReader& aResourceReader, RPointerArray<Usif::CDataType>& aDataTypes);
	void ReadLocalizableInfoL(const CResourceFile& aResourceFile, TUint aResourceId, TLanguage aLanguage);
	void ReadOpaqueDataL(TUint aResourceId, const CResourceFile* aRegistrationFile, RPointerArray<Usif::COpaqueData>& aOpaqueDataArray);
	HBufC* CreateFullIconFileNameL(const TDesC& aIconFileName) const;
	void FindLocalizableResourceFilesL(const TLanguage& aApplicationLanguage);
	void CheckForDefaultResourceFileL();
	TBool TypeUidIsForRegistrationFile(const TUidType& aUidType);    
     
    static void Panic(TInt aPanic);
	
public:
	//From ReadMandatoryInfoL
	HBufC* iAppBinaryFullName;
	//From ReadNonLocalizableInfoL
	Usif::TApplicationCharacteristics iAppCharacteristics;
    TUint iDefaultScreenNumber;
    RPointerArray<HBufC> iOwnedFileArray;
    //From ReadNonLocalizableOptionalInfoL
    RPointerArray<Usif::CServiceInfo> iServiceArray; 
    RPointerArray<Usif::COpaqueData> iOpaqueDataArray;
    //From ReadLocalizableInfoL
    RPointerArray<Usif::CLocalizableAppInfo> iLocalizableAppInfoArray;    
            	
private:
    RFs& iFs;
    TUid iAppUid;   
    TUidType iAppBinaryUidType;
    TFileName iTempLocalizableRscFileName; // Localizsed resource file name from rsc
    TBool iLegacyDataTypesPresent;
    RPointerArray<CLocalizableRsc> iLocalizableRscArray;    
            
	const TDesC* iRegistrationFileName;
	const RFile* iRegFileHandle;
	TBool iReadOnlyOneLocalizedRscInfo;
    TBool iUseRegFileHandle;
	};

#endif	// __APPRSCPARSER_H__
