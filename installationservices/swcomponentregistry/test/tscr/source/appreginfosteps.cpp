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
* Implements test steps for component management APIs in the SCR
*
*/

#include "appreginfosteps.h"
#include "tscrdefs.h"
#include <scs/cleanuputils.h>
#include <usif/usiferror.h>

using namespace Usif;

// -----------CScrAddApplicationEntryStep-----------------

CScrAddApplicationEntryStep::CScrAddApplicationEntryStep(CScrTestServer& aParent) : CScrTestStep(aParent)
    {
    }

void CScrAddApplicationEntryStep::ImplTestStepPreambleL()
    {
    User::LeaveIfError(iScrSession.Connect());    
    }

void CScrAddApplicationEntryStep::ImplTestStepL()
    {
    TComponentId compId = 0;
    TBool newComponentAdded(EFalse);
   
    // Checking if have already inserted the component
    TBool isMultiApp(EFalse);
    GetBoolFromConfig(ConfigSection(), _L("IsMultiApp"), isMultiApp);
    if (isMultiApp)
        {
        INFO_PRINTF1(_L("Checking if the component is already present"));
        CGlobalComponentId *globalId = GetGlobalComponentIdLC();
        TRAP_IGNORE(compId = iScrSession.GetComponentIdL(globalId->GlobalIdName(), globalId->SoftwareTypeName()););
        CleanupStack::PopAndDestroy(globalId);
        }

	// Try adding the app to a component. The add operation will fail if the component is not present.
	if (!compId) 
		{
		if (GetIntFromConfig(ConfigSection(), _L("UseComponentId"), compId))
			INFO_PRINTF2(_L("Adding a new AppRegInfo to component %d"), compId);
		}
    
    // The component is not present already
    if (!compId)   
        {
        INFO_PRINTF1(_L("Adding a new component."));
        compId = AddNonLocalisableComponentL(iScrSession);
        newComponentAdded = ETrue;
        }

	INFO_PRINTF1(_L("Get reg info from config file."));
	const CApplicationRegistrationData* appRegData = GetAppRegInfoFromConfigLC();
	
	if (iIsPerformanceTest)
        StartTimer(); // Start the timer in the case of a performance tests        
	TRAPD(err, iScrSession.AddApplicationEntryL(compId, *appRegData));
    if (iIsPerformanceTest)
        StopTimerAndPrintResultL(); // Stop the timer and analyze the time taken after performance tests
        
	if (err != KErrNone)
	    {
		if (newComponentAdded)
			{
			// We are going to delete the component with no apps
			iScrSession.DeleteComponentL(compId);
			}
		User::Leave(err);
	    }

	CleanupStack::PopAndDestroy();
	}
    
void CScrAddApplicationEntryStep::ImplTestStepPostambleL()
    {
   
    }


// -----------CScrDeleteApplicationEntriesStep-----------------

CScrDeleteApplicationEntriesStep::CScrDeleteApplicationEntriesStep(CScrTestServer& aParent) : CScrTestStep(aParent)
    {
    }

void CScrDeleteApplicationEntriesStep::ImplTestStepPreambleL()
    {
    CScrTestStep::ImplTestStepPreambleL();
    }

void CScrDeleteApplicationEntriesStep::ImplTestStepL()
    {
    TComponentId compId = 0;
   
    INFO_PRINTF1(_L("Fetching the component Id."));
    
    TUid appUid = TUid::Null();
    TRAP_IGNORE(GetAppUidL(appUid));
    
    if(appUid == TUid::Null())
        {
        SetTestStepResult(EPass);
        CGlobalComponentId *globalId = GetGlobalComponentIdLC();
        compId = iScrSession.GetComponentIdL(globalId->GlobalIdName(), globalId->SoftwareTypeName());
        CleanupStack::PopAndDestroy(globalId);
        }
    else
        {
        compId = iScrSession.GetComponentIdForAppL(appUid);
        }

    INFO_PRINTF2(_L("Deleting app entries for component %d"), compId);
    iScrSession.DeleteApplicationEntriesL(compId);
    INFO_PRINTF2(_L("Deleting component %d"), compId);
    iScrSession.DeleteComponentL(compId);    
    }
    
void CScrDeleteApplicationEntriesStep::ImplTestStepPostambleL()
    {
    CScrTestStep::ImplTestStepPostambleL();
    }

CSCRGetAppServicesUids::CSCRGetAppServicesUids(CScrTestServer& aParent) : CScrTestStep(aParent)
    {
    }

void CSCRGetAppServicesUids::ImplTestStepPreambleL()
    {
    CScrTestStep::ImplTestStepPreambleL();
    }

void CSCRGetAppServicesUids::ImplTestStepL()
    {
	TUid appUid, serviceUid;
	TInt noOfServiceUids = 0;
	RArray<TUid> expectedServiceUids;
	RArray<TUid> obtainedServiceUids;
	TBuf<20> serviceUidName;
    CleanupClosePushL(expectedServiceUids);
    CleanupClosePushL(obtainedServiceUids);
    GetIntFromConfig(ConfigSection(), _L("NoOfServiceUids"), noOfServiceUids);
    for (TUint i = 0; i < noOfServiceUids; ++i)
        {                
        serviceUidName = KServiceUid;
        GenerateIndexedAttributeNameL(serviceUidName, i);
        GetUidFromConfig(ConfigSection(), serviceUidName, serviceUid);
        expectedServiceUids.AppendL(serviceUid);
        }
        
	GetAppUidL(appUid);
	RRegistrationInfoForApplication InfoQuerySubSession;
	CleanupClosePushL(InfoQuerySubSession);	
	
	InfoQuerySubSession.OpenL(iScrSession, appUid);

	TRAPD(err,InfoQuerySubSession.GetAppServicesUidsL(obtainedServiceUids));
	
	TInt noOfExpectedServiceUids = expectedServiceUids.Count();
	TInt noOfObtainedServiceUids = obtainedServiceUids.Count();
	 if(err != KErrNone && err !=KErrNotFound)
         {
         INFO_PRINTF2(_L("Failed to Get App Uid , error: %d"), err);
         SetTestStepResult(EFail);        
         }      
	         	              
	    
	if(noOfExpectedServiceUids != noOfObtainedServiceUids)
	    {
	    INFO_PRINTF3(_L("Service Uid's Count doesn't match: expected/obtained Uid count: %d/%d"), noOfExpectedServiceUids, noOfObtainedServiceUids);
	    SetTestStepResult(EFail);	 
	    }
	else
	    {
        //Comparing expected result and Obtained Result
        for(TInt i=0 ; i < noOfExpectedServiceUids ; i++)
            {
            if(expectedServiceUids[i] != obtainedServiceUids[i])
                {
                INFO_PRINTF3(_L("Service Uid doesn't match: expected/obtained Uid: 0x%08x/0x%08x"), expectedServiceUids[i].iUid, obtainedServiceUids[i].iUid);
                SetTestStepResult(EFail);           
                }
            }	
	    }
	InfoQuerySubSession.Close();	
	obtainedServiceUids.Close();
	expectedServiceUids.Close();
	CleanupStack::Pop(3, &expectedServiceUids);	    
    }
    
void CSCRGetAppServicesUids::ImplTestStepPostambleL()
    {
    CScrTestStep::ImplTestStepPostambleL();
    }

CSCRGetAppForDataType::CSCRGetAppForDataType(CScrTestServer& aParent) : CScrTestStep(aParent)
    {
    }

void CSCRGetAppForDataType::ImplTestStepPreambleL()
    {
    CScrTestStep::ImplTestStepPreambleL();
    }

void CSCRGetAppForDataType::ImplTestStepL()
    {
    TUid obtainedAppUid,expectedAppUid;
        
    TPtrC dataType;
    if(!GetStringFromConfig(ConfigSection(), KServiceDataType, dataType))
        {
        INFO_PRINTF1(_L("Failed to Get Data Type from ini"));
        SetTestStepResult(EFail);      
        }
    
    TBool uidExists = EFalse;
    GetUidFromConfig(ConfigSection(), KAppUid, expectedAppUid);
    GetBoolFromConfig(ConfigSection(), _L("UidExists"), uidExists);

    RApplicationRegistrationInfo InfoQuerySubSession;
    CleanupClosePushL(InfoQuerySubSession);
    
    
    InfoQuerySubSession.OpenL(iScrSession);

    TRAPD(err,obtainedAppUid = InfoQuerySubSession.GetAppForDataTypeL(dataType));        
         
     if(err != KErrNone && uidExists)
         {
         INFO_PRINTF2(_L("Failed to Get App Uid , error: %d"), err);
         SetTestStepResult(EFail);        
         }      
         
     //Comparing expected result and Obtained Result
     if(expectedAppUid != obtainedAppUid && uidExists)
         {
         INFO_PRINTF3(_L("App Uid doesn't match: expected/obtained Uid: 0x%08x/0x%08x"), expectedAppUid, obtainedAppUid);
         SetTestStepResult(EFail);        
         }         
    
    InfoQuerySubSession.Close();        
    CleanupStack::PopAndDestroy();        
    }
    
void CSCRGetAppForDataType::ImplTestStepPostambleL()
    {
    CScrTestStep::ImplTestStepPostambleL();
    }

CSCRGetAppForDataTypeAndService::CSCRGetAppForDataTypeAndService(CScrTestServer& aParent) : CScrTestStep(aParent)
    {
    }

void CSCRGetAppForDataTypeAndService::ImplTestStepPreambleL()
    {
    CScrTestStep::ImplTestStepPreambleL();
    }

void CSCRGetAppForDataTypeAndService::ImplTestStepL()
    {
    TUid obtainedAppUid, expectedAppUid, serviceUid;    
        
    TPtrC dataType;
    if(!GetStringFromConfig(ConfigSection(), KServiceDataType, dataType))
        {
        INFO_PRINTF1(_L("Failed to Get Data Type from ini"));
        SetTestStepResult(EFail);      
        }
    
    //Get serviceUid from ini
    if(!GetUidFromConfig(ConfigSection(), KServiceUid, serviceUid))
        {
        INFO_PRINTF1(_L("Failed to Service uid from ini"));
        SetTestStepResult(EFail);
        }
    
    //Get expected Appuid from ini
    GetUidFromConfig(ConfigSection(), KAppUid, expectedAppUid);
    
    TBool uidExists = EFalse;
    GetBoolFromConfig(ConfigSection(), _L("UidExists"), uidExists);
    
    RApplicationRegistrationInfo InfoQuerySubSession;
    CleanupClosePushL(InfoQuerySubSession);
    InfoQuerySubSession.OpenL(iScrSession);
    
    TRAPD(err,obtainedAppUid = InfoQuerySubSession.GetAppForDataTypeAndServiceL(dataType,serviceUid));
        
    if(err != KErrNone && uidExists)
        {
        INFO_PRINTF2(_L("Failed to Get App Uid , error: %d"), err);
        SetTestStepResult(EFail);        
        }      
        
    //Comparing expected result and Obtained Result
    if(expectedAppUid != obtainedAppUid && uidExists)
        {
        INFO_PRINTF3(_L("App Uid doesn't match: expected/obtained Uid: 0x%08x/0x%08x"), expectedAppUid, obtainedAppUid);
        SetTestStepResult(EFail);        
        }    
    
    InfoQuerySubSession.Close();        
    CleanupStack::PopAndDestroy();        
    }
    
void CSCRGetAppForDataTypeAndService::ImplTestStepPostambleL()
    {
    CScrTestStep::ImplTestStepPostambleL();
    }

CSCRGetDefaultScreenNumber::CSCRGetDefaultScreenNumber(CScrTestServer& aParent) : CScrTestStep(aParent)
    {
    }

void CSCRGetDefaultScreenNumber::ImplTestStepPreambleL()
    {
    CScrTestStep::ImplTestStepPreambleL();
    }

void CSCRGetDefaultScreenNumber::ImplTestStepL()
    {
    TUid appUid;
    TInt obtaineddefaultScreenNumber = 0 , expecteddefaultScreenNumber = 0;
    
    //Get the Default screen Number from the ini to compare
    GetIntFromConfig(ConfigSection(), _L("DefaultScreenNumber"), expecteddefaultScreenNumber);
               
    GetAppUidL(appUid);
    RRegistrationInfoForApplication InfoQuerySubSession;
    CleanupClosePushL(InfoQuerySubSession);
    INFO_PRINTF1(_L("After subsession creation"));
    
    InfoQuerySubSession.OpenL(iScrSession, appUid);
    TRAPD(err, obtaineddefaultScreenNumber = InfoQuerySubSession.DefaultScreenNumberL());

    if(err != KErrNone && expecteddefaultScreenNumber != 0)
       {
       INFO_PRINTF2(_L("Failed to Get Default Screen Number, error: %d"), err);
       SetTestStepResult(EFail);        
       }      
       
   //Comparing expected result and Obtained Result
   if(expecteddefaultScreenNumber != obtaineddefaultScreenNumber && expecteddefaultScreenNumber != 0)
       {
       INFO_PRINTF3(_L("Screen Number doesn't match: expected/obtained Uid: %d/%d"), expecteddefaultScreenNumber, obtaineddefaultScreenNumber);
       SetTestStepResult(EFail);        
       }    
    
    InfoQuerySubSession.Close();        
    CleanupStack::PopAndDestroy();
    }
    
void CSCRGetDefaultScreenNumber::ImplTestStepPostambleL()
    {
    CScrTestStep::ImplTestStepPostambleL();
    }

CSCRGetNumberOfDefinedIcons::CSCRGetNumberOfDefinedIcons(CScrTestServer& aParent) : CScrTestStep(aParent)
    {
    }

void CSCRGetNumberOfDefinedIcons::ImplTestStepPreambleL()
    {
    CScrTestStep::ImplTestStepPreambleL();
    }

void CSCRGetNumberOfDefinedIcons::ImplTestStepL()
    {
    TUid appUid;
    TInt obtainedIconcount = 0, expectedIconcount = 0;
    
    //Get the Expected Number of App Icons from the ini to compare
    GetIntFromConfig(ConfigSection(), KLocNumberOfAppIcons, expectedIconcount);
    GetAppUidL(appUid);
    RRegistrationInfoForApplication InfoQuerySubSession;    
    CleanupClosePushL(InfoQuerySubSession);
    
    InfoQuerySubSession.OpenL(iScrSession, appUid);
            
    TRAPD(err,obtainedIconcount = InfoQuerySubSession.NumberOfOwnDefinedIconsL());
    
    if(err != KErrNone && expectedIconcount != 0)
       {
       INFO_PRINTF2(_L("Failed to Get Deafult Screen Number, error: %d"), err);
       SetTestStepResult(EFail);        
       }      
       
    //Comparing expected result and Obtained Result
    if(expectedIconcount != obtainedIconcount && expectedIconcount != -1)
       {
       INFO_PRINTF3(_L("Number of Defined Icons for App doesn't match: expected/obtained Uid: %d/%d"), expectedIconcount, obtainedIconcount);
       SetTestStepResult(EFail);        
       } 
    
    InfoQuerySubSession.Close();        
    CleanupStack::PopAndDestroy();
        
    }
    
void CSCRGetNumberOfDefinedIcons::ImplTestStepPostambleL()
    {
    CScrTestStep::ImplTestStepPostambleL();
    }

CSCRGetApplicationLanguage::CSCRGetApplicationLanguage(CScrTestServer& aParent) : CScrTestStep(aParent)
    {
    }

void CSCRGetApplicationLanguage::ImplTestStepPreambleL()
    {
    CScrTestStep::ImplTestStepPreambleL();
    }

void CSCRGetApplicationLanguage::ImplTestStepL()
    {
    TUid appUid;    
    TInt obtainedCurrentLanguage = 0, expectedCurrentLanguage = 0;
    
    //Get the Expected Current application Language from the ini to compare
    GetIntFromConfig(ConfigSection(), KLocAppLanguage, expectedCurrentLanguage);
    
    GetAppUidL(appUid);
    INFO_PRINTF1(_L("Before subsession creation"));

    RRegistrationInfoForApplication InfoQuerySubSession;
    CleanupClosePushL(InfoQuerySubSession);
    
    InfoQuerySubSession.OpenL(iScrSession, appUid);
    
    TRAPD(err, obtainedCurrentLanguage = (TInt)InfoQuerySubSession.ApplicationLanguageL());
    
    if(err != KErrNone && expectedCurrentLanguage != 0)
       {
       INFO_PRINTF2(_L("Failed to Get Current App Language, error: %d"), err);
       SetTestStepResult(EFail);        
       }      
       
    //Comparing expected result and Obtained Result
    if(expectedCurrentLanguage != obtainedCurrentLanguage && expectedCurrentLanguage != 0)
       {
       INFO_PRINTF3(_L("Number of Defined Icons for App doesn't match: expected/obtained Uid: %d/%d"), expectedCurrentLanguage, obtainedCurrentLanguage);
       SetTestStepResult(EFail);        
       } 
        
    InfoQuerySubSession.Close();     
    CleanupStack::PopAndDestroy(&InfoQuerySubSession);
    }
    
void CSCRGetApplicationLanguage::ImplTestStepPostambleL()
    {
    CScrTestStep::ImplTestStepPostambleL();
    }

// -----------CScrGetAppOwnedFilesEntryStep-----------------

CScrGetAppOwnedFilesEntryStep::CScrGetAppOwnedFilesEntryStep(CScrTestServer& aParent) : CScrTestStep(aParent)
    {
    }

void CScrGetAppOwnedFilesEntryStep::ImplTestStepPreambleL()
    {
    CScrTestStep::ImplTestStepPreambleL();
    }

void CScrGetAppOwnedFilesEntryStep::ImplTestStepL()
    {
    INFO_PRINTF1(_L("Get the UID from the ini for which the sub session is to be opened"));
    TUid  appUid;
    GetAppUidL(appUid);
    
    INFO_PRINTF1(_L("Get the expected App owned files from ini"));
    RPointerArray<HBufC> appOwnedFilesFromIni;
    GetAppOwnedFilesL(appOwnedFilesFromIni);
   
    INFO_PRINTF1(_L("Open the sub session for the UID"));
    RRegistrationInfoForApplication AppRegInfoQueryAppSubSession;    
    TRAPD(sessionerr,AppRegInfoQueryAppSubSession.OpenL(iScrSession,appUid));
    if(KErrNone != sessionerr)
           {
           INFO_PRINTF2(_L("Failed to create the subsession to the SCR server (Error:%d)."), sessionerr);
           User::Leave(sessionerr);
           }    
    
    INFO_PRINTF1(_L("Retrieving the App Owned Files form db."));
    RPointerArray<HBufC> appOwnedFilesArray;     
    TInt err = KErrNone;
    TRAP(err, AppRegInfoQueryAppSubSession.GetAppOwnedFilesL(appOwnedFilesArray));
    if(KErrNone == err)
        {
        INFO_PRINTF1(_L("Comparing the retrieved AppOwned files with the expected files within the ini"));
        if(CompareAppOwnedFileNamesL(appOwnedFilesArray,appOwnedFilesFromIni))
            SetTestStepResult(EPass);
        else
            SetTestStepResult(EFail);
        }
    else
        {
        appOwnedFilesArray.ResetAndDestroy();    
        appOwnedFilesFromIni.ResetAndDestroy();
        AppRegInfoQueryAppSubSession.Close();
        User::Leave(err);
        }               
   
    //closing the resource handles
    appOwnedFilesArray.ResetAndDestroy();    
    appOwnedFilesFromIni.ResetAndDestroy();
    AppRegInfoQueryAppSubSession.Close();
    }
    
TBool CScrGetAppOwnedFilesEntryStep::CompareAppOwnedFileNamesL(RPointerArray<HBufC>& aActualValue,RPointerArray<HBufC>& aExpetedValue)
    {
    TInt expectedCount = aExpetedValue.Count();
    TInt actualCount = aActualValue.Count();
    TBool result = ETrue;
    if(  expectedCount != actualCount )
        return EFalse;
        
    for(TInt i = 0 ; i < actualCount ; i++)            
        {
        if(!aExpetedValue.Find(aActualValue[i]))
             {
             result = EFalse;
             break;
             }
        }
    
    return result;
    }

void CScrGetAppOwnedFilesEntryStep::ImplTestStepPostambleL()
    {
    CScrTestStep::ImplTestStepPostambleL();
    }



// -----------CScrGetAppCharacteristicsEntryStep-----------------

CScrGetAppCharacteristicsEntryStep::CScrGetAppCharacteristicsEntryStep(CScrTestServer& aParent) : CScrTestStep(aParent)
    {
    }

void CScrGetAppCharacteristicsEntryStep::ImplTestStepPreambleL()
    {
    CScrTestStep::ImplTestStepPreambleL();    
    }

void CScrGetAppCharacteristicsEntryStep::ImplTestStepL()
    {
    INFO_PRINTF1(_L("Get the UID from the ini for which the sub session is to be opened"));
    TUid  appUid;
    GetAppUidL(appUid);
    
    INFO_PRINTF1(_L("Get the expected capabilty for an app from ini"));
    TInt attributes, hidden, embeddability, newFile, launch;
    TPtrC groupName;
    if(GetIntFromConfig(ConfigSection(), _L("Attributes"), attributes) == EFalse)
        PrintErrorL(_L("AppUid was not found!"), KErrNotFound);
    if(GetIntFromConfig(ConfigSection(), _L("Hidden"), hidden) == EFalse)
        PrintErrorL(_L("AppUid was not found!"), KErrNotFound);
    if(GetIntFromConfig(ConfigSection(), _L("Embeddability"), embeddability) == EFalse)
        PrintErrorL(_L("AppUid was not found!"), KErrNotFound);
    if(GetIntFromConfig(ConfigSection(), _L("NewFile"), newFile) == EFalse)
        PrintErrorL(_L("AppUid was not found!"), KErrNotFound);
    if(GetIntFromConfig(ConfigSection(), _L("Launch"), launch) == EFalse)
        PrintErrorL(_L("AppUid was not found!"), KErrNotFound);
    GetStringFromConfig(ConfigSection(), _L("GroupName"), groupName);
    
    INFO_PRINTF1(_L("Open the sub session for the UID"));
    RRegistrationInfoForApplication AppRegInfoQueryAppSubSession;
    TRAPD(sessionerr,AppRegInfoQueryAppSubSession.OpenL(iScrSession,appUid));
    if(KErrNone != sessionerr)
               {
               INFO_PRINTF2(_L("Failed to create the subsession to the SCR server (Error:%d)."), sessionerr);
               User::Leave(sessionerr);
               }    
    
    INFO_PRINTF1(_L("Retrieving characteristics for an app form db."));
    Usif::TApplicationCharacteristics appCharacteristics ;
    TInt err = KErrNone;
    TRAP(err, AppRegInfoQueryAppSubSession.GetAppCharacteristicsL(appCharacteristics));
    if(KErrNone ==err)
        {
        INFO_PRINTF1(_L("Comparing the retrieved App characteristics with the expected app characteristics within the ini"));
  
        if(appCharacteristics.iEmbeddability != embeddability || appCharacteristics.iAppIsHidden != hidden || appCharacteristics.iSupportsNewFile != newFile || appCharacteristics.iLaunchInBackground != launch || appCharacteristics.iGroupName.Compare(groupName))
            SetTestStepResult(EFail);   
        else
            SetTestStepResult(EPass);
        }
    else
        {
        AppRegInfoQueryAppSubSession.Close();        
        User::Leave(err);
        }  
    //closing the resource handles    
    AppRegInfoQueryAppSubSession.Close();
    }
    

void CScrGetAppCharacteristicsEntryStep::ImplTestStepPostambleL()
    {
    CScrTestStep::ImplTestStepPostambleL();    
    }


// -----------CScrGetAppIconFileNameEntryStep-----------------

CScrGetAppIconEntryStep::CScrGetAppIconEntryStep(CScrTestServer& aParent) : CScrTestStep(aParent)
    {
    }

void CScrGetAppIconEntryStep::ImplTestStepPreambleL()
    {
    CScrTestStep::ImplTestStepPreambleL();    
    }

void CScrGetAppIconEntryStep::ImplTestStepL()
    {        
    TVerdict result = EFail;
    INFO_PRINTF1(_L("Get the UID from the ini for which the sub session is to be opened"));
    TUid  appUid;
    GetAppUidL(appUid);
    
    INFO_PRINTF1(_L("Get the expected filename of the Icon for an app from ini"));
    TPtrC IconFileName;
    if(GetStringFromConfig(ConfigSection(), _L("LocIconFileName"), IconFileName) == EFalse)
            {           
            PrintErrorL(_L("AppUid was not found!"), KErrNotFound);
            }
    
    INFO_PRINTF1(_L("Open the sub session for the UID"));
    RRegistrationInfoForApplication AppRegInfoQueryAppSubSession;
    TRAPD(sessionerr,AppRegInfoQueryAppSubSession.OpenL(iScrSession,appUid));
    if(KErrNone != sessionerr)
            {
            INFO_PRINTF2(_L("Failed to create the subsession to the SCR server (Error:%d)."), sessionerr);
            User::Leave(sessionerr);
            }      
    
    
    HBufC* filename = NULL;        
    INFO_PRINTF1(_L("Retrieving the Icon for an app form db."));
   
    TInt err = KErrNotFound;
    TRAP(err, AppRegInfoQueryAppSubSession.GetAppIconL(filename));    
    if(KErrNone == err)
      {
      INFO_PRINTF1(_L("Comparing the retrieved App Icon with the expected app capability within the ini"));               
            if(!IconFileName.Compare(*filename))
            {
            result = EPass;            
            }            
       }
    else
      {
      if(NULL != filename)
          delete filename;
      AppRegInfoQueryAppSubSession.Close();      
      User::Leave(err);
      }     
       
  
    SetTestStepResult(result);   
    if(NULL != filename)
        delete filename;
    
    //closing the resource handles    
    AppRegInfoQueryAppSubSession.Close(); 
    }
    

void CScrGetAppIconEntryStep::ImplTestStepPostambleL()
    {
    CScrTestStep::ImplTestStepPostambleL();    
    }


// -----------CScrGetAppViewIconEntryStep-----------------

CScrGetAppViewIconEntryStep::CScrGetAppViewIconEntryStep(CScrTestServer& aParent) : CScrTestStep(aParent)
    {
    }

void CScrGetAppViewIconEntryStep::ImplTestStepPreambleL()
    {
    CScrTestStep::ImplTestStepPreambleL();    
    }

void CScrGetAppViewIconEntryStep::ImplTestStepL()
    {
    INFO_PRINTF1(_L("Get the App UID and View Uid from the ini for which the sub session is to be opened"));
    TUid  appUid;
    TInt viewUid;
    GetAppUidL(appUid);
    
    if(GetIntFromConfig(ConfigSection(), _L("VwUid"), viewUid) == EFalse)
        {
        PrintErrorL(_L("View Uid was not found!"), KErrNotFound);
        }
    
    INFO_PRINTF1(_L("Get the expected filename of the View Icon for an app from ini"));
    TPtrC appFileName; 
    if(GetStringFromConfig(ConfigSection(), _L("VwIconFileName"), appFileName)== EFalse)
       {
       PrintErrorL(_L("View Icon file Name was not found!"), KErrNotFound);
       }
    
    INFO_PRINTF1(_L("Open the sub session for the UID"));
    RRegistrationInfoForApplication AppRegInfoQueryAppSubSession;
    TRAPD(sessionerr,AppRegInfoQueryAppSubSession.OpenL(iScrSession,appUid));
    if(KErrNone != sessionerr)
          {
          INFO_PRINTF2(_L("Failed to create the subsession to the SCR server (Error:%d)."), sessionerr);
          User::Leave(sessionerr);
          }    
    
    INFO_PRINTF1(_L("Retrieving the view icon file for an app form db."));   
    
    HBufC* filename = NULL;     
    TUid viewId = TUid::Uid(viewUid);
    
    TInt err = KErrNotFound;
    TRAP(err, AppRegInfoQueryAppSubSession.GetAppViewIconL(viewId,filename));
    if(KErrNone == err)
        {
        INFO_PRINTF1(_L("Comparing the retrieved view icon file with the expected view icon file within the ini"));
       
        if(!appFileName.Compare(*filename))
            {
            SetTestStepResult(EPass);           
            }
        else
            {
            SetTestStepResult(EFail);    
            }
        }
    else
        {
        if(NULL != filename)
            delete filename;        
        AppRegInfoQueryAppSubSession.Close();
        User::Leave(err);
        }
    if(NULL != filename)
        delete filename; 
    //closing the resource handles    
    AppRegInfoQueryAppSubSession.Close();
    }
    

void CScrGetAppViewIconEntryStep::ImplTestStepPostambleL()
    {
    CScrTestStep::ImplTestStepPostambleL();    
    }


// -----------CScrGetAppViewsStep-----------------

CScrGetAppViewsStep::CScrGetAppViewsStep(CScrTestServer& aParent) : CScrTestStep(aParent)
    {
    }

void CScrGetAppViewsStep::ImplTestStepPreambleL()
    {
    CScrTestStep::ImplTestStepPreambleL();
    }

void CScrGetAppViewsStep::ImplTestStepL()
    {
    TUid appUid;    
    GetAppUidL(appUid);
    RPointerArray<Usif::CAppViewData> appViewInfoArrayExpected;
    CleanupResetAndDestroyPushL(appViewInfoArrayExpected);
    GetViewDataInfoFromConfigL(appViewInfoArrayExpected);
        
    RPointerArray<Usif::CAppViewData> appViewInfoArrayRetrieved;
    CleanupResetAndDestroyPushL(appViewInfoArrayRetrieved);   
      
    RRegistrationInfoForApplication infoQuerySubSession;
    CleanupClosePushL(infoQuerySubSession); 
            
    infoQuerySubSession.OpenL(iScrSession, appUid);
    
    // Start timer just before invocation of SCR API
    StartTimer();
    infoQuerySubSession.GetAppViewsL(appViewInfoArrayRetrieved);
            
    if(appViewInfoArrayExpected.Count()!=appViewInfoArrayRetrieved.Count())
        {
        ERR_PRINTF1(_L("Returned view data entry count doesnt match the expected count")); 
        SetTestStepResult(EFail);
        }
    else
        {
        for(TInt i=0;i<appViewInfoArrayExpected.Count();i++)
                {
                const CCaptionAndIconInfo *captionAndIconInfoExpected = appViewInfoArrayExpected[i]->CaptionAndIconInfo();
                const CCaptionAndIconInfo *captionAndIconInfoRetrieved = appViewInfoArrayRetrieved[i]->CaptionAndIconInfo();
                if((appViewInfoArrayExpected[i]->Uid()!= appViewInfoArrayRetrieved[i]->Uid())||(appViewInfoArrayExpected[i]->ScreenMode()!= appViewInfoArrayRetrieved[i]->ScreenMode())||(NotEqual(captionAndIconInfoExpected, captionAndIconInfoRetrieved)))  
                    {
                    ERR_PRINTF1(_L("Returned view data entry values don't match with the expected ones."));
                    SetTestStepResult(EFail);
                    }
                else
                    {
                    INFO_PRINTF2(_L("View details associated with view data entry %d"), i);
                    INFO_PRINTF3(_L("ViewData Expected: Uid-%d, ScreenMode-%d"), (appViewInfoArrayExpected[i]->Uid()).iUid, appViewInfoArrayExpected[i]->ScreenMode());
                    INFO_PRINTF3(_L("ViewData Retrieved: Uid-%d, ScreenMode-%d"), (appViewInfoArrayRetrieved[i]->Uid()).iUid, appViewInfoArrayRetrieved[i]->ScreenMode());
                    if(captionAndIconInfoExpected!=NULL)
                        {
                        INFO_PRINTF4(_L("Associated caption details Expected: Caption-%S, No.of Icons-%d, IconFilename-%S"),&(captionAndIconInfoExpected->Caption()), captionAndIconInfoExpected->NumOfAppIcons(), &(captionAndIconInfoExpected->IconFileName()));
                        INFO_PRINTF4(_L("Associated caption details Retrieved: Caption-%S, No.of Icons-%d, IconFilename-%S"), &(captionAndIconInfoRetrieved->Caption()), captionAndIconInfoRetrieved->NumOfAppIcons(), &(captionAndIconInfoRetrieved->IconFileName()));
                        }
                    else
                        {
                        INFO_PRINTF1(_L("No expected caption and icon info. No retrieved caption and icon info."));
                        }
                    }
                }
            }    
            infoQuerySubSession.Close();
            CleanupStack::PopAndDestroy(1, &infoQuerySubSession); 
            CleanupStack::Pop(2,&appViewInfoArrayExpected); 
            appViewInfoArrayRetrieved.ResetAndDestroy();
            appViewInfoArrayExpected.ResetAndDestroy();
            
        }
    
void CScrGetAppViewsStep::ImplTestStepPostambleL()
    {
    CScrTestStep::ImplTestStepPostambleL();
    }


// -----------CScrGetAppServiceInfoStep-----------------

CScrGetAppServiceInfoStep::CScrGetAppServiceInfoStep(CScrTestServer& aParent) : CScrTestStep(aParent)
    {
    }

void CScrGetAppServiceInfoStep::ImplTestStepPreambleL()
    {
    CScrTestStep::ImplTestStepPreambleL();
    }

void CScrGetAppServiceInfoStep::ImplTestStepL()
    {
    RApplicationRegistrationInfo infoQuerySubSession;
    CleanupClosePushL(infoQuerySubSession);
    infoQuerySubSession.OpenL(iScrSession);    
    
    TUid serviceUid, appUid;      
    RPointerArray<Usif::CServiceInfo> appServiceInfoArrayExpected;
    CleanupResetAndDestroyPushL(appServiceInfoArrayExpected);
               
    RPointerArray<Usif::CServiceInfo> appServiceInfoArrayRetrieved;
    CleanupResetAndDestroyPushL(appServiceInfoArrayRetrieved);   

    CAppServiceInfoFilter* appServiceInfoFilter = CAppServiceInfoFilter::NewLC();
    
    TInt filterToSet(0);
    if (!GetIntFromConfig(ConfigSection(), _L("FilterToSet"), filterToSet))
        PrintErrorL(_L("Filter to be set was not found!"), KErrNotFound);
    
    switch(filterToSet)
        {
        case 1:
            {
            
            //Testing GetAppServiceInfo given the appUid
            if (!GetUidFromConfig(ConfigSection(), _L("AppUid"), appUid))
                PrintErrorL(_L("AppUid was not found!"), KErrNotFound);
            INFO_PRINTF2(_L("AppUid is 0x%x"), appUid.iUid);
            
            appServiceInfoFilter->SetAppUidForServiceInfo(appUid);
            
            GetAppServiceInfoL(appServiceInfoArrayExpected);
            
            infoQuerySubSession.GetServiceInfoL(appServiceInfoFilter, appServiceInfoArrayRetrieved);
            break;
            }
        case 2:
            {
            //Testing GetAppServiceInfo given the serviceUid
           
            if(!GetUidFromConfig(ConfigSection(), _L("ServiceUid"), serviceUid))
                PrintErrorL(_L("ServiceUid was not found!"), KErrNotFound);            
            INFO_PRINTF2(_L("ServiceUid is 0x%x"), serviceUid.iUid);
            
            appServiceInfoFilter->SetServiceUidForServiceImplementations(serviceUid);
            
            GetAppServiceInfoL(appServiceInfoArrayExpected);
                                    
            infoQuerySubSession.GetServiceInfoL(appServiceInfoFilter, appServiceInfoArrayRetrieved);
            break;
            }
        case 3:
            {
            //Testing GetAppServiceInfo given the serviceUid and dataType
            TPtrC dataType;
            if (!GetUidFromConfig(ConfigSection(), _L("ServiceUid"), serviceUid))
                PrintErrorL(_L("ServiceUid was not found!"), KErrNotFound);            
            INFO_PRINTF2(_L("ServiceUid is 0x%x"), serviceUid.iUid);
            
            if(!GetStringFromConfig(ConfigSection(), _L("ServiceDataType"), dataType))
                PrintErrorL(_L("DataType was not found!"), KErrNotFound);
            INFO_PRINTF2(_L("DataType is %S"), &dataType);
            
            appServiceInfoFilter->SetServiceUidAndDatatTypeForServiceImplementationsL(serviceUid, dataType);
            
            GetAppServiceInfoL(appServiceInfoArrayExpected);
            
            infoQuerySubSession.GetServiceInfoL(appServiceInfoFilter, appServiceInfoArrayRetrieved);
            break;
            }
        case 4:
            {
            //Testing GetAppServiceInfo given the appUid and serviceUid
            if (!GetUidFromConfig(ConfigSection(), _L("AppUid"), appUid))
                PrintErrorL(_L("AppUid was not found!"), KErrNotFound);
            INFO_PRINTF2(_L("AppUid is 0x%x"), appUid.iUid);
            
            if (!GetUidFromConfig(ConfigSection(), _L("ServiceUid"), serviceUid))
                PrintErrorL(_L("ServiceUid was not found!"), KErrNotFound);            
            INFO_PRINTF2(_L("ServiceUid is 0x%x"), serviceUid.iUid);
            
            appServiceInfoFilter->SetAppUidAndServiceUidForOpaqueData(appUid, serviceUid);
            
            GetAppServiceInfoL(appServiceInfoArrayExpected);
            
            infoQuerySubSession.GetServiceInfoL(appServiceInfoFilter, appServiceInfoArrayRetrieved);
            break;
            }
        default:
            ERR_PRINTF1(_L("Incorrect value for filterToSet, values range from 1-4"));
            SetTestStepResult(EFail);
            break;
        }
    
    //check if they r equal
    if(NotEqualL(appServiceInfoArrayExpected, appServiceInfoArrayRetrieved))
        {
        ERR_PRINTF1(_L("Returned service data entry doesnt match the expected values")); 
        SetTestStepResult(EFail);
        }
    
    //clean up
    CleanupStack::PopAndDestroy(4,&infoQuerySubSession); //appServiceInfoFilter, appServiceInfoArrayRetrieved, appServiceInfoArrayExpected, infoQuerySubSession 
    }
    
void CScrGetAppServiceInfoStep::ImplTestStepPostambleL()
    {
    CScrTestStep::ImplTestStepPostambleL();
    }


// -----------CScrGetComponentIdForApp-----------------

CScrGetComponentIdForApp::CScrGetComponentIdForApp(CScrTestServer& aParent) : CScrTestStep(aParent)
    {
    }

void CScrGetComponentIdForApp::ImplTestStepPreambleL()
    {
    CScrTestStep::ImplTestStepPreambleL();
    }

void CScrGetComponentIdForApp::ImplTestStepL()
    {
    TUid appUid;
    CGlobalComponentId *globalId = GetGlobalComponentIdLC();
    TComponentId expectedCompId, retrievedCompId;
    if (!GetUidFromConfig(ConfigSection(), _L("AppUid"), appUid))
        PrintErrorL(_L("AppUid was not found!"), KErrNotFound);
    
    //Given the GlobalIdName and software type of the component anticipated, fetch the value of the expected ComponentId value.
    expectedCompId = iScrSession.GetComponentIdL(globalId->GlobalIdName(), globalId->SoftwareTypeName());    
    CleanupStack::PopAndDestroy(globalId);

    //Fetch the ComponentId associated to the given App.
    retrievedCompId = iScrSession.GetComponentIdForAppL(appUid);
    
    INFO_PRINTF3(_L("Retrieved ComponentId is %d, and Expected componentId is %d "), retrievedCompId, expectedCompId);
    if(retrievedCompId != expectedCompId)
        {
        ERR_PRINTF1(_L("Retrieved componentId  doesnot match the expected values")); 
        SetTestStepResult(EFail);
        }
    }
    
void CScrGetComponentIdForApp::ImplTestStepPostambleL()
    {
    CScrTestStep::ImplTestStepPostambleL();
    }


// -----------CScrGetAppUidsForComponent-----------------

CScrGetAppUidsForComponent::CScrGetAppUidsForComponent(CScrTestServer& aParent) : CScrTestStep(aParent)
    {
    }

void CScrGetAppUidsForComponent::ImplTestStepPreambleL()
    {
    CScrTestStep::ImplTestStepPreambleL();
    }

void CScrGetAppUidsForComponent::ImplTestStepL()
    {
    TComponentId compId;
    TInt appCount = 0;
    RArray<TUid> expectedAppUid, retrievedAppUid;
    CleanupClosePushL(expectedAppUid);
    CleanupClosePushL(retrievedAppUid);
    TUid appUid;
    TBuf<20> appUidTxt;
    CGlobalComponentId *globalId = GetGlobalComponentIdLC();
    
    //Given the GlobalIdName and software type of the component, fetch the value of the componentId value whose associated apps have to be subsequently fetched.
    compId = iScrSession.GetComponentIdL(globalId->GlobalIdName(), globalId->SoftwareTypeName());
    CleanupStack::PopAndDestroy(globalId);

    if (!GetIntFromConfig(ConfigSection(), _L("AppCount"), appCount))
        PrintErrorL(_L("AppCount was not found!"), KErrNotFound);
  
    for(TInt i=0; i<appCount; i++)
        {
        appUidTxt = KAppUid; 
        GenerateIndexedAttributeNameL(appUidTxt, i);
        if(!GetUidFromConfig(ConfigSection(), appUidTxt, appUid))
            PrintErrorL(_L("%S was not found!"), KErrNotFound, &appUidTxt);
        expectedAppUid.AppendL(appUid);
        }
    
    //Get the apps associated to the given componentId.
    INFO_PRINTF2(_L("Fetching apps asssociated to ComponentId %d"), compId);
    iScrSession.GetAppUidsForComponentL(compId, retrievedAppUid);
    if(expectedAppUid.Count()!= retrievedAppUid.Count())
        {
        ERR_PRINTF1(_L("Count of the retrieved appUids doesnt match the expected count")); 
        SetTestStepResult(EFail);
        }
    retrievedAppUid.SortSigned();
    expectedAppUid.SortSigned();
    
    for(TInt i=0; i<expectedAppUid.Count();i++)
        {
        if(retrievedAppUid[i].iUid != expectedAppUid[i].iUid)
            {
            ERR_PRINTF3(_L("Mismatch in expected and retrieved AppUids! Expected Uid: %d, Retrieved Uid: %d"), expectedAppUid[i].iUid, retrievedAppUid[i].iUid); 
            SetTestStepResult(EFail);
            break;
            }
        }
    
    CleanupStack::PopAndDestroy(2, &expectedAppUid);     //retrievedAppUid, expectedAppUid     
    }
    
void CScrGetAppUidsForComponent::ImplTestStepPostambleL()
    {
    CScrTestStep::ImplTestStepPostambleL();
    }

// -----------CScrAddConcurrentApplicationEntryStep-----------------

CScrAddConcurrentApplicationEntryStep::CScrAddConcurrentApplicationEntryStep(CScrTestServer& aParent) : CScrTestStep(aParent)
    {
    }

void CScrAddConcurrentApplicationEntryStep::ImplTestStepPreambleL()
    {
    CScrTestStep::ImplTestStepPreambleL();
    }

void CScrAddConcurrentApplicationEntryStep::ImplTestStepL()
    {
    TBool isSimultaneousCreateTransaction = EFalse;
    TBool isSimultaneousWriteReadAppInfoSameSession = EFalse;
    TBool isSimultaneousWriteAppInfoDiffSession = EFalse;
    TBool isSimultaneousReadAppInfoDiffSessionWithoutTransaction = EFalse;
        
    GetBoolFromConfig(ConfigSection(), _L("SimultaneousCreateTransaction"), isSimultaneousCreateTransaction);
    GetBoolFromConfig(ConfigSection(), _L("SimultaneousWriteReadAppInfoSameSession"), isSimultaneousWriteReadAppInfoSameSession);
    GetBoolFromConfig(ConfigSection(), _L("SimultaneousWriteAppInfoDiffSession"), isSimultaneousWriteAppInfoDiffSession);
    GetBoolFromConfig(ConfigSection(), _L("SimultaneousReadAppInfoDiffSessionWithoutTransaction"), isSimultaneousReadAppInfoDiffSessionWithoutTransaction);
    
    Usif::RSoftwareComponentRegistry scrSession1, scrSession2;    
    TBool newComponentAdded(EFalse);
    // connect to the SCR
    scrSession1.Connect();
    scrSession2.Connect();
        
    if(isSimultaneousCreateTransaction)
        {         
        // Create two transactions using two diffrent Scr Sessions, transactrion creation for 
        // second scr session will fail( KErrScrWriteOperationInProgress)
        TRAPD(err, scrSession1.CreateTransactionL());
        if(KErrNone != err)
            {
            INFO_PRINTF2(_L("Failed to Create Transaction , error: %d"), err);
            SetTestStepResult(EFail);        
            }
        else
            {
            //it will fail with KErrScrWriteOperationInProgress since two simultaneous transactions are not allowed.
            scrSession2.CreateTransactionL();            
            }
        }
    else if(isSimultaneousWriteReadAppInfoSameSession)
        {        
        //Create a transaction for the scr session , add the application info, then using the same scr session
        //create a subsession to read the app Info, will successfuly read the app info(fron SCR Journal)
        TComponentId compId = 0;        
        CAppInfoFilter* appinfoFilter=NULL ;
        TBool isMultiApp(EFalse);
        
        // Create transaction 
        TRAPD(err, scrSession1.CreateTransactionL());
        if(KErrNone != err)
            {
            INFO_PRINTF2(_L("Failed to Create Transaction , error: %d"), err);
            SetTestStepResult(EFail);        
            }
        
        //WRITE INTO THE DB(Add App Info)
        GetBoolFromConfig(ConfigSection(), _L("IsMultiApp"), isMultiApp);
        if (isMultiApp)
            {
            INFO_PRINTF1(_L("Checking if the component is already present"));
            CGlobalComponentId *globalId = GetGlobalComponentIdLC();
            TRAP_IGNORE(compId = iScrSession.GetComponentIdL(globalId->GlobalIdName(), globalId->SoftwareTypeName()););
            CleanupStack::PopAndDestroy(globalId);
            }
        
        // Try adding the app to a component. The add operation will fail if the component is not present.
        if (!compId) 
            {
            if (GetIntFromConfig(ConfigSection(), _L("UseComponentId"), compId))
                INFO_PRINTF2(_L("Adding a new AppRegInfo to component %d"), compId);
            }
        
        // The component is not present already
        if (!compId)   
            {
            INFO_PRINTF1(_L("Adding a new component."));
            compId = AddNonLocalisableComponentL(iScrSession);
            newComponentAdded = ETrue;
            }
        
        INFO_PRINTF1(_L("Get reg info from config file."));
        const CApplicationRegistrationData* appRegData = GetAppRegInfoFromConfigLC();
        TRAP(err, scrSession1.AddApplicationEntryL(compId, *appRegData));  
        if(KErrNone != err)
           {
           INFO_PRINTF2(_L("Failed to Create Transaction , error: %d"), err);
           SetTestStepResult(EFail); 
           if (newComponentAdded)
               {
               // We are going to delete the component with no apps
               scrSession1.DeleteComponentL(compId);
               }
           }

        INFO_PRINTF1(_L("Read appinfo filter from configuration"));
        TRAP(err, ReadAppInfoFilterFromConfigL(&appinfoFilter));
        if (KErrNotFound==err)
            {
            delete appinfoFilter;
            appinfoFilter=NULL;
            }
        
        //READ FROM th DB the AppRegInfo
        RApplicationInfoView  subSession;
        CleanupClosePushL(subSession);
        subSession.OpenViewL(scrSession1,appinfoFilter);
        if(appinfoFilter)
                delete appinfoFilter;
        
        RPointerArray<Usif::TAppRegInfo> appRegInfoSet;               
        subSession.GetNextAppInfoL(5, appRegInfoSet);                      
        
        //No Need To Commit The Transaction as we only need to check whether we can read while the transaction(Write) is Alive.
        
        appRegInfoSet.ResetAndDestroy();
        CleanupStack::PopAndDestroy(&subSession);        
        CleanupStack::Pop();
        delete appRegData;
        }
    
    else if(isSimultaneousWriteAppInfoDiffSession)
        {
        //Using two scr sessions ,create a transaction for only the first scr session(because creation of transaction for the second will result into an error)
        //, add the application info using both the scr sessions ,the second addition will result into an error( KErrScrWriteOperationInProgress) 
        TComponentId compId = 0;             
        TBool isMultiApp(EFalse);
        
        // Create transaction 
        TRAPD(err, scrSession1.CreateTransactionL());
        if(KErrNone != err)
            {
            INFO_PRINTF2(_L("Failed to Create Transaction , error: %d"), err);
            SetTestStepResult(EFail);        
            }
        
        //WRITE INTO THE DB(Add App Info)
        GetBoolFromConfig(ConfigSection(), _L("IsMultiApp"), isMultiApp);
        if (isMultiApp)
            {
            INFO_PRINTF1(_L("Checking if the component is already present"));
            CGlobalComponentId *globalId = GetGlobalComponentIdLC();
            TRAP_IGNORE(compId = iScrSession.GetComponentIdL(globalId->GlobalIdName(), globalId->SoftwareTypeName()););
            CleanupStack::PopAndDestroy(globalId);
            }
        
        // Try adding the app to a component. The add operation will fail if the component is not present.
        if (!compId) 
            {
            if (GetIntFromConfig(ConfigSection(), _L("UseComponentId"), compId))
                INFO_PRINTF2(_L("Adding a new AppRegInfo to component %d"), compId);
            }
        
        // The component is not present already
        if (!compId)   
            {
            INFO_PRINTF1(_L("Adding a new component."));
            compId = AddNonLocalisableComponentL(iScrSession);
            newComponentAdded = ETrue;
            }
        
        INFO_PRINTF1(_L("Get reg info from config file."));
        const CApplicationRegistrationData* appRegData = GetAppRegInfoFromConfigLC();
        TRAP(err, scrSession1.AddApplicationEntryL(compId, *appRegData));  
        if(KErrNone != err)
            {
            INFO_PRINTF2(_L("Failed to Create Transaction , error: %d"), err);
            SetTestStepResult(EFail); 
            if (newComponentAdded)
               {
               // We are going to delete the component with no apps
               scrSession1.DeleteComponentL(compId);
               }
            }
        
        //Trying to write(app app info) to the db while scrSession1 is alredy in process of writing. 
        //It will fail with KErrScrWriteOperationInProgress since a transaction is alredy alive.
        scrSession2.AddApplicationEntryL(compId, *appRegData);
        CleanupStack::Pop();
        delete appRegData;        
        }
    
    else if(isSimultaneousReadAppInfoDiffSessionWithoutTransaction)
        {
        //Two subsessions without a transaction simultaneously reading the db. 
        CAppInfoFilter* appinfoFilter=NULL ;                   
       
        INFO_PRINTF1(_L("Read appinfo filter from configuration"));
        TRAPD(err, ReadAppInfoFilterFromConfigL(&appinfoFilter));
        if (KErrNotFound==err)
           {
           delete appinfoFilter;
           appinfoFilter=NULL;
           }
        CleanupStack::PushL(appinfoFilter);
        
     
        //READ the AppRegInfo for (scrSession1/subSession) and (scrSession2/subSession2)
        RApplicationInfoView  subSession, subSession2;
        CleanupClosePushL(subSession);
        CleanupClosePushL(subSession2);
        
        subSession.OpenViewL(scrSession1,appinfoFilter);
        subSession2.OpenViewL(scrSession2,appinfoFilter);        
                
        
        RPointerArray<Usif::TAppRegInfo> appRegInfoSet, appRegInfoSet2;
               
        subSession.GetNextAppInfoL(5, appRegInfoSet);        
        subSession2.GetNextAppInfoL(5, appRegInfoSet2);
             
        appRegInfoSet.ResetAndDestroy();
        appRegInfoSet2.ResetAndDestroy();
        
        CleanupStack::PopAndDestroy(2, &subSession); 
        CleanupStack::PopAndDestroy(appinfoFilter);
        }    

    //Close the sessions
    scrSession1.Close();
    scrSession2.Close();    
    }
    
void CScrAddConcurrentApplicationEntryStep::ImplTestStepPostambleL()
    {
    CScrTestStep::ImplTestStepPostambleL();
    }

// -----------CScrGetAppInfoStep-----------------

CScrGetAppInfoStep::CScrGetAppInfoStep(CScrTestServer& aParent) : CScrTestStep(aParent)
    {
    }

void CScrGetAppInfoStep::ImplTestStepPreambleL()
    {
    CScrTestStep::ImplTestStepPreambleL();
    }

void CScrGetAppInfoStep::ImplTestStepL()
    {
    TLanguage locale;
    TBool result;
    TAppRegInfo retrievedAppRegEntry;
    TUid uid;
    TPtrC fullName, caption, shortCaption;

    _LIT(KAppInfoUid, "AppUid");
    if (!GetUidFromConfig(ConfigSection(), KAppInfoUid, uid))
        {
         ERR_PRINTF2(_L("The application Uid type param %S could not be found in configuration."), &KAppInfoUid);
         User::Leave(KErrNotFound);
        }
        
    _LIT(KAppInfoApplicationFilename, "AppFile");
    if(!GetStringFromConfig(ConfigSection(), KAppInfoApplicationFilename, fullName))
       {
       fullName.Set(KNull);
       }
 
    _LIT(KAppInfoCaption, "Caption");
    if(!GetStringFromConfig(ConfigSection(), KAppInfoCaption, caption))
       {
       caption.Set(KNull);
       }
 
    _LIT(KAppInfoShortCaption, "ShortCaption");
    if(!GetStringFromConfig(ConfigSection(), KAppInfoShortCaption, shortCaption))
       {
       shortCaption.Set(KNull);
       }

    RApplicationRegistrationInfo infoQuerySubSession;
    CleanupClosePushL(infoQuerySubSession);
    infoQuerySubSession.OpenL(iScrSession);    

    _LIT(KLocaleParam, "Locale");
    if(GetLocaleFromConfigL(KLocaleParam,locale))
        {
        result = infoQuerySubSession.GetAppInfoL(uid, retrievedAppRegEntry, locale);
        }
    else
        {
        result = infoQuerySubSession.GetAppInfoL(uid, retrievedAppRegEntry);
        }
    
    if(result)
        {
        INFO_PRINTF3(_L("Expected Uid:0x%x Retrieved Uid:0x%x"), uid, retrievedAppRegEntry.Uid()); 
        INFO_PRINTF3(_L("Expected filename:%S Retrieved Filename: %s"), &fullName, retrievedAppRegEntry.FullName().PtrZ());
        INFO_PRINTF3(_L("Expected caption:%S Retrieved caption: %s"), &caption, retrievedAppRegEntry.Caption().PtrZ());
        INFO_PRINTF3(_L("Expected short caption:%S Retrieved caption: %s"), &shortCaption, retrievedAppRegEntry.ShortCaption().PtrZ());
        }
    else
        {
        INFO_PRINTF1(_L("The given application Uid is absent in the database.")); 
        }
    
    if(fullName!=retrievedAppRegEntry.FullName() ||caption!=retrievedAppRegEntry.Caption()|| shortCaption!=retrievedAppRegEntry.ShortCaption())
        {
        ERR_PRINTF1(_L("Mismatch in expected and retrieved values! ")); 
        SetTestStepResult(EFail);
        }
    else
        {
        INFO_PRINTF1(_L("Expected and retrieved results match."));
        }

    CleanupStack::PopAndDestroy(&infoQuerySubSession);    
    }
 
void CScrGetAppInfoStep::ImplTestStepPostambleL()
    {
    CScrTestStep::ImplTestStepPostambleL();
    }
