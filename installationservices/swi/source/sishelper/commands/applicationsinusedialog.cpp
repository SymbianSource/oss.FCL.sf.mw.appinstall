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

#include "applicationsinusedialog.h"
#include "cleanuputils.h"
#include "msisuihandlers.h"
#include "uissstream.h"

#include "sisuihandler.h"
#include "uisssession.h"
#include "writestream.h"
#include "log.h"

namespace Swi
{

CApplicationsInUseDialogCmdHandler::CApplicationsInUseDialogCmdHandler(MUiHandler& aUiHandler)
		: CUissCmdHandler(aUiHandler)
	{
	}

void CApplicationsInUseDialogCmdHandler::HandleMessageL(const TDesC8& aInBuf, 
	TDes8& aOutBuf)
	{
	DEBUG_PRINTF(_L8("Sis Helper - Displaying Applications In Use dialog."));
	
	// Setup read stream
	RDesReadStream readStream(aInBuf);
	CleanupClosePushL(readStream);
	
	CAppInfo* appInfo=CAppInfo::NewLC(readStream);
	
	// Read application names 
	RPointerArray<TDesC> appNames;
	CleanupResetAndDestroy<RPointerArray<TDesC> >::PushL(appNames);

	StreamInArrayL(appNames, readStream);

	// Call UI Handler
	TBool result=iUiHandler.DisplayApplicationsInUseL(*appInfo, appNames);

	CWriteStream* os=CWriteStream::NewLC();
	os->Stream().WriteInt32L(result);
	aOutBuf=os->Ptr();
	// readStream, appInfo, appNames, os
	CleanupStack::PopAndDestroy(4, &readStream);
	}
	
} // namespace Swi
