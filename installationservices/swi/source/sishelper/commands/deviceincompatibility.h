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
* Definition of the device incompatibility dialog command handler and command request object.
*
*/


/**
 @file 
 @internalComponent
 @released
*/
 
#ifndef __DEVICEINCOMPATIBILITYDIALOG_H__
#define __DEVICEINCOMPATIBILITYDIALOG_H__

#include "uisscommand.h"

namespace Swi
{
class MUiHandler;

/**
 * Command handler for the device incompatibility dialog.
 *
 * @internalComponent
 * @released
 */
NONSHARABLE_CLASS(CDeviceIncompatibilityDialogCmdHandler) : public CUissCmdHandler
	{
public:
	virtual void HandleMessageL(const TDesC8& aInBuf, TDes8& aOutBuf);
	CDeviceIncompatibilityDialogCmdHandler(MUiHandler& aUiHandler);
	};

} // namespace Swi

#endif // #ifndef __DEVICEINCOMPATIBILITYDIALOG_H__
