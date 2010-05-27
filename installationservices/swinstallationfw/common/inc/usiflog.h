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
* Implements print functions for the Universal Software Install Framework components.
*
*/


/**
 @file 
 @internalComponent 
 @released
*/
 
#ifndef USIFLOG_H
#define USIFLOG_H

#include <scs/securitylog.h>

namespace Usif
	{
	 _LIT8(KComponentName, "[USIF]");
	   
	#define DEBUG_PRINTF(a) {SEC_DEBUG_PRINTF(KComponentName, a);}
	#define DEBUG_PRINTF2(a, b) {SEC_DEBUG_PRINTF2(KComponentName, a, b);}
	#define DEBUG_PRINTF3(a, b, c) {SEC_DEBUG_PRINTF3(KComponentName, a, b, c);}
	#define DEBUG_PRINTF4(a, b, c, d) {SEC_DEBUG_PRINTF4(KComponentName, a, b, c, d);}
	#define DEBUG_PRINTF5(a, b, c, d, e) {SEC_DEBUG_PRINTF5(KComponentName, a, b, c, d, e);}
	
	#define DEBUG_CODE_SECTION(a) {SEC_DEBUG_CODE_SECTION(a);}
	 
	}	// End of namespace StreamAccess

#endif // USIFLOG_H
