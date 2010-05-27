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

#include "sisuihandler.h"
#include "uisssession.h"
#include "writestream.h"

namespace Swi
{

EXPORT_C CDisplayQuestion* CDisplayQuestion::NewL(const TAppInfo& aAppInfo,
					 TQuestionDialog aQuestion, const TDesC& aDes)
	{
	CDisplayQuestion* self=NewLC(aAppInfo, aQuestion, aDes);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDisplayQuestion* CDisplayQuestion::NewLC(const TAppInfo& aAppInfo,
					 TQuestionDialog aQuestion, const TDesC& aDes)
	{
	CDisplayQuestion* self=new(ELeave)CDisplayQuestion(aAppInfo, aQuestion, aDes);
	CleanupStack::PushL(self);
	return self;
	}

CDisplayQuestion::CDisplayQuestion(const TAppInfo& aAppInfo,
					 TQuestionDialog aQuestion, const TDesC& aDes)
	: CUissCmd(CUissSession::KMessageQuestionDialog), iAppInfo(aAppInfo)
	, iQuestion(aQuestion), iDes(aDes)
	, iResult(EFalse), iResultPackage(iResult)
	{
	}

void CDisplayQuestion::MarshallArgumentsL(TIpcArgs& aArguments)
	{
	*iStream << iAppInfo;
	*iStream << TPckg<TQuestionDialog>(iQuestion);
	*iStream << iDes;
		
	iIpcInputPtr.Set(iStream->Ptr());
	aArguments.Set(0, &iIpcInputPtr);
	aArguments.Set(1, &iResultPackage);
	}

EXPORT_C TBool CDisplayQuestion::ReturnResult() const
	{
	return iResult;
	}

} // namespace Swi
