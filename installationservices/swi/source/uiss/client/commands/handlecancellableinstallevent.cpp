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

EXPORT_C CHandleCancellableInstallEvent* CHandleCancellableInstallEvent::NewL(
	const TAppInfo& aAppInfo, TInstallCancellableEvent aEvent, TInt aValue, 
	const TDesC& aDes)
	{
	CHandleCancellableInstallEvent* self=NewLC(aAppInfo, aEvent, aValue, aDes);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CHandleCancellableInstallEvent* CHandleCancellableInstallEvent::NewLC(
	const TAppInfo& aAppInfo, TInstallCancellableEvent aEvent, TInt aValue, 
	const TDesC& aDes)
	{
	CHandleCancellableInstallEvent* self=
		new(ELeave)CHandleCancellableInstallEvent(aAppInfo, aEvent, aValue, 
			aDes);
	CleanupStack::PushL(self);
	return self;
	}

CHandleCancellableInstallEvent::CHandleCancellableInstallEvent(
	const TAppInfo& aAppInfo, TInstallCancellableEvent aEvent, TInt aValue, 
	const TDesC& aDes)
	: CUissCmd(CUissSession::KMessageHandleCancellableInstallEvent), 
	iAppInfo(aAppInfo), iEvent(aEvent), iValue(aValue), iDes(aDes)
	{
	}

void CHandleCancellableInstallEvent::MarshallArgumentsL(TIpcArgs& aArguments)
	{
	*iStream << iAppInfo;
	*iStream << TPckgC<TInstallCancellableEvent>(iEvent);
	iStream->Stream().WriteInt32L(iValue);
	*iStream << iDes;
	
	iIpcInputPtr.Set(iStream->Ptr());
	aArguments.Set(0, &iIpcInputPtr);
	}	

} // namespace Swi
