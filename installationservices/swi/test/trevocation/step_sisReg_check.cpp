/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "trevocationStep.h"
#include <test/testexecutelog.h>

#include "swi/sisrevocationentry.h"
#include "swi/sistruststatus.h"	// Req for TRequestStatus
#include "swi/sisregistrysession.h" //Connecting
#include "swi/sisregistryentry.h" // entry



CTStepSisRegCheck::CTStepSisRegCheck()
	{
	SetTestStepName(KTStepSisRegCheck);
	}

TVerdict CTStepSisRegCheck::doTestStepL()
	{
	//If any test step leaves or panics, the test step thread exits and no further calls are made. 
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}

__UHEAP_MARK;		// Check for memory leaks
	
	
	SetTestStepResult(EPass);	
		
	Swi::RSisRegistrySession regSession;
	Swi::RSisRevocationEntry revEntry;

	User::LeaveIfError(regSession.Connect());
	CleanupClosePushL(regSession);

	while(NextPackage())
		{								
		
		//If the package is absent, exit			
		TBool installed = regSession.IsInstalledL(PackageUID());
		if (!installed)
			{
			INFO_PRINTF1(_L("Package is absent. Exiting function"));
			SetTestStepResult(EFail);
			}
		else
			{
			Swi::TSisTrustStatus trustStatus; 
			
			User::LeaveIfError(revEntry.Open(regSession, PackageUID()));
			CleanupClosePushL(revEntry);
			
			trustStatus = revEntry.TrustStatusL();
						
			TBool isTrustedValue;
			TPtrC validationStatusValue, revocationStatusValue;
			TInt resultDateValue;		
			TInt lastCheckDateValue;		
			TBool sameDateValue;
		
			
			if (GetBoolFromConfig(ConfigSection(),_L("isTrusted"), isTrustedValue) != EFalse) // the tag 'isTrusted' was present
				{
				//Do the compare
				
				if (trustStatus.IsTrusted() != isTrustedValue)
					{					
					ERR_PRINTF2(_L("isTrustedValue differs from expected, isTrustedValue = %d"), trustStatus.IsTrusted());
					SetTestStepResult(EFail);
					}
				}

			if (GetStringFromConfig(ConfigSection(),_L("validationStatus"), validationStatusValue) != EFalse) // the tag 'validationStatus' was present
				{
				//Do the compare
				TBuf<40> enumEquivalent = GetEnumValidationValue(trustStatus.ValidationStatus());
				
				if (enumEquivalent != validationStatusValue)
					{
					ERR_PRINTF3(_L("validationStatusValue differs from expected. validationStatusValue = %S, Expected = %S"), &enumEquivalent, &validationStatusValue);
					SetTestStepResult(EFail);
					}
				}
			
			if (GetStringFromConfig(ConfigSection(),_L("revocationStatus"), revocationStatusValue) != EFalse) // the tag 'revocationStatus' was present
				{
				//Do the compare
				TBuf<40> enumEquivalent = GetEnumRevocationValue(trustStatus.RevocationStatus());
				
				if (enumEquivalent != revocationStatusValue)
					{
					ERR_PRINTF3(_L("revocationStatusValue differs from expected, Value = %S, Expected = %S"), &enumEquivalent, &revocationStatusValue);
					SetTestStepResult(EFail);
					}
				}				
		
			
			if (GetIntFromConfig(ConfigSection(),_L("resultDate"), resultDateValue) != EFalse) // the tag 'resultDate' was present
				{
				//Do the compare
				if (trustStatus.ResultDate() != TTime((TInt64)resultDateValue) )
					{
					TBuf<30> resultDateString;
					_LIT(KDateString4,"%-B%:0%J%:1%T%:2%S%.%*C4%:3%+B");					
									
					trustStatus.ResultDate().FormatL(resultDateString,KDateString4);
					ERR_PRINTF3(_L("resultDateValue differs from expected, Value = %S, Expected = %d"), &resultDateString, resultDateValue);
					SetTestStepResult(EFail);
					}
				}
				
			if (GetIntFromConfig(ConfigSection(),_L("lastCheckDate"), lastCheckDateValue) != EFalse) // the tag 'lastCheckDate' was present
				{
				
				//Do the compare
				if (trustStatus.LastCheckDate() != TTime((TInt64)lastCheckDateValue) )
					{
					TBuf<30> lastCheckDateString;					
					_LIT(KDateString4,"%-B%:0%J%:1%T%:2%S%.%*C4%:3%+B");
					trustStatus.LastCheckDate().FormatL(lastCheckDateString,KDateString4);
				
					ERR_PRINTF3(_L("lastCheckDateValue differs from expected, Value = %S, Expected = %d"), &lastCheckDateString, lastCheckDateValue);
					SetTestStepResult(EFail);
					}
				}
			
			if (GetBoolFromConfig(ConfigSection(),_L("sameDate"), sameDateValue) != EFalse) // the tag 'sameDate' was present
				{
				
				TBuf<30> lastCheckDateString;
				TBuf<30> resultDateString;
				_LIT(KDateString4,"%-B%:0%J%:1%T%:2%S%.%*C4%:3%+B");
				// dateString contains "11:59:59.9999 pm", for example - first and third time separators are both '\0'; 
				// locale dependent decimal separator separates seconds and microseconds
				
				
				trustStatus.LastCheckDate().FormatL(lastCheckDateString,KDateString4);
				trustStatus.ResultDate().FormatL(resultDateString,KDateString4);
				
				if (sameDateValue) //ie true, the dates should be the same..
					{
					if ( lastCheckDateString != resultDateString )
						{						
						ERR_PRINTF3(_L("The dates differ, LastCheckDate = %S, ResultDate = %S"), &lastCheckDateString, &resultDateString);
						SetTestStepResult(EFail);
						}
					}		
				
				else	//They should be the different
					{
					if ( lastCheckDateString == resultDateString )
						{						
						ERR_PRINTF3(_L("The dates are the same, LastCheckDate = %S, ResultDate = %S"), &lastCheckDateString, &resultDateString);
						SetTestStepResult(EFail);
						}
					}						
				}	//sameDateValue check
				
				
			CleanupStack::PopAndDestroy(&revEntry);
			}	//End of else statement

		} //End of while loop	
	
	CleanupStack::PopAndDestroy(&regSession); // revEntry, regSession, in that order	

__UHEAP_MARKEND;
	return TestStepResult();
	}


TBufC<40> CTStepSisRegCheck::GetEnumValidationValue (TInt input)
{
	
	TBuf<40> bufOutput;
	switch (input)
		{
		case (Swi::EUnknown):							
			_LIT(KText01, "EUnknown");
			bufOutput = KText01;
			break;
							
		case (Swi::EExpired):						
			_LIT(KText02, "EExpired");
			bufOutput = KText02;
			break;
			
		case (Swi::EInvalid):							
			_LIT(KText03, "EInvalid");
			bufOutput = KText03;
			break;
			
		case (Swi::EUnsigned):							
			_LIT(KText04, "EUnsigned");			
			bufOutput = KText04;
			break;
			
		case (Swi::EValidated):							
			_LIT(KText05, "EValidated");			
			bufOutput = KText05;
			break;			
			
		case (Swi::EValidatedToAnchor):							
			_LIT(KText06, "EValidatedToAnchor");
			bufOutput = KText06;
			break;
			
		case (Swi::EPackageInRom):							
			_LIT(KText07, "EPackageInRom");			
			bufOutput = KText07;
			break;
						
		default:
			//INFO_PRINTF1(_L("Was unable to interpret the Validation Enum value in the SisRegistry"));			
			_LIT(KText08, "UnKnownEnum");
			bufOutput = KText08;
			break;
					
		}
	return bufOutput;
}

TBufC<40> CTStepSisRegCheck::GetEnumRevocationValue (TInt input)
{	
	TBuf<40> bufOutput;
	switch (input)
		{
		case (Swi::EUnknown2):							
			_LIT(KText01, "EUnknown2");
			bufOutput = KText01;
			break;
							
		case (Swi::EOcspNotPerformed):						
			_LIT(KText02, "EOcspNotPerformed");
			bufOutput = KText02;
			break;
			
		case (Swi::EOcspRevoked):							
			_LIT(KText03, "EOcspRevoked");
			bufOutput = KText03;
			break;
			
		case (Swi::EOcspUnknown):							
			_LIT(KText04, "EOcspUnknown");			
			bufOutput = KText04;
			break;
			
		case (Swi::EOcspTransient):							
			_LIT(KText05, "EOcspTransient");			
			bufOutput = KText05;
			break;			
			
		case (Swi::EOcspGood):							
			_LIT(KText06, "EOcspGood");
			bufOutput = KText06;
			break;
						
		default:
			//INFO_PRINTF1(_L("Was unable to interpret the Revocation Enum value in the SisRegistry"));			
			_LIT(KText07, "UnKnownEnum");
			bufOutput = KText07;
			break;
					
		}
	return bufOutput;
}
