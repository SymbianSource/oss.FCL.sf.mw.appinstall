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

EXPORT_C CDisplayGrantCapabilities* CDisplayGrantCapabilities::NewL(const TAppInfo& aAppInfo, 
						const TCapabilitySet& aCapabilitySet)
	{
	CDisplayGrantCapabilities* self=NewLC(aAppInfo, aCapabilitySet);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDisplayGrantCapabilities* CDisplayGrantCapabilities::NewLC(const TAppInfo& aAppInfo, 
						const TCapabilitySet& aCapabilitySet)
	{
	CDisplayGrantCapabilities* self=new(ELeave)CDisplayGrantCapabilities(aAppInfo, aCapabilitySet);
	CleanupStack::PushL(self);
	return self;
	}

CDisplayGrantCapabilities::CDisplayGrantCapabilities(const TAppInfo& aAppInfo, 
						const TCapabilitySet& aCapabilitySet)
	: CUissCmd(CUissSession::KMessageGrantCapabilitiesDialog), iAppInfo(aAppInfo)
	, iCapabilitySet(aCapabilitySet)
	, iResult(KErrGeneral), iResultPackage(iResult)
	{
	}

void CDisplayGrantCapabilities::MarshallArgumentsL(TIpcArgs& aArguments)
	{
	*iStream << iAppInfo;
	*iStream << TPckg<TCapabilitySet>(iCapabilitySet);
		
	iIpcInputPtr.Set(iStream->Ptr());
	aArguments.Set(0, &iIpcInputPtr);
	aArguments.Set(1, &iResultPackage);
	}

EXPORT_C TInt CDisplayGrantCapabilities::ReturnResult() const
	{
	return iResult;
	}

} // namespace Swi
