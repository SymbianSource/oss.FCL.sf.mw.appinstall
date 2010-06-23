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
#include "swi/sistruststatus.h"	// Req for trustStatus
#include "swi/sisregistrysession.h" //Connecting
#include "swi/sisregistryentry.h" // entry



CTStepPirCheck::CTStepPirCheck()
	{
	SetTestStepName(KTStepPirCheck);
	}

TVerdict CTStepPirCheck::doTestStepL()
	{
__UHEAP_MARK;		// Check for memory leaks
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
		
	Swi::RSisRegistrySession regSession;
	Swi::RSisRevocationEntry revEntry;
	
	User::LeaveIfError(regSession.Connect());
	CleanupClosePushL(regSession);
		
	while (NextPackage())
		{
		
		TBool installed = regSession.IsInstalledL(PackageUID());
		if (!installed)
			{
			INFO_PRINTF1(_L("Package is absent. Exiting function"));
			SetTestStepResult(EFail);
			}
		else
			{
			TRequestStatus reqStatus;	//for async revocationCheck
			Swi::TSisTrustStatus trustStatus; 
			TPtrC ocsp_uriValue;
			TBool synchronousCheckValue;
			TBool cancelValue = EFalse;
			
			const TUint KMaxUriLength=512;
			TBuf8<KMaxUriLength> tempUriValue;
			_LIT8(KOcspServerDefaultAddress, "cam-ocsptest01.intra:19003");	
			
			User::LeaveIfError(revEntry.Open(regSession, PackageUID()));
			CleanupClosePushL(revEntry);
			
			if(GetStringFromConfig(ConfigSection(),_L("ocsp_uri"), ocsp_uriValue) != EFalse) //Is this tag present?
				{
				//The string has been retrieved successfully
							
				
				//Use data directly from input-file	
				tempUriValue.Copy(ocsp_uriValue);						
				}
			else
				{
				// use the default value for the ocsp_uri
				tempUriValue.Copy(KOcspServerDefaultAddress);
                }
			

			
			if(GetBoolFromConfig(ConfigSection(), _L("SynchronousCheck"), synchronousCheckValue) == EFalse) //Is this tag absent?
				{
				//Use the sync version as the default if a preference was not specified in the ini file
				revEntry.CheckRevocationStatusL(tempUriValue);
				}	
				
			else	//The tag "SynchronousCheck" is present
				{
				//The tag is present, but otherwise, the sync version is run
				if (synchronousCheckValue)
					{
					//Do the sync version
					revEntry.CheckRevocationStatusL(tempUriValue);					
					}
				
				else
					{
					//do the async version of the PIR check
					
					if( (GetBoolFromConfig(ConfigSection(), _L("Cancel"), cancelValue) != EFalse) && (cancelValue != EFalse) )
						{									
						//The section will perform a PIR check, but cancel it after a very short delay
						
						TTimeIntervalMicroSeconds32 timeInterval = 20;	//20 Microseconds
						reqStatus = KRequestPending;						
						revEntry.CheckRevocationStatus(tempUriValue, reqStatus);					
						User::After(timeInterval);
						
						// only cancel if we know that there is an outstanding request    				    				
						if (reqStatus == KRequestPending)
							{
							TInt result1 = 0;
							TRAP (result1, revEntry.CancelRevocationStatusL());					 							
							
							//Check for a leave
							if (KErrNone != result1)
								{
								INFO_PRINTF2(_L("CancelRevocationStatusL 'left'. result = %d"), result1);
								SetTestStepResult(EFail);						
								}								
							}
						
						else
							{
							// Don't cancel the request, wait as normal
							User::WaitForRequest(reqStatus); //delay
							INFO_PRINTF2(_L("Status not changed properly. reqStatus = %d"), reqStatus.Int());
							}
							
						// Check reqStatus following the check
						switch(reqStatus.Int())
							{					
							case KErrNone:
								// The request had completed already
								INFO_PRINTF2(_L("Request had completed already. reqStatus = %d"), reqStatus.Int());	
								SetTestStepResult(EPass);
								break;
								
							case KErrCancel:
								// The request returned as expected
								INFO_PRINTF2(_L("Cancel was ok. reqStatus = %d"), reqStatus.Int());
								SetTestStepResult(EPass);
								break;
							
							default:
								// The request failed
								INFO_PRINTF2(_L("Cancel Request failed. reqStatus = %d"), reqStatus.Int());
								SetTestStepResult(EFail);
								break;
							}						
						}
						
					else
						{
						//the tag "Cancel" was absent, or was set to false, so the PIR check will not be cancelled during the operation
						reqStatus = KRequestPending;
						
						revEntry.CheckRevocationStatus(tempUriValue, reqStatus);
						User::WaitForRequest(reqStatus); //delay
						
						//Check reqStatus
						if (reqStatus.Int() == KErrNone)	//The request completed ok
							{					
							INFO_PRINTF2(_L("Request completed ok. reqStatus = %d"), reqStatus.Int());
							SetTestStepResult(EPass);
							}
						else 	// The request failed
							{					
							INFO_PRINTF2(_L("Request failed. reqStatus = %d"), reqStatus.Int());
							SetTestStepResult(EFail);
							}
						}//End of state where the "Cancel" tag was absent
					}//End of case where "SynchronousCheck" is True				
				}//End of case where "SynchronousCheck" tag is present
				
			//The check for trustStatus is done in step_SisRegCheck.cpp						
			
			CleanupStack::PopAndDestroy(&revEntry); 		
			}//End of 'else' statement, ie when the package is present
		}//End of while loop
	
	CleanupStack::PopAndDestroy(&regSession); // regSession, in that order;

__UHEAP_MARKEND;
	return TestStepResult();
	}
