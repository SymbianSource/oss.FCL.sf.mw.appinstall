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
* This file defines a SIF utility library.
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef SIFUTILS_H
#define SIFUTILS_H
#include <e32cmn.h> 

namespace Usif
	{
	
		/**
			This function uninstalls a component by removing it from the SCR and deleting its files.
			This is a synchronous API and therefore doesn't support cancellation. Hence, it is designated
			for simple installers that don't require this functionality.

			@param aComponentId The id of a component to be uninstalled
			@leave System wide error code
		*/
		IMPORT_C void UninstallL(TComponentId aComponentId);
		
		/**
           Generate a new AppUid to be used to register non native applications.
		   
		   The cenrep file of SCR contains a list of AppUid ranges preallocated to non native apps.
           This API can be used to request a new UID from these ranges.

			@return an AppUid if available, else a null object is returned.
		 */
		IMPORT_C TUid GenerateNewAppUidL();
	}

#endif // SIFUTILS_H
