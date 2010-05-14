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

#include "optionsdialog.h"
#include "cleanuputils.h"

#include "sisuihandler.h"
#include "uisssession.h"
#include "writestream.h"
#include "uissstream.h"
#include "log.h"

namespace Swi
{

COptionsDialogCmdHandler::COptionsDialogCmdHandler(MUiHandler& aUiHandler)
		: CUissCmdHandler(aUiHandler)
	{
	}

void COptionsDialogCmdHandler::HandleMessageL(const TDesC8& aInBuf,
	TDes8& aOutBuf)
	{
	DEBUG_PRINTF(_L8("Sis Helper - Displaying Options dialog."));
	
	// Setup read stream
	RDesReadStream readStream(aInBuf);
	CleanupClosePushL(readStream);
	
	// Read CAppInfo aAppInfo
	CAppInfo* appInfo=CAppInfo::NewLC(readStream);

	// Read	RPointerArray<TDesC> options
	RPointerArray<TDesC> options;
	CleanupResetAndDestroyPushL(options);
	StreamInArrayL(options, readStream);
	
	// Create RArray<TBool> selections
	// There is one entry for each option, to allow it to be set to ETrue, or EFalse
	// This data is not include in the Client-Server request to the UI Support server,
	// since the selections are due to user input only
	RArray<TBool> selections;
	CleanupClosePushL(selections);
	for (TInt i=0;i<options.Count();++i)
		{
		User::LeaveIfError(selections.Append(EFalse));
		}
		
	// Call UI Handler
	TBool result=iUiHandler.DisplayOptionsL(*appInfo, options, selections);
	
	DEBUG_CODE_SECTION(
		for (TInt j = 0; j < options.Count(); ++j)
			{
			if (selections[j])
				{
				DEBUG_PRINTF2(_L("Sis Helper - Option %S selected."), options[j]);
				}
			}
	);
	
	CWriteStream* selectionStream=CWriteStream::NewLC();

	selectionStream->Stream().WriteInt32L(result);
	StreamOutArrayL(selections, *selectionStream);
	aOutBuf=selectionStream->Ptr();
	// readStream, appInfo, options, selections, selectionStream
	CleanupStack::PopAndDestroy(5, &readStream);
	}

} // namespace Swi
