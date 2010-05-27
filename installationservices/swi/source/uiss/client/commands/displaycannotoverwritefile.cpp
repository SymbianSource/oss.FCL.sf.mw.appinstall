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
* cannotoverwritefiledialog.cpp
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

EXPORT_C CDisplayCannotOverwriteFile* CDisplayCannotOverwriteFile::NewL(const TAppInfo& aAppInfo,
						 const TAppInfo& aInstalledAppInfo,
						 const TDesC& aFileName)
	{
	CDisplayCannotOverwriteFile* self=NewLC(aAppInfo, aInstalledAppInfo, aFileName);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDisplayCannotOverwriteFile* CDisplayCannotOverwriteFile::NewLC(const TAppInfo& aAppInfo,
						 const TAppInfo& aInstalledAppInfo,
						 const TDesC& aFileName)
	{
	CDisplayCannotOverwriteFile* self=new(ELeave)CDisplayCannotOverwriteFile(aAppInfo, aInstalledAppInfo, aFileName);
	CleanupStack::PushL(self);
	return self;
	}

CDisplayCannotOverwriteFile::CDisplayCannotOverwriteFile(const TAppInfo& aAppInfo,
						 const TAppInfo& aInstalledAppInfo,
						 const TDesC& aFileName)
	: CUissCmd(CUissSession::KMessageCannotOverwriteFileDialog), iAppInfo(aAppInfo)
	, iInstalledAppInfo(aInstalledAppInfo), iFileName(aFileName)
	, iResult(EFalse), iResultPackage(iResult)
	{
	}

void CDisplayCannotOverwriteFile::MarshallArgumentsL(TIpcArgs& aArguments)
	{
	*iStream << iAppInfo;
	*iStream << iInstalledAppInfo;
	*iStream << iFileName;

	iIpcInputPtr.Set(iStream->Ptr());
	aArguments.Set(0, &iIpcInputPtr);
	aArguments.Set(1, &iResultPackage);
	}

EXPORT_C TBool CDisplayCannotOverwriteFile::ReturnResult() const
	{
	return iResult;
	}

} // namespace Swi
