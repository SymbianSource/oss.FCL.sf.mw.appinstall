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
#include "sisfiledescription.h"

namespace Swi
{

using namespace Sis;
	
EXPORT_C CDisplayInstall* CDisplayInstall::NewL(const TAppInfo& aAppInfo,
				RFs& aFs, RFile* aLogoFile,
				const RPointerArray<CCertificateInfo>& aCertificates)
	{
	CDisplayInstall* self=NewLC(aAppInfo, aFs, aLogoFile, aCertificates);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDisplayInstall* CDisplayInstall::NewLC(const TAppInfo& aAppInfo,
				RFs& aFs, RFile* aLogoFile,
				const RPointerArray<CCertificateInfo>& aCertificates)
	{
	CDisplayInstall* self=new(ELeave) CDisplayInstall(aAppInfo, aFs, aLogoFile,
		aCertificates);
	CleanupStack::PushL(self);
	return self;
	}

CDisplayInstall::CDisplayInstall(const TAppInfo& aAppInfo,
				RFs& aFs, RFile* aLogoFile,
				const RPointerArray<CCertificateInfo>& aCertificates)
	: 
	CUissCmd(CUissSession::KMessageInstallDialog), 
	iAppInfo(aAppInfo), 
	iFs(aFs),
	iLogoFile(aLogoFile), 
	iCertificates(aCertificates),
	iResult(EFalse),
	iResultPackage(iResult)
	{
	}

void CDisplayInstall::MarshallArgumentsL(TIpcArgs& aArguments)
	{
	if (iLogoFile)
		{
		// Add size of the file to the required buffer size, reallocate.
		*iStream << iAppInfo;
		
		// This means there is a logo following in the stream.
		*iStream << TPckgC<TBool>(ETrue);
		
		/* There are problems with sharing the file session and handle here
		   because the server that actually receives them is not the one that 
		   serves the message. So we must read the bitmap data from the file 
		   and pass it in a buffer to the server. I'm sure there is a better 
		   solution, but this will have to do for now. */
		   
		// Read logo file data.
		TInt logoSize;
		User::LeaveIfError(iLogoFile->Size(logoSize));
		HBufC8* logoBits=HBufC8::NewMaxLC(logoSize);
		TPtr8 p=logoBits->Des();
		User::LeaveIfError(iLogoFile->Read(p));
		
		// Write size and bits to the stream.
		*iStream << TPckgC<TInt>(logoSize);
		*iStream << *logoBits;
		
		// Cleanup.
		CleanupStack::PopAndDestroy(logoBits);

		}
	else
		{
		// No logo.
		*iStream << iAppInfo << TPckgC<TBool>(EFalse);
		}
	
	StreamOutArrayL(iCertificates, *iStream);

	iIpcInputPtr.Set(iStream->Ptr());
	aArguments.Set(0, &iIpcInputPtr);
	aArguments.Set(1, &iResultPackage);
	}

EXPORT_C TBool CDisplayInstall::ReturnResult() const
	{
	return iResult;
	}

} // namespace Swi
