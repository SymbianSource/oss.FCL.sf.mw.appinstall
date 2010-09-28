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
* Implements the test steps for component management APIs in the SCR
*
*/

#include "appreginfoapparcsteps.h"
#include "tscrdefs.h"
#include <scs/cleanuputils.h>
#include <barsread.h> 
using namespace Usif;

void CScrGetApplicationRegistrationViewSubsessionStep::PrintPerformanceLog(TTime aTime)
    {
    TDateTime timer = aTime.DateTime();
    INFO_PRINTF6(_L("%S,%d:%d:%d:%d"), &KPerformanceTestInfo(), timer.Hour(), timer.Minute(), timer.Second(), timer.MicroSecond());
    }

void CScrGetApplicationRegistrationViewSubsessionStep::StartTimer()
    {
    iStartTime.HomeTime();
    PrintPerformanceLog(iStartTime);
    }

void CScrGetApplicationRegistrationViewSubsessionStep::StopTimerAndPrintResultL()
    {
    TTime endTime;
    endTime.HomeTime();
    PrintPerformanceLog(endTime);
    
    TTimeIntervalMicroSeconds duration = endTime.MicroSecondsFrom(iStartTime);
    TInt actualDuration = I64INT(duration.Int64())/1000; // in millisecond
        
    TInt maxDuration = 0;
    if(!GetIntFromConfig(ConfigSection(), KMaxDurationName, maxDuration))
        {
        ERR_PRINTF2(_L("%S could not be found in configuration."), &KMaxDurationName());
        User::Leave(KErrNotFound);
        }
    else
        {
        INFO_PRINTF3(_L("%S,%d"), &KMaxTestCaseDuration(), maxDuration);
        INFO_PRINTF3(_L("%S,%d"), &KActualTestCaseDuration(), actualDuration);
        }
    
    if(actualDuration <= maxDuration)
        {
        INFO_PRINTF2(_L("This test meets performance requirement (Duration=%d)."), actualDuration);
        }
    else
        {
        ERR_PRINTF2(_L("This test does not meet performance requirement (Duration=%d)."), actualDuration);
        SetTestStepResult(EFail);
        }
    }

void CScrGetApplicationRegistrationViewSubsessionStep::GenerateIndexedAttributeNameL(TDes& aInitialAttributeName, TInt aIndex)
    {
    const TInt MAX_INT_STR_LEN = 8;
    TBuf<MAX_INT_STR_LEN> integerAppendStr;
    integerAppendStr.Format(_L("%d"), aIndex);
    aInitialAttributeName.Append(integerAppendStr);
    }

void CScrGetApplicationRegistrationViewSubsessionStep::GetAppOwnedFilesL(RPointerArray<HBufC>& aOwnedFileArray,HBufC* aConfigSection)
    {
    TInt ownedFileCount = 0;
    HBufC* configsection;
    if(aConfigSection == NULL)
       configsection = ConfigSection().AllocL();
    else
       configsection = aConfigSection->AllocL();
    
    GetIntFromConfig(*configsection, _L("OwnedFileCount"), ownedFileCount);
    for (TUint i = 0; i < ownedFileCount; ++i)
        {
        TPtrC tOwnedFileName;
        TBuf<20> fileName;
        fileName = KOwnedFileName;
        GenerateIndexedAttributeNameL(fileName, i);
        GetStringFromConfig(*configsection, fileName, tOwnedFileName);
        HBufC* ownedFileName = tOwnedFileName.AllocLC();
        aOwnedFileArray.AppendL(ownedFileName);
        CleanupStack::Pop(ownedFileName);
        }
    delete configsection;
    }

TBool CScrGetApplicationRegistrationViewSubsessionStep::GetUidFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TUid& aUid)
    {
    TInt val;
    if(GetHexFromConfig(aSectName, aKeyName, val))
        {
        aUid = TUid::Uid(val);
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

void CScrGetApplicationRegistrationViewSubsessionStep::PrintErrorL(const TDesC& aMsg, TInt aErrNum,...)
    {
    VA_LIST list;
    VA_START(list, aErrNum);
    
    RBuf msgBuf;
    msgBuf.CreateL(MAX_SCR_ERROR_MESSAGE_LENGTH);
    msgBuf.CleanupClosePushL();
    msgBuf.AppendFormatList(aMsg, list);
    
    ERR_PRINTF1(msgBuf);
    SetTestStepResult(EFail);

    CleanupStack::PopAndDestroy(&msgBuf);
    User::Leave(aErrNum);
    }

void CScrGetApplicationRegistrationViewSubsessionStep::GetAppUidL(TUid& aAppUid, HBufC* aConfigSection)
    {    
    HBufC* configsection;
    if(aConfigSection == NULL)
       configsection = ConfigSection().AllocL();
    else
       configsection = aConfigSection->AllocL();
    
    if (!GetUidFromConfig(*configsection, _L("AppUid"), aAppUid))
        {
        delete configsection;
        PrintErrorL(_L("AppUid was not found!"), KErrNotFound);        
        }    
    INFO_PRINTF2(_L("AppUid is 0x%x"), aAppUid.iUid);
    delete configsection;
    }
    
void CScrGetApplicationRegistrationViewSubsessionStep::GetAppServiceInfoL(
        RPointerArray<CServiceInfo>& aServiceInfoArray, HBufC* aConfigSection)
    {
    TInt serviceInfoCount = 0;
    TInt totalOpaqueDataCount = 0;
    TInt totalServiceDataTypeCount = 0;
    HBufC* configsection;
    if (aConfigSection == NULL)
        configsection = ConfigSection().AllocL();
    else
        configsection = aConfigSection->AllocL();

    GetIntFromConfig(*configsection, _L("ServiceInfoCount"),
            serviceInfoCount);
    for (TUint i = 0; i < serviceInfoCount; ++i)
        {
        // service Uid
        TBuf<20> uid;
        TUid serviceUid;
        uid = KServiceUid;
        GenerateIndexedAttributeNameL(uid, i);
        GetUidFromConfig(*configsection, uid, serviceUid);

        // service opaque data
        RPointerArray<COpaqueData> serviceOpaqueDataInfoArray;
        TBuf<27> servOpaqueDataCountStr;
        servOpaqueDataCountStr = KServiceOpaqueDataInfoCount;
        GenerateIndexedAttributeNameL(servOpaqueDataCountStr, i);
        TInt serviceOpaqueDataInfoCount = 0;
        GetIntFromConfig(*configsection, servOpaqueDataCountStr, serviceOpaqueDataInfoCount);
        GetServiceOpaqueDataInfoL(serviceOpaqueDataInfoArray, serviceOpaqueDataInfoCount, totalOpaqueDataCount, configsection);
        totalOpaqueDataCount += serviceOpaqueDataInfoCount;
                
        //GetServiceOpaqueDataInfoL(serviceOpaqueDataInfoArray);
        //CleanupStack::PushL(&serviceOpaqueDataInfoArray);

        // service data type
        TBuf<21> serviceDataTypeCountString;
        serviceDataTypeCountString = KServiceDataTypeCount;
        GenerateIndexedAttributeNameL(serviceDataTypeCountString, i);
        TInt serviceDataTypeCount = 0;
        GetIntFromConfig(*configsection, serviceDataTypeCountString,
                serviceDataTypeCount);
        totalServiceDataTypeCount += serviceDataTypeCount;
        RPointerArray<CDataType> serviceDataTypeArray;
        for (TUint j = 0; j < serviceDataTypeCount; ++j)
            {
            TInt indexToRead = totalServiceDataTypeCount
                    - serviceDataTypeCount + j;
            TInt serviceDataTypePriority;
            TPtrC serviceType;
            TBuf<25> priority, type;
            priority = KServiceDataTypePriority;
            GenerateIndexedAttributeNameL(priority, indexToRead);
            GetIntFromConfig(*configsection, priority,
                    serviceDataTypePriority);
            type = KServiceDataType;
            GenerateIndexedAttributeNameL(type, indexToRead);
            GetStringFromConfig(*configsection, type, serviceType);
            CDataType* serviceDataType = CDataType::NewLC(
                    serviceDataTypePriority, serviceType);
            serviceDataTypeArray.AppendL(serviceDataType);
            CleanupStack::Pop(serviceDataType);
            }

        CServiceInfo* serviceInfo = CServiceInfo::NewLC(serviceUid,
                serviceOpaqueDataInfoArray, serviceDataTypeArray);
        aServiceInfoArray.AppendL(serviceInfo);
        CleanupStack::Pop(serviceInfo);
        }
    delete configsection;
    }

void CScrGetApplicationRegistrationViewSubsessionStep::GetAppLocalizableInfoL(RPointerArray<CLocalizableAppInfo>& aLocalizableAppInfoArray, HBufC* aConfigSection)
    {
    TInt localizableAppInfoCount = 0;
    TInt totalViewDataCount=0;
    HBufC* configsection;
    if(aConfigSection == NULL)
      configsection = ConfigSection().AllocL();
    else
      configsection = aConfigSection->AllocL();
    
    GetIntFromConfig(*configsection, _L("LocalizableAppInfoCount"), localizableAppInfoCount);
    for (TUint i = 0; i < localizableAppInfoCount; ++i)
        {
        TPtrC locShortCaption;
        TLanguage locLanguage = (TLanguage)0;
        TInt lang;
        TPtrC locGroupName;
        TBuf<20> shortCaption, language, groupName, viewDataCount;
        shortCaption = KLocShortCaption;
        GenerateIndexedAttributeNameL(shortCaption, i);
        GetStringFromConfig(*configsection, shortCaption, locShortCaption);
        language = KLocAppLanguage;
        GenerateIndexedAttributeNameL(language, i);
        GetIntFromConfig(*configsection, language, lang);
        locLanguage = static_cast<TLanguage>(lang);
        groupName = KLocGroupName;
        GenerateIndexedAttributeNameL(groupName, i);
        GetStringFromConfig(*configsection, groupName, locGroupName);
        
        TPtrC locCaption;
        TPtrC locIconFileName;
        TInt locNoOfAppIcons = 0;
        TBuf<20> caption, iconFileName, noOfAppIcons;
        caption = KLocCaption;
        GenerateIndexedAttributeNameL(caption, i);
        GetStringFromConfig(*configsection, caption, locCaption);
        iconFileName = KLocIconFileName;
        GenerateIndexedAttributeNameL(iconFileName, i);
        GetStringFromConfig(*configsection, iconFileName, locIconFileName);
        noOfAppIcons = KLocNumberOfAppIcons;
        GenerateIndexedAttributeNameL(noOfAppIcons, i);
        GetIntFromConfig(*configsection, noOfAppIcons, locNoOfAppIcons);
        CCaptionAndIconInfo* captionAndIconInfo = NULL;
        if(locCaption.Length() != 0 || locIconFileName.Length() !=0 || locNoOfAppIcons != 0)
          {
           captionAndIconInfo = CCaptionAndIconInfo::NewLC(locCaption,locIconFileName,locNoOfAppIcons);
          }
        else
          CleanupStack::PushL(captionAndIconInfo);
        
        TInt viewDataCountForLocale = 0;
        viewDataCount=KViewDataCount;
        GenerateIndexedAttributeNameL(viewDataCount, i);
        GetIntFromConfig(*configsection, viewDataCount , viewDataCountForLocale);
        totalViewDataCount+=viewDataCountForLocale;
        RPointerArray<CAppViewData> viewDataArray;
        for (TUint i = 0; i < viewDataCountForLocale; ++i)
            {
            TInt viewScreenMode = 0 ,vUid,indexToRead;
            TBuf<20> uid, screenMode;
            uid = KVwUid;
            indexToRead=totalViewDataCount-viewDataCountForLocale+i;
            GenerateIndexedAttributeNameL(uid, indexToRead);
            GetIntFromConfig(*configsection, uid, vUid);
            TUid viewUid = TUid::Uid(vUid);
            screenMode = KVwScreenMode;
            GenerateIndexedAttributeNameL(screenMode, indexToRead);
            GetIntFromConfig(*configsection, screenMode, viewScreenMode);
            
            TPtrC viewCaption;
            TPtrC viewIconFileName;
            TInt viewNoOfAppIcons = 0;
            TBuf<20> caption, iconFileName, noOfAppIcons;
            caption = KVwCaption;
            GenerateIndexedAttributeNameL(caption, indexToRead);
            GetStringFromConfig(*configsection, caption, viewCaption);
            iconFileName = KVwIconFileName;
            GenerateIndexedAttributeNameL(iconFileName, indexToRead);
            GetStringFromConfig(*configsection, iconFileName, viewIconFileName);
            noOfAppIcons = KVwNumberOfAppIcons;
            GenerateIndexedAttributeNameL(noOfAppIcons, indexToRead);
            GetIntFromConfig(*configsection, noOfAppIcons, viewNoOfAppIcons);
            CCaptionAndIconInfo* viewCaptionAndIconInfo = NULL;
            if(viewCaption.Length() != 0 || viewIconFileName.Length() !=0 || viewNoOfAppIcons != 0)
              {
               viewCaptionAndIconInfo = CCaptionAndIconInfo::NewLC(viewCaption,viewIconFileName,viewNoOfAppIcons);
              }
            else
              CleanupStack::PushL(viewCaptionAndIconInfo);          
            
            CAppViewData* viewData = CAppViewData::NewLC(viewUid,viewScreenMode,viewCaptionAndIconInfo);
            viewDataArray.AppendL(viewData);
            CleanupStack::Pop(2, viewCaptionAndIconInfo);
            }

        CLocalizableAppInfo* localizableAppInfo = CLocalizableAppInfo::NewLC(locShortCaption,locLanguage,locGroupName,captionAndIconInfo,viewDataArray);
        aLocalizableAppInfoArray.AppendL(localizableAppInfo);
        CleanupStack::Pop(localizableAppInfo);
        CleanupStack::Pop(captionAndIconInfo);
        }
    delete configsection;
    }

void CScrGetApplicationRegistrationViewSubsessionStep::GetAppOpaqueDataInfoL(RPointerArray<Usif::COpaqueData>& aAppOpaqueDataInfoArray, HBufC* aConfigSection)
    {
    TInt appOpaqueDataInfoCount = 0;
    HBufC* configsection;
    if(aConfigSection == NULL)
      configsection = ConfigSection().AllocL();
    else
      configsection = aConfigSection->AllocL();

    GetIntFromConfig(*configsection, _L("AppOpaqueDataInfoCount"), appOpaqueDataInfoCount);
    for (TUint i = 0; i < appOpaqueDataInfoCount; ++i)
        {
        TBuf<16> localeAttr;
        localeAttr = KAppOpaqueDataLocale;
        TInt locale = 0;
        GenerateIndexedAttributeNameL(localeAttr, i);
        GetIntFromConfig(*configsection, localeAttr, locale);
                    
        TBuf<14> opaqueDataAttr;
        opaqueDataAttr = KAppOpaqueData;
        TPtrC opaqueData;
        GenerateIndexedAttributeNameL(opaqueDataAttr, i);
        GetStringFromConfig(*configsection, opaqueDataAttr, opaqueData);

        TPtrC8 blobOpaqueData((TUint8*)opaqueData.Ptr(), opaqueData.Length()*2);

        COpaqueData* appOpaqueData = COpaqueData::NewLC(blobOpaqueData, (TLanguage) locale);
        aAppOpaqueDataInfoArray.AppendL(appOpaqueData);
        CleanupStack::Pop(1, appOpaqueData);
        }
    delete configsection;
    }

void CScrGetApplicationRegistrationViewSubsessionStep::GetServiceOpaqueDataInfoL(RPointerArray<Usif::COpaqueData>& aServiceOpaqueDataInfoArray, TInt aServiceOpaqueDataInfoCount, TInt aStartingIndex, HBufC* aConfigSection)
    {
    for (TUint i = 0; i < aServiceOpaqueDataInfoCount; ++i)
        {
        TBuf<20> localeAttr;
        localeAttr = KServiceOpaqueLocale;
        TInt locale = 0;
        TInt indexToRead = i + aStartingIndex;
        GenerateIndexedAttributeNameL(localeAttr, indexToRead);
        GetIntFromConfig(*aConfigSection, localeAttr, locale);
                    
        TBuf<18> opaqueDataAttr;
        opaqueDataAttr = KServiceOpaqueData;
        TPtrC opaqueData;
        GenerateIndexedAttributeNameL(opaqueDataAttr, indexToRead);
        GetStringFromConfig(*aConfigSection, opaqueDataAttr, opaqueData);
                
        TPtrC8 blobOpaqueData((TUint8*)opaqueData.Ptr(), opaqueData.Length()*2);
        COpaqueData* serviceOpaqueData = COpaqueData::NewLC(blobOpaqueData, (TLanguage) locale);
        aServiceOpaqueDataInfoArray.AppendL(serviceOpaqueData);
        CleanupStack::Pop(1, serviceOpaqueData);
        }    
    }

TBool CScrGetApplicationRegistrationViewSubsessionStep::Get64BitIntegerFromConfigL(const TDesC& aConfigKeyName, TInt64& aRetVal,HBufC* aConfigSection)
    {
    TPtrC int64Str;
    HBufC* configsection;
        if(aConfigSection == NULL)
            configsection = ConfigSection().AllocLC();
        else
            configsection = aConfigSection->AllocLC();
    
    if (!GetStringFromConfig(*configsection, aConfigKeyName, int64Str))
        return EFalse;

    TLex lex(int64Str);
    User::LeaveIfError(lex.Val(aRetVal));
    CleanupStack::PopAndDestroy(configsection);
    return ETrue;
    }

CPropertyEntry* CScrGetApplicationRegistrationViewSubsessionStep::GetPropertyFromConfigLC(TBool aIsSingle, TInt aIndex, TBool aSupportLocalized,HBufC* aConfigSection)
    {
    // This function can be used for getting a single property, which is defined by attributes such as PropertyName etc.
    // or a property in a set - in this case the properties would be PropertyType0, PropertyName0 etc.
    // aIsSingle defines the working mode. aIndex applies only if aIsSingle is false
    
    // First, we need to generate the property name
    TBuf<MAX_SCR_PARAM_LENGTH> propertyTypeParam, propertyNameParam, propertyValueParam, propertyLocaleParam;
    propertyTypeParam = KPropertyTypeParam;
    propertyNameParam = KPropertyNameParam;
    propertyValueParam = KPropertyValueParam;
    propertyLocaleParam = KPropertyLocaleParam;
    if (!aIsSingle)
        {
        GenerateIndexedAttributeNameL(propertyTypeParam, aIndex);
        GenerateIndexedAttributeNameL(propertyNameParam, aIndex);
        GenerateIndexedAttributeNameL(propertyValueParam, aIndex);
        GenerateIndexedAttributeNameL(propertyLocaleParam, aIndex);
        }
    HBufC* configsection;
    if(aConfigSection == NULL)
        configsection = ConfigSection().AllocLC();
    else
        configsection = aConfigSection->AllocLC();
    
    TPtrC propertyName;
    if (!GetStringFromConfig(*configsection, propertyNameParam, propertyName))
        {
        ERR_PRINTF2(_L("The property name param %S could not be found in configuration."), &propertyNameParam);
        User::Leave(KErrNotFound);
        }
    
    TInt propertyTypeInt;
    if (!GetIntFromConfig(*configsection, propertyTypeParam, propertyTypeInt))
        {
        ERR_PRINTF2(_L("The property type param %S could not be found in configuration."), &propertyTypeParam);
        User::Leave(KErrNotFound);
        }
    
    CPropertyEntry::TPropertyType propertyType = static_cast<CPropertyEntry::TPropertyType>(propertyTypeInt);
    
    CPropertyEntry* propertyEntry(NULL);
    switch (propertyType)
        {
        case CPropertyEntry::EBinaryProperty:
        case CPropertyEntry::ELocalizedProperty:
            {
            TPtrC propertyStrValue;
            if (!GetStringFromConfig(*configsection, propertyValueParam, propertyStrValue))
                {
                ERR_PRINTF2(_L("The property value param %S could not be found in configuration."), &propertyValueParam);
                User::Leave(KErrNotFound);
                }
            if(!aSupportLocalized || propertyType == CPropertyEntry::EBinaryProperty)
                {
                HBufC8* buffer8Bit = ConvertBufferTo8bitL(propertyStrValue);
                CleanupStack::PushL(buffer8Bit);
                propertyEntry = CBinaryPropertyEntry::NewL(propertyName, *buffer8Bit);
                CleanupStack::PopAndDestroy(buffer8Bit);
                break;
                }
            // Handle ELocalProperty case
            TLanguage locale;
            TInt localeParam;
            if (!GetIntFromConfig(*configsection, propertyLocaleParam, localeParam))
                {
                ERR_PRINTF2(_L("The property locale param %S could not be found in configuration."), &propertyLocaleParam);
                User::Leave(KErrNotFound);  
                }
            locale = static_cast<TLanguage>(localeParam);
            propertyEntry = CLocalizablePropertyEntry::NewL(propertyName, propertyStrValue, locale);
            break;                      
            }
        case CPropertyEntry::EIntProperty:
            {
            TInt64 int64Value;
            if (!Get64BitIntegerFromConfigL(propertyValueParam, int64Value, configsection))
                {
                ERR_PRINTF2(_L("The integer param %S could not be found in configuration."), &propertyValueParam);
                User::Leave(KErrNotFound);
                }
            propertyEntry = CIntPropertyEntry::NewL(propertyName, int64Value);
            break;
            }           
        }
    CleanupStack::PopAndDestroy(configsection);
    CleanupStack::PushL(propertyEntry);
    return propertyEntry;
    }

void CScrGetApplicationRegistrationViewSubsessionStep::GetAppPropertiesL(RPointerArray<CPropertyEntry>& aAppPropertyArray,TBool aSupportLocalized, HBufC* aConfigSection)
    {
    TInt propertiesCount = 0;  
    HBufC* configsection;
    if(aConfigSection == NULL)
      configsection = ConfigSection().AllocLC();
    else
      configsection = aConfigSection->AllocLC();
     
    if (!GetIntFromConfig(*configsection, KPropertiesCountParamName, propertiesCount))
       {
        INFO_PRINTF1(_L("Properties count was not found!"));
        CleanupStack::PopAndDestroy(configsection);
        return;
        }
    
    if (propertiesCount < 0)
        {
        INFO_PRINTF1(_L("Properties count was negative !"));
        CleanupStack::PopAndDestroy(configsection);
        return;
        }
    
    for (TInt i = 0; i < propertiesCount; ++i)
        {
        CPropertyEntry *propertyEntry = GetPropertyFromConfigLC(EFalse, i, aSupportLocalized,configsection);
        User::LeaveIfError(aAppPropertyArray.Append(propertyEntry));
        CleanupStack::Pop(propertyEntry);
        }
    CleanupStack::PopAndDestroy(configsection);
    }

CApplicationRegistrationData* CScrGetApplicationRegistrationViewSubsessionStep::GetAppRegInfoFromConfigLC(HBufC* aConfigSection )
    {
    TUid appUid;
    HBufC* configsection;
    if(aConfigSection == NULL)
       configsection = ConfigSection().AllocLC();
    else
       configsection = aConfigSection->AllocLC();
    
    GetAppUidL(appUid, configsection);

    TPtrC appFile; 
    
    GetStringFromConfig(*configsection, _L("AppFile"), appFile);
    
    TInt attributes = 0, hidden = 0, embeddability = 0, newFile = 0, launch = 0, defScreenNo = 0;
    GetIntFromConfig(*configsection, _L("Attributes"), attributes);
    GetIntFromConfig(*configsection, _L("Hidden"), hidden);
    GetIntFromConfig(*configsection, _L("Embeddability"), embeddability);
    GetIntFromConfig(*configsection, _L("NewFile"), newFile);
    GetIntFromConfig(*configsection, _L("Launch"), launch);

    TPtrC groupName; 
    GetStringFromConfig(*configsection, _L("GroupName"), groupName);
    
    GetIntFromConfig(*configsection, _L("DefaultScreenNumber"), defScreenNo);
    
    RPointerArray<HBufC> ownedFileArray;
    GetAppOwnedFilesL(ownedFileArray, configsection);
    CleanupStack::PushL(&ownedFileArray);
    RPointerArray<CServiceInfo> serviceInfoArray;
    GetAppServiceInfoL(serviceInfoArray, configsection);
    CleanupStack::PushL(&serviceInfoArray);
    RPointerArray<CLocalizableAppInfo> localizableAppInfoArray;
    GetAppLocalizableInfoL(localizableAppInfoArray, configsection);
    CleanupStack::PushL(&localizableAppInfoArray);
    RPointerArray<COpaqueData> appOpaqueDataInfoArray;
    GetAppOpaqueDataInfoL(appOpaqueDataInfoArray, configsection);
    CleanupStack::PushL(&appOpaqueDataInfoArray);
    RPointerArray<CPropertyEntry> appPropertyArray;
    GetAppPropertiesL(appPropertyArray,ETrue, configsection);
    CleanupStack::PushL(&appPropertyArray);
    INFO_PRINTF1(_L("Going to create CApplicationRegistrationData obj."));
    
    TApplicationCharacteristics appCharacteristics;
    appCharacteristics.iAttributes = attributes;
    appCharacteristics.iAppIsHidden = hidden;
    appCharacteristics.iEmbeddability = TApplicationCharacteristics::TAppEmbeddability(embeddability);
    appCharacteristics.iGroupName = groupName;
    appCharacteristics.iLaunchInBackground = launch;
    CApplicationRegistrationData* appRegData = CApplicationRegistrationData::NewLC(ownedFileArray, serviceInfoArray, localizableAppInfoArray, appPropertyArray, appOpaqueDataInfoArray, appUid, appFile, appCharacteristics, defScreenNo);
    INFO_PRINTF1(_L("Created CApplicationRegistrationData obj."));
    CleanupStack::Pop(6);
    CleanupStack::PopAndDestroy(configsection);
    CleanupStack::PushL(appRegData);
    return appRegData;
    }

TBool CScrGetApplicationRegistrationViewSubsessionStep::CompareFileOwnershipInfo(RPointerArray<HBufC> aActualData, RPointerArray<HBufC> aExpectedData)
    {
    TInt expectedAppOwnedFilesCount = aExpectedData.Count();
    TInt actualAppOwnedFilesCount = aActualData.Count();
    
    if(actualAppOwnedFilesCount != expectedAppOwnedFilesCount)
        {        
        INFO_PRINTF1(_L("The no of expected owned files by an app is not same as the no of actual owned files"));
        return EFalse;
        }
    
    for (TInt i=0 ; i < expectedAppOwnedFilesCount ; i++)
        {
       if(!aExpectedData.Find(aActualData[i]))
          {
          INFO_PRINTF1(_L("The expected owned file name is not same as the actual file  name"));
          return EFalse;      
          }
        }    
    return ETrue;    
    }

TBool CScrGetApplicationRegistrationViewSubsessionStep::CompareDataType(RPointerArray<Usif::CDataType> aActualData,  RPointerArray<Usif::CDataType> aExpectedData)
    {
    TInt expectedDataTypeCount = aExpectedData.Count();
    TInt actualDataTypeCount = aActualData.Count();
           
    if(actualDataTypeCount != expectedDataTypeCount)
      {        
      INFO_PRINTF1(_L("The no of expected data types for a service is not same as the no of actual recieved data types"));
      return EFalse;
      }
    for(TInt i = 0 ; i < expectedDataTypeCount ; i++)
        {
        if(aExpectedData[i]->Priority()!= aActualData[i]->Priority())
           {
           INFO_PRINTF1(_L("The expected priority for a service data type is not same as the actual recieved"));
           return EFalse;      
           }
        
        if(!(aExpectedData[i]->Type() == aActualData[i]->Type()))
           {
           INFO_PRINTF1(_L("The expected type for a service data type is not same as the actual recieved"));
           return EFalse;      
           }
        }
    return ETrue;
    }

TBool CScrGetApplicationRegistrationViewSubsessionStep::CompareOpaqueDataL(RPointerArray<Usif::COpaqueData> aActualData,  RPointerArray<Usif::COpaqueData> aExpectedData)
    {
    TInt expectedCount = aExpectedData.Count();
    TInt actualCount = aActualData.Count();
           
    if(actualCount != expectedCount)
      {        

      INFO_PRINTF1(_L("The no of expected opaque data is not same as the no of actual opaque data"));
      INFO_PRINTF2(_L("The expected data : %d"), expectedCount);
      INFO_PRINTF2(_L("The actual data : %d"), actualCount);
      return EFalse;
      }
    for(TInt i = 0 ; i < expectedCount ; i++)
        {
        if(aExpectedData[i]->Language()!= aActualData[i]->Language())
           {
           INFO_PRINTF1(_L("The expected lang for a service data type is not same as the actual recieved"));
           return EFalse;      
           }
        
        TBool compareWithDb = EFalse;
        GetBoolFromConfig(ConfigSection(), _L("CompareWithDb"), compareWithDb);
        
        if(compareWithDb)
            {
            TResourceReader reader;
            reader.SetBuffer(&aActualData[i]->OpaqueData());
            HBufC16* actualDataBuffer = reader.ReadHBufCL();
            CleanupStack::PushL(actualDataBuffer);
            
            TPtrC16 ptr16(reinterpret_cast<const TUint16*>(aExpectedData[i]->OpaqueData().Ptr()),(aExpectedData[i]->OpaqueData().Length()/2));
            HBufC16* expectedDataBuffer = ptr16.AllocLC();
            
            if(*actualDataBuffer != *expectedDataBuffer)
               {                
               INFO_PRINTF1(_L("The expected data for a service opqdata is not same as the actual recieved"));
               INFO_PRINTF2(_L("The expected data %S"), expectedDataBuffer);
               INFO_PRINTF2(_L("The Actual data %S"), actualDataBuffer);
               CleanupStack::PopAndDestroy(2, actualDataBuffer);
               return EFalse;      
               }
            CleanupStack::PopAndDestroy(2, actualDataBuffer);

            }
        else
            {
            if(aActualData[i]->OpaqueData() != aExpectedData[i]->OpaqueData())
               {                
               INFO_PRINTF1(_L("The expected data for a service opqdata is not same as the actual recieved"));
               INFO_PRINTF2(_L("The expected data %S"), &aExpectedData[i]->OpaqueData());
               INFO_PRINTF2(_L("The Actual data %S"), &aActualData[i]->OpaqueData());
               return EFalse;      
               }
            }
        }
    return ETrue;
    }

TBool CScrGetApplicationRegistrationViewSubsessionStep::CompareServiceInfoL(RPointerArray<Usif::CServiceInfo> aActualData,  RPointerArray<Usif::CServiceInfo> aExpectedData)
    {
    TInt expectedServicesCount = aExpectedData.Count();
    TInt actualServicesCount = aActualData.Count();
       
    if(actualServicesCount != expectedServicesCount)
      {        
      INFO_PRINTF1(_L("The no of expected services for an app is not same as the no of actual services"));
      return EFalse;
      }
    
    for (TInt i=0 ; i < expectedServicesCount ; i++)
      {
        // Compare uids
      if(aExpectedData[i]->Uid() != aActualData[i]->Uid())
      {
      INFO_PRINTF2(_L("Expected Uid 0x%08x"), aExpectedData[i]->Uid());
      INFO_PRINTF2(_L("Actual Uid 0x%08x"), aActualData[i]->Uid());
      INFO_PRINTF1(_L("The expected service uid is not same as the actual service uid"));
      return EFalse;      
      }

      // Compare Opaque data
      if(!CompareOpaqueDataL(aActualData[i]->OpaqueData(), aExpectedData[i]->OpaqueData()))
        {
        return EFalse;      
        }
      
      //compare data type
      if(!CompareDataType(aActualData[i]->DataTypes(), aExpectedData[i]->DataTypes()))
          {
          return EFalse;
          }     
      }
    return ETrue; 
    }

TBool CScrGetApplicationRegistrationViewSubsessionStep::CompareCaptionandIconInfo(const CCaptionAndIconInfo* aActualData,const CCaptionAndIconInfo* aExpectedData)
    {       
    if(aExpectedData == NULL && aActualData == NULL)
      {
      return ETrue;  
      }
    
    if((aExpectedData == NULL && aActualData != NULL) || (aExpectedData != NULL && aActualData == NULL))
      {
      INFO_PRINTF1(_L("The caption and icon info  for a localizable is either not present ot not expected"));
      return EFalse;   
      }               
            
    if(aActualData->NumOfAppIcons() != aExpectedData->NumOfAppIcons())
      {
      INFO_PRINTF1(_L("The expected no od icons for a locale is not same as the actual recieved"));
      return EFalse;
      }
           
    if(aActualData->Caption().Compare(aExpectedData->Caption()))
      {
      INFO_PRINTF1(_L("The expected caption for a locale is not same as the actual recieved"));
      return EFalse;
      }
       
    if(aActualData->IconFileName().Compare(aExpectedData->IconFileName()))
      {
      INFO_PRINTF2(_L("Expected Icon File %S"), &aExpectedData->IconFileName());
      INFO_PRINTF2(_L("Actual Icon File %S"), &aActualData->IconFileName());
      INFO_PRINTF1(_L("The expected icon file name is not same as the actual recieves"));
      return EFalse;
      }
    return ETrue;
    }

TBool CScrGetApplicationRegistrationViewSubsessionStep::CompareViewData(RPointerArray<Usif::CAppViewData> aActualData,  RPointerArray<Usif::CAppViewData> aExpectedData)
    {
    TInt expectedViewDataCount = aExpectedData.Count();
    TInt actualViewDataCount = aActualData.Count();
              
    if(actualViewDataCount != expectedViewDataCount)
      {        
      INFO_PRINTF1(_L("The no of expected localizable info for an app is not same as the no of actual recieved"));
      return EFalse;
      }
    for (TInt i=0 ; i < expectedViewDataCount ; i++)
        {
        if(aExpectedData[i]->ScreenMode() != aActualData[i]->ScreenMode())
          {
          INFO_PRINTF1(_L("The expected screen mode for a view is not same as the actual recieved"));
          return EFalse;      
          }              
                 
        if(aExpectedData[i]->Uid() != aActualData[i]->Uid())
          {
          INFO_PRINTF1(_L("The expected uid for a view is not same as the actual recieved"));
          return EFalse;      
          } 
           
        //compare captionandicon info
       if(!CompareCaptionandIconInfo(aActualData[i]->CaptionAndIconInfo(),aExpectedData[i]->CaptionAndIconInfo()))
          {
          return EFalse;
          }
        }
    return ETrue;                      
    }

TBool CScrGetApplicationRegistrationViewSubsessionStep::CompareLocalizableAppInfo(RPointerArray<Usif::CLocalizableAppInfo> aActualData,  RPointerArray<Usif::CLocalizableAppInfo> aExpectedData)
    {
    TInt expectedLocalizableCount = aExpectedData.Count();
    TInt actualLocalizableCount = aActualData.Count();
          
    if(actualLocalizableCount != expectedLocalizableCount)
       {        
       INFO_PRINTF1(_L("The no of expected localizable info for an app is not same as the no of actual recieved"));
       return EFalse;
       }
    for (TInt i=0 ; i < expectedLocalizableCount ; i++)
       {
       if(aExpectedData[i]->ShortCaption().Compare(aActualData[i]->ShortCaption()))
         {
         INFO_PRINTF1(_L("The expected short caption for a localizable info is not same as the actual recieved"));
         return EFalse;      
         }
       
       if(aExpectedData[i]->GroupName().Compare(aActualData[i]->GroupName()))
         {
         INFO_PRINTF1(_L("The expected group name for a localizable info is not same as the actual recieved"));
         return EFalse;      
         }
             
       if(aExpectedData[i]->ApplicationLanguage() != aActualData[i]->ApplicationLanguage())
         {
         INFO_PRINTF1(_L("The expected locale for a localizable info is not same as the actual recieved"));
         INFO_PRINTF2(_L("The expected language %d"), aExpectedData[i]->ApplicationLanguage());
         INFO_PRINTF2(_L("The Actual language %d"), aActualData[i]->ApplicationLanguage());
         return EFalse;      
         } 
       
       //compare caption and icon info
       if(!CompareCaptionandIconInfo(aActualData[i]->CaptionAndIconInfo(),aExpectedData[i]->CaptionAndIconInfo()))
           {
          return EFalse;
           }
       
       // compare view data    
       if(!CompareViewData(aActualData[i]->ViewDataList(), aExpectedData[i]->ViewDataList()))
           {
           return EFalse;
           }
       }
    return ETrue;
    }

TBool CScrGetApplicationRegistrationViewSubsessionStep::CompareApplicationRegistrationDataL(
        Usif::CApplicationRegistrationData *aActualData,
        Usif::CApplicationRegistrationData *aExpectedData)
    {
    //comparing AppRegistrationInfo    
    if (aActualData->AppUid() != aExpectedData->AppUid())
        {
        INFO_PRINTF2(_L("Actual AppUid is 0x%x"), aActualData->AppUid().iUid);
        INFO_PRINTF2(_L("Expected AppUid is 0x%x"), aExpectedData->AppUid().iUid);
        INFO_PRINTF1(_L("The expected App Uid is not same as the actual AppUid"));
        return EFalse;
        }

    if (aActualData->AppFile().Compare(aExpectedData->AppFile()))
        {
        INFO_PRINTF2(_L("Expected app File %S"), &aExpectedData->AppFile());
        INFO_PRINTF2(_L("Actual app File %S"), &aActualData->AppFile());
        INFO_PRINTF1(_L("The expected App File is not same as the actual App File"));
        return EFalse;
        }

    if (aActualData->Attributes() != aExpectedData->Attributes())
        {
        INFO_PRINTF1(_L("The expected App Attribute is not same as the actual App Attribute"));
        return EFalse;
        }

    if (aActualData->Hidden() != aExpectedData->Hidden())
        {
        INFO_PRINTF1(_L("The expected Hidden Value is not same as the actual App Hidden Value"));
        return EFalse;
        }

    if (aActualData->Embeddability() != aExpectedData->Embeddability())
        {
        INFO_PRINTF1(_L("The expected embeddability value is not same as the actual embeddability value"));
        return EFalse;
        }

    if (aActualData->NewFile() != aExpectedData->NewFile())
        {
        INFO_PRINTF1(_L("The expected App NewFile Value is not same as the actual App NewFile Value"));
        return EFalse;
        }

    if (aActualData->Launch() != aExpectedData->Launch())
        {
        INFO_PRINTF1(_L("The expected App Launch Value is not same as the actual App Launch Value"));
        return EFalse;
        }

    if (aActualData->GroupName().Compare(aExpectedData->GroupName()))
        {
        INFO_PRINTF1(_L("The expected App Group Name is not same as the actual App Group Name"));
        return EFalse;
        }

    if (aActualData->DefaultScreenNumber()
            != aExpectedData->DefaultScreenNumber())
        {
        INFO_PRINTF1(_L("The expected App DefaultScreenNumber is not same as the actual App DefaultScreenNumber"));
        return EFalse;
        }

    // Compare Opaque data
    if (!CompareOpaqueDataL(aActualData->AppOpaqueData(),
            aExpectedData->AppOpaqueData()))
        {
        return EFalse;
        }

    // comparing FileOwnershipInfo
    if (!CompareFileOwnershipInfo(aActualData->OwnedFileArray(),
            aExpectedData->OwnedFileArray()))
        {
        return EFalse;
        }

    // comparing  Service info
    if (!CompareServiceInfoL(aActualData->ServiceArray(),
            aExpectedData->ServiceArray()))
        {
        return EFalse;
        }

    //comparing localizable app info
    if (!CompareLocalizableAppInfo(aActualData->LocalizableAppInfoList(),
            aExpectedData->LocalizableAppInfoList()))
        {
        return EFalse;
        }

    return ETrue;
    }

//-----------------CScrGetApplicationRegistrationViewStep-------------

CScrGetApplicationRegistrationViewSubsessionStep::CScrGetApplicationRegistrationViewSubsessionStep() 
    {
    
    }

CScrGetApplicationRegistrationViewSubsessionStep::~CScrGetApplicationRegistrationViewSubsessionStep()
// Destructor.
    {
    iScrSession.Close();
    }

void CScrGetApplicationRegistrationViewSubsessionStep:: MarkAsPerformanceStep()
    {
    iIsPerformanceTest = ETrue;
    }

void CScrGetApplicationRegistrationViewSubsessionStep::ImplTestStepPreambleL()
    {
    User::LeaveIfError(iScrSession.Connect());
    }


void CScrGetApplicationRegistrationViewSubsessionStep::GetApplicationRegistrationDataFromConfigL(RPointerArray<Usif::CApplicationRegistrationData>& aEntries)
    {
    TInt configSectionCount(0);
    CApplicationRegistrationData *applicationRegistrationData = NULL;
    TPtrC configSectionName(ConfigSection());
    HBufC* cnfSecName = NULL;
    
    GetIntFromConfig(ConfigSection(), _L("AppRegEntriesConfigCount"), configSectionCount);          

    for (TUint i = 0; i < configSectionCount; ++i)
        {
        TBuf<20> configSection = _L("configsection");
        GenerateIndexedAttributeNameL(configSection, i);
        GetStringFromConfig(ConfigSection(),configSection, configSectionName);
        cnfSecName = configSectionName.AllocLC();
        applicationRegistrationData = GetAppRegInfoFromConfigLC(cnfSecName);
        aEntries.AppendL(applicationRegistrationData);
        CleanupStack::Pop(applicationRegistrationData);
        CleanupStack::PopAndDestroy(cnfSecName);
        }  
    }

void CScrGetApplicationRegistrationViewSubsessionStep::ImplTestStepL()
    {            
    TInt locale;    
    TInt expextedAppRegDataCount(0);
    TInt actualAppRegDataCount(0);
    TInt count = 0;
    TInt remainingExpectedCount = 0;
    TBool isFinish = EFalse;
    TBool isError = EFalse;
    TInt index = 0;
      
    RPointerArray<Usif::CApplicationRegistrationData> expectedAppRegData;
    CleanupClosePushL(expectedAppRegData);
    INFO_PRINTF1(_L("Read expected entries for verification "));
    GetApplicationRegistrationDataFromConfigL(expectedAppRegData);
    expextedAppRegDataCount = expectedAppRegData.Count();
    remainingExpectedCount = expextedAppRegDataCount; 
    
    TInt noOfEntries(1);
    GetIntFromConfig(ConfigSection(), _L("NoOfEntries"), noOfEntries);
    
    TInt noOfAppRegAppUid = 0;
    TBuf<20> appRegAppUidName;
    RArray<TUid> expectedAppRegAppUids;
    CleanupClosePushL(expectedAppRegAppUids);

    GetIntFromConfig(ConfigSection(), _L("AppRegAppUidCount"), noOfAppRegAppUid);
    for (TUint i = 0; i < noOfAppRegAppUid; ++i)
        {   
        TUid appRegAppUid;
        appRegAppUidName = _L("AppRegAppUid");
        GenerateIndexedAttributeNameL(appRegAppUidName, i);
        GetUidFromConfig(ConfigSection(), appRegAppUidName, appRegAppUid);
        expectedAppRegAppUids.AppendL(appRegAppUid);
        }
    
    TInt err = KErrNone;
    RApplicationRegistryView subSession;
    CleanupClosePushL(subSession);
    if(GetIntFromConfig(ConfigSection(), _L("Locale"), locale))
        {
        if(expectedAppRegAppUids.Count())
            {
            TRAP(err,subSession.OpenViewL(iScrSession, expectedAppRegAppUids, (TLanguage)locale));
            }
        else
            {
            TRAP(err,subSession.OpenViewL(iScrSession, (TLanguage)locale));
            }
        if(KErrNone != err)
            {
            CleanupStack::PopAndDestroy(1,&subSession);
            expectedAppRegAppUids.Close();
            CleanupStack::Pop(2); //poping expectedAppRegAppUids expectedAppRegData            
            expectedAppRegData.ResetAndDestroy();
            User::Leave(err);
            }
        }
    else
        {
        if(expectedAppRegAppUids.Count())
            {
            TRAP(err,subSession.OpenViewL(iScrSession, expectedAppRegAppUids));
            }
        else
            {
            TRAP(err,subSession.OpenViewL(iScrSession));
            }
        if(KErrNone != err)
           {
            CleanupStack::PopAndDestroy(1,&subSession);
            expectedAppRegAppUids.Close();
            CleanupStack::Pop(2); //poping expectedAppRegAppUids expectedAppRegData            
            expectedAppRegData.ResetAndDestroy();
            User::Leave(err);
           }
        }
    
    RPointerArray<CApplicationRegistrationData> actualAppRegData;
    CleanupClosePushL(actualAppRegData);
    
    while(1)
    {    
    actualAppRegData.ResetAndDestroy();
    
    if(iIsPerformanceTest)
        StartTimer();
    TRAP(err,subSession.GetNextApplicationRegistrationInfoL(noOfEntries, actualAppRegData));
    if(iIsPerformanceTest)
        StopTimerAndPrintResultL();
    
    if( (KErrNone != err) || (iIsPerformanceTest && (EFail == TestStepResult())) )
        {
        CleanupStack::Pop(1); //poping actualAppRegData 
        actualAppRegData.ResetAndDestroy();
        CleanupStack::PopAndDestroy(1,&subSession);
        expectedAppRegAppUids.Close();
        CleanupStack::Pop(2); //poping expectedAppRegAppUids expectedAppRegData
        expectedAppRegData.ResetAndDestroy();
        User::Leave(err);
        }
    
    actualAppRegDataCount = actualAppRegData.Count();
    
    if(remainingExpectedCount >= noOfEntries)
        {
        count = noOfEntries;
        remainingExpectedCount -=  count;        
        }
    else
        {
        count =  remainingExpectedCount;
        isFinish = ETrue;
        }
        
    if(isFinish)
        {
        break;
        }
    
    if (count != actualAppRegDataCount)
        {
        INFO_PRINTF1(_L("actualAppRegDataCount is not same as expextedAppRegDataCount"));
        SetTestStepResult(EFail);  
        CleanupStack::Pop(1); //poping actualAppRegData 
        actualAppRegData.ResetAndDestroy();
        CleanupStack::PopAndDestroy(1,&subSession);
        expectedAppRegAppUids.Close();
        CleanupStack::Pop(2); //poping expectedAppRegAppUids expectedAppRegData
        expectedAppRegData.ResetAndDestroy();
        return;
        }       
    
    for(TInt i=0 ; i < count ; i++)
        {
      if(!CompareApplicationRegistrationDataL(actualAppRegData[i], expectedAppRegData[index]))
          {
          SetTestStepResult(EFail);
          isError = ETrue;
          break;
          }     
      index++;
        }
    
    if(isError)
        {
        break;
        }    
    }
    
    CleanupStack::Pop(1); //poping actualAppRegData 
    actualAppRegData.ResetAndDestroy();
    CleanupStack::PopAndDestroy(1,&subSession);
    expectedAppRegAppUids.Close();
    CleanupStack::Pop(2); //poping expectedAppRegAppUids expectedAppRegData
    expectedAppRegData.ResetAndDestroy();
    }

void CScrGetApplicationRegistrationViewSubsessionStep::ImplTestStepPostambleL()
    {
    
    }
//-----------------CScrMultipleSubsessionsForAppRegistryViewStep-------------

CScrMultipleSubsessionsForAppRegistryViewStep::CScrMultipleSubsessionsForAppRegistryViewStep() 
    {
    
    }

CScrMultipleSubsessionsForAppRegistryViewStep::~CScrMultipleSubsessionsForAppRegistryViewStep()
// Destructor.
    {
    iScrSession.Close();
    }

void CScrMultipleSubsessionsForAppRegistryViewStep::ImplTestStepPreambleL()
    {
    User::LeaveIfError(iScrSession.Connect());
    }

void CScrMultipleSubsessionsForAppRegistryViewStep::ImplTestStepL()
    {            
    TInt locale;    
    TInt actualAppRegDataCount(0);
    TInt count = 0;
    TInt index = 0;
      
    RPointerArray<Usif::CApplicationRegistrationData> expectedAppRegData;
    CleanupClosePushL(expectedAppRegData);
    INFO_PRINTF1(_L("Read expected entries for verification "));
    GetApplicationRegistrationDataFromConfigL(expectedAppRegData);
    
    TInt noOfEntries(1);
    GetIntFromConfig(ConfigSection(), _L("NoOfEntries"), noOfEntries);
    
    TInt noOfAppRegAppUid = 0;
    TBuf<20> appRegAppUidName;
    RArray<TUid> appUidsBeingQueried;
    CleanupClosePushL(appUidsBeingQueried);

    GetIntFromConfig(ConfigSection(), _L("AppRegAppUidCount"), noOfAppRegAppUid);
    for (TUint i = 0; i < noOfAppRegAppUid; ++i)
        {   
        TUid appRegAppUid;
        appRegAppUidName = _L("AppRegAppUid");
        GenerateIndexedAttributeNameL(appRegAppUidName, i);
        GetUidFromConfig(ConfigSection(), appRegAppUidName, appRegAppUid);
        appUidsBeingQueried.AppendL(appRegAppUid);
        }
    
    GetIntFromConfig(ConfigSection(), _L("Locale"), locale);
    
    TInt err = KErrNone;
    RApplicationRegistryView subSession;
    CleanupClosePushL(subSession);
    
    TRAP(err,subSession.OpenViewL(iScrSession, (TLanguage)locale));
    if(KErrNone != err)
        {
        CleanupStack::PopAndDestroy(1,&subSession);
        appUidsBeingQueried.Close();
        CleanupStack::Pop(2); //poping appUidsBeingQueried expectedAppRegData            
        expectedAppRegData.ResetAndDestroy();
        User::Leave(err);
        }

    RPointerArray<CApplicationRegistrationData> actualAppRegData;
    CleanupClosePushL(actualAppRegData);
    
    actualAppRegData.ResetAndDestroy();
    TRAP(err,subSession.GetNextApplicationRegistrationInfoL(noOfEntries, actualAppRegData));
    if(KErrNone != err)
        {
        CleanupStack::Pop(1); //poping actualAppRegData 
        actualAppRegData.ResetAndDestroy();
        CleanupStack::PopAndDestroy(1,&subSession);
        appUidsBeingQueried.Close();
        CleanupStack::Pop(2); //poping appUidsBeingQueried expectedAppRegData
        expectedAppRegData.ResetAndDestroy();
        User::Leave(err);
        }
              
    actualAppRegDataCount = actualAppRegData.Count();
    count = noOfEntries;
           
    if (count != actualAppRegDataCount)
        {
        INFO_PRINTF1(_L("actualAppRegDataCount is not same as noOfEntries"));
        SetTestStepResult(EFail);  
        CleanupStack::Pop(1); //poping actualAppRegData 
        actualAppRegData.ResetAndDestroy();
        CleanupStack::PopAndDestroy(1,&subSession);
        appUidsBeingQueried.Close();
        CleanupStack::Pop(2); //poping appUidsBeingQueried expectedAppRegData
        expectedAppRegData.ResetAndDestroy();
        return;
        }  
       
    for(TInt i=0 ; i < count ; i++)
        {
        // Compare the first "count" number of entries in the DB to the entries specified in the config file. 
        if(!CompareApplicationRegistrationDataL(actualAppRegData[i], expectedAppRegData[index]))
             {
             SetTestStepResult(EFail);
             break;
             }     
        index++;
        }
       
    actualAppRegData.ResetAndDestroy();
    TRAP(err,subSession.OpenViewL(iScrSession, appUidsBeingQueried, (TLanguage)locale));
    if(KErrNone != err)
        {
        CleanupStack::PopAndDestroy(1,&subSession);
        appUidsBeingQueried.Close();
        CleanupStack::Pop(2); //poping appUidsBeingQueried expectedAppRegData            
        expectedAppRegData.ResetAndDestroy();
        User::Leave(err);
        }
              
    TRAP(err,subSession.GetNextApplicationRegistrationInfoL(noOfEntries, actualAppRegData))
    if(KErrNone != err)
        {
        CleanupStack::Pop(1); //poping actualAppRegData 
        actualAppRegData.ResetAndDestroy();
        CleanupStack::PopAndDestroy(1,&subSession);
        appUidsBeingQueried.Close();
        CleanupStack::Pop(2); //poping appUidsBeingQueried expectedAppRegData
        expectedAppRegData.ResetAndDestroy();
        User::Leave(err);
        }
                    
     actualAppRegDataCount = actualAppRegData.Count();
     count = noOfAppRegAppUid;
                  
     if (count != actualAppRegDataCount)
        {
        INFO_PRINTF1(_L("actualAppRegDataCount is not same as noOfAppRegAppUid to be fetched"));
        SetTestStepResult(EFail);  
        CleanupStack::Pop(1); //poping actualAppRegData 
        actualAppRegData.ResetAndDestroy();
        CleanupStack::PopAndDestroy(1,&subSession);
        appUidsBeingQueried.Close();
        CleanupStack::Pop(2); //poping appUidsBeingQueried expectedAppRegData
        expectedAppRegData.ResetAndDestroy();
        return;
        }     
          
     index = 0;
     for(TInt i=0 ; i < count ; i++)
        {
         // Compare the entries fetched from the DB to the entries specified in the config file.
        if(!CompareApplicationRegistrationDataL(actualAppRegData[i], expectedAppRegData[index]))
           {
           SetTestStepResult(EFail);
           break;
           }     
        index++;
        }

    CleanupStack::Pop(1); //poping actualAppRegData 
    actualAppRegData.ResetAndDestroy();
    CleanupStack::PopAndDestroy(1,&subSession);
    appUidsBeingQueried.Close();
    CleanupStack::Pop(2); //poping appUidsBeingQueried expectedAppRegData
    expectedAppRegData.ResetAndDestroy();
    }

void CScrMultipleSubsessionsForAppRegistryViewStep::ImplTestStepPostambleL()
    {
    
    }
