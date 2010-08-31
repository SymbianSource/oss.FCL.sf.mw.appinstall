/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file defines an example implementation of the MInstallerUIHandler interface.
*
*/


/**
 @file
 @internalComponent
 exampleCode
*/

#ifndef SIFREFUIHANDLER_H
#define SIFREFUIHANDLER_H

#include "sifrefinstallertask.h"
#include "sifrefpkgparser.h"

namespace Usif
	{
	/**
	A dummy implementation of the @see MInstallerUIHandler interface for the Reference
	Installer. It doesn't interact with a real UI subsystem but returns hardcoded values only.
	
	Please see @see sifrefinstallertask.h for details of @see MInstallerUIHandler.
	*/
	class MSifRefUIHandler : public MInstallerUIHandler
		{
	public:
		// MInstallerUIHandler interface
		virtual TBool ConfirmationUIHandler(const TDesC& aQuestion);
		virtual void ErrorDescriptionUIHandler(const TDesC& aDescription);
		};
	} // namespace Usif

#endif // SIFREFUIHANDLER_H