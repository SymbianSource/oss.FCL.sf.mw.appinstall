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

#include "securitywarningdialog.h"
#include "msisuihandlers.h"
#include "cleanuputils.h"
#include "uissstream.h"

#include "sisuihandler.h"
#include "uisssession.h"
#include "writestream.h"
#include "log.h"

namespace Swi
{

CSecurityWarningDialogCmdHandler::CSecurityWarningDialogCmdHandler(
	MUiHandler& aUiHandler)
	: CUissCmdHandler(aUiHandler)
	{
	}

void CSecurityWarningDialogCmdHandler::HandleMessageL(const TDesC8& aInBuf,
	TDes8& aOutBuf)
	{
	DEBUG_PRINTF(_L8("Sis Helper - Displaying Security Warning dialog."));
	
	// Setup read stream
	RDesReadStream readStream(aInBuf);
	CleanupClosePushL(readStream);
	
	// Read CAppInfo
	CAppInfo* appInfo=CAppInfo::NewLC(readStream);

	// Read TSignatureValidationResult
	TSignatureValidationResult sigValidationResult;
	TPckg<TSignatureValidationResult> sigValidationResultPackage(
		sigValidationResult);
	readStream >> sigValidationResultPackage;

	// Read CPKIXValidationResultBase objects
	RPointerArray<CPKIXValidationResultBase> validationResults;
	CleanupResetAndDestroyPushL(validationResults);
	StreamInArrayL(validationResults, readStream);

	// Read certificates
	RPointerArray<CCertificateInfo> certs;
	CleanupResetAndDestroyPushL(certs);
	StreamInArrayL(certs, readStream);	
	
	// Read TBool
	TBool installAnyway;
	TPckg<TBool> installAnywayPackage(installAnyway);
	readStream >> installAnywayPackage;

	// Call UI Handler
	TBool result=iUiHandler.DisplaySecurityWarningL(*appInfo, 
		sigValidationResult, validationResults, certs, installAnyway);

	CWriteStream* os=CWriteStream::NewLC();
	os->Stream().WriteInt32L(result);
	aOutBuf=os->Ptr();
	
	// readStream, appInfo, pkixValidationResults, certs, os
	CleanupStack::PopAndDestroy(5, &readStream);
	}
	
} // namespace Swi
