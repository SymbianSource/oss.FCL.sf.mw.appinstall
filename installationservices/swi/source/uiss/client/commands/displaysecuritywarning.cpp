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
#include <pkixvalidationresult.h> 

#include "msisuihandlers.h"
#include "cleanuputils.h"
#include "uissstream.h"

#include "sisuihandler.h"
#include "uisssession.h"
#include "writestream.h"

namespace Swi
{

EXPORT_C CDisplaySecurityWarning* CDisplaySecurityWarning::NewL(
	const TAppInfo& aAppInfo,
	TSignatureValidationResult aSigValidationResult,
	RPointerArray<CPKIXValidationResultBase>& aPkixResults,
	RPointerArray<CCertificateInfo>& aCertificates,
	TBool aInstallAnyway)
	{
	CDisplaySecurityWarning* self=NewLC(aAppInfo, aSigValidationResult, 
		aPkixResults, aCertificates, aInstallAnyway);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDisplaySecurityWarning* CDisplaySecurityWarning::NewLC(
	const TAppInfo& aAppInfo,
	TSignatureValidationResult aSigValidationResult,
	RPointerArray<CPKIXValidationResultBase>& aPkixResults,
	RPointerArray<CCertificateInfo>& aCertificates,
	TBool aInstallAnyway)
	{
	CDisplaySecurityWarning* self=new(ELeave)CDisplaySecurityWarning(aAppInfo, 
		aSigValidationResult, aPkixResults, aCertificates, aInstallAnyway);
	CleanupStack::PushL(self);
	return self;
	}

CDisplaySecurityWarning::CDisplaySecurityWarning(const TAppInfo& aAppInfo,
	TSignatureValidationResult aSigValidationResult,
	RPointerArray<CPKIXValidationResultBase>& aPkixResults,
	RPointerArray<CCertificateInfo>& aCertificates,
	TBool aInstallAnyway)
	: CUissCmd(CUissSession::KMessageSecurityWarningDialog)
	, iAppInfo(aAppInfo)
	, iSigValidationResult(aSigValidationResult)
	, iPkixResults(aPkixResults) 
	, iCertificates(aCertificates)
	, iInstallAnyway(aInstallAnyway)
	, iResult(EFalse)
	, iResultPackage(iResult)
	{
	}

void CDisplaySecurityWarning::MarshallArgumentsL(TIpcArgs& aArguments)
	{
	*iStream << iAppInfo;
	*iStream << TPckgC<TSignatureValidationResult>(iSigValidationResult);

	StreamOutArrayL(iPkixResults, *iStream);
	StreamOutArrayL(iCertificates, *iStream);

	*iStream << TPckgC<TBool>(iInstallAnyway);
	
	iIpcInputPtr.Set(iStream->Ptr());
	aArguments.Set(0, &iIpcInputPtr);
	aArguments.Set(1, &iResultPackage);
	}

EXPORT_C TBool CDisplaySecurityWarning::ReturnResult() const
	{
	return iResult;
	}

} // namespace Swi
