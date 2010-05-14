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
#include <e32property.h>
#include <e32uid.h>

#ifndef SWI_TEXTSHELL_ROM
#include <imageconversion.h>
#endif

#include "installdialog.h"
#include "cleanuputils.h"

#include "sisuihandler.h"
#include "uisssession.h"
#include "writestream.h"
#include "uissstream.h"
#include "log.h"

namespace Swi
{

CInstallDialogCmdHandler::CInstallDialogCmdHandler(MUiHandler& aUiHandler)
		: CUissCmdHandler(aUiHandler)
		, iListener(NULL)
	{
	}
	
CInstallDialogCmdHandler::~CInstallDialogCmdHandler()
	{
	delete iListener;
	}

void CInstallDialogCmdHandler::HandleMessageL(const TDesC8& aInBuf, 
	TDes8& aOutBuf)
	{
	DEBUG_PRINTF(_L8("Sis Helper - Displaying Install dialog."));
	
	// Setup read stream
	RDesReadStream readStream(aInBuf);
	CleanupClosePushL(readStream);
	
	// Read CAppInfo
	CAppInfo* appInfo=CAppInfo::NewLC(readStream);

	CApaMaskedBitmap* logo = NULL;
	
	TBool logoPassed = EFalse;
	TPckg<TBool> logoPassedPackage(logoPassed);
	readStream >> logoPassedPackage;

	TInt logoSize = 0;

	if (logoPassed)
		{
		TPckg<TInt> logoSizePkg(logoSize);
		
		// Read logo size.
		readStream >> logoSizePkg;
		if (logoSize)
			{
			// The logo must  be read from the stream irrespective of logo is processed or not
			// otherwise the certificate info can't be extracted.

			// Read raw logo data from the stream.
			HBufC8* logoBits=HBufC8::NewLC(readStream, logoSize);

			TInt connectErr = KErrNotFound;
			// In textshell ROMs the imageconversion libraries are not available
			// so we don't attempt to convert the logo.		
		#ifndef SWI_TEXTSHELL_ROM
			connectErr = RFbsSession::Connect();
			if (connectErr != KErrNotFound)
				{			
				User::LeaveIfError(connectErr);					
				CleanupStack::PushL(TCleanupItem(CleanupFbsSession, 0));

				TBool schedulerInstalled=EFalse;
					
				if (CActiveScheduler::Current()==0)
					{
					// Install active scheduler necessary for image converter.
					CActiveScheduler* scheduler=new(ELeave) CActiveScheduler;
					CleanupStack::PushL(scheduler);
					CActiveScheduler::Install(scheduler);
					schedulerInstalled=ETrue;
					}
		
				// try converting the logo in to a symbian bitmap,
				// if we fail, just continue as if there were no logo	
				TRAP_IGNORE(logo = DecodeBitmapL(*logoBits));
		
				if (schedulerInstalled)
					{
					// Stop and uninstall active scheduler.
					CActiveScheduler::Install(NULL);
					CleanupStack::PopAndDestroy(); // scheduler
					}
				}
		#endif
			if (connectErr == KErrNotFound)
				{
				// emulator tests running in textshell or in textshell ROM (#def SWI_TEXTSHELL_ROM)
				// reset the logoSize so that CleanupFbsSession won't be popedup from cleanupstack
				CleanupStack::PopAndDestroy(logoBits);
				logoSize = 0;
				}
			if (logo)
				{
				CleanupStack::PushL(logo);
				}
			}
		}

	// Stream in the array of end certificates.
	RPointerArray<CCertificateInfo> certificates;
	CleanupResetAndDestroyPushL(certificates);
	StreamInArrayL(certificates, readStream);
	
	// Call UI Handler
	TBool result=iUiHandler.DisplayInstallL(*appInfo, logo, certificates);
	
	CleanupStack::PopAndDestroy(&certificates);
	
	if(logo)
		{
		CleanupStack::PopAndDestroy(logo);
		}
		
	CWriteStream* os=CWriteStream::NewLC();
	os->Stream().WriteInt32L(result);
	aOutBuf=os->Ptr();
	// readStream, appInfo, [logobits, CleanupFbsSession()], os
	CleanupStack::PopAndDestroy((logoSize != NULL) ? 5 : 3, &readStream);
	}

CApaMaskedBitmap* CInstallDialogCmdHandler::DecodeBitmapL(const TDesC8& aBitmapData)
	{
	CApaMaskedBitmap* ret = NULL;
#ifdef SWI_TEXTSHELL_ROM	
	(void) aBitmapData;
#else
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CImageDecoder* imgDecoder=CImageDecoder::DataNewL(fs, aBitmapData);
	CleanupStack::PushL(imgDecoder);
	
	// Get frame info needed to create bitmaps of the right size.
	TFrameInfo info=imgDecoder->FrameInfo();
	
	ret = CApaMaskedBitmap::NewLC();
	// Create placeholder bitmaps.
	TInt err=ret->Create(info.iOverallSizeInPixels,
		info.iFrameDisplayMode);
	User::LeaveIfError(err);
	
	CFbsBitmap* mask=new(ELeave) CFbsBitmap;
	CleanupStack::PushL(mask);
	err=mask->Create(info.iOverallSizeInPixels,
		(info.iFlags & TFrameInfo::EAlphaChannel) ? EGray256 : EGray2);
	User::LeaveIfError(err);
	
	// Set logo mask.
	ret->SetMaskBitmap(mask); // transfer ownership to the logo bitmap
	CleanupStack::Pop(mask);
	
	// Convert image to bitmaps.
	iListener=new(ELeave) CActiveListener;
	iListener->InitialiseActiveListener();
	imgDecoder->Convert(&iListener->iStatus, *ret, *mask, 0);
	CActiveScheduler::Start();
	delete iListener;
	iListener = NULL;
	
	CleanupStack::Pop(ret);
	CleanupStack::PopAndDestroy(2, &fs); // imgDecoder
#endif
	return ret;
	}

void CInstallDialogCmdHandler::CleanupFbsSession(TAny*)
	{
	#ifndef SWI_TEXTSHELL_ROM
	RFbsSession::Disconnect();
	#endif	
	}

} // namespace Swi
