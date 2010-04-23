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
* Definition of the handle cancellable install event command handler and command request object.
*
*/


/**
 @file 
 @internalComponent
 @released
*/
 
#ifndef __HANDLECANCELABLEINSTALLEVENT_H__
#define __HANDLECANCELABLEINSTALLEVENT_H__

#include "uisscommand.h"

namespace Swi
{
class MUiHandler;
class MCancelHandler;

/**
 * Command handler for the handle cancellable install event call.
 */
class CHandleCancellableInstallEventCmdHandler : public CUissCmdHandler
	{
public:
	//virtual void HandleMessageL(const RMessage2& aMessage);
	virtual void HandleMessageL(const TDesC8& aInBuf, TDes8& aOutBuf);

	CHandleCancellableInstallEventCmdHandler(MUiHandler& aUiHandler, MCancelHandler& aCancelHandler);
private:
	MCancelHandler& iCancelHandler;
	};

} // namespace Swi

#endif // #ifndef __HANDLECANCELABLEINSTALLEVENT_H__
