/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32uid.h>
#include <e32base.h>
#include <e32test.h>
#include <apgctl.h>


//const TInt KMinimalControlUidValue=98;

EXPORT_C void CreateControlL(const TDesC& aPath)
// The ordinal 1 export
//
	{
	RTest test(_L("Minimal system control"));
	test.Title();
	test.Start(_L(" @SYMTestCaseID API-SEC-M_CTRL-0001 The Minimal system control has been executed."));
	test.Printf(_L("\n"));
	test.Printf(aPath);
	test.Printf(_L("\n"));
	test.End();
	test.Close();
	}

