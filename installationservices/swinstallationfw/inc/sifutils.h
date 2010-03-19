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

namespace Usif
	{
		/**
			This function uninstalls a component by removing it from the SCR and deleting its files.
			This is a synchronous API and therefore doesn't support cancellation. Hence, it is designated
			for simple installers that don't require this functionlaity.

			@param aComponentId The id of a component to be uninstalled
			@leave System wide error code
		*/
		IMPORT_C void UninstallL(TComponentId aComponentId);
	}

#endif // SIFUTILS_H
