/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Executable entrypoint and server object factory function. The latter
* is supplied to the SCS boilerplate code which starts up the server.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#include "sisregistryhelperserver.h"

using namespace Swi;

static CScsServer* NewSisRegistryHelperServerLC()
/**
	This factory function is called by SCS framework.  It allocates
	the SCR Helper Server object and leaves it on the cleanup stack.

	@return		New initialized instance of CSisRegistryHelperServer.
				On return this is on the cleanup stack.
 */
	{
	return Swi::CSisRegistryHelperServer::NewLC();
	}


GLDEF_C TInt E32Main()
/**
	Executable entrypoint. Calls the SCS boilerplate code which starts the SISRegistry Helper server
	and does not return until the server shuts down.

	@return	Symbian OS error code where KErrNone indicates success 
			and any other value indicates failure.
 */
	{
	return StartScsServer(NewSisRegistryHelperServerLC);
	}
