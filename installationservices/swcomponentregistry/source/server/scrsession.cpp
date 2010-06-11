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
* Implements the SCR session class which is server-side session object.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#include "scrserver.h"
#include "scrsubsession.h"
#include "scrrequestimpl.h"
#include "usiflog.h"
#include "usiferror.h"

using namespace Usif;

//
// CScrSession
//

CScrSession::CScrSession(CScrServer& aServer)
/**
	Intializes the SCR session object with the server handle.
 */
	:	CScsSession(aServer)
		{
		DEBUG_PRINTF(_L8("SCR session construction!"));
		}


CScrSession::~CScrSession()
/**
	Destructor for the SCR session object.
 */
	{
	DEBUG_PRINTF(_L8("SCR session destruction!"));
	CScrServer *server = static_cast<CScrServer*>(&iServer);
	// Take care to close all outstanding subsesions first, as we wouldn't be able to roll-back with open statements
	TRAP_IGNORE(CScsSession::CloseAllSubsessionsL());
	// Now roll-back transaction in case it wasn't committed or rolled back explicitly
	if(server->IsTransactionOwner(this))
		{
		TRAPD(err, server->RequestImpL()->RollbackTransactionL());
		if(KErrNone != err)
			{
			TRAP_IGNORE(server->ReconnectL());
			}
		server->SetTransactionOwner(NULL);
		}
	// Any error during writing log entries can be ignored as it is done in the destructor of SCR session
	// and error logs are already written inside the function.
	TRAP_IGNORE(server->RequestImpL()->FlushLogEntriesArrayL());
	}

	
CScrSession* CScrSession::NewL(CScrServer &aServer, const RMessage2& aMessage)
/**
	Factory function allocates new instance of CScrSession.
	
	@param aServer  SCR Server object.
	@param aMessage	Standard server-side handle to message. Not used.
	@return			Newly created instance of CScrSession which is owned by the caller.
 */
	{
	DEBUG_PRINTF2(_L("A new SCR session for the process(0x%x) is being created."), aMessage.SecureId().iId);
	(void)aMessage; // Happy the compiler in release mode
	CScrSession* self = new (ELeave) CScrSession(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CancelTransaction(TAny* aParam)
	{
	CScrServer *server = static_cast<CScrServer*>(aParam);
	TRAP_IGNORE(server->RequestImpL()->RollbackTransactionL());
	server->SetTransactionOwner(NULL);
	}

TBool CScrSession::DoServiceL(TInt aFunction, const RMessage2& aMessage)
/**
	Implement CScsSession by handling the supplied message.

	@param	aFunction		Function identifier without SCS code.
	@param	aMessage		Standard server-side handle to message.  Not used.
 */
	{
	TScrSessionFunction f = static_cast<TScrSessionFunction>(aFunction);
	CScrServer *server = static_cast<CScrServer*>(&iServer);
	
	TBool isImplicitTransaction = EFalse;	
	MutatingOperationsPreambleL(*server, f, isImplicitTransaction);
	if(isImplicitTransaction)
		{
		CleanupStack::PushL(TCleanupItem(CancelTransaction, server));
		}
	
	switch(f)
		{
		case ECreateTransaction:
			server->RequestImpL()->CreateTransactionL();
			server->SetTransactionOwner(this);
			break;
		case ERollbackTransaction:
			server->RequestImpL()->RollbackTransactionL();
			server->SetTransactionOwner(NULL);
			break;
		case ECommitTransaction:
			server->RequestImpL()->CommitTransactionL();
			server->SetTransactionOwner(NULL);
			break;
		case EAddComponent:
			server->RequestImpL()->AddComponentL(aMessage);
			break;
		case ESetComponentLocalizableProperty:
			server->RequestImpL()->SetComponentLocalizablePropertyL(aMessage);
			break;
		case ESetComponentNumericProperty:
			server->RequestImpL()->SetComponentIntPropertyL(aMessage);
			break;
		case ESetComponentBinaryProperty:
			server->RequestImpL()->SetComponentBinaryPropertyL(aMessage);
			break;			
		case ERegisterComponentFile:
			server->RequestImpL()->RegisterComponentFileL(aMessage);
			break;
		case ESetFileStringProperty:
			server->RequestImpL()->SetFileStrPropertyL(aMessage);
			break;
		case ESetFileNumericProperty:
			server->RequestImpL()->SetFileIntPropertyL(aMessage);
			break;
		case ESetComponentName:
			server->RequestImpL()->SetComponentNameL(aMessage);
			break;
		case ESetComponentVendor:
			server->RequestImpL()->SetVendorNameL(aMessage);
			break;
		case ESetComponentVersion:
			server->RequestImpL()->SetComponentVersionL(aMessage);
			break;
		case ESetIsComponentRemovable:
			server->RequestImpL()->SetIsComponentRemovableL(aMessage);
			break;
		case ESetIsComponentDrmProtected:
			server->RequestImpL()->SetIsComponentDrmProtectedL(aMessage);
			break;
		case ESetIsComponentHidden:
			server->RequestImpL()->SetIsComponentHiddenL(aMessage);
			break;
		case ESetIsComponentKnownRevoked:
			server->RequestImpL()->SetIsComponentKnownRevokedL(aMessage);
			break;
		case ESetIsComponentOriginVerified:
			server->RequestImpL()->SetIsComponentOriginVerifiedL(aMessage);
			break;
		case ESetComponentSize:
			server->RequestImpL()->SetComponentSizeL(aMessage);
			break;
		case EDeleteComponentProperty:
			server->RequestImpL()->DeleteComponentPropertyL(aMessage);
			break;
		case EDeleteFileProperty:
			server->RequestImpL()->DeleteFilePropertyL(aMessage);
			break;
		case EUnregisterComponentFile:
			server->RequestImpL()->UnregisterComponentFileL(aMessage);
			break;
		case EDeleteComponent:
			server->RequestImpL()->DeleteComponentL(aMessage);
			break;
		case EGetSingleComponentSize:
			server->RequestImpL()->GetComponentEntrySizeL(aMessage);
			break;
		case EGetSingleComponentData:
			server->RequestImpL()->GetComponentEntryDataL(aMessage);
			break;
		case EGetFilePropertiesSize:
			server->RequestImpL()->GetFilePropertiesSizeL(aMessage);
			break;
		case EGetFilePropertiesData:
			server->RequestImpL()->GetFilePropertiesDataL(aMessage);
			break;
		case EGetSingleFilePropertySize:
			server->RequestImpL()->GetSingleFilePropertySizeL(aMessage);
			break;
		case EGetSingleFilePropertyData:
			server->RequestImpL()->GetSingleFilePropertyDataL(aMessage);
			break;
		case EGetFileComponentsSize:
			server->RequestImpL()->GetFileComponentsSizeL(aMessage);
			break;
		case EGetFileComponentsData:
			server->RequestImpL()->GetFileComponentsDataL(aMessage);
			break;
		case EGetComponentPropertiesSize:
			server->RequestImpL()->GetComponentPropertiesSizeL(aMessage);
			break;
		case EGetComponentPropertiesData:
			server->RequestImpL()->GetComponentPropertiesDataL(aMessage);
			break;
		case EGetComponentSinglePropertySize:
			server->RequestImpL()->GetComponentSinglePropertySizeL(aMessage);
			break;
		case EGetComponentSinglePropertyData:
			server->RequestImpL()->GetComponentSinglePropertyDataL(aMessage);
			break;
		case EGetIsMediaPresent:
			server->RequestImpL()->GetIsMediaPresentL(aMessage);
			break;
		case ESetScomoState:
			server->RequestImpL()->SetScomoStateL(aMessage);
			break;
		case EGetPluginUidWithMimeType:
			server->RequestImpL()->GetPluginUidWithMimeTypeL(aMessage);
			break;
		case EGetPluginUidWithComponentId:
			server->RequestImpL()->GetPluginUidWithComponentIdL(aMessage);
			break;
		case EGetComponentIdListSize:
			server->RequestImpL()->GetComponentIdListSizeL(aMessage);
			break;
		case EGetComponentIdListData:
			server->RequestImpL()->GetComponentIdListDataL(aMessage);
			break;
		case EGetComponentFilesCount:
			server->RequestImpL()->GetComponentFilesCountL(aMessage);
			break;
		case EAddComponentDependency:
			server->RequestImpL()->AddComponentDependencyL(aMessage);
			break;
		case EDeleteComponentDependency:
			server->RequestImpL()->DeleteComponentDependencyL(aMessage);
			break;
		case EGetLocalComponentId:
			server->RequestImpL()->GetComponentIdL(aMessage);
			break;
		case EGetComponentWithGlobalIdSize:
			server->RequestImpL()->GetComponentWithGlobalIdSizeL(aMessage);
			break;
		case EGetComponentWithGlobalIdData:
			server->RequestImpL()->GetComponentWithGlobalIdDataL(aMessage);
			break;
		case EGetSupplierComponentsSize:
			server->RequestImpL()->GetSupplierComponentsSizeL(aMessage);
			break;
		case EGetSupplierComponentsData:
			server->RequestImpL()->GetSupplierComponentsDataL(aMessage);
			break;
		case EGetDependantComponentsSize:
			server->RequestImpL()->GetDependantComponentsSizeL(aMessage);
			break;
		case EGetDependantComponentsData:
			server->RequestImpL()->GetDependantComponentsDataL(aMessage);
			break;
		case EAddSoftwareType:
			server->RequestImpL()->AddSoftwareTypeL(aMessage);
			break;
		case EDeleteSoftwareType:
			server->RequestImpL()->DeleteSoftwareTypeL(aMessage);
			break;
		case EGetDeletedMimeTypes:
			server->RequestImpL()->GetDeletedMimeTypesL(aMessage);
			break;
		case EGetIsComponentOrphaned:
			server->RequestImpL()->GetIsComponentOrphanedL(aMessage);
			break;
		case EGetLogFileHandle:
			server->RequestImpL()->GetLogFileHandleL(aMessage);
			//RMessage2 object is closed by both TransferToClient() and SCS framework.
			//return EFalse to prevent SCS to close the message object.
			return EFalse;
		case EGetIsComponentOnReadOnlyDrive:
			server->RequestImpL()->GetIsComponentOnReadOnlyDriveL(aMessage);
			break;
		case EGetIsComponentPresent:
			server->RequestImpL()->GetIsComponentPresentL(aMessage);
			break;
		case ESetIsComponentPresent:
			server->RequestImpL()->SetIsComponentPresentL(aMessage);
			break;
		case EGetComponentSupportedLocalesListSize:
			server->RequestImpL()->GetComponentSupportedLocalesListSizeL(aMessage);
			break;
		case EGetComponentSupportedLocalesListData:
			server->RequestImpL()->GetComponentSupportedLocalesListDataL(aMessage);
			break;
		case EGetLocalizedComponentSize:
		    server->RequestImpL()->GetComponentLocalizedEntrySizeL(aMessage);
		    break;
		case EGetLocalizedComponentData:
		    server->RequestImpL()->GetComponentLocalizedEntryDataL(aMessage);
		    break;
		case EAddApplicationEntry:
		    server->RequestImpL()->AddApplicationEntryL(aMessage);
		    break;
		case EDeleteApplicationEntries:
		    server->RequestImpL()->DeleteAllAppsWithinPackageL(aMessage);
		    break;
		case EDeleteApplicationEntry:
		    server->RequestImpL()->DeleteApplicationEntryL(aMessage);
		    break;
        case EGetComponentIdForApp:
            server->RequestImpL()->GetComponentIdForAppL(aMessage);
            break;
        case EGetAppUidsForComponentSize:
            server->RequestImpL()->GetAppUidsForComponentSizeL(aMessage);
            break;
        case EGetAppUidsForComponentData:
            server->RequestImpL()->GetAppUidsForComponentDataL(aMessage);
            break;
        case EGenerateNonNativeAppUid:
            server->RequestImpL()->GenerateNonNativeAppUidL(aMessage);
            break;
        case EGetApplicationLaunchersSize:
            server->RequestImpL()->GetApplicationLaunchersSizeL(aMessage);
            break;
        case EGetApplicationLaunchersData:
            server->RequestImpL()->GetApplicationLaunchersDataL(aMessage);
            break;          
		    
		default:
			User::Leave(KErrNotSupported);
			break;
		}//End of switch.
	
	if(isImplicitTransaction)
		{
		MutatingOperationsPostambleL(*server);
		CleanupStack::Pop(server);
		}
	
	return ETrue;
	}//End of function DoServiceL
	

void CScrSession::ApplySubsessionConstraintL(CScrServer& aServer)
	{// Check whether or not write operation is allowed. if a subsession is in progress
	 // a mutating operation is not allowed to make the subsession results consistent.
	if(aServer.SubsessionCount() > 0)
		{
		DEBUG_PRINTF(_L8("Subsession in progress. Database write access was rejected!"));
		User::Leave(KErrScrReadOperationInProgress);
		}
	}

void CScrSession::ApplyTransactionConstraintL(CScrServer& aServer, TBool& aCreateImplicitTransaction)
	{// Check whether a transaction is in progress and owned by this session.
	 // A mutating operation is not allowed if the transaction owned by another session.
	if(aServer.IsTransactionInProgress())
		{// A transaction is already in progress, check if it is owned by this session.
		if(!aServer.IsTransactionOwner(this))
			{
			DEBUG_PRINTF(_L8("Transaction in progress owned by another session. Database write access was rejected!"));
			User::Leave(KErrScrWriteOperationInProgress);
			}
		aCreateImplicitTransaction = EFalse;
		}
	else if(aCreateImplicitTransaction)
		{// No transaction is in progress. Start an implicit transaction to improve
		 // the performance of mutating APIs. The implicit transaction must be
		 // committed when the API request is completed successfully, or rolled back 
		 // if a failure occurs while performing the request.
		aServer.RequestImpL()->CreateTransactionL();
		aServer.SetTransactionOwner(this);
		}
	}

void CScrSession::MutatingOperationsPreambleL(CScrServer& aServer, TScrSessionFunction aFunction, TBool& aIsTransactionImplicit)
	{
	// In SCR server, there can be ONLY ONE active TRANSACTION. Concurrent and nested
	// transactions are not supported. A transaction can committed/cancelled only by 
	// its owning session. While a transaction in progress, other sessions can invoke
	// query APIs, but not mutating APIs. Only the owning session can invoke APIs altering
	// database data. Moreover, a reading session (subsession) cannot be created while a transaction
	// is in progress unless the transaction is owned by this session. A transaction cannot be created if a reading subsession exits.

	switch(aFunction)
		{
		case ECreateTransaction:
			{
			if(aServer.IsTransactionInProgress())
				{// A new transaction cannot be started if there is another in progress
				User::Leave(KErrScrWriteOperationInProgress);
				}			
			break;
			}
		case ERollbackTransaction:
		case ECommitTransaction:
			{
			if(!aServer.IsTransactionOwner(this))
				{
				DEBUG_PRINTF(_L8("There is no active transaction owned by this session!"));
				User::Leave(KErrScrNoActiveTransaction);
				}
			break;
			}
		case ESetComponentLocalizableProperty:
		case ESetComponentNumericProperty:
		case ESetComponentBinaryProperty:
		case ESetFileStringProperty:
		case ESetFileNumericProperty:
		case ESetComponentName:
		case ESetComponentVendor:
		case ESetComponentVersion:
		case ESetIsComponentRemovable:
		case ESetIsComponentDrmProtected:
		case ESetIsComponentHidden:
		case ESetIsComponentKnownRevoked:
		case ESetIsComponentOriginVerified:
		case ESetComponentSize:
		case EDeleteComponentProperty:
		case EDeleteFileProperty:
		case EAddComponentDependency:
		case EDeleteComponentDependency:
		case ESetIsComponentPresent:
			{ 			
			// These mutating operations consist of a single mutating database statement.
			// So, no need to create an implicit transaction
			TBool createImplicitTransaction = EFalse; 
			ApplyTransactionConstraintL(aServer,createImplicitTransaction);
			break;
			}
		case EAddComponent:
		case ERegisterComponentFile:
		case EUnregisterComponentFile:
		case EDeleteComponent:
		case EAddSoftwareType:
		case EDeleteSoftwareType:
		case EGetDeletedMimeTypes:
		case EAddApplicationEntry:
		case EDeleteApplicationEntries:
		case EDeleteApplicationEntry:
			{			
			// These mutating operations consist of two or more mutating database statements.
			// Therefore, an implicit transaction is begun if an explicit one doesn't exist.
			TBool createImplicitTransaction = ETrue; 
			ApplyTransactionConstraintL(aServer,createImplicitTransaction);
			aIsTransactionImplicit = createImplicitTransaction;
			break;
			}
		default:
			// Other operations do NOT require write permission. Do nothing.
			break;
		}
	}

void CScrSession::MutatingOperationsPostambleL(CScrServer& aServer)
	{
	// This function is called to commit the implicit transaction opened for a specific mutating request.
	aServer.RequestImpL()->CommitTransactionL();
	aServer.SetTransactionOwner(NULL);
	}

CScsSubsession* CScrSession::DoCreateSubsessionL(TInt aFunction, const RMessage2& /*aMessage*/)
/**
	Override CScsSession::DoCreateSubsessionL by allocating a new SCR subsession object.

	@param	aFunction		Function identifier without SCS code.
	@param	aMessage		Standard server-side handle to message.	
	@return					New, initialized instance of CScrSubsession, ownership is transferred.
 */
	{

	TScrSessionFunction f = static_cast<TScrSessionFunction>(aFunction);
    CScrServer *server = static_cast<CScrServer*>(&iServer);
    
	switch(f)
		{
	    // Allow read operation even if another transaction is in progress for these views.
		case ESubSessCreateComponentsView:
			return CComponentViewSubsession::NewL(*this);
        case ESubSessCreateAppRegistryView:
            return CAppRegistrySubsession::NewL(*this);
		}
		
	 // A read operation is disallowed for the following views incase a transaction from another session 
	 // is already in progress. 
    if(server->IsTransactionInProgress() && !server->IsTransactionOwner(this)) 
        {
        DEBUG_PRINTF(_L8("Transaction from another session is in progress. Subsession cannot be created!"));
        User::Leave(KErrScrWriteOperationInProgress);
        }   
    
	switch(f)
	    {  
		case ESubSessCreateFileList:
			return CFileListSubsession::NewL(*this);
		case ESubSessCreateAppInfoView:
		    return CAppInfoViewSubsession::NewL(*this);
		case ESubSessCreateAppRegInfo:
			return CApplicationRegInfoSubsession::NewL(*this);
		case ESubSessCreateRegInfoForApp:
		    return CRegInfoForApplicationSubsession::NewL(*this);
		default:
			User::Leave(KErrNotSupported);
			/*lint -unreachable */
			return 0;	// avoid compiler warning
		}
	}
