/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent
*/

#ifndef ASYNCCANCELSTEP_H
#define ASYNCCANCELSTEP_H



#include <f32file.h>
#include <test/testexecutestepbase.h>
#include "common.h"



using namespace Swi;

extern CConsoleBase* gConsole;



class CAsyncCancelStep : public CTestStep
	{
	
public:
	CAsyncCancelStep();
	~CAsyncCancelStep();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL(); 
	

	#define MY_INFO_PRINTF1(a) 		{ gConsole->Printf(a);		INFO_PRINTF1(a);	}
	#define MY_INFO_PRINTF2(a,b)	{ gConsole->Printf(a,b);	INFO_PRINTF2(a,b);	}
	#define MY_ERR_PRINTF1(a)		{ gConsole->Printf(a);		ERR_PRINTF1(a);	}
	#define MY_ERR_PRINTF2(a,b)		{ gConsole->Printf(a,b);	ERR_PRINTF2(a,b);	}
	
	/**
 	* Performs the asynchronous cancelation of installation tests
 	* Note that "microsecond" effectively means "as soon as possible"
 	* @param aWhenToCancelLow		The first point in time (microseconds) when to cancel the installation
 	* @param aWhenToCancelHigh		The last point in time (microseconds) when to cancel the installation
 	* @param aHowManyCancelations	How many cancelations to perform (approximate, up to one cancelation more or less should be expected, except when limited by aMinWhenToCancelInc, aWhenToCancelLow and aWhenToCancelHigh)
 	* @param aMinWhenToCancelInc	Minimal time (microseconds) period between two cancelations
 	* @param aSisToInstall			Full path to sis file to be installed
 	* @param aUid					UID of the sis file to be installed
 	*/
	TInt DoTestL(TInt aWhenToCancelLow, TInt aWhenToCancelHigh, TInt aHowManyCancelations, TInt aMinWhenToCancelInc,
			 		const TPtrC& aSisToInstall, TUid aUid,
			 		CInstallPrefs* aPrefs=NULL);
private:
	
	};
// String for the server create test step code
_LIT(KAsyncCancelStep,"AsyncCancelStep");


#endif
// End of file
