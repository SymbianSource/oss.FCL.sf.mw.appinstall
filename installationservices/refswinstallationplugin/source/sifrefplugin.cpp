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
* This file implements a SIF plug-in for the Reference Installer.
*
*/


#include "usiflog.h"
#include "sifrefplugin.h"
#include <usif/sif/sif.h>
#include <usif/sif/sifcommon.h>
#include <usif/usiferror.h>
#include <usif/sif/sifplugin.h>
#include <e32property.h>
#include <f32file.h>

using namespace Usif;

static const TInt KSifRefPluginImpId = 0x10286350;

static const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KSifRefPluginImpId, CSifRefPlugin::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

CSifRefPlugin* CSifRefPlugin::NewL()
	{
	DEBUG_PRINTF(_L8("Constructing CSifRefPlugin"));
	CSifRefPlugin* self = new (ELeave) CSifRefPlugin();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CSifRefPlugin::ConstructL()
	{
	iImpl = CSifRefPluginActiveImpl::NewL();
	}

CSifRefPlugin::~CSifRefPlugin()
	{
	delete iImpl;
	}

void CSifRefPlugin::CancelOperation()
	{
	DEBUG_PRINTF(_L8("Reference Installer plug-in - Cancel"));
	iImpl->Cancel();
	}

void CSifRefPlugin::GetComponentInfo(const TDesC& aFileName, const TSecurityContext& aSecurityContext,
										 CComponentInfo& aComponentInfo, TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Reference Installer plug-in - GetComponentInfo by filename"));
	iImpl->GetComponentInfo(aFileName, aSecurityContext, aComponentInfo, aStatus);
	}

void CSifRefPlugin::GetComponentInfo(RFile& aFileHandle, const TSecurityContext& aSecurityContext,
						 				CComponentInfo& aComponentInfo, TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Reference Installer plug-in - GetComponentInfo by file handle"));
	iImpl->GetComponentInfo(aFileHandle, aSecurityContext, aComponentInfo, aStatus);
	}

void CSifRefPlugin::Install(const TDesC& aFileName, const TSecurityContext& aSecurityContext,
								const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
								TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Reference Installer plug-in - Install by file name"));
	iImpl->Install(aFileName, aSecurityContext, aInputParams, aOutputParams, aStatus);
	}

void CSifRefPlugin::Install(RFile& aFileHandle, const TSecurityContext& aSecurityContext,
								const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
								TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Reference Installer plug-in - Install by file handle"));
	iImpl->Install(aFileHandle, aSecurityContext, aInputParams, aOutputParams, aStatus);
	}

void CSifRefPlugin::Uninstall(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
								const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams, TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Reference Installer plug-in - Uninstall"));
	iImpl->Uninstall(aComponentId, aSecurityContext, aInputParams, aOutputParams, aStatus);
	}

void CSifRefPlugin::Activate(TComponentId aComponentId, const TSecurityContext& aSecurityContext, TRequestStatus& aStatus)

	{
	DEBUG_PRINTF(_L8("Reference Installer plug-in - Activate"));
	iImpl->Activate(aComponentId, aSecurityContext, aStatus);
	}

void CSifRefPlugin::Deactivate(TComponentId aComponentId, const TSecurityContext& aSecurityContext, TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Reference Installer plug-in - Deactivate"));
	iImpl->Deactivate(aComponentId, aSecurityContext, aStatus);
	}

//------------------CSifRefPluginActiveImpl---------------------

CSifRefPluginActiveImpl* CSifRefPluginActiveImpl::NewL()
	{
	DEBUG_PRINTF(_L8("Reference Installer plug-in - Constructing CSifRefPluginActiveImpl"));
	CSifRefPluginActiveImpl* self = new (ELeave) CSifRefPluginActiveImpl();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(1, self);
	return self;
	}

void CSifRefPluginActiveImpl::ConstructL()
	{
	CActiveScheduler::Add(this);
	User::LeaveIfError(iTransporter.Connect());
	}

CSifRefPluginActiveImpl::~CSifRefPluginActiveImpl()
	{
	iTransporter.Close();
	}

void CSifRefPluginActiveImpl::RunL()
	{
	TInt res = iStatus.Int();
	DEBUG_PRINTF2(_L8("Reference Installer plug-in - Operation finished with result %d"), res);

	User::RequestComplete(iClientStatus, res);
	iClientStatus = NULL;
	}

void CSifRefPluginActiveImpl::DoCancel()
	{
	ASSERT(iClientStatus);
	
	iTransporter.CancelOperation();
	User::RequestComplete(iClientStatus, iStatus.Int());
	iClientStatus = NULL;
	}

TBool CSifRefPluginActiveImpl::CommonRequestPreamble(const TSecurityContext& aSecurityContext, const COpaqueNamedParams& aInputParams, 
				COpaqueNamedParams& aOutputParams, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iClientStatus = &aStatus;

	iInputParams = &aInputParams;
	iOutputParams = &aOutputParams;

	// Check required capabilities here
	TInt silent = EFalse;
	TRAP_IGNORE(aInputParams.GetIntByNameL(_L("Silent"), silent));
	if (silent)
		{
		if (!aSecurityContext.HasCapability(ECapabilityTrustedUI))
			{
			User::RequestComplete(iClientStatus, KErrPermissionDenied);
			return EFalse;
			}
		}

	return ETrue;
	}

void CSifRefPluginActiveImpl::GetComponentInfo(const TDesC& aFileName, const TSecurityContext& /*aSecurityContext*/,
												CComponentInfo& aComponentInfo, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iClientStatus = &aStatus;

	iTransporter.GetComponentInfo(aFileName, aComponentInfo, iStatus);
	
	SetActive();
	}

void CSifRefPluginActiveImpl::GetComponentInfo(RFile& aFileHandle, const TSecurityContext& /*aSecurityContext*/,
												CComponentInfo& aComponentInfo, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iClientStatus = &aStatus;

	iTransporter.GetComponentInfo(aFileHandle, aComponentInfo, iStatus);
	
	SetActive();
	}

void CSifRefPluginActiveImpl::Install(const TDesC& aFileName, const TSecurityContext& aSecurityContext,
										const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
										TRequestStatus& aStatus)
	{
	DEBUG_PRINTF2(_L("Reference Installer plug-in - install for %S"), &aFileName);

	if (CommonRequestPreamble(aSecurityContext, aInputParams, aOutputParams, aStatus))
		{
		iTransporter.Install(aFileName, aInputParams, aOutputParams, iStatus);

		SetActive();
		}
	}

void CSifRefPluginActiveImpl::Install(RFile& aFileHandle, const TSecurityContext& aSecurityContext,
										const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
										TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Reference Installer plug-in - install by file handle"));

	if (CommonRequestPreamble(aSecurityContext, aInputParams, aOutputParams, aStatus))
		{
		iTransporter.Install(aFileHandle, aInputParams, aOutputParams, iStatus);

		SetActive();
		}
	}

void CSifRefPluginActiveImpl::Uninstall(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
		  const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams, TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Reference Installer plug-in - uninstall"));

	if (CommonRequestPreamble(aSecurityContext, aInputParams, aOutputParams, aStatus))
		{
		iTransporter.Uninstall(aComponentId, aInputParams, aOutputParams, iStatus);

		SetActive();
		}
	}

void CSifRefPluginActiveImpl::Activate(TComponentId aComponentId, const TSecurityContext& /*aSecurityContext*/, TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Reference Installer plug-in - activate"));
	
	aStatus = KRequestPending;
	iClientStatus = &aStatus;

	iTransporter.Activate(aComponentId, iStatus);

	SetActive();
	}

void CSifRefPluginActiveImpl::Deactivate(TComponentId aComponentId, const TSecurityContext& /*aSecurityContext*/, TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Reference Installer plug-in - deactivate"));

	aStatus = KRequestPending;
	iClientStatus = &aStatus;

	iTransporter.Deactivate(aComponentId, iStatus);

	SetActive();
	}
