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

#include "cleanuputils.h"

#include "sisuihandler.h"
#include "uisssession.h"
#include "writestream.h"
#include "uissstream.h"

namespace Swi
{

EXPORT_C CDisplayOptions* CDisplayOptions::NewL(const TAppInfo& aAppInfo,
				      const RPointerArray<TDesC>& aOptions,
				      RArray<TBool>& aSelections)
	{
	CDisplayOptions* self=NewLC(aAppInfo, aOptions, aSelections);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDisplayOptions* CDisplayOptions::NewLC(const TAppInfo& aAppInfo,
				      const RPointerArray<TDesC>& aOptions,
				      RArray<TBool>& aSelections)
	{
	CDisplayOptions* self=new(ELeave)CDisplayOptions(aAppInfo, aOptions, aSelections);
	CleanupStack::PushL(self);
	return self;
	}

CDisplayOptions::CDisplayOptions(const TAppInfo& aAppInfo,
				      const RPointerArray<TDesC>& aOptions,
				      RArray<TBool>& aSelections)
	: CUissCmd(CUissSession::KMessageOptionsDialog), iAppInfo(aAppInfo)
	, iOptions(aOptions), iSelections(aSelections)
	, iResult(EFalse), iResultPackage(iResult)
	{
	}
	
CDisplayOptions::~CDisplayOptions()
	{
	delete iSelectionsBuffer;
	delete iSelectionsBufferPtr;
	}

void CDisplayOptions::MarshallArgumentsL(TIpcArgs& aArguments)
	{
	*iStream << iAppInfo;
	StreamOutArrayL(iOptions, *iStream);
	
	iSelectionsBuffer=HBufC8::NewL(sizeof(TInt32) + sizeof(TInt32) + iOptions.Count() * sizeof(TBool));
	iSelectionsBufferPtr= new(ELeave) TPtr8(iSelectionsBuffer->Des());

	iIpcInputPtr.Set(iStream->Ptr());
	aArguments.Set(0, &iIpcInputPtr);
	aArguments.Set(1,static_cast<TDes8*>(iSelectionsBufferPtr));
	}

void CDisplayOptions::UnmarshallArgumentsL()
	{
	RDesReadStream readStream(*iSelectionsBuffer);
	CleanupClosePushL(readStream);

	iResult=readStream.ReadInt32L();
	StreamInArrayL(iSelections, readStream);

	CleanupStack::PopAndDestroy(&readStream);
	}

EXPORT_C TBool CDisplayOptions::ReturnResult() const
	{
	return iResult;
	}

} // namespace Swi
