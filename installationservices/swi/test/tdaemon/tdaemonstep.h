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


#ifndef __TUISUPPORTSTEP_H__
#define __TUISUPPORTSTEP_H__

#include <test/testexecutestepbase.h>

namespace Swi
{
class TAppInfo;
class RUiHandler;

namespace Test
{

class CDaemonTestStep : public CTestStep
	{
	const static int KErrConfigRead = -18912; //To Do Get an error code

protected:
	
	// wrapper functions, leave with KErrConfigRead if there's an error
	TBool CheckedGetIntFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult);
	TBool CheckedGetStringFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, TPtrC& aResult);
	TBool CheckedGetBoolFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, TBool& aResult);

	TBool CheckedGetIntFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult, const TDesC& aObjectName);
	TBool CheckedGetStringFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, TPtrC& aResult, const TDesC& aObjectName);
	TBool CheckedGetBoolFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, TBool& aResult, const TDesC& aObjectName);

	virtual TVerdict doTestStepL();
	virtual TVerdict runTestStepL(TBool aOomTest)=0;

	/**
	 * Used by the test step to free memory, before HEAP_MARK_END
	 */
	virtual void FreeMemory();

	void ReadIntReturnValueL(TInt& aValue);
	void ReadBoolReturnValueL(TBool& aValue);

	template <class T>
	void ReadEnumL(T& aValue, const TDesC& aName);

private:
	};

template <class T>
void CDaemonTestStep::ReadEnumL(T& aValue, const TDesC& aName)
	{
	TInt value;
	CheckedGetIntFromConfigL(ConfigSection(), aName, value);
	aValue=T(value);
	}
	
} // namespace Swi::Test

} //namespace Swi

#endif // #ifndef __TUISUPPORTSTEP_H__

