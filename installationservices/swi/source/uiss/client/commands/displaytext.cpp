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

#include "sisuihandler.h"
#include "uisssession.h"
#include "writestream.h"

namespace Swi
{

EXPORT_C CDisplayText* CDisplayText::NewL(const TAppInfo& aAppInfo, 
				    TFileTextOption aOption,
				    const TDesC8& aText)
	{
	CDisplayText* self=NewLC(aAppInfo, aOption, aText);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDisplayText* CDisplayText::NewLC(const TAppInfo& aAppInfo, 
				    TFileTextOption aOption,
				    const TDesC8& aText)
	{
	CDisplayText* self=new(ELeave)CDisplayText(aAppInfo, aOption, aText);
	CleanupStack::PushL(self);
	return self;
	}

CDisplayText::CDisplayText(const TAppInfo& aAppInfo, 
				    TFileTextOption aOption,
				    const TDesC8& aText)
	: CUissCmd(CUissSession::KMessageTextDialog), iAppInfo(aAppInfo)
	, iOption(aOption), iText(aText)
	, iResult(EFalse), iResultPackage(iResult)
	{
	}

void CDisplayText::MarshallArgumentsL(TIpcArgs& aArguments)
	{
	*iStream << iAppInfo;
	*iStream << TPckgC<TFileTextOption>(iOption);
	*iStream << iText;
	
	iIpcInputPtr.Set(iStream->Ptr());
	aArguments.Set(0, &iIpcInputPtr);
	aArguments.Set(1, &iResultPackage);
	}

EXPORT_C TBool CDisplayText::ReturnResult() const
	{
	return iResult;
	}

} // namespace Swi
