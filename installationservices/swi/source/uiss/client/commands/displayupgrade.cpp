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


#include <s32mem.h>

#include "msisuihandlers.h"
#include "uissstream.h"

#include "sisuihandler.h"
#include "uisssession.h"
#include "writestream.h"

namespace Swi
{

EXPORT_C CDisplayUpgrade* CDisplayUpgrade::NewL(const TAppInfo& aAppInfo, 
				      const TAppInfo& aExistingAppInfo)
	{
	CDisplayUpgrade* self=NewLC(aAppInfo, aExistingAppInfo);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDisplayUpgrade* CDisplayUpgrade::NewLC(const TAppInfo& aAppInfo, 
				      const TAppInfo& aExistingAppInfo)
	{
	CDisplayUpgrade* self=new(ELeave)CDisplayUpgrade(aAppInfo, aExistingAppInfo);
	CleanupStack::PushL(self);
	return self;
	}

CDisplayUpgrade::CDisplayUpgrade(const TAppInfo& aAppInfo, 
				      const TAppInfo& aExistingAppInfo)
	: CUissCmd(CUissSession::KMessageUpgradeDialog), iAppInfo(aAppInfo)
	, iExistingAppInfo(aExistingAppInfo)
	, iResult(EFalse), iResultPackage(iResult)
	{
	}

void CDisplayUpgrade::MarshallArgumentsL(TIpcArgs& aArguments)
	{
	*iStream << iAppInfo;
	*iStream << iExistingAppInfo;
		
	iIpcInputPtr.Set(iStream->Ptr());
	aArguments.Set(0, &iIpcInputPtr);
	aArguments.Set(1, &iResultPackage);	
	}

EXPORT_C TBool CDisplayUpgrade::ReturnResult() const
	{
	return iResult;
	}

} // namespace Swi
