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
* Definition of the handle install event command handler and command request object.
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __HANDLEINSTALLEVENT_H__
#define __HANDLEINSTALLEVENT_H__

#include "uisscommand.h"

namespace Swi
{

/**
 * Command handler for the handle install event call.
 */
class CHandleInstallEventCmdHandler : public CUissCmdHandler
	{
public:
	//virtual void HandleMessageL(const RMessage2& aMessage);
	virtual void HandleMessageL(const TDesC8& aInBuf, TDes8& aOutBuf);
	CHandleInstallEventCmdHandler(MUiHandler& aUiHandler);
	};

} // namespace Swi

#endif // #ifndef __HANDLEINSTALLEVENT_H__
