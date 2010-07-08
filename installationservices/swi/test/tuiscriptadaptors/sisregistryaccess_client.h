/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* SisRegistryAccess - client interface
*
*/


/**
 @file 
 @test
 @internalComponent
*/

#ifndef __SISREGISTRYACCESSCLIENT_H__
#define __SISREGISTRYACCESSCLIENT_H__

#include <e32std.h>
#include "sisregistrypackage.h"

class CApplication;

class RSisRegistryAccessSession : public RSessionBase
	{
public: 
	TInt Connect();

	/**
	 	Adds a registry entry representing this controller data
	 
	   @param aControllerData The controller data.
	 */
	void AddEntryL(const TDesC8& aControllerData, TInt& aSpentTimeInMillisecond);
	
	/**
		Updates the registry entry representing this controller data
		 
		@param aControllerData The controller data.
	 */
	void UpdateEntryL(const TDesC8& aControllerData, TInt& aSpentTimeInMillisecond);
	
	/**
	 * Removes the specified package from SCR (using SisRegistryServer)
	 */
	TInt DeleteEntryL(Swi::CSisRegistryPackage& aPackage, TInt& aSpentTimeInMillisecond);

	/**
	 * Adds the Application Registration Info from *_reg.rsc
	 */
	TInt AddAppRegInfoL(const TDesC& aAppRegFile, TInt& aSpentTimeInMillisecond);
	
	/**
     * Removes the Application Registration Info
     */
    TInt RemoveAppRegInfoL(const TDesC& aAppRegFile, TInt& aSpentTimeInMillisecond);
	};

#endif	// __SISREGISTRYACCESSCLIENT_H__
