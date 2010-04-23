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

EXPORT_C CDisplayError* CDisplayError::NewL(const TAppInfo& aAppInfo,
				   TErrorDialog aType, const TDesC& aParam)
	{
	CDisplayError* self=NewLC(aAppInfo, aType, aParam);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDisplayError* CDisplayError::NewLC(const TAppInfo& aAppInfo,
				   TErrorDialog aType, const TDesC& aParam)
	{
	CDisplayError* self=new(ELeave)CDisplayError(aAppInfo, aType, aParam);
	CleanupStack::PushL(self);
	return self;
	}

CDisplayError::CDisplayError(const TAppInfo& aAppInfo,
				   TErrorDialog aType, const TDesC& aParam)
	: CUissCmd(CUissSession::KMessageErrorDialog), iAppInfo(aAppInfo)
	, iType(aType), iParam(aParam)
	{
	}

void CDisplayError::MarshallArgumentsL(TIpcArgs& aArguments)
	{
	*iStream << iAppInfo;
	*iStream << TPckgC<TErrorDialog>(iType);
	*iStream << iParam;
	
	iIpcInputPtr.Set(iStream->Ptr());
	aArguments.Set(0, &iIpcInputPtr);
	}	


} // namespace Swi
