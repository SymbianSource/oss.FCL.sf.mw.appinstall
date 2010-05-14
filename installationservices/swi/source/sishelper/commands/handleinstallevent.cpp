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
#include "handleinstallevent.h"
#include "sisuihandler.h"
#include "uisssession.h"
#include "writestream.h"
#include "log.h"

namespace Swi
{
// CHandleInstallEventCmdHandler

CHandleInstallEventCmdHandler::CHandleInstallEventCmdHandler(MUiHandler& aUiHandler)
		: CUissCmdHandler(aUiHandler)
	{
	}

void CHandleInstallEventCmdHandler::HandleMessageL(const TDesC8& aInBuf, 
	TDes8& aOutBuf)
	{
	DEBUG_PRINTF(_L8("Sis Helper - Calling Install Event Callback"));
	
	// Setup read stream
	RDesReadStream readStream(aInBuf);
	CleanupClosePushL(readStream);
						    
	// Read CAppInfo
	CAppInfo* appInfo=CAppInfo::NewLC(readStream);

	// Read TInstallEvent
	TInstallEvent installEvent;
	TPckg<TInstallEvent> installEventPackage(installEvent);
	readStream >>  installEventPackage;

	// TInt32
	TInt value=readStream.ReadInt32L();
	
	// descriptor
	HBufC* des=HBufC::NewLC(readStream, KMaxTInt);

	// Call UI Handler
	TBool result=iUiHandler.HandleInstallEventL(*appInfo, installEvent, value, *des);

	CWriteStream* os=CWriteStream::NewLC();
	os->Stream().WriteInt32L(result);
	aOutBuf=os->Ptr();

	CleanupStack::PopAndDestroy(4, &readStream); // readStream, appInfo, des
	}

} // namespace Swi
