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
* Configure preprocessor macros according to the use platform (ie: TOOLS/TOOLS2)
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef	TOOLSCONF_H
#define	TOOLSCONF_H

#ifdef __TOOLS2__
	#define DllExport
	#define GCC_COMPILER
#else
	#ifdef DLLEXPORTS
		#define DllExport __declspec(dllexport)
	#else
		#define DllExport __declspec(dllimport)
	#endif // DLLEXPORTS
#endif

#endif	/* TOOLSCONF_H */
