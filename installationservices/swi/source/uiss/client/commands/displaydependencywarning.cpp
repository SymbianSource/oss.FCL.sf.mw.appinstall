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
#include "uissstream.h"

namespace Swi
{

EXPORT_C CDisplayDependencyWarning* CDisplayDependencyWarning::NewL(const TAppInfo& aAppInfo, 
						const TDesC& aDependencyName,
						TBool aWantedFromValid, const TVersion& aWantedFrom,
						TBool aWantedToValid,   const TVersion& aWantedTo,
						TBool aAvailableValid,  const TVersion& aAvailable)
	{
	CDisplayDependencyWarning* self=new(ELeave)CDisplayDependencyWarning(aAppInfo, aDependencyName,
						aWantedFromValid, aWantedFrom,
						aWantedToValid, aWantedTo,
						aAvailableValid, aAvailable);
	return self;
	}

EXPORT_C CDisplayDependencyWarning* CDisplayDependencyWarning::NewLC(const TAppInfo& aAppInfo, 
						const TDesC& aDependencyName,
						TBool aWantedFromValid, const TVersion& aWantedFrom,
						TBool aWantedToValid,   const TVersion& aWantedTo,
						TBool aAvailableValid,  const TVersion& aAvailable)
	{
	CDisplayDependencyWarning* self=new(ELeave)CDisplayDependencyWarning(aAppInfo, aDependencyName,
						aWantedFromValid, aWantedFrom,
						aWantedToValid, aWantedTo,
						aAvailableValid, aAvailable);
	CleanupStack::PushL(self);
	return self;
	}

CDisplayDependencyWarning::CDisplayDependencyWarning(const TAppInfo& aAppInfo, 
						const TDesC& aDependencyName,
						TBool aWantedFromValid, const TVersion& aWantedFrom,
						TBool aWantedToValid,   const TVersion& aWantedTo,
						TBool aAvailableValid,  const TVersion& aAvailable)
	: CUissCmd(CUissSession::KMessageMissingDependency), iAppInfo(aAppInfo)
	, iDependencyName(aDependencyName)
	, iWantedFromValid(aWantedFromValid), iWantedFrom(aWantedFrom)
	, iWantedToValid(aWantedToValid), iWantedTo(aWantedTo)
	, iAvailableValid(aAvailableValid), iAvailable(aAvailable)
	, iResult(KErrGeneral), iResultPackage(iResult)
	{
	}

void CDisplayDependencyWarning::MarshallArgumentsL(TIpcArgs& aArguments)
	{
	*iStream << iAppInfo;
	*iStream << iDependencyName;
	*iStream << TPckg<TBool>(iWantedFromValid);
	if (iWantedFromValid)
		{
		*iStream << TPckg<TVersion>(iWantedFrom);
		}
	*iStream << TPckg<TBool>(iWantedToValid);
	if (iWantedToValid)
		{
		*iStream << TPckg<TVersion>(iWantedTo);
		}
	*iStream << TPckg<TBool>(iAvailableValid);
	if (iAvailableValid)
		{
		*iStream << TPckg<TVersion>(iAvailable);
		}

	iIpcInputPtr.Set(iStream->Ptr());
	aArguments.Set(0, &iIpcInputPtr);
	aArguments.Set(1, &iResultPackage);
	}

EXPORT_C TInt CDisplayDependencyWarning::ReturnResult() const
	{
	return iResult;
	}

} // namespace Swi
