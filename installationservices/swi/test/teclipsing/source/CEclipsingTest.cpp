/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


/**
 @file
*/

#include "CEclipsingTest.h"

EXPORT_C MEclipsingTest* Instance()
	{
	return new CEclipsingTest();
	}
	
TInt CEclipsingTest::DllVersionQuery()
	{
	
	return
#ifdef __ECLIPSING_TEST_RETURN_1__
		1;
#elif __ECLIPSING_TEST_RETURN_2__
		2;
#endif /* __ECLIPSING_TEST_RETURN__ */

	}
