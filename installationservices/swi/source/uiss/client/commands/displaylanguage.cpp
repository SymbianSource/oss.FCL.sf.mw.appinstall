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

EXPORT_C CDisplayLanguage* CDisplayLanguage::NewL(const TAppInfo& aAppInfo, 
				      const RArray<TLanguage>& aLanguages)
	{
	CDisplayLanguage* self=NewLC(aAppInfo, aLanguages);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDisplayLanguage* CDisplayLanguage::NewLC(const TAppInfo& aAppInfo, 
				      const RArray<TLanguage>& aLanguages)
	{
	CDisplayLanguage* self=new(ELeave)CDisplayLanguage(aAppInfo, aLanguages);
	CleanupStack::PushL(self);
	return self;
	}

CDisplayLanguage::CDisplayLanguage(const TAppInfo& aAppInfo, 
				      const RArray<TLanguage>& aLanguages)
	: CUissCmd(CUissSession::KMessageLanguageDialog), iAppInfo(aAppInfo)
	, iLanguages(aLanguages)
	, iResult(KErrGeneral), iResultPackage(iResult)
	{
	}

void CDisplayLanguage::MarshallArgumentsL(TIpcArgs& aArguments)
	{
	*iStream << iAppInfo;
	StreamOutArrayL(iLanguages, *iStream);;
		
	iIpcInputPtr.Set(iStream->Ptr());
	aArguments.Set(0, &iIpcInputPtr);
	aArguments.Set(1,  &iResultPackage);
	}

EXPORT_C TInt CDisplayLanguage::ReturnResult() const
	{
	return iResult;
	}

} // namespace Swi
