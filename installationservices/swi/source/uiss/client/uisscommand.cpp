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
* uisscommand.h
*
*/


#include <e32std.h>

#include "uisscommand.h"

namespace Swi
{
EXPORT_C CUissCmdHandler::CUissCmdHandler(MUiHandler& aUiHandler)
	: iUiHandler(aUiHandler)
	{
	}

void CUissCmdHandler::CompleteL(const RMessage2& aMessage, TInt aResult, 
	const TDesC8& aData) const
	{
	// Write the result back and complete the message
	// 1 is the IPC slot used to pass back the return value
	aMessage.WriteL(1,aData);
	aMessage.Complete(aResult);
	}
	
} // namespace Swi
