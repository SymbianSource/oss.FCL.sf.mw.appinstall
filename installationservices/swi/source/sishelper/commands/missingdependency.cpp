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
#include <e32cmn.h>
#include <s32mem.h>

#include "missingdependency.h"
#include "msisuihandlers.h"
#include "uissstream.h"
#include "cleanuputils.h"

#include "sisuihandler.h"
#include "uisssession.h"
#include "writestream.h"
#include "log.h"

namespace Swi
{

CMissingDependencyDialogCmdHandler::CMissingDependencyDialogCmdHandler(MUiHandler& aUiHandler)
		: CUissCmdHandler(aUiHandler)
	{
	}

void CMissingDependencyDialogCmdHandler::HandleMessageL(const TDesC8& aInBuf, 
	TDes8& aOutBuf)
	{
	DEBUG_PRINTF(_L8("Sis Helper - Displaying Missing Dependency Dialog."));
	
	// Setup read stream
	RDesReadStream readStream(aInBuf);
	CleanupClosePushL(readStream);
	
	// Read CAppInfo
	CAppInfo* appInfo=CAppInfo::NewLC(readStream);

	// Read dependency name.
	HBufC* depName=HBufC::NewLC(readStream, KMaxTInt);

	TBool wantedFromValid(ETrue);
	TPckg<TBool> wantedFromValidPckg(wantedFromValid);
	TVersion wantedFrom(-1,-1,-1);
	readStream >> wantedFromValidPckg;
	if (wantedFromValid)
		{
		TPckg<TVersion> pkg(wantedFrom);
		readStream >> pkg;
		}

	TBool wantedToValid(ETrue);
	TPckg<TBool> wantedToValidPckg(wantedToValid);
	TVersion wantedTo(-1,-1,-1);
	readStream >> wantedToValidPckg;
	if (wantedToValid)
		{
		TPckg<TVersion> pkg(wantedTo);
		readStream >> pkg;
		}

	TBool availableValid(ETrue);
	TPckg<TBool> availableValidPckg(availableValid);
	TVersion available(0,0,0);
	readStream >> availableValidPckg;
	if (availableValid)
		{
		TPckg<TVersion> pkg(available);
		readStream >> pkg;
		}
	
	// Call UI Handler
	TBool result=iUiHandler.DisplayMissingDependencyL(*appInfo,*depName,
		wantedFrom,wantedTo,available);

	CWriteStream* os=CWriteStream::NewLC();
	os->Stream().WriteInt32L(result);
	aOutBuf=os->Ptr();
	// readStream, appInfo, depName, os
	CleanupStack::PopAndDestroy(4, &readStream);
	}

} // namespace Swi
