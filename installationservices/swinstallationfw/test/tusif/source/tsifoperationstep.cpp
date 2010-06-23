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
*
*/


/**
 @file
 @internalTechnology 
*/

#include "tsifoperationstep.h"
#include "tsifsuitedefs.h"
#include <ct/rcpointerarray.h>

using namespace Usif;

CSifOperationStep::~CSifOperationStep()
/**
* Destructor
*/
	{
	}

CSifOperationStep::CSifOperationStep() : iExclusiveOperation(ETrue)
/**
* Constructor
*/
	{
	}

void CSifOperationStep::ImplTestStepPreambleL()
/**
* @return - TVerdict code
* Override of base class virtual
*/
	{
	CSifSuiteStepBase::ImplTestStepPreambleL();
	INFO_PRINTF1(_L("I am in CSifOperationStep::ImplTestStepPreambleL()."));
	if (TestStepResult()!=EPass)
		{
		return;
		}

	if (!GetIntFromConfig(ConfigSection(),KTe_CancelAfter, iCancelAfter))
		{
		iCancelAfter = -1;
		}

	TInt err = iSif.Connect();
	if (err != KErrNone)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF2(_L("Failed to connect to the SIF server, error code: %d"), err);
		}
	else
		{
		SetTestStepResult(EPass);
		}

	LoadExclusiveOperationFlagFromConfigL();
	}

void CSifOperationStep::ImplTestStepPostambleL()
/**
* @return - TVerdict code
* Override of base class virtual
*/
	{	
	INFO_PRINTF1(_L("Cleanup in CSifOperationStep::~CSifOperationStep()"));
	
	iSif.Close();
	
	delete iComponentInfo;
	iComponentInfo = NULL;
	ClearOpaqueParams();
	}
	
void CSifOperationStep::ClearOpaqueParams()
	{
	delete iPluginOpaqueArguments;
	iPluginOpaqueArguments = NULL;
	delete iPluginOpaqueResults;
	iPluginOpaqueResults = NULL;
	delete iPluginRefOpaqueResults;
	iPluginRefOpaqueResults = NULL;	
	}	

void CSifOperationStep::CancelableWait()
	{
	if (iCancelAfter >= 0)
		{
		User::After(iCancelAfter);
		iSif.CancelOperation();
		}
	else
		{
		User::WaitForRequest(iStatus);
		}
	}


void CSifOperationStep::LoadFileNameFromConfigL()
	{
	if(!GetStringFromConfig(ConfigSection(),KTe_PackageFile, iFileName))
		{
		INFO_PRINTF2(_L("Parameter %S not found in the ini file"), &KTe_PackageFile);
		User::Leave(KErrNotFound);
		}
	}

void CSifOperationStep::LoadComponentIdFromConfigL()
	{
	TPtrC componentName, componentVendor;
	if (GetStringFromConfig(ConfigSection(), KTe_ComponentName, componentName) &&
		GetStringFromConfig(ConfigSection(), KTe_ComponentVendor, componentVendor))
		{
		iComponentId = FindComponentInScrL(componentName, componentVendor);
		if (iComponentId != 0)
			{
			return;
			}
		INFO_PRINTF3(_L("Component name = %S, vendor = %S not found in SCR"), &componentName, &componentVendor);
		User::Leave(KErrNotFound);
		}


	if(!GetIntFromConfig(ConfigSection(), KTe_ComponentId, iComponentId))
		{
		User::Leave(KErrNotFound);
		}
	}

void CSifOperationStep::LoadComponentInfoFromConfigL()
	{
	}

void CSifOperationStep::LoadExclusiveOperationFlagFromConfigL()
	{
	GetBoolFromConfig(ConfigSection(),KTe_ExclusiveOperation, iExclusiveOperation);
	}

void CSifOperationStep::LoadPluginOpaqueParamsFromConfigL(const TDesC& aNamePattern, const TDesC& aTypePattern, const TDesC& aValuePattern, COpaqueNamedParams& aOpaqueParams)
	{
	// name pattern
	HBufC* namePattern = HBufC::NewLC(aNamePattern.Length()+KMaxIntDigits);
	TPtr bufNamePattern(namePattern->Des());
	// value pattern
	HBufC* valuePattern = HBufC::NewLC(aValuePattern.Length()+KMaxIntDigits);
	TPtr bufValuePattern(valuePattern->Des());
	// value type pattern
	HBufC* typePattern = HBufC::NewLC(aTypePattern.Length()+KMaxIntDigits);
	TPtr bufTypePattern(typePattern->Des());

	const TInt maxNumParams = 16;
	for (TInt i=0; i<maxNumParams; ++i)
		{
		// param name
		bufNamePattern.Copy(aNamePattern);
		bufNamePattern.AppendNum(i);
		TPtrC name;
		if (!GetStringFromConfig(ConfigSection(), *namePattern, name))
			{
			break;
			}
		
		//param type
		bufTypePattern.Copy(aTypePattern);
		bufTypePattern.AppendNum(i);
		TPtrC type;
		if (!GetStringFromConfig(ConfigSection(), *typePattern, type))
			{
			INFO_PRINTF2(_L("Expected opaque param type: %S not found "), typePattern);
			User::Leave(KErrNotFound);
			}

		//param value
		bufValuePattern.Copy(aValuePattern);
		bufValuePattern.AppendNum(i);
		if (type.Compare(KTe_PluginOpaqueValueTypeString) == 0)
			{
			TPtrC value;
			if (GetStringFromConfig(ConfigSection(), *valuePattern, value))
				{
				aOpaqueParams.AddStringL(name, value);
				}
			else
				{
				INFO_PRINTF2(_L("Expected opaque param value: %S not found "), valuePattern);
				User::Leave(KErrNotFound);
				}
			}
		else if (type.Compare(KTe_PluginOpaqueValueTypeInt) == 0)
			{
			TInt value = 0;
			if(GetIntFromConfig(ConfigSection(), *valuePattern, value))
				{
				aOpaqueParams.AddIntL(name, value);
				}
			else
				{
				INFO_PRINTF2(_L("Expected opaque param value: %S not found "), valuePattern);
				User::Leave(KErrNotFound);
				}
			}
		else
			{
			INFO_PRINTF2(_L("Unknow type of opaque param value: %S "), &type);
			User::Leave(KErrUnknown);
			}
		
		}
	CleanupStack::PopAndDestroy(3, namePattern);
	}

void ClearOpaqueParamsCleanup(TAny* aParam)
	{
	static_cast<CSifOperationStep*>(aParam)->ClearOpaqueParams();
	}	
	
void CSifOperationStep::LoadOpaqueParamsL()
	{
	GetBoolFromConfig(ConfigSection(),KTe_UseEnhancedApi, iUseEnhancedApi);

	CleanupStack::PushL(TCleanupItem(ClearOpaqueParamsCleanup, this));
	if (iUseEnhancedApi)
		{
		delete iPluginOpaqueArguments;
		iPluginOpaqueArguments = NULL;
		iPluginOpaqueArguments = COpaqueNamedParams::NewL();
		delete iPluginOpaqueResults;
		iPluginOpaqueResults = NULL;
		iPluginOpaqueResults = COpaqueNamedParams::NewL();
		delete iPluginRefOpaqueResults;
		iPluginRefOpaqueResults = NULL;
		iPluginRefOpaqueResults = COpaqueNamedParams::NewL();

		LoadPluginOpaqueParamsFromConfigL(KTe_PluginOpaqueArgumentName, KTe_PluginOpaqueArgumentType, KTe_PluginOpaqueArgumentValue, *iPluginOpaqueArguments);
		LoadPluginOpaqueParamsFromConfigL(KTe_PluginRefOpaqueResultName, KTe_PluginRefOpaqueResultType, KTe_PluginRefOpaqueResultValue, *iPluginRefOpaqueResults);
		}
	CleanupStack::Pop(); // ClearOpaqueParams
	}

TBool CSifOperationStep::CompareOpaqueResultsL()
	{
	if (iPluginOpaqueResults == NULL && iPluginRefOpaqueResults == NULL)
		{
		return ETrue;
		}
	else if (iPluginOpaqueResults == NULL || iPluginRefOpaqueResults == NULL)
		{
		return EFalse;
		}
	
	const TInt llen = iPluginOpaqueResults->CountL();
	const TInt rlen = iPluginRefOpaqueResults->CountL();
	if (llen != rlen)
		{
		INFO_PRINTF3(_L("The number of expected and returned opaque params differs. Expected: %d, returned: %d "), rlen, llen);
		return EFalse;
		}

	RCPointerArray<HBufC> names;
	CleanupClosePushL(names);
	iPluginOpaqueResults->GetNamesL(names);

	for (TInt i=0; i<llen; ++i)
		{
		const TDesC& lName = *names[i];
		const TDesC& lValue = iPluginOpaqueResults->StringByNameL(lName);
		const TDesC& rValue = iPluginRefOpaqueResults->StringByNameL(lName);
		
		if (rValue == KTe_AnyValuePermitted && rValue != KNullDesC)
			{
			continue;
			}

		if (lValue.Size() != rValue.Size())
			{
			INFO_PRINTF4(_L("Mismatch on param sizes for param %S. Expected size %d, returned size %d"), &lName, rValue.Size(), lValue.Size());
			CleanupStack::PopAndDestroy(&names);
			return EFalse;
			}
		if (lValue.Size() != sizeof(TInt) && lValue != rValue)
			{
			INFO_PRINTF4(_L("Mismatch for string param %S. Expected %S, returned %S"), &lName, &rValue, &lValue);
			CleanupStack::PopAndDestroy(&names);
			return EFalse;
			}
		if (lValue.Size() == sizeof(TInt))
			{
			TInt lIntValue = iPluginOpaqueResults->IntByNameL(lName);
			TInt rIntValue = iPluginRefOpaqueResults->IntByNameL(lName);
			if (lIntValue != rIntValue)
				{
				INFO_PRINTF4(_L("Mismatch for integer param %S. Expected %d, returned %d"), &lName, rIntValue, lIntValue);
				CleanupStack::PopAndDestroy(&names);
				return EFalse;
				}
			}
		}

	CleanupStack::PopAndDestroy(&names);
	return ETrue;
	}

void CSifOperationStep::PrintOpaqueParamsL(const COpaqueNamedParams& aOpaqueParams)
	{
	RCPointerArray<HBufC> names;
	CleanupClosePushL(names);
	aOpaqueParams.GetNamesL(names);
	for (TInt i=0; i<names.Count(); ++i)
		{
		const TDesC& name = *names[i];
		const TDesC& value = aOpaqueParams.StringByNameL(name);
		if (value.Size() == sizeof(TInt))
			{
			const TInt intValue = aOpaqueParams.IntByNameL(name);
			INFO_PRINTF4(_L("Param: %d, Name: %S, IntValue: %d"), i, &name, intValue);
			}
		else
			{
			INFO_PRINTF4(_L("Param: %d, Name: %S, Value: %S"), i, &name, &value);
			}
		}
	CleanupStack::PopAndDestroy(&names);
	}

TBool CSifOperationStep::CheckOpaqueResultsL()
	{
	if (CompareOpaqueResultsL())
		{
		return ETrue;
		}

	INFO_PRINTF1(_L("==================== Begin of opaque results ===================="));

	if (iPluginOpaqueResults)
		{
		INFO_PRINTF1(_L("Expected opaque results:"));
		PrintOpaqueParamsL(*iPluginOpaqueResults);
		}

	INFO_PRINTF1(_L("-----------------------------------------------------------------"));

	if (iPluginRefOpaqueResults)
		{
		INFO_PRINTF1(_L("Returned opaque results:"));
		PrintOpaqueParamsL(*iPluginRefOpaqueResults);
		}

	INFO_PRINTF1(_L("===================== End of opaque results ====================="));

	return EFalse;
	}


// *****************************************************************************************************


CSifGetComponentInfoStep::~CSifGetComponentInfoStep()
/**
* Destructor
*/
	{
	delete iComponentInfo;
	}

CSifGetComponentInfoStep::CSifGetComponentInfoStep() : iCompareMaxInstalledSize(ETrue)
/**
* Constructor
*/
	{
	SetTestStepName(KSifGetComponentInfoStep);
	iconIndex = 0;
	}

namespace
	{
	TInt PackCapabilitySet(const TCapabilitySet& aCapSet)
		{
		TInt caps=0;
		for (TInt c=0; c<ECapability_Limit; ++c)
			{
			if (aCapSet.HasCapability(TCapability(c)))
				{
				caps+=(1<<c);
				}
			}
		return caps;
		}

	void UnpackCapabilitySet(TInt aPackedCapSet, TCapabilitySet& aCapSet)
		{
		for (TInt c=0; c<ECapability_Limit; ++c)
			{
			const TInt cap = 1<<c;
			if (aPackedCapSet&cap)
				{
				aCapSet.AddCapability(TCapability(c));
				}
			}
		}
	}

void CSifGetComponentInfoStep::LoadComponentInfoL()
	{
	ASSERT(iComponentInfo == NULL);

	// Load the tree of CComponentInfo nodes
	CComponentInfo::CNode* rootNode = LoadCompInfoNodeLC(KTe_CompInfoRootNodePrefix);
	
	// Create an instance of CComponentInfo
	iComponentInfo = CComponentInfo::NewL();
	iComponentInfo->SetRootNodeL(rootNode);
	CleanupStack::Pop(rootNode);
	}

namespace
	{
	HBufC* CompInfoParamNameLC(const TDesC& aStr1, const TDesC& aStr2)
		{
		HBufC* conc = HBufC::NewLC(aStr1.Length()+aStr2.Length()+1);
		TPtr bufConc(conc->Des());
		bufConc.Copy(aStr1);
		bufConc.Append(_L("."));
		bufConc.Append(aStr2);
		return conc;
		}
	}

TPtrC CSifGetComponentInfoStep::LoadCompInfoNodeStringParamL(const TDesC& aNodePrefix, const TDesC& aParamName)
	{
	HBufC* name = CompInfoParamNameLC(aNodePrefix, aParamName);

	TPtrC value;
	if (!GetStringFromConfig(ConfigSection(), *name, value))
		{
		INFO_PRINTF2(_L("ComponentInfo param: %S not found in the ini file"), name);
		User::Leave(KErrNotFound);
		}

	CleanupStack::PopAndDestroy(name);
	return value;
	}

TInt CSifGetComponentInfoStep::LoadCompInfoNodeIntParamL(const TDesC& aNodePrefix, const TDesC& aParamName, TBool aMandatory)
	{
	HBufC* name = CompInfoParamNameLC(aNodePrefix, aParamName);

	TInt value(0);
	if (!GetIntFromConfig(ConfigSection(), *name, value) && aMandatory)
		{
		INFO_PRINTF2(_L("ComponentInfo param: %S not found in the ini file"), name);
		User::Leave(KErrNotFound);
		}

	CleanupStack::PopAndDestroy(name);
	return value;
	}

TBool CSifGetComponentInfoStep::LoadCompInfoNodeBoolParamL(const TDesC& aNodePrefix, const TDesC& aParamName, TBool aMandatory)
	{
	HBufC* name = CompInfoParamNameLC(aNodePrefix, aParamName);

	TBool value(EFalse);
	if (!GetBoolFromConfig(ConfigSection(), *name, value) && aMandatory)
		{
		INFO_PRINTF2(_L("ComponentInfo param: %S not found in the ini file"), name);
		User::Leave(KErrNotFound);
		}

	CleanupStack::PopAndDestroy(name);
	return value;
	}

CComponentInfo::CNode* CSifGetComponentInfoStep::LoadCompInfoNodeLC(const TDesC& aNodeName)
	{
	TPtrC swType(LoadCompInfoNodeStringParamL(aNodeName, KTe_SoftwareTypeName));
	TPtrC name(LoadCompInfoNodeStringParamL(aNodeName, KTe_ComponentName));
	TPtrC vendor(LoadCompInfoNodeStringParamL(aNodeName, KTe_ComponentVendor));
	TPtrC version(LoadCompInfoNodeStringParamL(aNodeName, KTe_ComponentVersion));
	TPtrC globalId(LoadCompInfoNodeStringParamL(aNodeName, KTe_GlobalComponentId));

	TInt scomoState(LoadCompInfoNodeIntParamL(aNodeName, KTe_ScomoState));
	TInt installStatus(LoadCompInfoNodeIntParamL(aNodeName, KTe_InstallStatus));
	TInt componentId(LoadCompInfoNodeIntParamL(aNodeName, KTe_ComponentId));
	TInt authenticity(LoadCompInfoNodeIntParamL(aNodeName, KTe_Authenticity));
	TInt size(LoadCompInfoNodeIntParamL(aNodeName, KTe_MaxInstalledSize));
	TBool hasexe(LoadCompInfoNodeBoolParamL(aNodeName, KTe_HasExecutable));
	TBool driveSelectionRequired(LoadCompInfoNodeBoolParamL(aNodeName, KTe_DriveSelectionRequired));
	TInt noOfApps(LoadCompInfoNodeIntParamL(aNodeName,KTe_NumberOfApplications));
	
	TBuf<20> appUidTxt;
	TBuf<20> appFileNameTxt;
	TBuf<20> appGroupNameTxt;
	TBuf<20> appIconFileNameTxt;
	TBuf<20> appIconFileSize;
	const TInt MAX_INT_STR_LEN = 8;
    TBuf<MAX_INT_STR_LEN> integerAppendStr;
    RPointerArray<Usif::CComponentInfo::CApplicationInfo> applications;
	
	for(TInt index = 0; index < noOfApps ; index++)
	   {
	   integerAppendStr.Format(_L("%d"), index);
	   appUidTxt = KTe_ApplicationUid; 
	   appUidTxt.Append(integerAppendStr);
	   TInt val = 0;
	   HBufC* name = CompInfoParamNameLC(aNodeName, appUidTxt);	   
	   GetHexFromConfig(ConfigSection(),*name, val);
	   TUid appuid = TUid::Uid(val);
	   CleanupStack::PopAndDestroy(name);
	   
	   appFileNameTxt=KTe_ApplicationName;
	   appFileNameTxt.Append(integerAppendStr);
	   TPtrC appFileName(LoadCompInfoNodeStringParamL(aNodeName,appFileNameTxt));
	   
	   appGroupNameTxt=KTe_ApplicationGroupName;
	   appGroupNameTxt.Append(integerAppendStr);
	   TPtrC appGroupName(LoadCompInfoNodeStringParamL(aNodeName,appGroupNameTxt));
	   
	   appIconFileNameTxt=KTe_ApplicationIconFileName;
	   appIconFileNameTxt.Append(integerAppendStr);
	   TPtrC appIconFileName(LoadCompInfoNodeStringParamL(aNodeName,appIconFileNameTxt));
	   	   
	   Usif::CComponentInfo::CApplicationInfo* app = NULL;
	   app = Usif::CComponentInfo::CApplicationInfo::NewLC(appuid, appFileName, appGroupName, appIconFileName);
	   
	   appIconFileSize = KTe_ApplicationIconFileSize;
	   appIconFileSize.Append(integerAppendStr);
	   TInt fileSize(LoadCompInfoNodeIntParamL(aNodeName,appIconFileSize));
	   iIconFileSizes.Append(fileSize);
	   
	   applications.AppendL(app);
	   CleanupStack::Pop(app);
	   }	
	
	
	TInt packedCaps(LoadCompInfoNodeIntParamL(aNodeName, KTe_UserGrantableCaps));
	TCapabilitySet capSet;
	capSet.SetEmpty();
	UnpackCapabilitySet(packedCaps, capSet);

	CComponentInfo::CNode* node = CComponentInfo::CNode::NewLC(swType, name, version, vendor,
					static_cast<TScomoState>(scomoState), static_cast<TInstallStatus>(installStatus),
					static_cast<TComponentId>(componentId), globalId, static_cast<TAuthenticity>(authenticity),
					capSet, size, hasexe, driveSelectionRequired, &applications);
	
	
	// Load children
	TInt numChildren(LoadCompInfoNodeIntParamL(aNodeName, KTe_CompInfoNumChildren, EFalse));
	for (TInt i=0; i<numChildren; ++i)
		{
		HBufC* childNodeName = HBufC::NewLC(aNodeName.Length() + KTe_CompInfoChildNodePrefix.iTypeLength + KMaxIntDigits);
		TPtr childNodeNamePtr(childNodeName->Des());
		childNodeNamePtr.Copy(aNodeName);
		childNodeNamePtr.Append(KTe_CompInfoChildNodePrefix);
		childNodeNamePtr.AppendNum(i);
		
		CComponentInfo::CNode* childNode = LoadCompInfoNodeLC(*childNodeName);
		node->AddChildL(childNode);
		CleanupStack::Pop(childNode);
		
		CleanupStack::PopAndDestroy(childNodeName);
		}
	applications.Close();
	return node;
	}


TBool CSifGetComponentInfoStep::CompareAppInfoL(const CComponentInfo::CNode& aExpectedNode, const CComponentInfo::CNode& aObtainedNode)
    {
    RPointerArray<Usif::CComponentInfo::CApplicationInfo> obtainedApplicationInfo; 
    RPointerArray<Usif::CComponentInfo::CApplicationInfo> expectedApplicationInfo;
    obtainedApplicationInfo = aObtainedNode.Applications();
    expectedApplicationInfo = aExpectedNode.Applications();
    TInt obtainedApplicationInfoCount = obtainedApplicationInfo.Count();
    TInt expectedApplicationInfoCount = expectedApplicationInfo.Count();       
    
    if(obtainedApplicationInfoCount != expectedApplicationInfoCount)
        {
        INFO_PRINTF3(_L("CComponentInfo doesn't match: expected/obtained Application Info Count: %d/%d"), expectedApplicationInfoCount, obtainedApplicationInfoCount);
        return EFalse;
        }
    if(0 != expectedApplicationInfoCount)
        {                   
        for(TInt i=0 ; i < expectedApplicationInfoCount ; i++)
            {
            
            if(expectedApplicationInfo[i]->AppUid() != obtainedApplicationInfo[i]->AppUid())
               {
               INFO_PRINTF3(_L("CComponentInfo doesn't match: expected/obtained Application UID: 0x%08x/0x%08x"), expectedApplicationInfo[i]->AppUid(), obtainedApplicationInfo[i]->AppUid());
               return EFalse;
               }
                
            if(expectedApplicationInfo[i]->Name().Compare(obtainedApplicationInfo[i]->Name()))
               {
               INFO_PRINTF3(_L("CComponentInfo doesn't match: expected/obtained Application Name: %S/%S"), &expectedApplicationInfo[i]->Name(), &obtainedApplicationInfo[i]->Name());
               return EFalse;
               }
    
            if(expectedApplicationInfo[i]->GroupName().Compare(obtainedApplicationInfo[i]->GroupName()))
               {
               INFO_PRINTF3(_L("CComponentInfo doesn't match: expected/obtained Application Group Name: %S/%S"), &expectedApplicationInfo[i]->GroupName(), &obtainedApplicationInfo[i]->GroupName());
               return EFalse;
               }
            
            _LIT(emtyString,"");
            if(obtainedApplicationInfo[i]->IconFileName().Compare(emtyString))
                {
                //Opening a file server session for icon file size comparison
                RFs fs;            
                User::LeaveIfError(fs.Connect());
                CleanupClosePushL(fs);       
                TEntry entry;
                User::LeaveIfError(fs.Entry(obtainedApplicationInfo[i]->IconFileName(),entry));  
                CleanupStack::Pop(&fs);
                fs.Close();
                
                if(iCompareIconFileSize && iIconFileSizes[iconIndex++] != entry.iSize)
                   {
                   INFO_PRINTF3(_L("CComponentInfo doesn't match: expected/obtained Application Icon File Size: %d/%d"), iIconFileSizes[--iconIndex], entry.iSize);                                  
                   return EFalse;
                   }                 
                }
            if(expectedApplicationInfo[i]->IconFileName().Compare(obtainedApplicationInfo[i]->IconFileName()))
               {
               INFO_PRINTF3(_L("CComponentInfo doesn't match: expected/obtained Application Icon File Name: %S/%S"), &expectedApplicationInfo[i]->IconFileName(), &obtainedApplicationInfo[i]->IconFileName());
               return EFalse;
               }            
            }
        }
    return ETrue;
    }


TBool CSifGetComponentInfoStep::CompareCompInfoNodeL(const CComponentInfo::CNode& aExpectedNode, const CComponentInfo::CNode& aObtainedNode)
	{	
	INFO_PRINTF2(_L("Checking CComponentInfo for component: '%S'"), &aExpectedNode.ComponentName());
	if (aExpectedNode.SoftwareTypeName() != aObtainedNode.SoftwareTypeName())
		{
		INFO_PRINTF3(_L("CComponentInfo doesn't match: expected/obtained SoftwareTypeName: '%S'/'%S'"), &aExpectedNode.SoftwareTypeName(), &aObtainedNode.SoftwareTypeName());
		return EFalse;
		}

	if (aExpectedNode.ComponentName() != aObtainedNode.ComponentName())
		{
		INFO_PRINTF3(_L("CComponentInfo doesn't match: expected/obtained ComponentName: '%S'/'%S'"), &aExpectedNode.ComponentName(), &aObtainedNode.ComponentName());
		return EFalse;
		}

	if (aExpectedNode.Version() != aObtainedNode.Version())
		{
		INFO_PRINTF3(_L("CComponentInfo doesn't match: expected/obtained Version: '%S'/'%S'"), &aExpectedNode.Version(), &aObtainedNode.Version());
		return EFalse;
		}

	if (aExpectedNode.Vendor() != aObtainedNode.Vendor())
		{
		INFO_PRINTF3(_L("CComponentInfo doesn't match: expected/obtained Vendor: '%S'/'%S'"), &aExpectedNode.Vendor(), &aObtainedNode.Vendor());
		return EFalse;
		}

	if (aExpectedNode.ScomoState() != aObtainedNode.ScomoState() && aObtainedNode.InstallStatus() != EInvalid &&
		aObtainedNode.InstallStatus() != ENewComponent) // SCOMO status is meaningless on non-existing components
		{
		INFO_PRINTF3(_L("CComponentInfo doesn't match: expected/obtained ScomoState: %d/%d"), aExpectedNode.ScomoState(), aObtainedNode.ScomoState());
		return EFalse;
		}

	if (aExpectedNode.InstallStatus() != aObtainedNode.InstallStatus())
		{
		INFO_PRINTF3(_L("CComponentInfo doesn't match: expected/obtained InstallStatus: %d/%d"), aExpectedNode.InstallStatus(), aObtainedNode.InstallStatus());
		return EFalse;
		}

	if (aExpectedNode.ComponentId() != aObtainedNode.ComponentId())
		{
		INFO_PRINTF3(_L("CComponentInfo doesn't match: expected/obtained ComponentId: %d/%d"), aExpectedNode.ComponentId(), aObtainedNode.ComponentId());
		return EFalse;
		}

	if (aExpectedNode.GlobalComponentId() != aObtainedNode.GlobalComponentId())
		{
		INFO_PRINTF3(_L("CComponentInfo doesn't match: expected/obtained GlobalComponentId: '%S'/'%S'"), &aExpectedNode.GlobalComponentId(), &aObtainedNode.GlobalComponentId());
		return EFalse;
		}

	if (aExpectedNode.Authenticity() != aObtainedNode.Authenticity())
		{
		INFO_PRINTF3(_L("CComponentInfo doesn't match: expected/obtained Authenticity: %d/%d"), aExpectedNode.Authenticity(), aObtainedNode.Authenticity());
		return EFalse;
		}

	if (PackCapabilitySet(aExpectedNode.UserGrantableCaps()) != PackCapabilitySet(aObtainedNode.UserGrantableCaps()))
		{
		INFO_PRINTF3(_L("CComponentInfo doesn't match: expected/obtained UserGrantableCaps: %d/%d"), PackCapabilitySet(aExpectedNode.UserGrantableCaps()), PackCapabilitySet(aObtainedNode.UserGrantableCaps()));
		return EFalse;
		}

	if (iCompareMaxInstalledSize && aExpectedNode.MaxInstalledSize() != aObtainedNode.MaxInstalledSize())
		{
		INFO_PRINTF3(_L("CComponentInfo doesn't match: expected/obtained MaxInstalledSize: %d/%d"), aExpectedNode.MaxInstalledSize(), aObtainedNode.MaxInstalledSize());
		return EFalse;
		}
	if (aExpectedNode.HasExecutable() != aObtainedNode.HasExecutable())
		{
		INFO_PRINTF3(_L("CComponentInfo doesn't match: expected/obtained Has Executable Flag: %d/%d"), aExpectedNode.HasExecutable(), aObtainedNode.HasExecutable());
		return EFalse;
		}
	
	if (aExpectedNode.DriveSeletionRequired() != aObtainedNode.DriveSeletionRequired())
	    {
	    INFO_PRINTF3(_L("CComponentInfo doesn't match: expected/obtained Drive selection required: %d/%d"), aExpectedNode.DriveSeletionRequired(), aObtainedNode.DriveSeletionRequired());
	    return EFalse;
	    }
	
	if(!CompareAppInfoL(aExpectedNode, aObtainedNode))
	    {
	    iIconFileSizes.Close();
	    return EFalse;
	    }
	
	const RPointerArray<CComponentInfo::CNode>& expectedChildren = aExpectedNode.Children();
	const RPointerArray<CComponentInfo::CNode>& obtainedChildren = aObtainedNode.Children();
	if (expectedChildren.Count() != obtainedChildren.Count())
		{
		INFO_PRINTF4(_L("CComponentInfo doesn't match: component %S has different number of embedded components, expected/obtained: %d/%d"), &aExpectedNode.ComponentName(), expectedChildren.Count(), obtainedChildren.Count());
		return EFalse;
		}
	
	for (TInt i=0; i<expectedChildren.Count(); ++i)
		{
		const CComponentInfo::CNode& expectedChild = *expectedChildren[i];
		const CComponentInfo::CNode& obtainedChild = *obtainedChildren[i];
		if (!CompareCompInfoNodeL(expectedChild, obtainedChild))
			{
			INFO_PRINTF3(_L("CComponentInfo tree comparison failed at component: %S, vendor: %S"), &aExpectedNode.ComponentName(), &aExpectedNode.Vendor());
			return EFalse;
			}
		}		
	return ETrue;
	}

void CSifGetComponentInfoStep::ImplTestStepPreambleL()
/**
* @return - TVerdict code
* Override of base class virtual
*/
	{
	CSifOperationStep::ImplTestStepPreambleL();
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CSifGetComponentInfoStep"));

	if (TestStepResult()==EPass)
		{
		LoadFileNameFromConfigL();

		LoadComponentInfoL();

		GetBoolFromConfig(ConfigSection(),KTe_OperationByFileHandle, iOperationByFileHandle);
		
		GetBoolFromConfig(ConfigSection(),KTe_CompareMaxInstalledSize, iCompareMaxInstalledSize);
		
		GetBoolFromConfig(ConfigSection(),KTe_CompareIconFileSize, iCompareIconFileSize);
		}
	}


void CSifGetComponentInfoStep::ImplTestStepL()
/**
* @return - TVerdict code
* Override of base class pure virtual
* Our implementation only gets called if the base class ImplTestStepPreambleL() did
* not leave. That being the case, the current test result value will be EPass.
*/
	{
	if (TestStepResult()!=EPass)
		{
		INFO_PRINTF1(_L("CSifGetComponentInfoStep::ImplTestStepL() called with TestStepResult()!=EPass"));
		return;
		}

	INFO_PRINTF1(_L("In the file CSifGetComponentInfoStep::ImplTestStepL()"));

	CComponentInfo* compInfo = CComponentInfo::NewLC();

	RFs fs;
	RFile file;
	if (iOperationByFileHandle)
		{
		User::LeaveIfError(fs.Connect());
		fs.ShareProtected();
		CleanupClosePushL(fs);
		User::LeaveIfError(file.Open(fs, iFileName, EFileShareReadersOnly));
		CleanupClosePushL(file);

		iSif.GetComponentInfo(file, *compInfo, iStatus);
		}
	else
		{
		iSif.GetComponentInfo(iFileName, *compInfo, iStatus);
		}

	CancelableWait();

	if (iOperationByFileHandle)
		{
		CleanupStack::PopAndDestroy(2, &fs);
		}
	
	User::LeaveIfError(iStatus.Int());

	TInt err = 0;
	TRAP(err, SetTestStepResult(CompareCompInfoNodeL(iComponentInfo->RootNodeL(), compInfo->RootNodeL()) ? EPass : EFail););
	iIconFileSizes.Close();
	CleanupStack::PopAndDestroy(compInfo);
	}

// *****************************************************************************************************


CSifInstallStep::~CSifInstallStep()
/**
* Destructor
*/
	{
	}

CSifInstallStep::CSifInstallStep()
/**
* Constructor
*/
	{
	SetTestStepName(KSifInstallStep);
	}

void CSifInstallStep::ImplTestStepPreambleL()
/**
* @return - TVerdict code
* Override of base class virtual
*/
	{
	CSifOperationStep::ImplTestStepPreambleL();
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CSifInstallStep"));
	}


void CSifInstallStep::ImplTestStepL()
/**
* @return - TVerdict code
* Override of base class pure virtual
* Our implementation only gets called if the base class ImplTestStepPreambleL() did
* not leave. That being the case, the current test result value will be EPass.
*/
	{
	if (TestStepResult()!=EPass)
		{
		INFO_PRINTF1(_L("CSifInstallStep::ImplTestStepL() called with TestStepResult()!=EPass"));
		return;
		}

	INFO_PRINTF1(_L("In the file CSifInstallStep::ImplTestStepL()"));	
	
	LoadOpaqueParamsL();
	CleanupStack::PushL(TCleanupItem(ClearOpaqueParamsCleanup, this));
	
	LoadFileNameFromConfigL();
		
	GetBoolFromConfig(ConfigSection(),KTe_OperationByFileHandle, iInstallByFileHandle);
		
	RFs fs;
	RFile file;
	if (iInstallByFileHandle)
		{
		User::LeaveIfError(fs.Connect());
		fs.ShareProtected();
		CleanupClosePushL(fs);
		User::LeaveIfError(file.Open(fs, iFileName, EFileRead | EFileShareReadersOnly));
		CleanupClosePushL(file);

		if (iUseEnhancedApi)
			{
			iSif.Install(file, *iPluginOpaqueArguments, *iPluginOpaqueResults, iStatus, iExclusiveOperation);
			}
		else
			{
			iSif.Install(file, iStatus, iExclusiveOperation);
			}
		}
	else
		{
		if (iUseEnhancedApi)
			{
			iSif.Install(iFileName, *iPluginOpaqueArguments, *iPluginOpaqueResults, iStatus, iExclusiveOperation);
			}
		else
			{
			iSif.Install(iFileName, iStatus, iExclusiveOperation);
			}
		}

	CancelableWait();

	if (iInstallByFileHandle)
		{
		CleanupStack::PopAndDestroy(2, &fs);
		}

	TInt err = iStatus.Int();
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("CSifInstallStep::ImplTestStepL failed with error code = %d"), err);
		User::LeaveIfError(err);
		}
	
	TBool res = EFalse;
	TRAP(err, res = CompareOpaqueResultsL());
	if (err != KErrNoMemory)
		User::Leave(err); // Do not treat OOM errors as fatal by this point - otherwise OOM tests will atempt to uninstall the component again after uninstall has completed	
	
	if (err == KErrNoMemory) // Handling KErrNoMemory as it is done purposefully as part of OOM Test
	    SetTestStepResult(EPass);
	else if (!res)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF1(_L("CSifInstallStep::ImplTestStepL failed because of CompareOpaqueResultsL"));
		INFO_PRINTF1(_L("=================== Begin of Received Opaque Results ==================="));
		PrintOpaqueParamsL(*iPluginOpaqueResults);
		INFO_PRINTF1(_L("=================== End of Received Opaque Results ==================="));
		INFO_PRINTF1(_L("=================== Begin of Expected Opaque Results ==================="));
		PrintOpaqueParamsL(*iPluginRefOpaqueResults);
		INFO_PRINTF1(_L("=================== End of Expected Opaque Results ==================="));
		}
	CleanupStack::PopAndDestroy(); //ClearOpaqueParamsCleanup
	}

// *****************************************************************************************************


CSifUninstallStep::~CSifUninstallStep()
/**
* Destructor
*/
	{
	}

CSifUninstallStep::CSifUninstallStep()
/**
* Constructor
*/
	{
	SetTestStepName(KSifUninstallStep);
	}

void CSifUninstallStep::ImplTestStepPreambleL()
/**
* @return - TVerdict code
* Override of base class virtual
*/
	{
	CSifOperationStep::ImplTestStepPreambleL();
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CSifUninstallStep"));
	
	LoadComponentIdFromConfigL();
	}

void CSifUninstallStep::ImplTestStepL()
/**
* @return - TVerdict code
* Override of base class pure virtual
* Our implementation only gets called if the base class ImplTestStepPreambleL() did
* not leave. That being the case, the current test result value will be EPass.
*/
	{
	INFO_PRINTF1(_L("In the file CSifUninstallStep::ImplTestStepL()"));  //Block start	
	LoadOpaqueParamsL();
	CleanupStack::PushL(TCleanupItem(ClearOpaqueParamsCleanup, this));
	
	if (iUseEnhancedApi)
		{
		iSif.Uninstall(iComponentId, *iPluginOpaqueArguments, *iPluginOpaqueResults, iStatus, iExclusiveOperation);
		}
	else
		{
		iSif.Uninstall(iComponentId, iStatus, iExclusiveOperation);
		}

	CancelableWait();
	
	User::LeaveIfError(iStatus.Int());

	TBool res = EFalse;
	TRAPD(err, res = CompareOpaqueResultsL());
	if (err != KErrNoMemory)
		User::Leave(err); // Do not treat OOM errors as fatal by this point - otherwise OOM tests will atempt to uninstall the component again after uninstall has completed
	
	if (err == KErrNoMemory) // Handling KErrNoMemory as it is done purposefully as part of OOM Test
	    SetTestStepResult(EPass);
	else if (!res)
		SetTestStepResult(EFail);
		
	CleanupStack::PopAndDestroy(); //ClearOpaqueParamsCleanup		
	}
	
// *****************************************************************************************************

	
CSifActivateStep::CSifActivateStep(TBool aActivate): iActivate(aActivate)
	{
	SetTestStepName(KSifActivateStep);
	}

CSifActivateStep::~CSifActivateStep()
	{
	}

void CSifActivateStep::ImplTestStepPreambleL()
/**
* @return - TVerdict code
* Override of base class virtual
*/
	{
	CSifOperationStep::ImplTestStepPreambleL();
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CSifActivateStep"));

	if (TestStepResult()==EPass)
		{
		LoadComponentIdFromConfigL();
		}

	}

void CSifActivateStep::ImplTestStepL()
/**
* @return - TVerdict code
* Override of base class pure virtual
* Our implementation only gets called if the base class ImplTestStepPreambleL() did
* not leave. That being the case, the current test result value will be EPass.
*/
	{
	INFO_PRINTF1(_L("In the file CSifActivateStep::ImplTestStepL()"));

	if (iActivate)
		{
		iSif.Activate(iComponentId, iStatus);
		}
	else
		{
		iSif.Deactivate(iComponentId, iStatus);
		}

	CancelableWait();
	
	User::LeaveIfError(iStatus.Int());
	
	if (!CompareOpaqueResultsL())
		SetTestStepResult(EFail);
	}

/**
* This test step implements verification of concurrent operations.
*/

CSifMultipleInstallStep::~CSifMultipleInstallStep()
	/**
	* Destructor
	*/
	{
	}

CSifMultipleInstallStep::CSifMultipleInstallStep()
	/**
	* Constructor
	*/
	{
	SetTestStepName(KSifMultipleInstallStep);
	}

void CSifMultipleInstallStep::ImplTestStepPreambleL()
/**
* @return - TVerdict code
* Override of base class virtual
*/
	{
	INFO_PRINTF1(_L("This is TestStepPreamble method of class CSifMultipleInstallStep"));

	LoadFileNameFromConfigL();
	LoadExclusiveOperationFlagFromConfigL();
	}

void CSifMultipleInstallStep::ImplTestStepL()
	{
	INFO_PRINTF1(_L("In the file CSifMultipleInstallStep::ImplTestStepL()"));
	LoadOpaqueParamsL();

	if (TestStepResult()!=EPass)
		{
		INFO_PRINTF1(_L("CSifMultipleInstallStep::ImplTestStepL() called with TestStepResult()!=EPass"));
		return;
		}

	TInt expectedConcurrentResult = 0;
	TInt overlapRequest = EFalse;
	if (!GetIntFromConfig(ConfigSection(), KTe_ExpectedConcurrentResult, expectedConcurrentResult) ||
		!GetBoolFromConfig(ConfigSection(), KTe_OverlapRequest, overlapRequest))
		{
		INFO_PRINTF3(_L("Expected test parameters: %S or %S not found "), &KTe_ExpectedConcurrentResult, &KTe_OverlapRequest);
		User::Leave(KErrNotFound);
		}

	/* First installation request */
	RSoftwareInstall sif1;
	TInt error = sif1.Connect();
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("Failed to connect to the SIF server, error code: %d"), error);
		User::Leave(error);
		}
	CleanupClosePushL(sif1);

	TRequestStatus status1;
	sif1.Install(iFileName, status1);

	if (!overlapRequest)
		{
		User::WaitForRequest(status1);
		}

	/* Second installation request */
	
	// Use different file name if provided
	TPtrC fileName2;
	if (!GetStringFromConfig(ConfigSection(), _L("packageFile2"), fileName2))
		{
		fileName2.Set(iFileName);
		}

	RSoftwareInstall sif2;
	error = sif2.Connect();
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("Failed to connect to the SIF server, error code: %d"), error);
		User::Leave(error);
		}
	CleanupClosePushL(sif2);

	TRequestStatus status2;
	
	if (iUseEnhancedApi)
		{
		sif2.Install(fileName2, *iPluginOpaqueArguments, *iPluginOpaqueResults, status2, iExclusiveOperation);
		}
	else
		{
		sif2.Install(fileName2, status2, iExclusiveOperation);
		}

	if (overlapRequest)
		{
		User::WaitForRequest(status1);
		}

	User::WaitForRequest(status2);

	//close both the sessions at the end.
	CleanupStack::PopAndDestroy(2, &sif1);

	if(status2 != expectedConcurrentResult) 
		{
		INFO_PRINTF3(_L("status2: %d expectedResult1: %d"), status2.Int(), expectedConcurrentResult);
		SetTestStepResult(EFail);
		}
	User::LeaveIfError(status1.Int());
	}
