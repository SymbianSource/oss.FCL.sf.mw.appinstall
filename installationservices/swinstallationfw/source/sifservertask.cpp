/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file implements a SIF Transport task for the SIF Server.
*
*/


#include <e32base.h>
#include <e32std.h>
#include <usif/usiferror.h>
#include <usif/sif/sifplugin.h>
#include <usif/scr/scr.h>
#include <apgcli.h>
#include "sifservertask.h"
#include "siftransportcommon.h"
#include "usiflog.h"
#include "sifutils.h"
#include "e32property.h" 

using namespace Usif;

CSifServerTask* CSifServerTask::NewL(TransportTaskFactory::TTaskType aTaskType, TTransportTaskParams& aParams)
	{
	CSifServerTask* self = new (ELeave) CSifServerTask(aTaskType, aParams);
	CleanupStack::PushL(self);
   
	// Query AppArc about the (MIME) type of the package to be processed and instantiate an appropriate SIF plug-in
	self->SelectAndLoadPluginL();

	CleanupStack::Pop(self);
	return self;
	}

CSifServerTask::CSifServerTask(TransportTaskFactory::TTaskType aTaskType, TTransportTaskParams& aParams)
	: CSifTransportTask(aParams, EFalse), iTaskType(aTaskType)
	{
	}

CSifServerTask::~CSifServerTask()
	{
	DEBUG_PRINTF2(_L8("CSifServerTask::~CSifServerTask() for aFunction = %d\n"),iTaskType);
	                
	// Clean up the plugin
	delete iPlugin;
	iPlugin = NULL;
	REComSession::DestroyedImplementation(iEcomKey);
	REComSession::FinalClose();
	}

TBool CSifServerTask::ExecuteImplL()
	{
	DEBUG_PRINTF2(_L8("CSifServerTask::ExecuteImpl() for aRequest = %d\n"),iTaskType);
	
	// Plug-in does not exists and request is not for default uninstallation so leave.
	if (!iPlugin && iTaskType != TransportTaskFactory::EUninstall)
		User::Leave(KErrNotFound);

	// Hand over the software management request to the plug-in
	switch (iTaskType)
		{
		case TransportTaskFactory::EGetComponentInfo:
			{
			if (FileName())
				{
				iPlugin->GetComponentInfo(*FileName(), *SecurityContext(), *ComponentInfo(), *RequestStatus());
				}
			else
				{
				iPlugin->GetComponentInfo(*FileHandle(), *SecurityContext(), *ComponentInfo(), *RequestStatus());
				}
			break;
			}

		case TransportTaskFactory::EInstall:
			{
			if (FileName())
				{
				iPlugin->Install(*FileName(), *SecurityContext(), *CustomArguments(), *CustomResults(), *RequestStatus());
				}
			else
				{
				iPlugin->Install(*FileHandle(), *SecurityContext(), *CustomArguments(), *CustomResults(), *RequestStatus());
				}
			break;
			}

		case TransportTaskFactory::EUninstall:
			{
			if (!iPlugin)
				{
				// The default uninstaller is used when the Plugin is no longer present.
				UninstallL(ComponentId());
				TRequestStatus* requestStatusPtr(RequestStatus());
				User::RequestComplete(requestStatusPtr, KErrNone);
				}
			else
				{
				iPlugin->Uninstall(ComponentId(), *SecurityContext(), *CustomArguments(), *CustomResults(), *RequestStatus());
				}
			break;
			}

		case TransportTaskFactory::EActivate:
			{
			iPlugin->Activate(ComponentId(), *SecurityContext(), *RequestStatus());
			break;
			}

		case TransportTaskFactory::EDeactivate:
			{
			iPlugin->Deactivate(ComponentId(), *SecurityContext(), *RequestStatus());
			break;
			}

		default:
			ASSERT(0);
		}
	return ETrue;
	}

void CSifServerTask::CancelImpl()
	{
	DEBUG_PRINTF2(_L8("CSifServerTask::Cancel for aTaskType = %d\n"), iTaskType);
	ASSERT(iPlugin);
	iPlugin->CancelOperation();
	}

void CSifServerTask::SelectAndLoadPluginL()
	{
	DEBUG_PRINTF2(_L8("CSifServerTask::SelectAndLoadPluginL() for aFunction = %d\n"),iTaskType);

	// The code below assumes that each SIF request provides a component id xor a file handle xor a file name.
	// In case of changes that invalidate this assumption the code must be modified.

	TDataType dataType;
	if (iTaskType == TransportTaskFactory::EGetComponentInfo || iTaskType  == TransportTaskFactory::EInstall)
		{
		// Get the MIME type of the component to be installed from AppArc
		DEBUG_PRINTF(_L8("Getting the MIME type of the component to be installed from AppArc\n"));
		RApaLsSession apa;
		TInt err = apa.Connect();
		if (err != KErrNone)
			{
			DEBUG_PRINTF2(_L8("Failed to connect to the AppArc server, err = %d\n"),err);
			User::Leave(err);
			}
		CleanupClosePushL(apa);
		TUid appUid = TUid::Null();
		if (FileName())
			{
			err = apa.AppForDocument(*FileName(), appUid, dataType);
			}
		else
			{
			err = apa.AppForDocument(*FileHandle(), appUid, dataType);
			}
		// A possible problem with recognizers is returning a successful result, but forgetting to set the MIME type. The second condition below protects against that.
		if (err != KErrNone || dataType.Des8().Ptr() == NULL) 
			{
			DEBUG_PRINTF2(_L8("Failed to obtain the MIME type of a component, err = %d\n"),err);
			User::Leave(err);
			}
		CleanupStack::PopAndDestroy(&apa);
		}

	// Get the uid of the matching plug-in from SCR for ...
	RSoftwareComponentRegistry scr;
	TInt err = scr.Connect();
	if (err != KErrNone)
		{
		DEBUG_PRINTF2(_L8("Failed to connect to the SCR, err = %d\n"),err);
		User::Leave(err);
		}
	CleanupClosePushL(scr);

	//... iComponentId or the MIME type obtained above
	TUid pluginUid = TUid::Null();
	if (iTaskType != TransportTaskFactory::EInstall && iTaskType != TransportTaskFactory::EGetComponentInfo)
		{
		// Verify the given component id
		CComponentEntry* entry = CComponentEntry::NewLC();
		if (!scr.GetComponentL(ComponentId(), *entry))
			{
			DEBUG_PRINTF2(_L8("ComponentId = %d not found in the SCR"), ComponentId());
			User::Leave(KErrSifBadComponentId);
			}
		CleanupStack::PopAndDestroy(entry);
		
		// Get the PluginUid only when an Installer Plugin is present.
		if (!scr.IsComponentOrphanedL(ComponentId()))
			{
			pluginUid = scr.GetPluginUidL(ComponentId());
			}
		}
	else
		{
		pluginUid = scr.GetPluginUidL(dataType.Des());
		}
	CleanupStack::PopAndDestroy(&scr);

	// Load the plug-in
	if( pluginUid != TUid::Null() )
		{
		iPlugin = reinterpret_cast<CSifPlugin*>(REComSession::CreateImplementationL(pluginUid, iEcomKey));
		}
	}
