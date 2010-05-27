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

EXPORT_C CHandleInstallEvent* CHandleInstallEvent::NewL(const TAppInfo& aAppInfo, TInstallEvent aEvent, 
						 TInt aValue, const TDesC& aDes)
	{
	CHandleInstallEvent* self=NewLC(aAppInfo, aEvent, aValue, aDes);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CHandleInstallEvent* CHandleInstallEvent::NewLC(const TAppInfo& aAppInfo, TInstallEvent aEvent, 
						 TInt aValue, const TDesC& aDes)
	{
	CHandleInstallEvent* self=new(ELeave)CHandleInstallEvent(aAppInfo, aEvent, aValue, aDes);
	CleanupStack::PushL(self);
	return self;
	}

CHandleInstallEvent::CHandleInstallEvent(const TAppInfo& aAppInfo, TInstallEvent aEvent, 
						 TInt aValue, const TDesC& aDes)
	: CUissCmd(CUissSession::KMessageHandleInstallEvent), iAppInfo(aAppInfo)
	, iEvent(aEvent), iValue(aValue), iDes(aDes)
	, iResult(EFalse), iResultPackage(iResult)
	{
	}

void CHandleInstallEvent::MarshallArgumentsL(TIpcArgs& aArguments)
	{
	*iStream << iAppInfo;
	*iStream << TPckgC<TInstallEvent>(iEvent);
	iStream->Stream().WriteInt32L(iValue);
	*iStream << iDes;
	
	iIpcInputPtr.Set(iStream->Ptr());
	aArguments.Set(0, &iIpcInputPtr);
	aArguments.Set(1, &iResultPackage);
	}	

EXPORT_C TBool CHandleInstallEvent::ReturnResult() const
	{
	return iResult;
	}
	
} // namespace Swi
