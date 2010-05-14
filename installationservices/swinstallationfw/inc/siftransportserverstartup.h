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
* Defines a SIF Transport Server startup function
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef SIFTRANSPORTSERVERSTARTUP_H
#define SIFTRANSPORTSERVERSTARTUP_H

#include <usif/sif/siftransporttask.h>

namespace Usif
	{
	/** Default shutdown period is 2 seconds */
	const TInt KDefaultShutdownPeriodUs = 2 * 1000 * 1000;

	/**
	Starts a transient server. This function should be called from an executable entrypoint
	which is the @see E32Main() function.
	
	@param aServerName The name of a server to be started.
	@param aVersion The version of a server to be started.
	@param aTaskFactory A factory function. It is used to generate tasks which implement
	software management requests for the installer.
	@param aShutdownPeriodUs A shutdown period after which a server shuts down itself
	if there are no sessions.
	*/
	IMPORT_C TInt StartTransportServer(const TDesC& aServerName, const TVersion& aVersion, TransportTaskFactory::GenerateTask aTaskFactory, TInt aShutdownPeriodUs = KDefaultShutdownPeriodUs);

	} // End of namespace Usif

#endif	// #ifndef SIFTRANSPORTSERVERSTARTUP_H
