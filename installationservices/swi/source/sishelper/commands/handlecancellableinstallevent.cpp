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

#include "handlecancellableinstallevent.h"
#include "msisuihandlers.h"
#include "sisuihandler.h"
#include "uisssession.h"
#include "writestream.h"
#include "log.h"

namespace Swi
{

CHandleCancellableInstallEventCmdHandler::CHandleCancellableInstallEventCmdHandler(
	MUiHandler& aUiHandler, MCancelHandler& aCancelHandler)
		: CUissCmdHandler(aUiHandler), iCancelHandler(aCancelHandler)
	{
	}

void CHandleCancellableInstallEventCmdHandler::HandleMessageL(
	const TDesC8& aInBuf, TDes8& aOutBuf)
	{
	DEBUG_PRINTF(_L8("Sis Helper - Calling Cancellable Install Event callback."));
	
	// Setup read stream
	RDesReadStream readStream(aInBuf);
	CleanupClosePushL(readStream);
						    
	// Read CAppInfo
	CAppInfo* appInfo=CAppInfo::NewLC(readStream);

	// Read TInstallEvent
	TInstallCancellableEvent installEvent;
	TPckg<TInstallCancellableEvent> installEventPackage(installEvent);
	readStream >> installEventPackage;

	// TInt32
	TInt value=readStream.ReadInt32L();
	
	// descriptor
	HBufC* des=HBufC::NewLC(readStream, KMaxTInt);

	// Call UI Handler
	iUiHandler.HandleCancellableInstallEventL(*appInfo, installEvent, 
		iCancelHandler, value, *des);
		
	// set length of the output descriptor to zero, meaning no output is 
	// available
	aOutBuf.Zero();

	CleanupStack::PopAndDestroy(3, &readStream); // readStream, appInfo, des
	}

} // namespace Swi
