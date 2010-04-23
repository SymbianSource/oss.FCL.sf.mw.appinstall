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

#include "drivedialog.h"
#include "msisuihandlers.h"
#include "uissstream.h"

#include "sisuihandler.h"
#include "uisssession.h"
#include "writestream.h"
#include "log.h"

namespace Swi
{

CDriveDialogCmdHandler::CDriveDialogCmdHandler(MUiHandler& aUiHandler)
		: CUissCmdHandler(aUiHandler)
	{
	}

void CDriveDialogCmdHandler::HandleMessageL(const TDesC8& aInBuf, 
	TDes8& aOutBuf)
	{
	DEBUG_PRINTF(_L8("Sis Helper - Displaying Drive Selection dialog."));
	
	// Setup read stream
	RDesReadStream readStream(aInBuf);
	CleanupClosePushL(readStream);
	
	// Read CAppInfo
	CAppInfo* appInfo=CAppInfo::NewLC(readStream);
	
	TInt64 installedSize=0;
	readStream >> installedSize;
	
	RArray<TChar> driveLetters;
	CleanupClosePushL(driveLetters);
	StreamInArrayL(driveLetters, readStream);
	
	RArray<TInt64> driveSpaces;
	CleanupClosePushL(driveSpaces);
	StreamInArrayL(driveSpaces, readStream);

	// Call UI Handler
	TInt result=iUiHandler.DisplayDriveL(*appInfo, installedSize, driveLetters,
		driveSpaces);
	
	DEBUG_PRINTF2(_L8("Sis Helper - Selected drive was %d."), result);
	
	CWriteStream* os=CWriteStream::NewLC();
	os->Stream().WriteInt32L(result);
	aOutBuf=os->Ptr();
	CleanupStack::PopAndDestroy(5, &readStream); // readStream, appInfo, driveLetters, driveSpaces, os
	}

} // namespace Swi
