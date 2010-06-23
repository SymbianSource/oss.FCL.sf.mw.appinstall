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

#include "cleanuputils.h"
#include "msisuihandlers.h"
#include "uissstream.h"

#include "sisuihandler.h"
#include "uisssession.h"
#include "writestream.h"

namespace Swi
{

EXPORT_C CDisplayApplicationsInUse* CDisplayApplicationsInUse::NewL(const TAppInfo& aAppInfo,
										const RPointerArray<TDesC>& aAppNames)
	{
	CDisplayApplicationsInUse* self=NewLC(aAppInfo, aAppNames);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDisplayApplicationsInUse* CDisplayApplicationsInUse::NewLC(const TAppInfo& aAppInfo,
										const RPointerArray<TDesC>& aAppNames)
	{
	CDisplayApplicationsInUse* self=new(ELeave)CDisplayApplicationsInUse(aAppInfo, aAppNames);
	CleanupStack::PushL(self);
	return self;
	}

CDisplayApplicationsInUse::CDisplayApplicationsInUse(const TAppInfo& aAppInfo,
										const RPointerArray<TDesC>& aAppNames)
	: CUissCmd(CUissSession::KMessageApplicationsInUseDialog), iAppInfo(aAppInfo)
	, iAppNames(aAppNames)
	, iResult(EFalse), iResultPackage(iResult)
	{
	}

void CDisplayApplicationsInUse::MarshallArgumentsL(TIpcArgs& aArguments)
	{
	*iStream << iAppInfo;
	StreamOutArrayL(iAppNames,*iStream);
		
	iIpcInputPtr.Set(iStream->Ptr());
	aArguments.Set(0, &iIpcInputPtr);
	aArguments.Set(1, &iResultPackage);
	}

EXPORT_C TBool CDisplayApplicationsInUse::ReturnResult() const
	{
	return iResult;
	}

} // namespace Swi
