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
 @test
*/

#include "scrpluginmanagement.h"

#include <usif/scr/scr.h>
#include <scs/cleanuputils.h>
#include <e32def.h>

_LIT_SECURE_ID(KSisRegistryServerSid, 0x10202DCA);
_LIT(KScrPluginManagementSecName, "SCR Plugin Management APIs test");

using namespace Usif;

CScrPluginManagementSecTest* CScrPluginManagementSecTest::NewL()
	{
	CScrPluginManagementSecTest* self=new(ELeave) CScrPluginManagementSecTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CScrPluginManagementSecTest::CScrPluginManagementSecTest()
	{
	SetSidRequired(KSisRegistryServerSid);
	}
	
void CScrPluginManagementSecTest::ConstructL()
	{
	SetNameL(KScrPluginManagementSecName);
	}

void CScrPluginManagementSecTest::RunTestL()
	{
	RSoftwareComponentRegistry scrSession;
	User::LeaveIfError(scrSession.Connect());
	CleanupClosePushL(scrSession);
	
	// The transaction APIs are tested separately. If this command fails, then we can proceed, as the following operations would fail also, and no transaction management would be required
	TRAP_IGNORE(scrSession.CreateTransactionL()); 
	
	TInt err(0);
	
	// First, add a new sofwtare type
	_LIT(KSwTypeName, "usifsecuritytestsoftwaretype");
	TUid sifPluginUid = {0xA01B7211};
	
	RArray<TCustomAccessInfo> sidArray;
	CleanupClosePushL(sidArray);
	
	TCustomAccessInfo id1(TSecureId(0xAAFFEE11), static_cast<TAccessMode>(1));
	TCustomAccessInfo id2(TSecureId(0xAAEEDD11), static_cast<TAccessMode>(1));

	sidArray.AppendL(id1);
	sidArray.AppendL(id2);
	
	_LIT(KMimeType, "usifsecuritytestmimetype");
	RPointerArray<HBufC> mimeTypes;
	CleanupResetAndDestroyPushL(mimeTypes);
	mimeTypes.Append(KMimeType().AllocL());
	
	_LIT(KLauncherExecutable, "usifsecuritytestLauncherExec");
	
	Usif::CSoftwareTypeRegInfo* swType = Usif::CSoftwareTypeRegInfo::NewL(KSwTypeName);
	CleanupStack::PushL(swType);
	swType->SetSifPluginUid(sifPluginUid);
	for(TInt i=0;i<sidArray.Count(); ++i)
		{
		swType->SetCustomAccessL(sidArray[i]);
		}
	for(TInt i=0;i<mimeTypes.Count(); ++i)
		{
		swType->SetMimeTypeL(mimeTypes[i]->Des());
		}

	swType->SetLauncherExecutableL(KLauncherExecutable);
	
	TRAP(err, scrSession.AddSoftwareTypeL(*swType));
	CheckFailL(err, _L("AddSoftwareTypeL"));
	CleanupStack::PopAndDestroy(3, &sidArray);
	
	// Then, delete the newly added sofwtare type. 
	RPointerArray<HBufC> deletedMimeTypes;
	CleanupResetAndDestroyPushL(deletedMimeTypes);
	TRAP(err, scrSession.DeleteSoftwareTypeL(KSwTypeName,deletedMimeTypes));
	CheckFailL(err, _L("DeleteSoftwareTypeL"));
	CleanupStack::PopAndDestroy(&deletedMimeTypes); 
		
	TRAP_IGNORE(scrSession.CommitTransactionL());
	CleanupStack::PopAndDestroy(&scrSession);
	}
