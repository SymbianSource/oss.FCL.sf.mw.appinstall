/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

 
#include <e32base.h>
#include <s32mem.h>

#include "msisuihandlers.h"

#include "sisuihandler.h"
#include "uisssession.h"
#include "writestream.h"

namespace Swi
{

EXPORT_C CDisplayUninstall* CDisplayUninstall::NewL(const TAppInfo& aAppInfo)
	{
	CDisplayUninstall* self=NewLC(aAppInfo);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDisplayUninstall* CDisplayUninstall::NewLC(const TAppInfo& aAppInfo)
	{
	CDisplayUninstall* self=new(ELeave)CDisplayUninstall(aAppInfo);
	CleanupStack::PushL(self);
	return self;
	}

CDisplayUninstall::CDisplayUninstall(const TAppInfo& aAppInfo)
	: CUissCmd(CUissSession::KMessageUninstallDialog), iAppInfo(aAppInfo)
	, iResult(EFalse), iResultPackage(iResult)
	{
	}

void CDisplayUninstall::MarshallArgumentsL(TIpcArgs& aArguments)
	{
	*iStream << iAppInfo;

	iIpcInputPtr.Set(iStream->Ptr());
	aArguments.Set(0, &iIpcInputPtr);
	aArguments.Set(1, &iResultPackage);
	}

EXPORT_C TBool CDisplayUninstall::ReturnResult() const
	{
	return iResult;
	}

} // namespace Swi
