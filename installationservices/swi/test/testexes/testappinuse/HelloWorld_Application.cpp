/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* HelloWorld_CExampleApplication.cpp
* Source file for the implementation of the 
* application class - CExampleApplication
*
*/


#include "HelloWorld.h"

// if HELLOWORLD is defined in the MMP file use the appropriate Uid
#ifdef HELLOWORLD2
const TUid KUidHelloWorld = { 0x90008ACF }; 

#else
const TUid KUidHelloWorld = { 0X90008ACE }; 
#endif

//             The function is called by the UI framework to ask for the
//             application's UID. The returned value is defined by the
//             constant KUidHelloWorlde and must match the second value
//             defined in the project definition file.
// 
TUid CExampleApplication::AppDllUid() const
	{
	return KUidHelloWorld;
	}

//             This function is called by the UI framework at
//             application start-up. It creates an instance of the
//             document class.
//
CApaDocument* CExampleApplication::CreateDocumentL()
	{
	return new (ELeave) CExampleDocument(*this);
	}
