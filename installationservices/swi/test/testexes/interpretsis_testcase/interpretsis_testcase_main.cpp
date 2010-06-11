// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// HelloWorld 
// The example is a simple application containing a single view with
// the text "Hello World !" drawn on it.
// The example includes code for displaying a very simple menu.
// This source file contains the single exported function required by 
// all UI applications.
//


#include "interpretsis_testcase.h"

//             The entry point for the application code. It creates
//             an instance of the CApaApplication derived
//             class, CExampleApplication.
//



#include <eikstart.h>
LOCAL_C CApaApplication* NewApplication()
	{
	return new CExampleApplication;
	}
	
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}
	

