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
* This consists of common framework code needed for DLLs and exe
*
*/


/**
 @file
 CommonFramework.h
*/

#ifndef __CommonFramework_H
#define __CommonFramework_H

//System include
#include <e32base.h>
#include <e32cons.h>

_LIT(KTxtEPOC32EX,"CommonFramework");
_LIT(KTxtCommonCode,"Symbian OS Common Framework Code");
_LIT(KFormatFailed,"failed: leave code=%d");
_LIT(KTxtOK,"ok");
_LIT(KTxtPressAnyKey," [press any key]");

/** public */
/** write all your messages to this */
LOCAL_D CConsoleBase* console; 
/** code this function for actual implementation */
LOCAL_C void doPerformL(); 

/** private */
/** initialize with cleanup stack, then do Perform */
LOCAL_C void callPerformL(); 

/**
main function called by E32
@internalTechnology
@return TInt - exit code 
@test
*/

GLDEF_C TInt E32Main() 
	{
	__UHEAP_MARK;
	/** get clean-up stack */
	CTrapCleanup* cleanup=CTrapCleanup::New(); 
	/** more initialization, then do Perform */
	TRAPD(error,callPerformL()); 
	__ASSERT_ALWAYS(!error,User::Panic(KTxtEPOC32EX,error));
	/** destroy clean-up stack */
	delete cleanup; 
	__UHEAP_MARKEND;	
	return 0; 
	}


/**
initialize and call Perform code under cleanup stack
@internalTechnology
@return void
@test
*/
LOCAL_C void callPerformL() 
    	{
	console=Console::NewL(KTxtCommonCode,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	/** do perform function */
	TRAPD(error,doPerformL()); 
	if (error)
		console->Printf(KFormatFailed, error);
	else
		console->Printf(KTxtOK);
	console->Printf(KTxtPressAnyKey);
	/** get and ignore character */
	console->Getch(); 
	/** close console */
	CleanupStack::PopAndDestroy(); 
    	}

#endif
