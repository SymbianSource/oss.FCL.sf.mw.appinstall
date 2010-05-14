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

EXPORT_C CDisplayDrive* CDisplayDrive::NewL(const TAppInfo& aAppInfo, 
				   TInt64 aSize,
				   const RArray<TChar>& aDriveLetters,
				   const RArray<TInt64>& aDriveSpaces)
	{
	CDisplayDrive* self=NewLC(aAppInfo, aSize, aDriveLetters, aDriveSpaces);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDisplayDrive* CDisplayDrive::NewLC(const TAppInfo& aAppInfo, 
				   TInt64 aSize,
				   const RArray<TChar>& aDriveLetters,
				   const RArray<TInt64>& aDriveSpaces)
	{
	CDisplayDrive* self=new(ELeave)CDisplayDrive(aAppInfo, aSize, aDriveLetters, aDriveSpaces);
	CleanupStack::PushL(self);
	return self;
	}

CDisplayDrive::CDisplayDrive(const TAppInfo& aAppInfo, 
				   TInt64 aSize,
				   const RArray<TChar>& aDriveLetters,
				   const RArray<TInt64>& aDriveSpaces)
	: CUissCmd(CUissSession::KMessageDriveDialog), iAppInfo(aAppInfo)
	, iSize(aSize), iDriveLetters(aDriveLetters), iDriveSpaces(aDriveSpaces)
	, iResult(KErrGeneral), iResultPackage(iResult)
	{
	}

void CDisplayDrive::MarshallArgumentsL(TIpcArgs& aArguments)
	{
	if (iDriveLetters.Count() != iDriveSpaces.Count())
		{
		User::Leave(KErrArgument);
		}

	*iStream << iAppInfo;
	*iStream << iSize;
	StreamOutArrayL(iDriveLetters, *iStream);
	StreamOutArrayL(iDriveSpaces, *iStream);
	
	iIpcInputPtr.Set(iStream->Ptr());
	aArguments.Set(0, &iIpcInputPtr);
	aArguments.Set(1, &iResultPackage);
	}

EXPORT_C TInt CDisplayDrive::ReturnResult() const
	{
	return iResult;
	}

} // namespace Swi
