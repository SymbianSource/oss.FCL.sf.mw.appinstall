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
* RSoftwareComponentRegistry implementation. See class and function 
* declarations for more detail.
*
*/


#include "scr.h"
#include "scrclient.inl"
#include "scrcommon.h"
#include "usiflog.h"
#include "scr_internal.h"
#include <e32cmn.h>
#include <scs/streamingarray.h>
#include <scs/scscommon.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "screntries_internal.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

using namespace Usif;

TInt RSoftwareComponentRegistry::SendSyncMessage(TInt aFunction, const TIpcArgs& aArgs) const
	{
	return CallSessionFunction(aFunction, aArgs);
	}

EXPORT_C RSoftwareComponentRegistry::RSoftwareComponentRegistry()
	:	RScsClientBase()
	{
	// empty
	}

EXPORT_C TInt RSoftwareComponentRegistry::Connect()
	{
	DEBUG_PRINTF2(_L("Connecting to %S."), &KSoftwareComponentRegistryName);
	TVersion version = ScrServerVersion();
	TUidType scrFullUid = ScrServerImageFullUid();
	
	return RScsClientBase::Connect(KSoftwareComponentRegistryName(), version, KScrServerImageName(), scrFullUid);
	}

EXPORT_C void RSoftwareComponentRegistry::Close()
	{
	DEBUG_PRINTF2(_L("Closing connection to %S."), &KSoftwareComponentRegistryName);
	RScsClientBase::Close();
	}

EXPORT_C void RSoftwareComponentRegistry::CreateTransactionL()
	{
	DEBUG_PRINTF(_L("Sending Create Transaction request."));
	User::LeaveIfError(CallSessionFunction(ECreateTransaction));
	}
		
EXPORT_C void RSoftwareComponentRegistry::RollbackTransactionL()
	{
	DEBUG_PRINTF(_L("Sending Rollback Transaction request."));
	User::LeaveIfError(CallSessionFunction(ERollbackTransaction));
	}

EXPORT_C void RSoftwareComponentRegistry::CommitTransactionL()
	{
	DEBUG_PRINTF(_L("Sending Commit Transaction request."));
	User::LeaveIfError(CallSessionFunction(ECommitTransaction));
	}


EXPORT_C TComponentId RSoftwareComponentRegistry::AddComponentL(const TDesC& aName, const TDesC& aVendor, const TDesC& aUniqueSwTypeName, const TDesC* aGlobalId, TScrComponentOperationType aCompOpType)
	{
	DEBUG_PRINTF3(_L("Sending add a new component ('%S','%S') request."), &aName, &aVendor);

	CLocalizableComponentInfo *cmpInfo = CLocalizableComponentInfo::NewLC(aName, aVendor, KNonLocalized);
	RPointerArray<CLocalizableComponentInfo> cmpInfoList;
	cmpInfoList.AppendL(cmpInfo);
	CleanupStack::Pop(cmpInfo);
	CleanupResetAndDestroyPushL(cmpInfoList);
	TComponentId compId = AddComponentL(cmpInfoList, aUniqueSwTypeName, aGlobalId, aCompOpType);
	CleanupStack::PopAndDestroy(&cmpInfoList);
	return compId;
	}

EXPORT_C TComponentId RSoftwareComponentRegistry::AddComponentL(const RPointerArray<CLocalizableComponentInfo>& aComponentInfo, const TDesC& aUniqueSwTypeName, const TDesC* aGlobalId, TScrComponentOperationType aCompOpType)
	{
	if(!aComponentInfo.Count())
		{
		DEBUG_PRINTF(_L("The localizables component info array is empty!."));
		User::Leave(KErrArgument);
		}
	DEBUG_PRINTF(_L("Sending add a new component request with a set of localizable names."));
	
	TComponentId compId = static_cast<TComponentId>(aCompOpType);
	// compId variable carries the component operation type to the SCR server.
	// On return, it will contain the actual component Id value.
	TPckg<TComponentId> compIdPckg(compId);
	HBufC8* arrayBuf = ExternalizePointersArrayLC(aComponentInfo);
	const TDesC *globalId = aGlobalId ? aGlobalId : &KNullDesC();
	TIpcArgs args(&aUniqueSwTypeName, arrayBuf, globalId, &compIdPckg);
	User::LeaveIfError(CallSessionFunction(EAddComponent, args));
	CleanupStack::PopAndDestroy(arrayBuf);
	return compId;
	}

template <class C>
void RSoftwareComponentRegistry::SendDependencyArgsL(TInt aFunction, const C& aSupplierId, const CGlobalComponentId& aDependantGlobalCompId)
	{
	RBuf8 suppVerCompId;
	suppVerCompId.CleanupClosePushL();
	ExternalizeObjectL(&aSupplierId, suppVerCompId);
		
	RBuf8 depGlobalId;
	depGlobalId.CleanupClosePushL();
	ExternalizeObjectL(&aDependantGlobalCompId, depGlobalId);
	// Send the software type of the dependant component seperately in order to apply custom security check properly on the server side.
	TIpcArgs args(&aDependantGlobalCompId.SoftwareTypeName(),&suppVerCompId, &depGlobalId);
	User::LeaveIfError(CallSessionFunction(aFunction, args));
	CleanupStack::PopAndDestroy(2, &suppVerCompId); // suppVerCompId, depGlobalId
	}

EXPORT_C void RSoftwareComponentRegistry::AddComponentDependencyL(const CVersionedComponentId& aSupplierVerCompId, const CGlobalComponentId& aDependantGlobalCompId)
	{
	DEBUG_PRINTF3(_L("Sending a request to add a new dependency between two components (%S depends on %S)."), &aDependantGlobalCompId.GlobalIdName(), &aSupplierVerCompId.GlobalId().GlobalIdName());
	SendDependencyArgsL(EAddComponentDependency, aSupplierVerCompId, aDependantGlobalCompId);
	}

EXPORT_C void RSoftwareComponentRegistry::SetComponentPropertyL(TComponentId aComponentId, const TDesC& aName, const TDesC8& aValue)
	{
	DEBUG_PRINTF3(_L("Sending a request to set the binary property(%S) of the component(%d)."), &aName, aComponentId);
	TIpcArgs args(aComponentId, &aName, &aValue);
	User::LeaveIfError(CallSessionFunction(ESetComponentBinaryProperty, args));
	}

EXPORT_C void RSoftwareComponentRegistry::SetComponentPropertyL(TComponentId aComponentId, const TDesC& aName, const TDesC& aValue, TLanguage aLocale)
	{
	DEBUG_PRINTF4(_L("Sending a request to set the string property(%S,%S) of the component(%d)."), &aName, &aValue, aComponentId);
	TIpcArgs args(aComponentId, &aName, &aValue, aLocale);
	User::LeaveIfError(CallSessionFunction(ESetComponentLocalizableProperty, args));
	}

EXPORT_C void RSoftwareComponentRegistry::SetComponentPropertyL(TComponentId aComponentId, const TDesC& aName, TInt64 aValue)
	{
	DEBUG_PRINTF4(_L("Sending a request to set the numeric property(%S,%Ld) of the component(%d)."), &aName, aValue, aComponentId);
	TIpcArgs args(aComponentId, &aName, I64HIGH(aValue), I64LOW(aValue));
	User::LeaveIfError(CallSessionFunction(ESetComponentNumericProperty, args));
	}

EXPORT_C void RSoftwareComponentRegistry::RegisterComponentFileL(TComponentId aComponentId, const TDesC& aFileName, TBool aConsiderForInstalledDrives /*= ETrue*/ )
	{
	DEBUG_PRINTF4(_L("Sending a request to register the file(%S) for the component(%d). ConsiderFilesInDriveList:%d"), &aFileName, aComponentId, aConsiderForInstalledDrives);
	TIpcArgs args(aComponentId, &aFileName, aConsiderForInstalledDrives);
	User::LeaveIfError(CallSessionFunction(ERegisterComponentFile, args));
	}

EXPORT_C void RSoftwareComponentRegistry::SetFilePropertyL(TComponentId aComponentId, const TDesC& aFileName, const TDesC& aPropName, const TDesC8& aPropValue)
	{
	DEBUG_PRINTF4(_L("Sending a request to set the string property(%S) of the file(%S) of the component(%d)."), &aPropName, &aFileName, aComponentId);
	TIpcArgs args(aComponentId, &aFileName, &aPropName, &aPropValue);
	User::LeaveIfError(CallSessionFunction(ESetFileStringProperty, args));
	}
EXPORT_C void RSoftwareComponentRegistry::SetFilePropertyL(TComponentId aComponentId, const TDesC& aFileName, const TDesC& aPropName, TInt aPropValue)
	{
	DEBUG_PRINTF5(_L("Sending a request to set the numeric property(%S,%d) of the file(%S) of the component(%d)."), &aPropName, aPropValue, &aFileName, aComponentId);
	TIpcArgs args(aComponentId, &aFileName, &aPropName, aPropValue);
	User::LeaveIfError(CallSessionFunction(ESetFileNumericProperty, args));
	}

EXPORT_C void RSoftwareComponentRegistry::SetComponentNameL(TComponentId aComponentId, const TDesC& aName, TLanguage aLocale)
	{
	DEBUG_PRINTF4(_L("Sending a request to update the name(%S,%d) of the component(%d)."), &aName, aLocale, aComponentId);
	TIpcArgs args(aComponentId, &aName, aLocale);
	User::LeaveIfError(CallSessionFunction(ESetComponentName, args));
	}

EXPORT_C void RSoftwareComponentRegistry::SetVendorNameL(TComponentId aComponentId, const TDesC& aVendor, TLanguage aLocale)
	{
	DEBUG_PRINTF4(_L("Sending a request to update the vendor(%S,%d) of the component(%d)."), &aVendor, aLocale, aComponentId);
	TIpcArgs args(aComponentId, &aVendor, aLocale);
	User::LeaveIfError(CallSessionFunction(ESetComponentVendor, args));
	}

EXPORT_C void RSoftwareComponentRegistry::SetComponentVersionL(TComponentId aComponentId, const TDesC& aVersion)
	{
	DEBUG_PRINTF3(_L("Sending a request to set the version(%S) of the component(%d)."), &aVersion, aComponentId);
	TIpcArgs args(aComponentId, &aVersion);
	User::LeaveIfError(CallSessionFunction(ESetComponentVersion, args));
	}

EXPORT_C void RSoftwareComponentRegistry::SetIsComponentRemovableL(TComponentId aComponentId, TBool aValue)
	{
	DEBUG_PRINTF3(_L("Sending a request to set the removable attribute(%d) of the component(%d)."), aValue, aComponentId);
	TIpcArgs args(aComponentId, aValue);
	User::LeaveIfError(CallSessionFunction(ESetIsComponentRemovable, args));
	}

EXPORT_C void RSoftwareComponentRegistry::SetIsComponentDrmProtectedL(TComponentId aComponentId, TBool aValue)
	{
	DEBUG_PRINTF3(_L("Sending a request to set the DRM protected attribute(%d) of the component(%d)."), aValue, aComponentId);
	TIpcArgs args(aComponentId, aValue);
	User::LeaveIfError(CallSessionFunction(ESetIsComponentDrmProtected, args));
	}

EXPORT_C void RSoftwareComponentRegistry::SetIsComponentHiddenL(TComponentId aComponentId, TBool aValue)
	{
	DEBUG_PRINTF3(_L("Sending a request to set the hidden attribute(%d) of the component(%d)."), aValue, aComponentId);
	TIpcArgs args(aComponentId, aValue);
	User::LeaveIfError(CallSessionFunction(ESetIsComponentHidden, args));
	}

EXPORT_C void RSoftwareComponentRegistry::SetIsComponentKnownRevokedL(TComponentId aComponentId, TBool aValue)
	{
	DEBUG_PRINTF3(_L("Sending a request to set the known-revoked attribute(%d) of the component(%d)."), aValue, aComponentId);
	TIpcArgs args(aComponentId, aValue);
	User::LeaveIfError(CallSessionFunction(ESetIsComponentKnownRevoked, args));
	}

EXPORT_C void RSoftwareComponentRegistry::SetIsComponentOriginVerifiedL(TComponentId aComponentId, TBool aValue)
	{
	DEBUG_PRINTF3(_L("Sending a request to set the origin-verified attribute(%d) of the component(%d)."), aValue, aComponentId);
	TIpcArgs args(aComponentId, aValue);
	User::LeaveIfError(CallSessionFunction(ESetIsComponentOriginVerified, args));
	}

EXPORT_C void RSoftwareComponentRegistry::SetComponentSizeL(TComponentId aComponentId, TInt64 aComponentSizeInBytes)
	{
	DEBUG_PRINTF3(_L("Sending a request to set the install-time size (%Ld) of the component(%d)."), aComponentSizeInBytes, aComponentId);
	TIpcArgs args(aComponentId, I64HIGH(aComponentSizeInBytes), I64LOW(aComponentSizeInBytes));
	User::LeaveIfError(CallSessionFunction(ESetComponentSize, args));
	}

EXPORT_C void RSoftwareComponentRegistry::DeleteComponentPropertyL(TComponentId aComponentId, const TDesC& aPropName)
	{
	DEBUG_PRINTF3(_L("Sending a request to delete the property(%S) of the component(%d)."), &aPropName, aComponentId);
	TIpcArgs args(aComponentId, &aPropName);
	User::LeaveIfError(CallSessionFunction(EDeleteComponentProperty, args));
	}

EXPORT_C void RSoftwareComponentRegistry::DeleteFilePropertyL(TComponentId aComponentId, const TDesC& aFileName, const TDesC& aPropName)
	{
	DEBUG_PRINTF4(_L("Sending a request to delete the property(%S) of the file(%S) of the component(%d)."), &aPropName, &aFileName, aComponentId);
	TIpcArgs args(aComponentId, &aFileName, &aPropName);
	User::LeaveIfError(CallSessionFunction(EDeleteFileProperty, args));
	}

EXPORT_C void RSoftwareComponentRegistry::UnregisterComponentFileL(TComponentId aComponentId, const TDesC& aFileName)
	{
	DEBUG_PRINTF3(_L("Sending a request to deregister the file(%S) for the component(%d)."), &aFileName, aComponentId);
	TIpcArgs args(aComponentId, &aFileName);
	User::LeaveIfError(CallSessionFunction(EUnregisterComponentFile, args));
	}

EXPORT_C void RSoftwareComponentRegistry::DeleteComponentL(TComponentId aComponentId)
	{
	DEBUG_PRINTF2(_L("Sending a request to delete the component(%d)."), aComponentId);
	TIpcArgs args(aComponentId);
	User::LeaveIfError(CallSessionFunction(EDeleteComponent, args));
	}

EXPORT_C void RSoftwareComponentRegistry::DeleteComponentDependencyL(const CGlobalComponentId& aSupplierGlobalCompId, const CGlobalComponentId& aDependantGlobalCompId)
	{
	DEBUG_PRINTF3(_L("Sending a request to delete an existing dependency between two components (%S depends on %S)."), &aDependantGlobalCompId.GlobalIdName(), &aSupplierGlobalCompId.GlobalIdName());
	SendDependencyArgsL(EDeleteComponentDependency, aSupplierGlobalCompId, aDependantGlobalCompId);
	}

EXPORT_C TBool RSoftwareComponentRegistry::GetComponentL(TComponentId aComponentId, CComponentEntry& aEntry, TLanguage aLocale) const
	{
	DEBUG_PRINTF2(_L("Sending a request to retrieve the entry of the component(%d)."), aComponentId);
	TIpcArgs args(aComponentId, aLocale);
	TInt argNum = 2; // size descriptor will be added to this slot
	return GetObjectL(*this, aEntry, EGetSingleComponentSize, EGetSingleComponentData, argNum, args);
	}
	
EXPORT_C void RSoftwareComponentRegistry::GetComponentLocalizedInfoL(TComponentId aComponentId, RPointerArray<CLocalizableComponentInfo>& aCompLocalizedInfoArray) const
    {
    DEBUG_PRINTF2(_L("Sending a request to retrieve the localized entry of the component(%d)."), aComponentId);
    TIpcArgs args(aComponentId);
    TInt argNum = 1; // size descriptor will be added to this slot
    GetObjectArrayL(*this, EGetLocalizedComponentSize, EGetLocalizedComponentData, argNum, args, aCompLocalizedInfoArray);
    }

EXPORT_C void RSoftwareComponentRegistry::GetComponentIdsL(RArray<TComponentId>& aComponentIdList, CComponentFilter* aFilter) const
	{
	RBuf8 buf;
	buf.CleanupClosePushL();
	ExternalizeObjectL(aFilter, buf);
	
	TIpcArgs args(&buf);
	TInt argNum = 1;
	GetObjectArrayL(*this, EGetComponentIdListSize, EGetComponentIdListData, argNum, args, aComponentIdList);
	CleanupStack::PopAndDestroy(&buf);
	}

EXPORT_C TComponentId RSoftwareComponentRegistry::GetComponentIdL(const TDesC& aGlobalIdName, const TDesC& aUniqueSwTypeName) const
	{
	DEBUG_PRINTF3(_L("Sending a request to retrieve the local component Id of a global component id (%S,%S)."), &aGlobalIdName, &aUniqueSwTypeName);
	
	TComponentId compId;
	TPckg<TComponentId> compIdPckg(compId);
		
	TIpcArgs args(&aGlobalIdName, &aUniqueSwTypeName, &compIdPckg);
	User::LeaveIfError(CallSessionFunction(EGetLocalComponentId, args));
	
	return compId;
	}

EXPORT_C CComponentEntry* RSoftwareComponentRegistry::GetComponentL(const TDesC& aGlobalIdName, const TDesC& aUniqueSwTypeName, TLanguage aLocale) const
	{
	DEBUG_PRINTF3(_L("Sending a request with the global id(%S,%S) to retrieve the entry of the component."), &aGlobalIdName, &aUniqueSwTypeName);

	TIpcArgs args(&aGlobalIdName, &aUniqueSwTypeName, aLocale);
	TInt argNum = 3; // size descriptor will be added to this slot
	return GetObjectL<CComponentEntry>(*this, EGetComponentWithGlobalIdSize, EGetComponentWithGlobalIdData, argNum, args);	
	}

EXPORT_C void RSoftwareComponentRegistry::GetSupplierComponentsL(const CGlobalComponentId& aDependantGlobalId, RPointerArray<CVersionedComponentId>& aSupplierList) const
	{
	DEBUG_PRINTF2(_L("Sending a request to retrieve the supplier list of a dependant component (%S)."), &aDependantGlobalId.GlobalIdName());
	
	RBuf8 depGlobalId;
	depGlobalId.CleanupClosePushL();
	ExternalizeObjectL(&aDependantGlobalId, depGlobalId);
	
	TIpcArgs args(&depGlobalId);
	TInt argNum = 1;
	GetObjectArrayL(*this, EGetSupplierComponentsSize, EGetSupplierComponentsData, argNum, args, aSupplierList);
	CleanupStack::PopAndDestroy(&depGlobalId);
	}

EXPORT_C void RSoftwareComponentRegistry::GetDependantComponentsL(const CGlobalComponentId& aSupplierGlobalId, RPointerArray<CVersionedComponentId>& aDependantList) const
	{
	DEBUG_PRINTF2(_L("Sending a request to retrieve the dependant list of a supplier component (%S)."), &aSupplierGlobalId.GlobalIdName());
	
	RBuf8 supGlobalId;
	supGlobalId.CleanupClosePushL();
	ExternalizeObjectL(&aSupplierGlobalId, supGlobalId);
	
	TIpcArgs args(&supGlobalId);
	TInt argNum = 1;
	GetObjectArrayL(*this, EGetDependantComponentsSize, EGetDependantComponentsData, argNum, args, aDependantList);
	CleanupStack::PopAndDestroy(&supGlobalId);
	}

EXPORT_C void RSoftwareComponentRegistry::GetFilePropertiesL(TComponentId aComponentId, const TDesC& aFileName, RPointerArray<CPropertyEntry>& aProperties) const
	{
	DEBUG_PRINTF2(_L("Sending a request to retrieve the properties of the file(%S)."), &aFileName);
	TIpcArgs args(aComponentId, &aFileName);
	TInt argNum = 2;
	GetObjectArrayL(*this, EGetFilePropertiesSize, EGetFilePropertiesData, argNum, args, aProperties);
	}

EXPORT_C CPropertyEntry* RSoftwareComponentRegistry::GetFilePropertyL(TComponentId aComponentId, const TDesC& aFileName, const TDesC& aPropertyName) const
	{
	DEBUG_PRINTF3(_L("Sending a request to retrieve the property(%S) of the file(%S)."), &aPropertyName, &aFileName);
	TIpcArgs args(aComponentId, &aFileName, &aPropertyName);
	TInt argNum = 3;
	return GetObjectL<CPropertyEntry>(*this, EGetSingleFilePropertySize, EGetSingleFilePropertyData, argNum, args);
	}

EXPORT_C TUint RSoftwareComponentRegistry::GetComponentFilesCountL(TComponentId aComponentId) const
	{
	TUint registeredFilesNum;
	TPckg<TUint> resPckg(registeredFilesNum);
	TIpcArgs args(aComponentId, &resPckg);
	User::LeaveIfError(CallSessionFunction(EGetComponentFilesCount, args));
	return registeredFilesNum;	
	}

EXPORT_C void RSoftwareComponentRegistry::GetComponentsL(const TDesC& aFileName, RArray<TComponentId>& aComponents) const
	{
	TIpcArgs args(&aFileName);
	TInt argNum = 1;
	GetObjectArrayL(*this, EGetFileComponentsSize, EGetFileComponentsData, argNum, args, aComponents);
	}

EXPORT_C CPropertyEntry* RSoftwareComponentRegistry::GetComponentPropertyL(TComponentId aComponentId, const TDesC& aPropertyName, TLanguage aLocale) const
	{
	TIpcArgs args(aComponentId, &aPropertyName, aLocale);
	TInt argNum = 3;
	return GetObjectL<CPropertyEntry>(*this, EGetComponentSinglePropertySize, EGetComponentSinglePropertyData, argNum, args);
	}

EXPORT_C void RSoftwareComponentRegistry::GetComponentPropertiesL(TComponentId aComponentId, RPointerArray<CPropertyEntry>& aProperties, TLanguage aLocale) const
	{
	TIpcArgs args(aComponentId, aLocale);
	TInt argNum = 2;
	GetObjectArrayL(*this, EGetComponentPropertiesSize, EGetComponentPropertiesData, argNum, args, aProperties);
	}

EXPORT_C TBool RSoftwareComponentRegistry::IsMediaPresentL(TComponentId aComponentId) const
	{
	DEBUG_PRINTF2(_L("Sending a request to retrieve the media presence status of component (%d)."), aComponentId);
	TBool result;
	TPckg<TBool> isMediaPresent(result);
	User::LeaveIfError(CallSessionFunction(EGetIsMediaPresent, TIpcArgs(aComponentId, &isMediaPresent)));	

	DEBUG_PRINTF3(_L("Received media presence status of component (%d) - result is %d."), aComponentId, result);
	return result;
	}

EXPORT_C void RSoftwareComponentRegistry::SetScomoStateL(TComponentId aComponentId, TScomoState aScomoState) const
	{
	DEBUG_PRINTF3(_L("Sending a request to set the scomo state (%d) of component(%d) at install time."), aScomoState, aComponentId);
	TIpcArgs args(aComponentId, aScomoState);
	User::LeaveIfError(CallSessionFunction(ESetScomoState, args));
	}

EXPORT_C TUid RSoftwareComponentRegistry::GetPluginUidL(const TDesC& aMimeType) const
	{
	DEBUG_PRINTF2(_L("Sending a request to retrieve the list of plugins for mime types %S."), &aMimeType);
	TUint32 uidNum(0);
	TPckg<TUint32> uidDes(uidNum);
	TIpcArgs args(&aMimeType, &uidDes);
	User::LeaveIfError(CallSessionFunction(EGetPluginUidWithMimeType, args));
	TUid pluginUid = TUid::Uid(uidNum);
	return pluginUid;
	}

EXPORT_C TUid RSoftwareComponentRegistry::GetPluginUidL(TComponentId aComponentId) const
	{
	DEBUG_PRINTF2(_L("Sending a request to retrieve the list of plugins for the component(%d)."), aComponentId);
	TUint32 uidNum(0);
	TPckg<TUint32> uidDes(uidNum);
	TIpcArgs args(aComponentId, &uidDes);
	User::LeaveIfError(CallSessionFunction(EGetPluginUidWithComponentId, args));
	TUid pluginUid = TUid::Uid(uidNum);
	return pluginUid;
	}

EXPORT_C void RSoftwareComponentRegistry::AddSoftwareTypeL(const CSoftwareTypeRegInfo& aSwTypeRegInfo)
	{
	if(!aSwTypeRegInfo.MimeTypes().Count())
	    {
	    DEBUG_PRINTF(_L("MIME types list cannot be empty!"));
	    User::Leave(KErrArgument);
	    }
	
	RBuf8 buf;
	buf.CleanupClosePushL();
	ExternalizeRefObjectL(aSwTypeRegInfo, buf);
	      
	TIpcArgs swTypeArgs(&buf);
	User::LeaveIfError(CallSessionFunction(EAddSoftwareType, swTypeArgs));
 
	CleanupStack::PopAndDestroy();//buf, reginfo
	}

EXPORT_C void RSoftwareComponentRegistry::DeleteSoftwareTypeL(const TDesC& aUniqueSwTypeName, RPointerArray<HBufC>& aDeletedMimeTypes)
	{
	DEBUG_PRINTF2(_L("Sending a request to delete the software type (%S)."), &aUniqueSwTypeName);
	TIpcArgs args(&aUniqueSwTypeName);
	TInt argNum = 1;
	GetObjectArrayL(*this, EDeleteSoftwareType, EGetDeletedMimeTypes, argNum, args, aDeletedMimeTypes);
	// Please note that EDeleteSoftwareType IPC function returns the size of the deleted MIME types list.
	}

EXPORT_C TBool RSoftwareComponentRegistry::IsComponentOrphanedL(TComponentId aComponentId)
	{
	DEBUG_PRINTF2(_L("Sending a request to get whether the component (%d) is orphaned."), aComponentId);
	TBool result;
	TPckg<TBool> isComponentOrphaned(result);
	User::LeaveIfError(CallSessionFunction(EGetIsComponentOrphaned, TIpcArgs(aComponentId, &isComponentOrphaned)));	
	return result;
	}

EXPORT_C void RSoftwareComponentRegistry::RetrieveLogEntriesL(RPointerArray<CScrLogEntry>& aLogEntries, const TDesC* aUniqueSwTypeName) const
	{
	DEBUG_PRINTF(_L("Sending a request to retrieve log entries."));
	// Retrieve the RFs and RFile handles from the server
	TInt fsh;          	 // File server handle (RFs - session)
	TPckgBuf<TInt> fh;   // File handle (RFile - subsession)
			
	fsh = CallSessionFunction(EGetLogFileHandle, TIpcArgs(&fh));   // pointer to fh in slot 0
	User::LeaveIfError(fsh);
		
	// Adopt the file using the returned handles
	RFile logFile;
	User::LeaveIfError(logFile.AdoptFromServer(fsh, fh()));
	CleanupClosePushL(logFile);
	
	RFileReadStream logStream(logFile);		 
	CleanupClosePushL(logStream);
	(void)logStream.ReadInt32L(); // skip the major version of the log file
	(void)logStream.ReadInt32L(); // skip the minor version of the log file
	TInt logCount = logStream.ReadInt32L();
	
	CScrLogEntry *log = NULL;
	
	for (TInt i = 0; i < logCount; ++i)
		{
		log = CScrLogEntry::NewLC(logStream);
		if(aUniqueSwTypeName && aUniqueSwTypeName->CompareF(log->SoftwareTypeName()))
			{ // if unique sw type name is given and the log doesn't belong to that sw type, continue.
			CleanupStack::PopAndDestroy(log);
			continue;
			}
		// Otherwise, append the log into the log entries list
		aLogEntries.AppendL(log);			
		CleanupStack::Pop(log); // Ownership is transferred 					
		} // for
	
	CleanupStack::PopAndDestroy(2, &logFile); // logFile, logStream
	}

TInt GetNextNumberFromVersionL(TLex& aVersion)
	{
	TInt number = KErrNotFound;
	if(aVersion.Eos())
		return number; // if the end of string has already been reached, return KErrNotFound.
	
	TChar c = aVersion.Peek(); // show the next character
	
	while(c != 0 && c != '.')
		{ // Go forward until coming across a dot or reaching the end of the string
		aVersion.Inc();
		c = aVersion.Peek();	
		}
	
	if(!aVersion.TokenLength())
		{// unexpected format
		DEBUG_PRINTF(_L8("Version token length is unexpectedly zero."));
		User::Leave(KErrArgument);
		}
	
	// Get the token and convert it to a decimal number
	TInt err = TLex(aVersion.MarkedToken()).Val(number);
	if(KErrNone != err)
		{
		if(KErrGeneral == err)
			{ // Convert KErrGeneral (if nun-numeric chars exist in the token) to KErrArgument
			err = KErrArgument;
			}
		User::Leave(err);
		}
			
	if(c != 0)
		{ // if it is not the end of the string
		aVersion.Inc(); // Increment to get over the last dot
		aVersion.Mark(); // and mark the extraction position
		}
	return number;
	}

EXPORT_C TInt RSoftwareComponentRegistry::CompareVersionsL(const TDesC& aVersionLeft, const TDesC& aVersionRight)
	{
	DEBUG_PRINTF3(_L("Comparing version left (%S) to version right (%S)."), &aVersionLeft, &aVersionRight);
	__ASSERT_ALWAYS(aVersionLeft.Length()>0 && aVersionRight.Length()>0, User::Leave(KErrArgument));
	
	if(!aVersionLeft.Compare(aVersionRight))
		return 0; // if the version strings are identical, simply return zero (means equality)
	
	TLex lVer(aVersionLeft);
	TLex rVer(aVersionRight);
	
	TInt lNum = GetNextNumberFromVersionL(lVer);
	TInt rNum = GetNextNumberFromVersionL(rVer);
	
	while(lNum != KErrNotFound && rNum != KErrNotFound)
		{
		TInt diff = lNum - rNum;
		if(diff != 0)
			return diff;
		
		lNum = GetNextNumberFromVersionL(lVer);
		rNum = GetNextNumberFromVersionL(rVer);
		}
	
	TInt lVal (0);
	TInt rVal (0);
	
	// The following two while-loops are used to consider trailing zeros.
	// Theoritically, 1.2.0 is equal to 1.2, however, 1.2.0.1 is greater than 1.2.
	// In order to handle these conditions the extra parts of the version are sum up
	// to see whether it is greater than zero.
	while(lNum != KErrNotFound)
		{
		lVal += lNum;
		lNum = GetNextNumberFromVersionL(lVer);
		}
	
	while(rNum != KErrNotFound)
		{
		rVal += rNum;
		rNum = GetNextNumberFromVersionL(rVer);
		}
	
	return (lVal - rVal);
	}

EXPORT_C TBool RSoftwareComponentRegistry::IsComponentOnReadOnlyDriveL(TComponentId aComponentId) const
	{
	DEBUG_PRINTF2(_L("Sending a request to determine if the component (%d) is on read-only drive."), aComponentId);
	
	TBool result;
	TPckg<TBool> isOnReadOnlyDrive(result);
	User::LeaveIfError(CallSessionFunction(EGetIsComponentOnReadOnlyDrive, TIpcArgs(aComponentId, &isOnReadOnlyDrive)));	
	return result;
	}

EXPORT_C TBool RSoftwareComponentRegistry::IsComponentPresentL(TComponentId aComponentId) const
	{
	DEBUG_PRINTF2(_L("Sending a request to determine if the component(%d) is fully present."), aComponentId);	
	TBool result;
	TPckg<TBool> isCompPresent(result);
	User::LeaveIfError(CallSessionFunction(EGetIsComponentPresent, TIpcArgs(aComponentId, &isCompPresent)));	
	return result;
	}

EXPORT_C void RSoftwareComponentRegistry::SetIsComponentPresentL(TComponentId aComponentId, TBool aValue)
	{
	DEBUG_PRINTF3(_L("Sending a request to set the CompPresence attribute(%d) of the component(%d)."), aValue, aComponentId);
	TIpcArgs args(aComponentId, aValue);
	User::LeaveIfError(CallSessionFunction(ESetIsComponentPresent, args));
	}

EXPORT_C void RSoftwareComponentRegistry::GetComponentSupportedLocalesListL(TComponentId aComponentId,RArray<TLanguage>& aMatchingSupportedLanguages) const
	{
	TIpcArgs args(aComponentId);
	TInt argNum = 1;
	GetObjectArrayL(*this, EGetComponentSupportedLocalesListSize, EGetComponentSupportedLocalesListData, argNum, args, aMatchingSupportedLanguages);
	}


EXPORT_C void RSoftwareComponentRegistry::AddApplicationEntryL(const TComponentId aComponentId, const CApplicationRegistrationData& aApplicationRegistrationData) 
    {
    DEBUG_PRINTF2(_L("Sending a request to add an application to be associated with component (%d) from SCR"), aComponentId);

    if(aApplicationRegistrationData.AppUid() == KNullUid || !((aApplicationRegistrationData.AppFile()).CompareF(KNullDesC())) ) 
        {
        DEBUG_PRINTF(_L8("AppUid is zero or the App filename is absent. In either of the above cases values cannot be inserted into the db"));
        User::Leave(KErrArgument);
        }    
    RBuf8 buf;
    buf.CleanupClosePushL();
    ExternalizeObjectL(&aApplicationRegistrationData, buf);
      
    TIpcArgs args(aComponentId, &buf);
    User::LeaveIfError(CallSessionFunction(EAddApplicationEntry, args));
    CleanupStack::PopAndDestroy(&buf);
    }

EXPORT_C void RSoftwareComponentRegistry::DeleteApplicationEntriesL(TComponentId aComponentId) 
    {
    TIpcArgs args(aComponentId);
    DEBUG_PRINTF2(_L("Sending a request to delete the applications associated with component (%d) from SCR"), aComponentId);
    User::LeaveIfError(CallSessionFunction(EDeleteApplicationEntries, args));
    }

EXPORT_C void RSoftwareComponentRegistry::DeleteApplicationEntryL(TUid aApplicationUid) 
    {
    TIpcArgs args(aApplicationUid.iUid);
    DEBUG_PRINTF2(_L("Sending a request to delete an application 0x%x from SCR" ), aApplicationUid.iUid);
    User::LeaveIfError(CallSessionFunction(EDeleteApplicationEntry, args));
    }

EXPORT_C TComponentId RSoftwareComponentRegistry::GetComponentIdForAppL(TUid aAppUid) const
    {
    DEBUG_PRINTF2(_L("Sending a request to retrieve the component id for AppUid(%d)."), aAppUid);
    TPckg<TUid> appUidPckg(aAppUid);
    TComponentId compId;
    TPckg<TComponentId> compIdPckg(compId);
    TIpcArgs args(&appUidPckg, &compIdPckg);
    User::LeaveIfError(CallSessionFunction(EGetComponentIdForApp, args));
    return compId;
    }

EXPORT_C void RSoftwareComponentRegistry::GetAppUidsForComponentL(TComponentId aCompId, RArray<TUid>& aAppUids) const
    {
    DEBUG_PRINTF2(_L("Sending a request to retrieve the list of apps associated with component(%d)."), aCompId);
    TIpcArgs args(aCompId);
    TInt argNum = 1; // size descriptor will be added to this slot
    GetObjectArrayL(*this, EGetAppUidsForComponentSize, EGetAppUidsForComponentData, argNum, args, aAppUids);    
    }

EXPORT_C void RSoftwareComponentRegistry::GetApplicationLaunchersL(RPointerArray<CLauncherExecutable>& aLaunchers) const
	{
    DEBUG_PRINTF(_L("Getting the list of applicaiton launchers."));
    TIpcArgs args;
    TInt argNum = 1;
    GetObjectArrayL(*this, EGetApplicationLaunchersSize, EGetApplicationLaunchersData, argNum, args, aLaunchers);
	}

/*
 * RScrInternalClient Implementation.
 */
EXPORT_C RScrInternalClient::RScrInternalClient()
    :   RScsClientBase()
    {
    // empty
    }

EXPORT_C TInt RScrInternalClient::Connect()
    {
    DEBUG_PRINTF2(_L("Connecting to %S using the internal client(RScrInternalClient)."), &KSoftwareComponentRegistryName);
    TVersion version = ScrServerVersion();
    TUidType scrFullUid = ScrServerImageFullUid();
    
    return RScsClientBase::Connect(KSoftwareComponentRegistryName(), version, KScrServerImageName(), scrFullUid);
    }

EXPORT_C void RScrInternalClient::Close()
    {
    DEBUG_PRINTF2(_L("Closing connection to %S using the internal client(RScrInternalClient)."), &KSoftwareComponentRegistryName);
    RScsClientBase::Close();
    }


EXPORT_C TUid RScrInternalClient::GenerateNewAppUidL() const
    {
    DEBUG_PRINTF(_L("Sending a request to generate a new Application Uid for non native applications."));

    TUid newUid = TUid::Null();
    TPckg<TUid> appUidPckg(newUid);
    
    TIpcArgs args(&appUidPckg);
    User::LeaveIfError(CallSessionFunction(EGenerateNonNativeAppUid, args));
    return newUid;
    }

