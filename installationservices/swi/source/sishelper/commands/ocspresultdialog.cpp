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

#include "ocspresultdialog.h"
#include "msisuihandlers.h"

#include "sisuihandler.h"
#include "uisssession.h"
#include "uissstream.h"
#include "writestream.h"
#include "cleanuputils.h"
#include "log.h"

namespace Swi
{

COcspResultDialogCmdHandler::COcspResultDialogCmdHandler(MUiHandler& aUiHandler)
		: CUissCmdHandler(aUiHandler)
	{
	}

void COcspResultDialogCmdHandler::HandleMessageL(const TDesC8& aInBuf,
	TDes8& aOutBuf)
	{
	DEBUG_PRINTF(_L8("Sis Helper - Displaying OCSP Results dialog."));
	
	// Setup read stream
	RDesReadStream readStream(aInBuf);
	CleanupClosePushL(readStream);
	
	// Read CAppInfo
	CAppInfo* appInfo=CAppInfo::NewLC(readStream);

	// Read TRevocationDialogMessage
	TRevocationDialogMessage revocationDialog;
	TPckg<TRevocationDialogMessage> revocationDialogPackage(revocationDialog);
	readStream >>  revocationDialogPackage;

	// Read TOCSPOutcome objects
	RPointerArray<TOCSPOutcome> outcomes;
	CleanupResetAndDestroyPushL(outcomes);
	StreamInArrayL(outcomes, readStream);
	
	// Read CCertificateInfo (optional)
	RPointerArray<CCertificateInfo> certs;
	CleanupResetAndDestroyPushL(certs);
	StreamInArrayL(certs, readStream);

	// Read TBool
	TBool warningOnly;
	TPckg<TBool> warningOnlyPackage(warningOnly);
	readStream >> warningOnlyPackage;

	// Call UI Handler
	TBool result=iUiHandler.DisplayOcspResultL(*appInfo, revocationDialog, 
		outcomes, certs, warningOnly);

	CWriteStream* os=CWriteStream::NewLC();
	os->Stream().WriteInt32L(result);
	aOutBuf=os->Ptr();
	
	// readStream, appInfo, outcomes, certs, os
	CleanupStack::PopAndDestroy(5, &readStream); 
	}

} // namespace Swi
