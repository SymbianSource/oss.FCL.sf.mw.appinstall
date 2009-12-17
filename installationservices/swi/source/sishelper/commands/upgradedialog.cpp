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


#include <s32mem.h>

#include "upgradedialog.h"
#include "msisuihandlers.h"
#include "uissstream.h"

#include "sisuihandler.h"
#include "uisssession.h"
#include "writestream.h"
#include "log.h"

namespace Swi
{

CUpgradeDialogCmdHandler::CUpgradeDialogCmdHandler(MUiHandler& aUiHandler)
		: CUissCmdHandler(aUiHandler)
	{
	}

void CUpgradeDialogCmdHandler::HandleMessageL(const TDesC8& aInBuf,
	TDes8& aOutBuf)
	{
	DEBUG_PRINTF(_L8("Sis Helper - Displaying Upgrade dialog."));
	
	// Setup read stream
	RDesReadStream readStream(aInBuf);
	CleanupClosePushL(readStream);
	
	// Read CAppInfo
	CAppInfo* appInfo=CAppInfo::NewLC(readStream);

	// Read existing app CAppInfo
	CAppInfo* existingAppInfo=CAppInfo::NewLC(readStream);

	// Call UI Handler
	TBool result=iUiHandler.DisplayUpgradeL(*appInfo, *existingAppInfo);

	CWriteStream* os=CWriteStream::NewLC();
	os->Stream().WriteInt32L(result);
	aOutBuf=os->Ptr();
	// readStream, appInfo, existingAppInfo, os
	CleanupStack::PopAndDestroy(4, &readStream);
	}

} // namespace Swi
