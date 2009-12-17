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

#include "errordialog.h"
#include "msisuihandlers.h"
#include "sisuihandler.h"
#include "uisssession.h"
#include "writestream.h"
#include "log.h"

namespace Swi
{

CErrorDialogCmdHandler::CErrorDialogCmdHandler(MUiHandler& aUiHandler)
		: CUissCmdHandler(aUiHandler)
	{
	}

void CErrorDialogCmdHandler::HandleMessageL(const TDesC8& aInBuf, 
	TDes8& aOutBuf)
	{
	DEBUG_PRINTF(_L8("Sis Helper - Displaying Error Dialog."));
	
	// Setup read stream
	RDesReadStream readStream(aInBuf);
	CleanupClosePushL(readStream);
	
	// Read CAppInfo
	CAppInfo* appInfo=CAppInfo::NewLC(readStream);

	// Read TErrorDialog
	TErrorDialog errorDialog;
	TPckg<TErrorDialog> errorDialogPackage(errorDialog);
	readStream >>  errorDialogPackage;

	HBufC* des=HBufC::NewLC(readStream, KMaxTInt);

	// Call UI Handler
	iUiHandler.DisplayErrorL(*appInfo, errorDialog, *des);

	// set length of the output descriptor to zero, meaning no output is 
	// available
	aOutBuf.Zero();

	CleanupStack::PopAndDestroy(3, &readStream); // readStream, appInfo, des
	}

} // namespace Swi
