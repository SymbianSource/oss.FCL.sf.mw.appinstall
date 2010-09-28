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
* apprscreader.cpp 
*
*/

#include "apprscparser.h"
#include "log.h"
#include <bautils.h>
#include <barsc2.h>
#include <barsread2.h>
#include <e32uid.h>
#include "cleanuputils.h"

const TUint KResourceOffsetMask = 0xFFFFF000;
const TUid KOpenServiceUid = { 0x10208DCA };
//const TInt KMaxOpaqueDataLength = 0x1000; TODO: Need to enforce this

_LIT(KAppBinaryPathAndExtension, "\\sys\\bin\\.exe");
_LIT(KAppResourceFileExtension,".rsc");
_LIT(KThreeDigitSuffix,"%03d");
_LIT(KTwoDigitSuffix,"%02d");
_LIT(KApparcFilePath, "*10003a3f*");

const TInt KAppRegistrationInfoResourceId = 1;
// The 2nd UID that defines a resource file as being an application registration resource file.
const TUid KUidAppRegistrationFile = {0x101F8021};
const TInt EPanicNullPointer = 1;
const TInt KAppUidValue16 = 0x100039CE;
const TUid KUidApp={KAppUidValue16};

CLocalizableRsc::~CLocalizableRsc()
    {
    delete iRscFile;
    }

CLocalizableRsc* CLocalizableRsc::NewL()
    {
    CLocalizableRsc *self = CLocalizableRsc::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CLocalizableRsc* CLocalizableRsc::NewLC()
    {
    CLocalizableRsc *self = new(ELeave) CLocalizableRsc();
    CleanupStack::PushL(self);
    return self;
    }

//
// CAppRegInfoReader
//

EXPORT_C CAppRegInfoReader* CAppRegInfoReader::NewL(RFs& aFs, const TDesC& aRegistrationFileName)
	{
	CAppRegInfoReader* self = new(ELeave) CAppRegInfoReader(aFs, aRegistrationFileName);
	return self;
	}

EXPORT_C CAppRegInfoReader* CAppRegInfoReader::NewL(RFs& aFs, const RFile& aRegistrationFile)
    {
    CAppRegInfoReader* self = new(ELeave) CAppRegInfoReader(aFs, aRegistrationFile);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
    return self;
    }

CAppRegInfoReader::CAppRegInfoReader(RFs& aFs, const TDesC& aRegistrationFileName) :
	iFs(aFs),
	iAppUid(TUid::Null()),	
	iRegistrationFileName(&aRegistrationFileName)
	{	
	}

CAppRegInfoReader::CAppRegInfoReader(RFs& aFs, const RFile& aRegistrationFile) :
    iFs(aFs),
    iAppUid(TUid::Null()),
    iRegFileHandle(&aRegistrationFile)
    {
    iUseRegFileHandle = ETrue;	
	}

void CAppRegInfoReader::ConstructL()
	{
	TFileName fileName;
	User::LeaveIfError(iRegFileHandle->FullName(fileName));	
	iRegistrationFileName = fileName.AllocL();	
	}

EXPORT_C CAppRegInfoReader::~CAppRegInfoReader()
	{
	delete iAppBinaryFullName;		
	iLocalizableRscArray.ResetAndDestroy();
	
	if (iUseRegFileHandle)
	    delete iRegistrationFileName; // We had created the filename from the handle
	}

EXPORT_C Usif::CApplicationRegistrationData* CAppRegInfoReader::ReadL(const RArray<TLanguage>& aAppLanguages)
    {
	DEBUG_PRINTF(_L("Reading the application rsc file"));
	TEntry entry;
	if (!iUseRegFileHandle)
	    {
        User::LeaveIfError(iFs.Entry(*iRegistrationFileName, entry));        
	    }
	else
	    {
	    // Reading the TUidType information fron the reg rsc file header
	    TBuf8<sizeof(TCheckedUid)> uidBuf;
	    TInt err = iRegFileHandle->Read(0, uidBuf, sizeof(TCheckedUid));
	    if (err != KErrNone || uidBuf.Size() != sizeof(TCheckedUid))
	        {
	        DEBUG_PRINTF(_L("The file is not a valid registration resource file"));
	        User::Leave(KErrCorrupt);
	        }
	    TCheckedUid uid(uidBuf);
	    entry.iType=uid.UidType();
	    }
	
	if (!TypeUidIsForRegistrationFile(entry.iType))
        {
        DEBUG_PRINTF(_L("The resource file doesn't have any application registration information"));
        User::Leave(KErrCorrupt); // We are only interested in application reg resource files
        }
	
	TUint fileOffset = 0;
	TInt fileLength = 0;
	TUid firstUid(KExecutableImageUid);
	TUid middleUid(KUidApp);
	
	// Read the AppUid from the rsc file
	iAppUid = entry.iType[2];
	if (iAppUid == TUid::Null())
	    {
	    DEBUG_PRINTF2(_L("Application UID in the registration resource file %S is NULL"), iRegistrationFileName);
	    User::Leave(KErrCorrupt); // Mandatory information missing
	    }
	
	// Set the TUidType for the app binary
	iAppBinaryUidType = TUidType(firstUid, middleUid, iAppUid);
	// Check to see if we are only interested in any localized info
	iReadOnlyOneLocalizedRscInfo = aAppLanguages.Count()?EFalse:ETrue;
	
	DEBUG_PRINTF2(_L("Opening rsc file %S"), iRegistrationFileName); //TODO
	CResourceFile* registrationFile = NULL;
	if (iUseRegFileHandle)
	    {
	    fileLength = 0;        
        User::LeaveIfError(iRegFileHandle->Size(fileLength));

        if (fileLength > 0)
            {
            // Read the reg rsc file to a buffer
            HBufC8* fileBuffer = HBufC8::NewLC(fileLength);
            TPtr8 fileBufferPtr(fileBuffer->Des());
            iRegFileHandle->Read(0, fileBufferPtr);
            registrationFile = CResourceFile::NewL(*fileBuffer);
            CleanupStack::PopAndDestroy(fileBuffer);
            }
	    }
	else
	    {
	    registrationFile = CResourceFile::NewL(iFs, *iRegistrationFileName, fileOffset, fileLength);
	    }
	CleanupStack::PushL(registrationFile);
	
	RResourceReader	resourceReader;
	resourceReader.OpenLC(registrationFile, KAppRegistrationInfoResourceId);	
	DEBUG_PRINTF(_L("rsc file opened")); //TODO
	
	TRAPD(err, ReadMandatoryInfoL(resourceReader));
	if (err)
		{
		DEBUG_PRINTF(_L("Error in ReadMandatoryInfoL"));
		CleanupStack::PopAndDestroy(2, registrationFile); // resourceReader
		User::Leave(err); // Might have read something, but failed to setup enough info to make it worthwhile trying to read any more
		}
	
	TUint localizableResourceId = 1; // Only initialising this here to keep the compiler happy
	TRAP(err, ReadNonLocalizableInfoL(resourceReader, localizableResourceId, aAppLanguages));

	if (!err)
	    {
        // Open the localizable resource file for identified languages
        for (TInt i=0; i < iLocalizableRscArray.Count(); ++i)
            {
            iLocalizableRscArray[i]->iRscFile = CResourceFile::NewL(iFs, iLocalizableRscArray[i]->iFileName, 0, 0);
            CResourceFile* currLocalizableFile = iLocalizableRscArray[i]->iRscFile;
	        if (currLocalizableFile && (localizableResourceId & KResourceOffsetMask))
	            currLocalizableFile->ConfirmSignatureL();	        
	        }	    
		TRAP(err, ReadNonLocalizableOptionalInfoL(resourceReader, registrationFile));
		}
		
	if (!err)
	    {
	    // Read the localized resource information the identified languages
	    for (TInt i=0; i < iLocalizableRscArray.Count(); ++i)
	        {
	        CResourceFile* currLocalizableFile = iLocalizableRscArray[i]->iRscFile;
	        TFileName fileName = iLocalizableRscArray[i]->iFileName;
	        DEBUG_PRINTF2(_L("Reading Localizable Info from : %S"), &fileName);
	        // The following call can leave if the localized resource file is ill formed.
	        // We'll try to parse other loclized files (if possible) in such a case.
	        TRAPD(errLocalized, ReadLocalizableInfoL(*currLocalizableFile, localizableResourceId, iLocalizableRscArray[i]->iLanguage));
	        if (errLocalized)
                DEBUG_PRINTF3(_L("Error while reading file (%S) : %d"), &fileName, errLocalized);
	        }
	    }
	
	const TBool readSuccessful = (err == KErrNone);
	DEBUG_PRINTF2(_L("Application rsc file parsed. Status : %d"), readSuccessful);
	CleanupStack::PopAndDestroy(2, registrationFile); // resourceReader	
	
	Usif::CApplicationRegistrationData* appRegInfo = NULL;
	if (readSuccessful)
	    {	    
	    RPointerArray<Usif::CPropertyEntry> appPropertiesArray;	    	    
		appRegInfo = Usif::CApplicationRegistrationData::NewL(iOwnedFileArray, iServiceArray, iLocalizableAppInfoArray, 
                                        appPropertiesArray, iOpaqueDataArray, iAppUid, *iAppBinaryFullName, iAppCharacteristics, iDefaultScreenNumber);
		
		DEBUG_PRINTF2(_L("Count Languages : %d"), aAppLanguages.Count());
		DEBUG_PRINTF2(_L("Count of Loc files parsed : %d"), iLocalizableRscArray.Count());
		DEBUG_PRINTF2(_L("Count of Loc data passed to SWI : %d"), iLocalizableAppInfoArray.Count());				
	   }
	else
	    {
        // Cleanup the member arrays
        iOwnedFileArray.ResetAndDestroy(); 
        iServiceArray.ResetAndDestroy();
        iLocalizableAppInfoArray.ResetAndDestroy();
        iOpaqueDataArray.ResetAndDestroy();
	    }
	
	return appRegInfo;
	}

// This method reads the minimum information required to register an app.
// If this fails, we say the read has been unsuccessful.
void CAppRegInfoReader::ReadMandatoryInfoL(RResourceReader& aResourceReader)
	{
	DEBUG_PRINTF(_L("Reading the mandatory application info"));
	aResourceReader.ReadUint32L(); // skip over LONG reserved_long
	aResourceReader.ReadUint32L(); // skip over LLINK reserved_llink

	// Read LTEXT app_file
	const TPtrC appFile(aResourceReader.ReadTPtrCL());
	// This object gets used for 2 purposes: first to check that a TParsePtrC can be created 
	// over "appFile" without it panicking, and second to construct iAppBinaryFullName
	TParse parse;
	// Do this before creating a TParsePtrC, since TParsePtrC's constructor panics if it fails 
	// (which would provide an easy way for malware to kill the Apparc server)
	User::LeaveIfError(parse.SetNoWild(appFile, NULL, NULL)); 
	const TParsePtrC appFileParser(appFile);

	// Read LONG attributes
	iAppCharacteristics.iAttributes = aResourceReader.ReadUint32L();

	if (!appFileParser.NamePresent())
	    {
	    DEBUG_PRINTF2(_L("Application Name in the registration resource file %S is not present"),iRegistrationFileName);
		User::Leave(KErrCorrupt);
	    }

	const TPtrC appNameWithoutExtension(appFileParser.Name());
	const TPtrC registrationFileDrive(TParsePtrC(*iRegistrationFileName).Drive());
	
	if (iAppCharacteristics.iAttributes & ENonNative)
		{
        User::Leave(KErrNotSupported); // Non native apps need not have an applicaiton reg rsc file
		}
	else if (iAppCharacteristics.iAttributes & EBuiltAsDll)
		{
		User::Leave(KErrNotSupported); // Legacy dll-style app
		}
	else
		{
		// Exe-style app
		User::LeaveIfError(parse.SetNoWild(registrationFileDrive, &KAppBinaryPathAndExtension, &appNameWithoutExtension));
		}

	iAppBinaryFullName = parse.FullName().AllocL();
	DEBUG_PRINTF2(_L("App Binary FullName : %S"),iAppBinaryFullName); //TODO
	DEBUG_PRINTF2(_L("AppUid : 0x%X"), iAppUid.iUid); //TODO
	DEBUG_PRINTF2(_L("Attributes : %d"), iAppCharacteristics.iAttributes); //TODO
	}

HBufC* CAppRegInfoReader::CreateFullIconFileNameL(const TDesC& aIconFileName) const
	{
	HBufC* filename = NULL;
	if (aIconFileName.Length() == 0)
		return NULL;
	
	//aIconFileName may contain a valid string in some format (for eg. URI format) other than path to a regular file on disk
	//and that can be a mbm or non-mbm file. Such a filename will be reported as invalid filename by iFs.IsValidName() method. 
	//aIconFileName will be returned since it is a valid string. 	
	if(!iFs.IsValidName(aIconFileName))
		{
		filename = aIconFileName.AllocL();
		return filename;
		}
	
	TParsePtrC parsePtr(aIconFileName);
	if (parsePtr.IsWild() || !parsePtr.PathPresent() || !parsePtr.NamePresent())
		return NULL;

	// Check for fully qualified icon filename
	if (parsePtr.DrivePresent() && BaflUtils::FileExists(iFs, aIconFileName))
		filename = aIconFileName.AllocL();
	else
		{
		// Check for icon file on same drive as localizable resource file
		TParse parse;
		TPtrC localizableResourceFileDrive = TParsePtrC(iTempLocalizableRscFileName).Drive();
		TInt ret = parse.SetNoWild(localizableResourceFileDrive, &aIconFileName, NULL);
		if (ret == KErrNone && BaflUtils::FileExists(iFs, parse.FullName()))
			filename = parse.FullName().AllocL();
		else
			{
			TPtrC registrationFileDrive = TParsePtrC(*iRegistrationFileName).Drive();
			if (TInt(TDriveUnit(registrationFileDrive)) != TInt(TDriveUnit(localizableResourceFileDrive)))
				{
				// Check for icon file on same drive as registration file
				ret = parse.SetNoWild(registrationFileDrive, &aIconFileName, NULL);
				if (ret == KErrNone && BaflUtils::FileExists(iFs, parse.FullName()))
					filename = parse.FullName().AllocL();
				}
			}
		}
	
	return filename;
	}

void CAppRegInfoReader::ReadLocalizableInfoL(const CResourceFile& aResourceFile, TUint aResourceId, TLanguage aLanguage)
	{
	HBufC* caption = NULL;
    HBufC* shortCaption = NULL;   
    TInt numOfAppIcons = 0;
    HBufC* iconFileName = NULL;
	    
	RResourceReader resourceReader;
	resourceReader.OpenLC(&aResourceFile, aResourceId);

	resourceReader.ReadUint32L(); // skip over LONG reserved_long
	resourceReader.ReadUint32L(); // skip over LLINK reserved_llink
		
	// Read LTEXT short_caption
	shortCaption = resourceReader.ReadHBufCL();
	
	if (shortCaption)
        {
        DEBUG_PRINTF2(_L("ShortCaption : %S"), shortCaption); //TODO
        CleanupStack::PushL(shortCaption);
        }
    else
        {
        DEBUG_PRINTF(_L("ShortCaption is NULL")); //TODO
        }
	
	resourceReader.ReadUint32L(); // skip over LONG reserved_long
	resourceReader.ReadUint32L(); // skip over LLINK reserved_llink

	// Read LTEXT caption	
	caption = resourceReader.ReadHBufCL();
	
	if (caption)
            {
            DEBUG_PRINTF2(_L("Caption : %S"), caption); //TODO
            CleanupStack::PushL(caption);
            }
        else
            {
            DEBUG_PRINTF(_L("Caption is NULL")); //TODO
            }

	// Read WORD number_of_icons
	numOfAppIcons = resourceReader.ReadInt16L();
	DEBUG_PRINTF2(_L("NumOfIcons : %d"), numOfAppIcons); //TODO
	
	// Read LTEXT icon_file
	HBufC* iconFile = resourceReader.ReadHBufCL();
	if (iconFile)
	    {
        if (iReadOnlyOneLocalizedRscInfo)
            iconFileName = iconFile; // We do not need to check if the icon file is present
        else
            {
            CleanupStack::PushL(iconFile);
            iconFileName = CreateFullIconFileNameL(*iconFile);
            CleanupStack::PopAndDestroy(iconFile);
            }
	    }

	if (iconFileName)
	    {
	    DEBUG_PRINTF2(_L("IconFileName : %S"), iconFileName); //TODO
	    CleanupStack::PushL(iconFileName);
	    }
	else
	    {
	    DEBUG_PRINTF(_L("IconFileName is NULL")); //TODO
	    }

	// Create CCaptionAndIconInfo for the locale
	Usif::CCaptionAndIconInfo* captionAndIconInfo = Usif::CCaptionAndIconInfo::NewLC(caption?*caption:_L(""), iconFileName?*iconFileName:_L(""), numOfAppIcons);			
	
	// Read LEN WORD STRUCT view_list[]
	const TInt numOfViews = resourceReader.ReadInt16L();
	
	RPointerArray<Usif::CAppViewData> viewDataList;
	CleanupResetAndDestroyPushL(viewDataList);
	for(TInt view = 0; view < numOfViews; ++view)
		{
		DEBUG_PRINTF2(_L(" *** View Details for index %d ***"), view); //TODO
		resourceReader.ReadUint32L(); // skip over LONG reserved_long
		resourceReader.ReadUint32L(); // skip over LLINK reserved_llink

		// Read LONG uid
		const TUid viewUid = {resourceReader.ReadInt32L()};
		DEBUG_PRINTF2(_L("ViewUid : 0x%X"), viewUid); //TODO
		// Read LONG screen_mode
		const TInt screenMode = {resourceReader.ReadInt32L()};
		DEBUG_PRINTF2(_L("ScreenMode : %d"), screenMode); //TODO
		
		resourceReader.ReadUint32L(); // skip over LONG reserved_long
		resourceReader.ReadUint32L(); // skip over LLINK reserved_llink

		// Read LTEXT caption
		HBufC* viewCaption = resourceReader.ReadHBufCL();
		if (viewCaption)
            {
            DEBUG_PRINTF2(_L("ViewCaption : %S"), viewCaption); //TODO
            CleanupStack::PushL(viewCaption);
            }
        else
            {
            DEBUG_PRINTF(_L("ViewCaption is NULL")); //TODO
            }
		
		// Read WORD number_of_icons
		const TInt numOfViewIcons = resourceReader.ReadInt16L();
		DEBUG_PRINTF2(_L("NumOfViewIcons : %d"), numOfViewIcons); //TODO
		
		HBufC* fullViewIconFileName = NULL;
		// Read LTEXT icon_file
		HBufC* viewIconFile = resourceReader.ReadHBufCL();  
		
        if (viewIconFile)
		    {
		    CleanupStack::PushL(viewIconFile);
            fullViewIconFileName = CreateFullIconFileNameL(*viewIconFile); 
            CleanupStack::PopAndDestroy(viewIconFile);
            if (fullViewIconFileName)
                {               
                CleanupStack::PushL(fullViewIconFileName);
                DEBUG_PRINTF2(_L("ViewIconFileName : %S"), fullViewIconFileName); //TODO
                }
            else
                {
                DEBUG_PRINTF(_L("ViewIconFileName is NULL")); //TODO
                }            
		    }
				
		// Create CCaptionAndIconInfo for the view
		Usif::CCaptionAndIconInfo* viewCaptionAndIcon = Usif::CCaptionAndIconInfo::NewLC(viewCaption?*viewCaption:_L(""), fullViewIconFileName?*fullViewIconFileName:_L(""), numOfViewIcons);
		// Create the view
		Usif::CAppViewData* viewData = Usif::CAppViewData::NewLC(viewUid, screenMode, viewCaptionAndIcon);
		
		viewDataList.AppendL(viewData);
		
		CleanupStack::Pop(2, viewCaptionAndIcon); // viewData
		if (fullViewIconFileName)
		    CleanupStack::PopAndDestroy(fullViewIconFileName);
		if (viewCaption)
		    CleanupStack::PopAndDestroy(viewCaption);
		}

	// Read LTEXT group_name
	TAppGroupName groupName;
	TRAPD(ret, (groupName = resourceReader.ReadTPtrCL()));
	if (ret != KErrNone)
		{
		 if (ret != KErrEof)
	         User::Leave(ret);
		}
	
	DEBUG_PRINTF2(_L("GroupName : %S"), &groupName); //TODO	
	Usif::CLocalizableAppInfo* localizableAppInfo = Usif::CLocalizableAppInfo::NewLC(shortCaption?*shortCaption:_L(""), aLanguage, groupName, captionAndIconInfo, viewDataList);
	iLocalizableAppInfoArray.AppendL(localizableAppInfo);
	
	CleanupStack::Pop(3, captionAndIconInfo); // localizableAppInfo, viewDataList
	if (iconFileName)
	    CleanupStack::PopAndDestroy(iconFileName);
	if (caption)
	    CleanupStack::PopAndDestroy(caption);
	if (shortCaption)
	    CleanupStack::PopAndDestroy(shortCaption);
	CleanupStack::PopAndDestroy(&resourceReader);
	}

void CAppRegInfoReader::ReadOpaqueDataL(TUint aResourceId, const CResourceFile* aRegistrationFile, RPointerArray<Usif::COpaqueData>& aOpaqueDataArray)
    {
    if (aResourceId == 0)
        return;
    else
        {
        if (aResourceId & KResourceOffsetMask)
            {
            for (TInt i=0; i < iLocalizableRscArray.Count(); ++i)
                {
                CResourceFile* currLocalizableFile = iLocalizableRscArray[i]->iRscFile;                
                currLocalizableFile->ConfirmSignatureL();
                HBufC8* data = NULL;
                TRAPD(err, data = currLocalizableFile->AllocReadL(aResourceId));                  
                if(err == KErrNone)
                    {
                    DEBUG_PRINTF3(_L8("Opaque Data read (length %d) from the localizable resouce file : %S"), data->Length(), data);
                    CleanupStack::PushL(data);
                    Usif::COpaqueData* opaqueData = Usif::COpaqueData::NewL(*data, iLocalizableRscArray[i]->iLanguage);
                    aOpaqueDataArray.AppendL(opaqueData);
                    CleanupStack::PopAndDestroy(data);
                    }
                }            
            }
        else
            {
            // Expecting opaque data to be in the registration file
            __ASSERT_ALWAYS(aRegistrationFile, Panic(EPanicNullPointer));
            HBufC8* data = aRegistrationFile->AllocReadLC(aResourceId);
            DEBUG_PRINTF3(_L8("Opaque Data read (length %d) from the registration resouce file : %S"), data->Length(), data);
            Usif::COpaqueData* opaqueData = Usif::COpaqueData::NewL(*data, TLanguage(0));
            aOpaqueDataArray.AppendL(opaqueData);
            CleanupStack::PopAndDestroy(data);
            }
        }
    }

void CAppRegInfoReader::ReadNonLocalizableOptionalInfoL(RResourceReader& aResourceReader, const CResourceFile* aRegistrationFile)
	{
	DEBUG_PRINTF(_L("Reading the application non localized optional info"));
	// Read LEN WORD STRUCT service_list[]
	TInt serviceCount = 0;
	// Service information was not present in the first release of the registration file
	// APP_REGISTRATION_INFO resource struct.
	// This method must not leave if the registration file doesn't contain service information, so the
	// following call to ReadInt16L is trapped to ensure this method doesn't leave just because
	// there is no more information in the resource to read (KErrEof)
	TRAPD(err, serviceCount = aResourceReader.ReadInt16L());
	if (err)
		{
		if (err == KErrEof)
			return; // End of resource reached
		
		User::Leave(err);
		}
	DEBUG_PRINTF2(_L("Service count is : %d"), serviceCount); //TODO
	
	while (serviceCount--)
		{
		const TUid serviceUid = {aResourceReader.ReadUint32L()};
		
		if ((serviceUid == KOpenServiceUid) && (iLegacyDataTypesPresent))
			{
            __ASSERT_DEBUG( iServiceArray.Count(), Panic(EPanicNullPointer) );
			// Deleting the legacy datatypes
            Usif::CServiceInfo* firstElement = iServiceArray[0];
            iServiceArray.Remove(0);
            delete firstElement;
            iLegacyDataTypesPresent = EFalse;
			}
		
		RPointerArray<Usif::CDataType> serviceDataTypes;
		CleanupResetAndDestroyPushL(serviceDataTypes);
		ReadMimeTypesSupportedL(aResourceReader, serviceDataTypes);
		
		const TUint resourceId = aResourceReader.ReadUint32L();
		RPointerArray<Usif::COpaqueData> serviceOpaqueDataArray;
		CleanupResetAndDestroyPushL(serviceOpaqueDataArray);
		ReadOpaqueDataL(resourceId, aRegistrationFile, serviceOpaqueDataArray);
						
		Usif::CServiceInfo* serviceInfo = Usif::CServiceInfo::NewLC(serviceUid, serviceOpaqueDataArray, serviceDataTypes);
		iServiceArray.AppendL(serviceInfo);
		DEBUG_PRINTF3(_L("ServiceUid (index %d) is : 0x%X "), iServiceArray.Count()-1, serviceUid.iUid); //TODO
		
		CleanupStack::Pop(3, &serviceDataTypes); // serviceInfo, serviceOpaqueDataArray
		}
	
	// Read LLINK opaque_data
	const TUint resourceId = aResourceReader.ReadUint32L();
	ReadOpaqueDataL(resourceId, aRegistrationFile, iOpaqueDataArray);
	}

void CAppRegInfoReader::ReadNonLocalizableInfoL(RResourceReader& aResourceReader, TUint& aLocalizableResourceId, const RArray<TLanguage>& aAppLanguages)
	{
	DEBUG_PRINTF(_L("Reading the application non localized info"));
	
	// Read LTEXT localizable_resource_file
	TPtrC localizableResourceFileName(aResourceReader.ReadTPtrCL());
	if (localizableResourceFileName.Length() > 0 && iFs.IsValidName(localizableResourceFileName))
		{
		// Determine the language specific name of the localizable resource file
		TParse parse;
		TParsePtrC parsePtr(*iRegistrationFileName);
		User::LeaveIfError(parse.SetNoWild(parsePtr.Drive(), &KAppResourceFileExtension, &localizableResourceFileName));
		TFileName localizableRscFileName(parse.FullName());
		DEBUG_PRINTF2(_L("Localizable filename from rsc is : %S"), &localizableRscFileName); 
		iTempLocalizableRscFileName = localizableRscFileName; // Store the rsc filename as read (before bafl makes any changes)
		
		// Check if we need to read more than one localized resource file
		if (!aAppLanguages.Count())
		    {
            TLanguage applicationLanguage;		    
            BaflUtils::NearestLanguageFileV2(iFs, localizableRscFileName, applicationLanguage);
		
            // We are able to find a match
            if (BaflUtils::FileExists(iFs, iTempLocalizableRscFileName))
                {
                // We read here only for one locale, similar to AppArc's behaviour when parsing the resource file
                CLocalizableRsc* localizedRsc = CLocalizableRsc::NewL();
                localizedRsc->iFileName = localizableRscFileName;
                if (ELangNone == applicationLanguage)
                    localizedRsc->iLanguage = TLanguage(0);
                else
                    localizedRsc->iLanguage = applicationLanguage;
                iLocalizableRscArray.Append(localizedRsc);
                }
            else 
                {
                // The resource files are located in non-standard location (like in the case of GetComponentInfo).
                // We need to find the temporary paths for localized resource files in this case.
                // The reg file will be in some location like <path>\regfilename_reg.rsc and localized rsc file name
                // would point to \resource\apps\localregfilename.rsc or \resource\apps\localregfilename.r01 etc.
                // Changing the localized rsc file name to <path>\localregfilename.rsc
                TFileName nonStdLocalizedRscFileName = parsePtr.DriveAndPath(); // Drive and path from Registration FileName
                nonStdLocalizedRscFileName.Append(TParsePtrC(localizableRscFileName).NameAndExt()); // Name and extension of localized rsc file
                iTempLocalizableRscFileName = nonStdLocalizedRscFileName; // For FindLocalizableResourceFilesL
                
                if (KErrNotFound == iTempLocalizableRscFileName.Match(KApparcFilePath))
                    {
                    applicationLanguage = User::Language();                
                    FindLocalizableResourceFilesL(applicationLanguage); 
                
                    // If there is no match, check for default localizable rsc file
                    if (!iLocalizableRscArray.Count())
                        CheckForDefaultResourceFileL();
                    }
                }            
		    }
		else
		    {
            for (TInt i=0; i < aAppLanguages.Count(); ++i)
                FindLocalizableResourceFilesL(aAppLanguages[i]);
            
            CheckForDefaultResourceFileL();
		    }
		}
        
	// Read LONG localizable_resource_id
	aLocalizableResourceId = aResourceReader.ReadUint32L();
	DEBUG_PRINTF3(_L("LocalizableResourceId : %d : 0x%X "), aLocalizableResourceId, aLocalizableResourceId); //TODO
	
	DEBUG_PRINTF(_L("Reading app characteristics")); //TODO
	iAppCharacteristics.iAppIsHidden = aResourceReader.ReadInt8L();
	iAppCharacteristics.iEmbeddability = (Usif::TApplicationCharacteristics::TAppEmbeddability)aResourceReader.ReadInt8L();
	iAppCharacteristics.iSupportsNewFile = aResourceReader.ReadInt8L();
	iAppCharacteristics.iLaunchInBackground = aResourceReader.ReadInt8L();
	iAppCharacteristics.iGroupName = aResourceReader.ReadTPtrCL();
	iDefaultScreenNumber = aResourceReader.ReadUint8L();
 	
	DEBUG_PRINTF2(_L("iAppIsHidden : %d"), iAppCharacteristics.iAppIsHidden); //TODO
	DEBUG_PRINTF2(_L("iEmbeddability : %d"), iAppCharacteristics.iEmbeddability); //TODO
	DEBUG_PRINTF2(_L("iSupportsNewFile : %d"), iAppCharacteristics.iSupportsNewFile); //TODO
	DEBUG_PRINTF2(_L("iLaunchInBackground : %d"), iAppCharacteristics.iLaunchInBackground); //TODO
	DEBUG_PRINTF2(_L("iGroupName : %S"), &(iAppCharacteristics.iGroupName)); //TODO
	DEBUG_PRINTF2(_L("iDefaultScreenNumber : %d"), iDefaultScreenNumber); //TODO
	
	// Read the datatypes
	RPointerArray<Usif::CDataType> dataTypes;
	CleanupResetAndDestroyPushL(dataTypes);
	RPointerArray<Usif::COpaqueData> opaqueDataArray;
	CleanupResetAndDestroyPushL(opaqueDataArray);
	ReadMimeTypesSupportedL(aResourceReader, dataTypes);
	
	// DataTypes are deleted if 
	//  A. There are no legacy datatypes
	//  B. Control panel plugin apps are not allowed to register MIME types.If they happen to have any, 
	//     these datatypes should be ignored.
	if ((iAppCharacteristics.iAttributes & EControlPanelItem) || (dataTypes.Count() == 0))
	    {
	    CleanupStack::Pop(2, &dataTypes); // opaqueDataArray
	    dataTypes.ResetAndDestroy();
	    opaqueDataArray.ResetAndDestroy();	    
	    }
	else
		{
		DEBUG_PRINTF(_L("Reading Open Service Datatypes")); //TODO
		
		Usif::CServiceInfo* serviceInfo = Usif::CServiceInfo::NewLC(KOpenServiceUid, opaqueDataArray, dataTypes);
		iServiceArray.AppendL(serviceInfo);
		DEBUG_PRINTF2(_L("ServiceUid (index %d) is : OpenServiceUid "), iServiceArray.Count()-1); //TODO
		
		CleanupStack::Pop(3, &dataTypes); // serviceInfo, opaqueDataArray
		iLegacyDataTypesPresent = ETrue;
		}

	// Read LEN WORD STRUCT file_ownership_list[]
	const TInt fileOwnershipArraySize = aResourceReader.ReadInt16L();
	DEBUG_PRINTF2(_L("Owned File Array Size is : %d"), fileOwnershipArraySize); //TODO

	DEBUG_PRINTF(_L("Reading Owned File Array")); //TODO
	for (TInt i=0; i < fileOwnershipArraySize; i++)
		{
		TPtrC fileNamePtr = aResourceReader.ReadTPtrCL();
		iOwnedFileArray.AppendL(fileNamePtr.AllocL());
		DEBUG_PRINTF3(_L("File (index %d) is : %S "), i, &fileNamePtr); //TODO
		}
	}
	
void CAppRegInfoReader::ReadMimeTypesSupportedL(RResourceReader& aResourceReader, RPointerArray<Usif::CDataType>& aDataTypes)
    {
    DEBUG_PRINTF(_L("Reading MimeTypes supported")); //TODO
    // Read LEN WORD STRUCT datatype_list[]
    const TInt dataTypeArraySize = aResourceReader.ReadInt16L();
    DEBUG_PRINTF2(_L("DataType Array Size is : %d"), dataTypeArraySize); //TODO
    if (dataTypeArraySize <= 0)
        return;
    
    for (TInt i=0; i < dataTypeArraySize; i++)
        {
        TInt32 priority = aResourceReader.ReadInt32L();
                   
        TPtrC8 typePtr = aResourceReader.ReadTPtrC8L();
        TBuf16<KMaximumDataTypeLength> buf;
        buf.Copy(typePtr); 
        DEBUG_PRINTF3(_L("Service Info Priority (index %d) is : %d "), i, priority); //TODO
        DEBUG_PRINTF3(_L("Service Info Type (index %d) is : %S "), i, &buf); //TODO
        
        Usif::CDataType* dataType = Usif::CDataType::NewLC(priority, buf);        
        aDataTypes.AppendL(dataType);
        CleanupStack::Pop(dataType);
        }
    }

void CAppRegInfoReader::CheckForDefaultResourceFileL()
    {
    if (BaflUtils::FileExists(iFs, iTempLocalizableRscFileName))   
        {      
        DEBUG_PRINTF2(_L("Default localized resource file is present : %S"), &iTempLocalizableRscFileName);
        CLocalizableRsc* localizedRsc = CLocalizableRsc::NewL();
        localizedRsc->iFileName = iTempLocalizableRscFileName;
        localizedRsc->iLanguage = TLanguage(0); //Non-localized details
        iLocalizableRscArray.Append(localizedRsc);
        }
    }

// Find the correct localized resource file based on the given language
void CAppRegInfoReader::FindLocalizableResourceFilesL(const TLanguage& aApplicationLanguage)
    {
    DEBUG_PRINTF2(_L("Finding localizable resource files for language : %d"), aApplicationLanguage);
        
    TLanguagePath langEquivalents;
    RArray<TLanguage> appLanguages;
    BaflUtils::GetEquivalentLanguageList(aApplicationLanguage, langEquivalents);
    TInt i = 0;
    while (langEquivalents[i] != ELangNone)
        {
        appLanguages.Append(langEquivalents[i]);
        ++i;
        }
    
    TInt newLength = iTempLocalizableRscFileName.Length() - 2;
    TFileName localizedRscFileNamePrefix(iTempLocalizableRscFileName.Left(newLength));
    DEBUG_PRINTF2(_L("FileName after trimming last digits : %S"), &localizedRscFileNamePrefix); //TODO
            
    for (TInt i=0; i < appLanguages.Count(); ++i)
        {                
        TFileName localizedRscFileName(localizedRscFileNamePrefix);
        if (appLanguages[i] > 99)
            localizedRscFileName.AppendFormat(KThreeDigitSuffix, appLanguages[i]);
        else
            localizedRscFileName.AppendFormat(KTwoDigitSuffix, appLanguages[i]);
        
        // Check if the file exist
        if (BaflUtils::FileExists(iFs, localizedRscFileName))
            {
            // Check if language already exists
            TInt rscCount = iLocalizableRscArray.Count();
            TBool isExists = EFalse;
            for(TInt j = 0; j < rscCount; j++)
                {
                if(appLanguages[i] == iLocalizableRscArray[j]->iLanguage)
                    {
                    isExists = ETrue;
                    break;
                    }
                }
            
            if(!isExists)
                {
                CLocalizableRsc* localizedRsc = CLocalizableRsc::NewL();
                localizedRsc->iFileName = localizedRscFileName;
                localizedRsc->iLanguage = appLanguages[i];
                iLocalizableRscArray.Append(localizedRsc);
                DEBUG_PRINTF2(_L("Localized rsc file is : %S"), &localizedRscFileName);
                }            
            break;
            }
        } 
    
    appLanguages.Close();
    }

TBool CAppRegInfoReader::TypeUidIsForRegistrationFile(const TUidType& aUidType)
    {
    return (aUidType[1].iUid==KUidAppRegistrationFile.iUid);
    }

void CAppRegInfoReader::Panic(TInt aPanic)
    {
    _LIT(KSWIAppRegInfoReaderPanic,"SWIAppRegInfoReaderPanic");
    User::Panic(KSWIAppRegInfoReaderPanic, aPanic);
    }
