/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation for the subsessions of RSoftwareComponentRegistry. 
* See class and function declarations for more detail.
*
*/


#include "scr.h"
#include "scrclient.inl"
#include "scrcommon.h"
#include "usiflog.h"
#include <scs/streamingarray.h>

using namespace Usif;

//
// RSoftwareComponentRegistryView
//

TInt RSoftwareComponentRegistryView::SendSyncMessage(TInt aFunction, const TIpcArgs& aArgs) const
	{
	return CallSubsessionFunction(aFunction, aArgs);
	}

EXPORT_C RSoftwareComponentRegistryView::RSoftwareComponentRegistryView()
/**
	This constructor provides a single point of definition from
	which the superclass constructor is called.
 */
	: RScsClientSubsessionBase()
	{
	// empty
	}

EXPORT_C void RSoftwareComponentRegistryView::Close()
	{
	RScsClientSubsessionBase::Close();
	}

EXPORT_C void RSoftwareComponentRegistryView::OpenViewL(const RSoftwareComponentRegistry& aCompReg, CComponentFilter* aFilter)
	{
	DEBUG_PRINTF(_L("Creating a subsession to the SCR server for the components view."));
	
	Close(); //Close the SubSession before Re-Opening the same.
	TInt err = CreateSubsession(aCompReg, ESubSessCreateComponentsView, TIpcArgs());
	if(KErrNone != err)
		{
		DEBUG_PRINTF2(_L("Failed to create the subsession to the SCR server for the components view (Error:%d)"), err);
		User::Leave(err);
		}
	
	RBuf8 buf;
	buf.CleanupClosePushL();
	ExternalizeObjectL(aFilter, buf);
	
	DEBUG_PRINTF(_L("Sending the request to create a component view on the server side."));
	TIpcArgs args(&buf);
	User::LeaveIfError(CallSubsessionFunction(EOpenComponentsView, args));
	CleanupStack::PopAndDestroy(&buf);
	}

EXPORT_C CComponentEntry* RSoftwareComponentRegistryView::NextComponentL(TLanguage aLocale) const
	{
	CComponentEntry* entry = CComponentEntry::NewLC();
	if(!NextComponentL(*entry, aLocale))
		{
		CleanupStack::PopAndDestroy(entry);
		return NULL;
		}
	CleanupStack::Pop(entry);
	return entry;
	}

EXPORT_C TBool RSoftwareComponentRegistryView::NextComponentL(CComponentEntry& aEntry, TLanguage aLocale) const
	{
	DEBUG_PRINTF(_L("Sending the request to retrieve the next component from the component view."));
    //To avoid call to NextComponentL before calling OpenViewL 
	CheckSubSessionHandleL();
    
	TIpcArgs args(aLocale);
	TInt argNum = 1;
	return GetObjectL(*this, aEntry, EGetNextComponentSize, EGetNextComponentData, argNum, args);
	}

EXPORT_C void RSoftwareComponentRegistryView::NextComponentSetL(TInt aMaxCount, RPointerArray<CComponentEntry>& aComponentList, TLanguage aLocale) const
	{
	DEBUG_PRINTF(_L("Sending the request to retrieve the next set of components from the component view."));
    //To avoid call to NextComponentSetL before calling OpenViewL 
	CheckSubSessionHandleL();
    
	TIpcArgs args(aMaxCount, aLocale);
	TInt argNum = 2;
	GetObjectArrayL(*this, EGetNextComponentSetSize, EGetNextComponentSetData, argNum, args, aComponentList);
	}

void RSoftwareComponentRegistryView::CheckSubSessionHandleL() const
    {
    if(KNullHandle == SubSessionHandle())
        {
        DEBUG_PRINTF(_L("An invalid handle has been passed."));
        User::Leave(KErrBadHandle);
        }
    }

//
// RSoftwareComponentRegistryFilesList
//

TInt RSoftwareComponentRegistryFilesList::SendSyncMessage(TInt aFunction, const TIpcArgs& aArgs) const
	{
	return CallSubsessionFunction(aFunction, aArgs);
	}

EXPORT_C RSoftwareComponentRegistryFilesList::RSoftwareComponentRegistryFilesList()
/**
	This constructor provides a single point of definition from
	which the superclass constructor is called.
 */
	: RScsClientSubsessionBase()
	{
	// empty
	}

EXPORT_C void RSoftwareComponentRegistryFilesList::OpenListL(const RSoftwareComponentRegistry& aCompReg, TComponentId aComponentId)
	{
	DEBUG_PRINTF(_L("Creating a subsession to the SCR server for the file list."));
	
	Close(); //Close the SubSession before Re-Opening the same.
	TInt err = CreateSubsession(aCompReg, ESubSessCreateFileList, TIpcArgs());
	if(KErrNone != err)
		{
		DEBUG_PRINTF2(_L("Failed to create the subsession to the SCR server for the file list (Error:%d)."), err);
		User::Leave(err);
		}
	
	DEBUG_PRINTF(_L("Sending a request to create a file list on the server side."));
	TIpcArgs args(aComponentId);
	User::LeaveIfError(CallSubsessionFunction(EOpenFileList, args));
	}

EXPORT_C HBufC* RSoftwareComponentRegistryFilesList::NextFileL() const
	{
    //To avoid call to NextFileL before calling OpenListL 
    CheckSubSessionHandleL();
    
	TIpcArgs args(TIpcArgs::ENothing);
	TInt argNum = 0;
	return GetObjectL<HBufC>(*this, EGetNextFileSize, EGetNextFileData, argNum, args);
	}

EXPORT_C void RSoftwareComponentRegistryFilesList::NextFileSetL(TInt aMaxCount, RPointerArray<HBufC>& aFileList) const
	{
	DEBUG_PRINTF(_L("Sending the request to retrieve the next set of components from the component view."));
    //To avoid call to NextFileSetL before calling OpenListL 
	CheckSubSessionHandleL();
    
	TIpcArgs args(aMaxCount);
	TInt argNum = 1;
	GetObjectArrayL(*this, EGetNextFileSetSize, EGetNextFileSetData, argNum, args, aFileList);
	}

EXPORT_C void RSoftwareComponentRegistryFilesList::Close()
	{
	RScsClientSubsessionBase::Close();
	}

void RSoftwareComponentRegistryFilesList::CheckSubSessionHandleL() const
    {
    if(KNullHandle == SubSessionHandle())
        {
        DEBUG_PRINTF(_L("An invalid handle has been passed."));
        User::Leave(KErrBadHandle);
        }
    }

//
// RApplicationInfoView
//

TInt RApplicationInfoView::SendSyncMessage(TInt aFunction, const TIpcArgs& aArgs) const
    {
    return CallSubsessionFunction(aFunction, aArgs);
    }

EXPORT_C RApplicationInfoView::RApplicationInfoView()
/**
    This constructor provides a single point of definition from
    which the superclass constructor is called.
 */
    : RScsClientSubsessionBase()
    {
    }

EXPORT_C void RApplicationInfoView::Close()
    {
    RScsClientSubsessionBase::Close();
    }

EXPORT_C void RApplicationInfoView::OpenViewL(const RSoftwareComponentRegistry& aCompReg, CAppInfoFilter* aAppInfoFilter,TLanguage aLocale)
    {

    DEBUG_PRINTF(_L("Creating a subsession to the SCR server for the AppInfo view."));
    
    Close(); //Close the SubSession before Re-Opening the same.
    TInt err = CreateSubsession(aCompReg, ESubSessCreateAppInfoView, TIpcArgs());
    if(KErrNone != err)
        {
        DEBUG_PRINTF2(_L("Failed to create the subsession to the SCR server for the appinfo view (Error:%d)"), err);
        User::Leave(err);
        }
    
    RBuf8 buf;
    buf.CleanupClosePushL();
    ExternalizeObjectL(aAppInfoFilter, buf);
    
    DEBUG_PRINTF(_L("Sending the request to create a component view on the server side."));
    TIpcArgs args(&buf, aLocale);
    User::LeaveIfError(CallSubsessionFunction(EOpenAppInfoView, args));
    CleanupStack::PopAndDestroy(&buf);
    }


EXPORT_C void  RApplicationInfoView::GetNextAppInfoL(TInt aNoOfEntries, RPointerArray<TAppRegInfo>& aAppInfoList) const
    {
    if(0 >= aNoOfEntries)
         {
         DEBUG_PRINTF(_L("No of Required AppInfo entries passed should be greater than 0"));
         User::Leave(KErrArgument);
         }

    //To avoid call to GetNextAppInfoL before calling OpenAppInfoViewL 
    if(KNullHandle == SubSessionHandle())
        {
        DEBUG_PRINTF(_L("An invalid handle has been passed."));
        User::Leave(KErrBadHandle);
        }
    
    TAppRegInfo* appInfo = NULL;         
    TBool isAppInfoPresent = EFalse;
    for(TInt i=0; i < aNoOfEntries ; ++i)
       {
       appInfo = new(ELeave) TAppRegInfo();
       DEBUG_PRINTF(_L("Sending the request to retrieve the next Application Info from the AppInfo view."));
       TIpcArgs args(TIpcArgs::ENothing);
       TInt argNum = 1;
            
       TRAPD(err,isAppInfoPresent = GetObjectL(*this, *appInfo, EGetNextAppInfoSize, EGetNextAppInfoData, argNum, args));
       if(KErrNone != err)
           {
           delete appInfo;
           User::Leave(err);
           }
                        
       if(!isAppInfoPresent)
         {
         DEBUG_PRINTF(_L8("Reached the end of the view."));     
         delete appInfo;
         break;
         }
       else
         {
         DEBUG_PRINTF2(_L("The Uid of the App received from the SCR server is 0x%x "), appInfo->Uid());
         DEBUG_PRINTF2(_L("The App filename of the App received from the SCR server is %s "), appInfo->FullName().PtrZ());
         DEBUG_PRINTF2(_L("The Short caption of the received from the SCR server App is %s "), appInfo->ShortCaption().PtrZ());
         DEBUG_PRINTF2(_L("The Caption of the App received from the SCR server is %s "), appInfo->Caption().PtrZ());
         aAppInfoList.AppendL(appInfo);
         }            
       }
    }

//
// RRegistrationInfoForApplication

TInt RRegistrationInfoForApplication::SendSyncMessage(TInt aFunction, const TIpcArgs& aArgs) const
	{
	return CallSubsessionFunction(aFunction, aArgs);
	}

EXPORT_C RRegistrationInfoForApplication::RRegistrationInfoForApplication()	: RScsClientSubsessionBase()
	{
	// empty
	}

EXPORT_C void RRegistrationInfoForApplication::OpenL(const RSoftwareComponentRegistry& aCompReg,const TUid aAppUid,TLanguage aLocale)
	{
	DEBUG_PRINTF(_L("Creating a subsession to the SCR server for the App list based on AppId."));
	
	Close(); //Close the SubSession before Re-Opening the same.
	TInt err = CreateSubsession(aCompReg, ESubSessCreateRegInfoForApp, TIpcArgs());
	if(KErrNone != err)
		{
		DEBUG_PRINTF2(_L("Failed to create the subsession to the SCR server (Error:%d)."), err);
		User::Leave(err);
		}
	
	DEBUG_PRINTF(_L("Sending a request to set the AppId on the server side."));
	TIpcArgs args(aAppUid.iUid,aLocale);
	User::LeaveIfError(CallSubsessionFunction(ESetAppUid, args));
	}

EXPORT_C  void RRegistrationInfoForApplication::GetAppServicesUidsL(RArray<TUid>& aServiceUids) const
	{
	DEBUG_PRINTF(_L("Getting the Service UID array for the subsession appUid"));
    //To avoid call to GetAppServicesUidsL before calling OpenL 
	CheckSubSessionHandleL();
    
	TIpcArgs args(TIpcArgs::ENothing);
	TInt argNum = 1;
	
	GetObjectArrayL(*this, EGetServiceUidSize,EGetServiceUidList,argNum, args, aServiceUids);	
	}

EXPORT_C  TLanguage RRegistrationInfoForApplication::ApplicationLanguageL() const
	{
	DEBUG_PRINTF(_L("Getting the Locale of the specified App that best fits the current device language"));
    //To avoid call to GetApplicationLanguageL before calling OpenL 
	CheckSubSessionHandleL();
    
	TLanguage aLanguage = ELangNone;
	TPckg<TLanguage> applicationLanguage(aLanguage);
	    
	TIpcArgs args(&applicationLanguage);
	
	User::LeaveIfError(CallSubsessionFunction(EGetApplicationLanguage, args));
	return(aLanguage);
	}

EXPORT_C  TInt RRegistrationInfoForApplication::DefaultScreenNumberL() const
	{
	DEBUG_PRINTF(_L("Getting the Default screen number from the App reg info"));
    //To avoid call to GetDefaultScreenNumberL before calling OpenL 
	CheckSubSessionHandleL();
    
	TInt aDefaultScreenNumber = 0;
	TPckg<TInt> uidDes(aDefaultScreenNumber);
	
	TIpcArgs args(&uidDes);
	User::LeaveIfError(CallSubsessionFunction(EGetDefaultScreenNumber, args));	
	return(aDefaultScreenNumber);
	}

EXPORT_C  TInt RRegistrationInfoForApplication::NumberOfOwnDefinedIconsL() const
	{
	DEBUG_PRINTF(_L("Getting the number of defined icons for the associated application and locale "));
    //To avoid call to GetNumberOfOwnDefinedIconsL before calling OpenL 
	CheckSubSessionHandleL();
    
	TInt aCount = 0;
	TPckg<TInt> uidDes(aCount);
	TIpcArgs args(&uidDes);
	
	User::LeaveIfError(CallSubsessionFunction(EGetNumberOfOwnDefinedIcons, args));
	return(aCount);
	}

EXPORT_C void RRegistrationInfoForApplication::GetAppViewsL(RPointerArray<Usif::CAppViewData>& aAppViewInfoArray) const
    {
        DEBUG_PRINTF(_L("Getting the view details associated with subsession appUid"));
        //To avoid call to GetAppViewsL before calling OpenL 
        CheckSubSessionHandleL();
        
        TIpcArgs args(TIpcArgs::ENothing);
	 	TInt argNum = 1;
        GetObjectArrayL(*this, EGetViewSize, EGetViewData, argNum, args, aAppViewInfoArray);
    }

EXPORT_C void RRegistrationInfoForApplication::Close()
    {
    RScsClientSubsessionBase::Close();
    }

EXPORT_C void RRegistrationInfoForApplication::GetAppOwnedFilesL(RPointerArray<HBufC>& aAppOwnedFiles) const
    {
     DEBUG_PRINTF(_L("Sending the request to retrieve the list of files owned by appUid associated with subsession "));
     //To avoid call to GetAppOwnedFilesL before calling OpenL 
     CheckSubSessionHandleL();
     
     TIpcArgs args(TIpcArgs::ENothing);
     TInt argNum = 1;
     GetObjectArrayL(*this, EGetAppOwnedFilesSize, EGetAppOwnedFilesData, argNum, args,aAppOwnedFiles);
    }

EXPORT_C void RRegistrationInfoForApplication::GetAppCharacteristicsL(TApplicationCharacteristics &aApplicationCharacteristics) const
    {
    DEBUG_PRINTF(_L("Sending the request to retrieve application characteristics."));
    //To avoid call to GetAppCharacteristicsL before calling OpenL 
    CheckSubSessionHandleL();
    
    TPckg<Usif::TApplicationCharacteristics> pckg(aApplicationCharacteristics);
    TIpcArgs args(&pckg);
    User::LeaveIfError(CallSubsessionFunction(EGetAppCharacteristics, args));
    }
	
EXPORT_C void RRegistrationInfoForApplication::GetAppIconL(HBufC*& aFullFileName) const
    {
    DEBUG_PRINTF(_L("Sending the request to get application icon file name ."));    
    //To avoid call to GetAppIconL before calling OpenL 
    CheckSubSessionHandleL();
    
    TFileName fileName;
    TPckg<TFileName> filenamePckg(fileName);
    TIpcArgs args(&filenamePckg);

    User::LeaveIfError(CallSubsessionFunction(EGetAppIconForFileName, args));
    
    aFullFileName = fileName.AllocL();

    }
	
EXPORT_C void RRegistrationInfoForApplication::GetAppViewIconL(TUid aViewUid,HBufC*& aFullFileName) const
    {
    DEBUG_PRINTF(_L("Sending the request to get application view icon file name ."));
    //To avoid call to GetAppViewIconL before calling OpenL 
    CheckSubSessionHandleL();
    
    TFileName fileName;
    TPckg<TFileName> filenamePckg(fileName);
    TIpcArgs args(aViewUid.iUid,&filenamePckg);
    
    User::LeaveIfError(CallSubsessionFunction(EGetAppViewIconFileName, args));
 
    aFullFileName = fileName.AllocL();
    }
	
void RRegistrationInfoForApplication::CheckSubSessionHandleL() const
    {
    if(KNullHandle == SubSessionHandle())
        {
        DEBUG_PRINTF(_L("An invalid handle has been passed."));
        User::Leave(KErrBadHandle);
        }
    }

//
// RApplicationRegistrationInfo

TInt RApplicationRegistrationInfo::SendSyncMessage(TInt aFunction, const TIpcArgs& aArgs) const
    {
    return CallSubsessionFunction(aFunction, aArgs);
    }

EXPORT_C RApplicationRegistrationInfo::RApplicationRegistrationInfo() : RScsClientSubsessionBase()
    {
    // empty
    }

EXPORT_C void RApplicationRegistrationInfo::OpenL(const RSoftwareComponentRegistry& aCompReg)
    {
    DEBUG_PRINTF(_L("Creating a subsession to the SCR server for the App list."));
    
    Close(); //Close the SubSession before Re-Opening the same.
    TInt err = CreateSubsession(aCompReg, ESubSessCreateAppRegInfo, TIpcArgs());
    if(KErrNone != err)
        {
        DEBUG_PRINTF2(_L("Failed to create the subsession to the SCR server (Error:%d)."), err);
        User::Leave(err);
        }    
    }

EXPORT_C  TUid RApplicationRegistrationInfo::GetAppForDataTypeAndServiceL(const TDesC &aName,const TUid aServiceUid) const
	{
	DEBUG_PRINTF(_L("Get the AppUid corresponding to the mentioned Service and Type with the highest priority"));
    //To avoid call to GetAppForDataTypeAndServiceL before calling OpenL 
	CheckSubSessionHandleL();
    
	TUid appUid;
	
	TPckg<TUid> uidDes(appUid);	    
	TIpcArgs args(&aName, aServiceUid.iUid,&uidDes );
	
	User::LeaveIfError(CallSubsessionFunction(EGetAppForDataTypeAndService, args));
	return(appUid);
	}

EXPORT_C  TUid RApplicationRegistrationInfo::GetAppForDataTypeL(const TDesC &aName) const
	{
	DEBUG_PRINTF(_L("Getting the AppUid corresponding to the mentioned Type with the highest priority"));
    //To avoid call to GetAppForDataTypeL before calling OpenL 
	CheckSubSessionHandleL();
    
	TUid appUid;
	
    TPckg<TUid> uidDes(appUid);  
	TIpcArgs args(&aName, &uidDes);
	 
	User::LeaveIfError(CallSubsessionFunction(EGetAppForDataType, args));
	return(appUid);
	
	}

EXPORT_C void RApplicationRegistrationInfo::GetServiceInfoL(CAppServiceInfoFilter* aAppServiceInfoFilter, RPointerArray<Usif::CServiceInfo>& aAppServiceInfoArray, TLanguage aLocale) const
    {
    DEBUG_PRINTF(_L("Getting the Service Info details associated with the parameters set in the filter."));
    //To avoid call to GetServiceInfoL before calling OpenL 
    CheckSubSessionHandleL();
    
    RBuf8 buf;
    buf.CleanupClosePushL();
    ExternalizeObjectL(aAppServiceInfoFilter, buf);
        
    TIpcArgs args(&buf, aLocale);
    TInt argNum = 2;
    GetObjectArrayL(*this, EGetAppServiceInfoSize, EGetAppServiceInfoData, argNum, args, aAppServiceInfoArray);
    CleanupStack::PopAndDestroy(&buf);
    }

EXPORT_C TBool RApplicationRegistrationInfo::GetAppInfoL(TUid aAppUid, TAppRegInfo& aAppInfo, TLanguage aLocale)
    {
    DEBUG_PRINTF2(_L("Getting information details of app 0x%x."), aAppUid.iUid);
    
    //To avoid call to GetAppInfoL before calling OpenL 
    CheckSubSessionHandleL();
    
    TPckg<TAppRegInfo> appRegInfoPckg(aAppInfo);
   
    TIpcArgs args(aAppUid.iUid, aLocale, &appRegInfoPckg);
    User::LeaveIfError(CallSubsessionFunction(EGetApplicationInfo, args));
    if(aAppInfo.Uid().iUid!= NULL)
        {
        return ETrue;
        }
    else
        return EFalse;
    }


EXPORT_C void RApplicationRegistrationInfo::Close()
	{
	RScsClientSubsessionBase::Close();
	}

void RApplicationRegistrationInfo::CheckSubSessionHandleL() const
    {
    if(KNullHandle == SubSessionHandle())
        {
        DEBUG_PRINTF(_L("An invalid handle has been passed."));
        User::Leave(KErrBadHandle);
        }
    }


//
// RApplicationRegistryView
//

TInt RApplicationRegistryView::SendSyncMessage(TInt aFunction, const TIpcArgs& aArgs) const
    {
    return CallSubsessionFunction(aFunction, aArgs);
    }

EXPORT_C RApplicationRegistryView::RApplicationRegistryView() : RScsClientSubsessionBase()
    {
    }

EXPORT_C void RApplicationRegistryView::Close()
    {
    RScsClientSubsessionBase::Close();
    }

EXPORT_C void RApplicationRegistryView::OpenViewL(const RSoftwareComponentRegistry& aCompReg, TLanguage aLocale)
    {   
    DEBUG_PRINTF(_L("Creating a subsession to the SCR server for the Application Registration view."));

    Close(); //Close the SubSession before Re-Opening the same.
    
    TInt err = CreateSubsession(aCompReg, ESubSessCreateAppRegistryView, TIpcArgs());
    if(KErrNone != err)
        {
        DEBUG_PRINTF2(_L("Failed to create the subsession to the SCR server for the Application Registration view (Error:%d)"), err);
        User::Leave(err);
        }                
    DEBUG_PRINTF(_L("Sending the request to create an applicaiton registry view on the server side."));
    TIpcArgs args(aLocale);
    User::LeaveIfError(CallSubsessionFunction(EOpenApplicationRegistrationInfoView, args));        
    }
	
EXPORT_C void RApplicationRegistryView::OpenViewL(const RSoftwareComponentRegistry& aCompReg, const RArray<TUid>& aAppRegAppUids, TLanguage aLocale)
    {   
    DEBUG_PRINTF(_L("Creating a subsession to the SCR server for the Application Registration view with specified AppUids."));
    
    Close();  //Close the SubSession before Re-Opening the same.
    
    TInt err = CreateSubsession(aCompReg, ESubSessCreateAppRegistryView, TIpcArgs());
    if(KErrNone != err)
        {
        DEBUG_PRINTF2(_L("Failed to create the subsession to the SCR server for the Application Registration view with specified AppUids (Error:%d)"), err);
        User::Leave(err);
        }                
    DEBUG_PRINTF(_L("Sending the request to create an applicaiton registry view on the server side."));
     
    TInt32 size = aAppRegAppUids.Count();
    const TInt32 maxBufSize= sizeof(TInt32)+ size*sizeof(TInt32);   // number of entries +  number of entry * size of AppUid stored as TUid
      
    // allocate buffer for the array
    HBufC8* bufForAppUids = HBufC8::NewMaxLC(maxBufSize);
    TPtr8 ptrBufForAppUids = bufForAppUids->Des();
    RDesWriteStream instream(ptrBufForAppUids);
    CleanupClosePushL(instream);
    instream.WriteInt32L(size);
    for (TInt i = 0; i < size; ++i)
        {
        instream.WriteInt32L(aAppRegAppUids[i].iUid);
        }
        
    instream.CommitL();
 
    TIpcArgs args(aLocale);
    args.Set(1,&ptrBufForAppUids);
    User::LeaveIfError(CallSubsessionFunction(EOpenApplicationRegistrationInfoForAppUidsView, args));  
   
    CleanupStack::PopAndDestroy(2,bufForAppUids); // bufForAppUid, instream 
    }

EXPORT_C void RApplicationRegistryView::GetNextApplicationRegistrationInfoL(TInt aNoOfEntries, RPointerArray<CApplicationRegistrationData>& aApplicationRegistration) const
    {
    if(0 >= aNoOfEntries)
      {
      DEBUG_PRINTF(_L("No of Required View entries passed should be greater than 0"));
      User::Leave(KErrArgument);
      }
   //To avoid call to GetNextApplicationRegistrationInfoL before calling OpenApplicationRegistrationViewL 
   if(KNullHandle == SubSessionHandle())
     {
     DEBUG_PRINTF(_L("An invalid handle has been passed."));
     User::Leave(KErrBadHandle);
     }
    
    CApplicationRegistrationData* applicationRegistration = NULL;
    for(TInt i = 0 ; i < aNoOfEntries ; ++i)
        {
          applicationRegistration = CApplicationRegistrationData::NewLC();
          TBool isApplicationRegistrationDataPresent = EFalse;                                    
          DEBUG_PRINTF(_L("Sending the request to retrieve the next application details from the Application Registration view."));
          TIpcArgs args;
          TInt argNum = 1;
          isApplicationRegistrationDataPresent = GetObjectL(*this, *applicationRegistration, EGetApplicationRegistrationSize, EGetApplicationRegistrationData, argNum, args);
          if(!isApplicationRegistrationDataPresent)
            {
            DEBUG_PRINTF(_L8("Reached the end of the view."));  
            CleanupStack::PopAndDestroy(applicationRegistration);
            break;
            }
          else
            {
            DEBUG_PRINTF(_L("received the application registration data from the SCR Server "));                                       
            aApplicationRegistration.AppendL(applicationRegistration);
            CleanupStack::Pop(applicationRegistration);
            }            
        }
    }
