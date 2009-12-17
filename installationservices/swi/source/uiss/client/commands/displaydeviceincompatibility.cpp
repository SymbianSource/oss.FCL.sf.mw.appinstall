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
#include "uissstream.h"

namespace Swi
{

EXPORT_C CDisplayDeviceIncompatibility* CDisplayDeviceIncompatibility::NewL(const TAppInfo& aAppInfo)
	{
	CDisplayDeviceIncompatibility* self=NewLC(aAppInfo);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDisplayDeviceIncompatibility* CDisplayDeviceIncompatibility::NewLC(const TAppInfo& aAppInfo)
	{
	CDisplayDeviceIncompatibility* self=new(ELeave)CDisplayDeviceIncompatibility(aAppInfo);
	CleanupStack::PushL(self);
	return self;
	}

CDisplayDeviceIncompatibility::CDisplayDeviceIncompatibility(const TAppInfo& aAppInfo)
	: CUissCmd(CUissSession::KMessageDeviceIncompatibility), iAppInfo(aAppInfo)
	, iResult(KErrGeneral), iResultPackage(iResult)
	{
	}

void CDisplayDeviceIncompatibility::MarshallArgumentsL(TIpcArgs& aArguments)
	{
	*iStream << iAppInfo;
		
	iIpcInputPtr.Set(iStream->Ptr());
	aArguments.Set(0, &iIpcInputPtr);
	aArguments.Set(1, &iResultPackage);
	}

EXPORT_C TInt CDisplayDeviceIncompatibility::ReturnResult() const
	{
	return iResult;
	}

} // namespace Swi
