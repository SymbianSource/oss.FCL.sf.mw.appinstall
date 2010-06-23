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


/**
 @file
 @publishedAll
 @released
*/

#include <e32base.h>
#include <s32mem.h>

#include "msisuihandlers.h"

#include "sisuihandler.h"
#include "uisssession.h"
#include "uissstream.h"
#include "writestream.h"

namespace Swi
{

EXPORT_C CDisplayOcspResult* CDisplayOcspResult::NewL(
	const TAppInfo& aAppInfo,
	TRevocationDialogMessage aMessage,
	RPointerArray<TOCSPOutcome>& aOutcomes, 
	RPointerArray<CCertificateInfo>& aCertificates, 
	TBool aWarningOnly)
	{
	CDisplayOcspResult* self=NewLC(aAppInfo, aMessage, aOutcomes, aCertificates, 
		aWarningOnly);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDisplayOcspResult* CDisplayOcspResult::NewLC(
	const TAppInfo& aAppInfo,
	TRevocationDialogMessage aMessage,
	RPointerArray<TOCSPOutcome>& aOutcomes, 
	RPointerArray<CCertificateInfo>& aCertificates, 
	TBool aWarningOnly)
	{
	CDisplayOcspResult* self=new(ELeave) CDisplayOcspResult(aAppInfo, aMessage, 
		aOutcomes, aCertificates, aWarningOnly);
	CleanupStack::PushL(self);
	return self;
	}

CDisplayOcspResult::CDisplayOcspResult(const TAppInfo& aAppInfo,
					 TRevocationDialogMessage aMessage,
					 RPointerArray<TOCSPOutcome>& aOutcomes, 
					 RPointerArray<CCertificateInfo>& aCertificates, 
					 TBool aWarningOnly)
	: CUissCmd(CUissSession::KMessageOcspResultDialog)
	, iAppInfo(aAppInfo)
	, iMessage(aMessage)
	, iOutcomes(aOutcomes)
	, iCertificates(aCertificates)
	, iWarningOnly(aWarningOnly)
	, iResult(EFalse)
	, iResultPackage(iResult)
	{
	}
	
void CDisplayOcspResult::MarshallArgumentsL(TIpcArgs& aArguments)
	{
	*iStream << iAppInfo;
	*iStream << TPckg<TRevocationDialogMessage>(iMessage);

	StreamOutArrayL(iOutcomes, *iStream);
	StreamOutArrayL(iCertificates, *iStream);
		
	*iStream << TPckgC<TBool>(iWarningOnly);

	iIpcInputPtr.Set(iStream->Ptr());
	aArguments.Set(0, &iIpcInputPtr);
	aArguments.Set(1, &iResultPackage);
	}

EXPORT_C TBool CDisplayOcspResult::ReturnResult() const
	{
	return iResult;
	}

} // namespace Swi
