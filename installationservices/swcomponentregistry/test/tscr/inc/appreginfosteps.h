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
* Defines test steps for component management APIs in the SCR
*
*/


/**
 @file 
 @internalComponent
 @test
*/

#ifndef APPREGINFOSTEPS_H
#define APPREGINFOSTEPS_H

#include "tscrstep.h"
#include <usif/scr/appregentries.h>
class CScrTestServer;

_LIT(KScrAddApplicationEntry, "SCRAddApplicationEntry");
_LIT(KScrDeleteApplicationEntry, "SCRDeleteApplicationEntry");
_LIT(KSCRGetAppServicesUids,"SCRGetAppServicesUids");
_LIT(KSCRGetAppForDataType,"SCRGetAppForDataType");
_LIT(KSCRGetAppForDataTypeAndService,"SCRGetAppForDataTypeAndService");
_LIT(KSCRGetDefaultScreenNumber,"SCRGetDefaultScreenNumber");
_LIT(KSCRGetNumberOfDefinedIcons,"SCRGetNumberOfDefinedIcons");
_LIT(KSCRGetApplicationLanguage,"SCRGetApplicationLanguage");
_LIT(KScrGetAppOwnedFilesEntry, "SCRGetAppOwnedFilesEntry");
_LIT(KScrGetAppCapabilityEntry, "SCRGetAppCapabilityEntry");
_LIT(KScrGetAppIconEntry, "SCRGetAppIconEntry");
_LIT(KScrGetAppViewIconEntry, "SCRGetAppViewIconEntry");
_LIT(KScrGetAppViewInfoStep,"ScrGetAppViewInfo");
_LIT(KScrGetAppServiceInfoStep,"ScrGetAppServiceInfo");
_LIT(KScrGetComponentIdForAppStep,"ScrGetComponentIdForApp");
_LIT(KScrGetAppUidsForComponentStep,"ScrGetAppUidsForComponent");
_LIT(KScrAddConcurrentApplicationEntryStep,"ScrAddConcurrentApplicationEntry");
_LIT(KScrGetAppInfoStep,"ScrGetAppInfo");

class CScrAddApplicationEntryStep : public CScrTestStep
/**
    TEF test step which exercises the SCR AddApplicationEntryL interface
 */
    {
public:
    CScrAddApplicationEntryStep(CScrTestServer& aParent);
    
protected:
    // From CScrTestStep
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();
    };

class CScrDeleteApplicationEntriesStep : public CScrTestStep
/**
    TEF test step which exercises the SCR DeleteApplicationEntriesL interface
 */
    {
public:
    CScrDeleteApplicationEntriesStep(CScrTestServer& aParent);
    
protected:
    // From CScrTestStep
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();
    };

class CSCRGetAppServicesUids : public CScrTestStep
/**
 * TEF test step which exercises the RRegistrationInfoForApplication::GetAppServicesUidsL interface
 */
    {
public:
    CSCRGetAppServicesUids(CScrTestServer& aParent);
            
protected:
    // From CScrTestStep
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();
    };  

class CSCRGetAppForDataType : public CScrTestStep
/**
 * TEF test step which exercises the RApplicationRegistrationInfo::GetAppForDataTypeL interface
 */
    {
public:
    CSCRGetAppForDataType(CScrTestServer& aParent);
           
protected:
    // From CScrTestStep
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();
    };  

class CSCRGetAppForDataTypeAndService : public CScrTestStep
/**
 * TEF test step which exercises the TEF test step which exercises the RApplicationRegistrationInfo::GetAppForDataTypeL interface
 */
    {
public:
    CSCRGetAppForDataTypeAndService(CScrTestServer& aParent);
           
protected:
    // From CScrTestStep
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();
    
private:
    };  

class CSCRGetDefaultScreenNumber : public CScrTestStep
/**
 * TEF test step which exercises the TEF test step which exercises the RRegistrationInfoForApplication::GetDefaultScreenNumberL interface
 */
    {
public:
    CSCRGetDefaultScreenNumber(CScrTestServer& aParent);
           
protected:
    // From CScrTestStep
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();
    };  

class CSCRGetNumberOfDefinedIcons : public CScrTestStep
/**
 * TEF test step which exercises the TEF test step which exercises the RRegistrationInfoForApplication::GetNumberOfOwnDefinedIconsL interface
 */
	{
public:
    CSCRGetNumberOfDefinedIcons(CScrTestServer& aParent);
			
protected:
	// From CScrTestStep
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();
	};	

class CSCRGetApplicationLanguage : public CScrTestStep
/**
 * TEF test step which exercises the TEF test step which exercises the RRegistrationInfoForApplication::GetApplicationLanguageL interface
 */
    {
public:
    CSCRGetApplicationLanguage(CScrTestServer& aParent);
            
protected:
    // From CScrTestStep
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();
    };  

class CScrGetAppOwnedFilesEntryStep : public CScrTestStep
/**
    TEF test step which exercises the RRegistrationInfoForApplication::GetAppOwnedFiles interface
 */
    {
public:
    CScrGetAppOwnedFilesEntryStep(CScrTestServer& aParent);
    
protected:
    // From CScrTestStep
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();

private:
    TBool CompareAppOwnedFileNamesL(RPointerArray<HBufC>& aExpetedValue,RPointerArray<HBufC>& aActualValue);
    };

class CScrGetAppCharacteristicsEntryStep : public CScrTestStep
/**
    TEF test step which exercises the RRegistrationInfoForApplication::GetAppCharacteristics interface
 */
    {
public:
    CScrGetAppCharacteristicsEntryStep(CScrTestServer& aParent);
    
protected:
    // From CScrTestStep
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();
    };

class CScrGetAppIconEntryStep : public CScrTestStep
/**
    TEF test step which exercises the RRegistrationInfoForApplication::GetAppIcon interface
 */
    {
public:
    CScrGetAppIconEntryStep(CScrTestServer& aParent);
    
protected:
    // From CScrTestStep
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();
   };

class CScrGetAppViewIconEntryStep : public CScrTestStep
/**
    TEF test step which exercises the RRegistrationInfoForApplication::GetAppViewIcon interface
 */
    {
public:
    CScrGetAppViewIconEntryStep(CScrTestServer& aParent);
  
protected:
    // From CScrTestStep
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();
   };

class CScrGetAppViewsStep : public CScrTestStep
/**
    TEF test step which exercises the RRegistrationInfoForApplication::GetAppViewsL interface
 */
    {
public:
    CScrGetAppViewsStep(CScrTestServer& aParent);
    
protected:
    // From CScrTestStep
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();
    };

class CScrGetAppServiceInfoStep : public CScrTestStep
/**
    TEF test step which exercises the RApplicationRegistrationInfo::GetServiceInfoL interface
 */
    {
public:
    CScrGetAppServiceInfoStep(CScrTestServer& aParent);
    
protected:
    // From CScrTestStep
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();
    };

class CScrGetComponentIdForApp : public CScrTestStep
/**
    TEF test step which exercises the SCR GetComponentIdForAppL interface
 */
    {
public:
    CScrGetComponentIdForApp(CScrTestServer& aParent);
    
protected:
    // From CScrTestStep
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();
    };

class CScrGetAppUidsForComponent : public CScrTestStep
/**
    TEF test step which exercises the SCR ScrGetAppUidsForComponentL interface
 */
    {
public:
    CScrGetAppUidsForComponent(CScrTestServer& aParent);
    
protected:
    // From CScrTestStep
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();
    };
class CScrAddConcurrentApplicationEntryStep : public CScrTestStep
/**
    TEF test step which exercises the SCR AddConcurrentApplicationEntryStep interface
 */
    {
public:
    CScrAddConcurrentApplicationEntryStep(CScrTestServer& aParent);
    
protected:
    // From CScrTestStep
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();
    };

class CScrGetAppInfoStep : public CScrTestStep
/**
    TEF test step which exercises the RApplicationRegistrationInfo::GetAppInfoL interface
 */
    {
public:
    CScrGetAppInfoStep(CScrTestServer& aParent);
    
protected:
    // From CScrTestStep
    void ImplTestStepPreambleL();
    void ImplTestStepL();
    void ImplTestStepPostambleL();
    };


#endif /* APPREGINFOSTEPS_H */
