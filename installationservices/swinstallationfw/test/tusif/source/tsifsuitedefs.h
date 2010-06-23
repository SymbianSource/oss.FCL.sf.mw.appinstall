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

#if (!defined TSIFSUITEDEFS_H)
#define TSIFSUITEDEFS_H

namespace Usif
	{
	_LIT(KTe_ExpectedResult,"expectedResult");
	_LIT(KTe_UiScript,"uiScript");
	_LIT(KTe_CancelAfter,"cancelAfter");
	_LIT(KTe_OperationByFileHandle,"operationByFileHandle");
	_LIT(KTe_CompareMaxInstalledSize,"compareMaxInstalledSize");
	_LIT(KTe_CompareIconFileSize,"compareIconFileSize");	
	_LIT(KTe_UseEnhancedApi,"useEnhancedApi");
	_LIT(KTe_PackageFile,"packageFile");
	_LIT(KTe_ComponentId,"componentId");
	_LIT(KTe_PluginOpaqueArgumentName,"pluginOpaqueArgumentName");
	_LIT(KTe_PluginOpaqueArgumentType,"pluginOpaqueArgumentType");
	_LIT(KTe_PluginOpaqueArgumentValue,"pluginOpaqueArgumentValue");
	_LIT(KTe_PluginRefOpaqueResultName,"pluginRefOpaqueResultName");
	_LIT(KTe_PluginRefOpaqueResultType,"pluginRefOpaqueResultType");
	_LIT(KTe_PluginRefOpaqueResultValue,"pluginRefOpaqueResultValue");
	_LIT(KTe_PluginOpaqueValueTypeInt,"Int");
	_LIT(KTe_PluginOpaqueValueTypeString,"String");
	_LIT(KTe_SoftwareTypeName,"softwareTypeName");
	_LIT(KTe_ComponentName,"componentName");
	_LIT(KTe_ComponentVersion,"componentVersion");
	_LIT(KTe_ComponentVendor,"componentVendor");
	_LIT(KTe_GlobalComponentId,"globalComponentId");
	_LIT(KTe_Authenticity,"authenticity");
	_LIT(KTe_UserGrantableCaps,"userGrantableCapsBitMask");
	_LIT(KTe_MaxInstalledSize,"maxInstalledSize");
	_LIT(KTe_HasExecutable,"hasExecutable");
	_LIT(KTe_DriveSelectionRequired,"driveSelectionRequired");
	_LIT(KTe_NumberOfApplications,"noOfApps");
	_LIT(KTe_ApplicationUid,"appUid");
	_LIT(KTe_ApplicationName,"appName");
	_LIT(KTe_ApplicationGroupName,"appGroupName");
	_LIT(KTe_ApplicationIconFileName,"appIconFileName");
	_LIT(KTe_ApplicationIconFileSize,"appIconFileSize");
	_LIT(KTe_VerifyPackageExistence,"verifyExistence");
	_LIT(KTe_CheckSisRegistry,"checkSisRegistry");
	_LIT(KTe_ExecutableName,"executableName");
	_LIT(KTe_ExecutableReturnCode,"executableReturnCode");
	_LIT(KTe_AnyValuePermitted,"__ANY_VALUE_PERMITTED__");
	_LIT(KTe_ScomoState,"scomoState");
	_LIT(KTe_ExpectedConcurrentResult ,"expectedConcurrentResult");
	_LIT(KTe_OverlapRequest,"overlapRequest");
	_LIT(KTe_InstallStatus,"installStatus");
	_LIT(KTe_ExclusiveOperation,"exclusiveOperation");
	_LIT(KTe_CompInfoRootNodePrefix,"root");
	_LIT(KTe_CompInfoChildNodePrefix,".child");
	_LIT(KTe_CompInfoNumChildren,"numChildren");
	_LIT(KTe_OperationType,"operation");
	// For test step panics
	_LIT(KTe_SifSuitePanic,"Te_SifSuite");

	const TInt KMaxIntDigits = 10;

	} // namespace Sif

#endif
