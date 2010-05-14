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

#include "tdaemonstep.h"


namespace Swi
{
namespace Test
{
	
/**
 * Checked version of GetIntFromConfigL. Leaves with KErrConfigRead, and logs a message if an error occurs.
 *
 * @see CTestStep::GetIntFromConfig
 */
TBool CDaemonTestStep::CheckedGetIntFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult)
	{
	TBool result=GetIntFromConfig(aSectName,aKeyName,aResult);
	
	if (EFalse==result)
		{
		_LIT(KLogFormat,"Error reading int %S, from config section %S");
		ERR_PRINTF3(KLogFormat, &aKeyName, &aSectName);
		
		User::Leave(KErrConfigRead);
		}
	
	return result;
	}

/**
 * Checked version of GetStringFromConfigL. Leaves with KErrConfigRead, and logs a message if an error occurs.
 *
 * @see CTestStep::GetStringFromConfig
 */
TBool CDaemonTestStep::CheckedGetStringFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, TPtrC& aResult)
	{
	TBool result=GetStringFromConfig(aSectName,aKeyName,aResult);
	
	if (EFalse==result)
		{
		_LIT(KLogFormat,"Error reading string %S, from config section %S");
		ERR_PRINTF3(KLogFormat, &aKeyName, &aSectName);
		
		User::Leave(KErrConfigRead);
		}
	
	return result;
	}
	
/**
 * Checked version of GetBoolFromConfigL. Leaves with KErrConfigRead, and logs a message if an error occurs.
 *
 * @see CTestStep::GetBoolFromConfig
 */
TBool CDaemonTestStep::CheckedGetBoolFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, TBool& aResult)
	{
	TBool result=GetBoolFromConfig(aSectName,aKeyName,aResult);
	
	if (EFalse==result)
		{
		_LIT(KLogFormat,"Error reading boolean %S, from config section %S");
		ERR_PRINTF3(KLogFormat, &aKeyName, &aSectName);
		
		User::Leave(KErrConfigRead);
		}
	
	return result;
	}
	
/**
 * Checked version of GetIntFromConfigL. Leaves with KErrConfigRead, and logs a message if an error occurs.
 *
 * @see CTestStep::GetIntFromConfig
 */
TBool CDaemonTestStep::CheckedGetIntFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult, const TDesC& aObjectName)
	{
	HBufC* nameBuf=HBufC::NewLC(aObjectName.Length()+aKeyName.Length()+5);
	TPtr nameBufDes=nameBuf->Des();
	nameBufDes.AppendFormat(_L("%S.%S"), &aObjectName, &aKeyName);
	
	TBool result=GetIntFromConfig(aSectName,nameBufDes,aResult);
	
	if (EFalse==result)
		{
		_LIT(KLogFormat,"Error reading int %S, from config section %S");
		ERR_PRINTF3(KLogFormat, &nameBufDes, &aSectName);
		
		User::Leave(KErrConfigRead);
		}
	CleanupStack::PopAndDestroy(nameBuf);
	return result;
	}

/**
 * Checked version of GetStringFromConfigL. Leaves with KErrConfigRead, and logs a message if an error occurs.
 *
 * @see CTestStep::GetStringFromConfig
 */
TBool CDaemonTestStep::CheckedGetStringFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, TPtrC& aResult, const TDesC& aObjectName)
	{
	HBufC* nameBuf=HBufC::NewLC(aObjectName.Length()+aKeyName.Length()+5);
	TPtr nameBufDes=nameBuf->Des();
	_LIT(KAppendFormat,"%S.%S");
	nameBufDes.AppendFormat(KAppendFormat, &aObjectName, &aKeyName);
	
	TBool result=GetStringFromConfig(aSectName,nameBufDes,aResult);
	
	if (EFalse==result)
		{
		_LIT(KLogFormat,"Error reading string %S, from config section %S");
		ERR_PRINTF3(KLogFormat, &nameBufDes, &aSectName);
		
		User::Leave(KErrConfigRead);
		}
	CleanupStack::PopAndDestroy(nameBuf);
	return result;
	}
	
/**
 * Checked version of GetBoolFromConfigL. Leaves with KErrConfigRead, and logs a message if an error occurs.
 *
 * @see CTestStep::GetBoolFromConfig
 */
TBool CDaemonTestStep::CheckedGetBoolFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, TBool& aResult, const TDesC& aObjectName)
	{
	HBufC* nameBuf=HBufC::NewLC(aObjectName.Length()+aKeyName.Length()+5);
	TPtr nameBufDes=nameBuf->Des();
	_LIT(KAppendFormat,"%S.%S");
	nameBufDes.AppendFormat(KAppendFormat, &aObjectName, &aKeyName);
	
	TBool result=GetBoolFromConfig(aSectName,nameBufDes,aResult);
	
	if (EFalse==result)
		{
		_LIT(KLogFormat,"Error reading bool %S, from config section %S");
		ERR_PRINTF3(KLogFormat, &nameBufDes, &aSectName);
		
		User::Leave(KErrConfigRead);
		}
	CleanupStack::PopAndDestroy(nameBuf);
	return result;
	}

void CDaemonTestStep::ReadIntReturnValueL(TInt& aValue)
	{
	_LIT(KReturnValueKeyName,"ReturnValue");
	CheckedGetIntFromConfigL(ConfigSection(),KReturnValueKeyName,aValue);
	}
	
void CDaemonTestStep::ReadBoolReturnValueL(TBool& aValue)
	{
	_LIT(KReturnValueKeyName,"ReturnValue");
	CheckedGetBoolFromConfigL(ConfigSection(),KReturnValueKeyName,aValue);
	}

TVerdict CDaemonTestStep::doTestStepL()
	{
	// Check for OOM tests
	TBool oomTest=EFalse;
	GetBoolFromConfig(ConfigSection(), _L("OOM"),oomTest);

	CActiveScheduler* sched=new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(sched);

	if (oomTest)
		{
		INFO_PRINTF1(_L("Running OOM Test"));

		TInt err=KErrNoMemory;
		TInt nextFailure=1;
		TVerdict verdict=EFail;
			
		while (err!=KErrNone)
			{
			__UHEAP_MARK;
			__UHEAP_FAILNEXT(nextFailure);
			TRAP(err, verdict=runTestStepL(ETrue));		
			FreeMemory();
			__UHEAP_MARKEND;
			++nextFailure;
			
			if (err==KErrNone)
				{
				return verdict;
				}
			}
		}
	else
		{
		__UHEAP_MARK;
		TVerdict v=runTestStepL(EFalse);		
		FreeMemory();
		__UHEAP_MARKEND;
		return v;
		}

	CActiveScheduler::Install(NULL);
	delete sched;
	sched=0;
	
	return EFail;
	}


void CDaemonTestStep::FreeMemory()
	{
	// no memory to free
	}

} // namespace Swi::Test

} //namespace Swi
