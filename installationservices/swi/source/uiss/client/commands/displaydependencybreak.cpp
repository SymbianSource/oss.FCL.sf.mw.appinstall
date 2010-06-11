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
#include "uissstream.h"
#include "cleanuputils.h"

#include "sisuihandler.h"
#include "uisssession.h"
#include "writestream.h"

namespace Swi
{

EXPORT_C CDisplayDependencyBreak* CDisplayDependencyBreak::NewL(const TAppInfo& aAppInfo,
					      const RPointerArray<TDesC>& aComponents)
	{
	CDisplayDependencyBreak* self=NewLC(aAppInfo, aComponents);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDisplayDependencyBreak* CDisplayDependencyBreak::NewLC(const TAppInfo& aAppInfo,
					      const RPointerArray<TDesC>& aComponents)
	{
	CDisplayDependencyBreak* self=new(ELeave)CDisplayDependencyBreak(aAppInfo, aComponents);
	CleanupStack::PushL(self);
	return self;
	}

CDisplayDependencyBreak::CDisplayDependencyBreak(const TAppInfo& aAppInfo,
					      const RPointerArray<TDesC>& aComponents)
	: CUissCmd(CUissSession::KMessageDependencyBreakDialog), iAppInfo(aAppInfo)
	, iComponents(aComponents)
	, iResult(EFalse), iResultPackage(iResult)
	{
	}

void CDisplayDependencyBreak::MarshallArgumentsL(TIpcArgs& aArguments)
	{
	*iStream << iAppInfo;
	StreamOutArrayL(iComponents, *iStream);
	
	iIpcInputPtr.Set(iStream->Ptr());
	aArguments.Set(0, &iIpcInputPtr);
	aArguments.Set(1, &iResultPackage);
	}

EXPORT_C TBool CDisplayDependencyBreak::ReturnResult() const
	{
	return iResult;
	}

} // namespace Swi
