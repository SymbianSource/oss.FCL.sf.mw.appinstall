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
* @file verifysignature.cpp
*
*/


#include "verifysignature.h"
#include "tsis.h"
#include "filesisdataprovider.h"

#include "siscontents.h"
#include "sisstring.h"
#include "siscontroller.h"
#include "sisfiledescription.h"
#include "sisinstallblock.h"
#include "sishash.h"
#include "securitymanager.h"

#include <pkixvalidationresult.h>

using namespace Swi;
using namespace Swi::Sis;


_LIT(KExpectedValidationResultKey, "validationresult");

namespace Swi
{
	namespace Sis
	{ 

		namespace Test
		{ 
			CVerifySignatureStep::CVerifySignatureStep(CTestParserServer& /*aParent*/)
				{	  
				}  
				
			CVerifySignatureStep::~CVerifySignatureStep()
				{
				if (iActiveStep)	
					{
					delete iActiveStep;					
					}
				if (iSched)
					{
					delete iSched;						
					}
				}
		
		
			EXPORT_C enum TVerdict CVerifySignatureStep::doTestStepPreambleL()
				{
				TInt result = KErrNone;
				TBool found = GetIntFromConfig(ConfigSection(), KExpectedValidationResultKey, result);
				User::LeaveIfError(iExpectedResults.Append(result));

				if (!found)
					{				
					WARN_PRINTF1(_L("Missing Expected result defaulting to KErrNone"));	
					}		
				else
					{
					TBuf<128> mymsg;
					
					mymsg.Format(_L("Expected result: (%d)"), iExpectedResults[0]);

					INFO_PRINTF1(mymsg);
					}
					
				if (CActiveScheduler::Current() == NULL)
				    {
				    INFO_PRINTF1(_L("Installing scheduler"));
				    iSched = new(ELeave) CActiveScheduler;
				    CActiveScheduler::Install (iSched);
				    }		
				    			    				    
				return TestStepResult();		
				}
				
			EXPORT_C enum TVerdict CVerifySignatureStep::doTestStepPostambleL()
				{			
			    INFO_PRINTF1(_L("Uninstalling scheduler"));
		    	CActiveScheduler::Install (NULL);
			
				return TestStepResult();
				}

			EXPORT_C TVerdict CVerifySignatureStep::doTestStepL()
				{	
				__UHEAP_MARK;
															
			    iActiveStep = new (ELeave) CVerifySignatureActiveStep(*this);
			    iActiveStep->doTestStepL();
			    CActiveScheduler::Start();
			    delete iActiveStep; 
			    iActiveStep = NULL;
			   	delete iControllerData;
				iControllerData = NULL;
			    delete iDataProvider;
			    iDataProvider = NULL;
				iExpectedResults.Close();			    
			    CSecurityPolicy::ReleaseResource();
			    __UHEAP_MARKEND;
			    
				return TestStepResult();			    	    
				}


			//
			// Active Step
			//

			CVerifySignatureActiveStep::CVerifySignatureActiveStep(CVerifySignatureStep& aParent) : CActive(EPriorityNormal), iParent(aParent)
				{
				CActiveScheduler::Add(this);
				}

			CVerifySignatureActiveStep::~CVerifySignatureActiveStep()
				{
				delete iSecMan;									
				
				if (iController)
					{
					delete iController;      // Make heap marks happy					
					}
				if (iControllerData)
					{
					delete iControllerData;					
					}
				if (iContents)
					{
					delete iContents;
					}

				iEndCertificates.ResetAndDestroy();			

				iPkixResults.ResetAndDestroy();
				}

			void CVerifySignatureActiveStep::doTestStepL()
				{
				iSecMan = CSecurityManager::NewL();
			    SetActive();		
			    TRequestStatus* status = &iStatus;		    
			    User::RequestComplete(status, KErrNone);
			    iState = EVerify;				
				}
				

			void CVerifySignatureActiveStep::DoVerifyL()
				{		
				TRAPD(err, iContents = iParent.GetContentsL());
						
				if ((err != KErrNone) || (iContents == NULL))
					{
					iParent.ERR_PRINTF1(_L("The contents could not be read!"));				
					iParent.SetTestStepResult(err);
					User::Leave(err);				
					return;
					}
					
				TRAP(err, iControllerData = iContents->ReadControllerL());

				if ((err != KErrNone) || (iControllerData == NULL))
					{
					iParent.ERR_PRINTF1(_L("The controller data could not be read!"));				
					iParent.SetTestStepResult(err);	
					User::Leave(err);												
					return;
					}

				TRAP(err, iController = iParent.GetControllerL(*iContents));

				if (iController == NULL)
					{
					iParent.ERR_PRINTF1(_L("The controller could not be read on the previous step!"));				
					iParent.SetTestStepResult(err);				
					User::Leave(err);				
					return;
					}	
				TPtr8 controllerBuf(iControllerData->Des());
				TBool embeddedController = EFalse;
				iSecMan->VerifyControllerL(controllerBuf, *iController, &iResult, 
				                           iPkixResults, iEndCertificates, 
				                           &iCapabilitySet, iAllowUnsigned, embeddedController,
										   iStatus);
				SetActive();				
				}				
				
		  	void CVerifySignatureActiveStep::RunL()
		  		{
				if (iStatus.Int() != KErrNone)
					{
					iParent.SetTestStepResult(EFail);
					iParent.ERR_PRINTF2(_L("Cannot verify the signature = %d"), iStatus.Int());						

					CActiveScheduler::Stop();
					return;
					}
					
				switch (iState)
					{
					case EVerify:
						{
						iState = EDone;
						DoVerifyL();
						}
						break;

					case EDone:
						{			
						for (TInt k = 0; k < iPkixResults.Count(); k++)
							{
							TValidationStatus err = iPkixResults[k]->Error();
							if (err.iReason != EValidatedOK)
								{
								iParent.ERR_PRINTF2(_L("Validation failed with code %d"), err.iReason);								
								}
							}

						// Print result in logs
						
						switch (iResult)
							{
						case EValidationSucceeded:
							iParent.INFO_PRINTF1(_L("\tResult:\tEValidationSucceeded"));
					      break;
					    case ESignatureSelfSigned:
					    	iParent.INFO_PRINTF1(_L("\tResult:\tESignatureSelfSigned"));
					    	break;
					 	case ENoCertificate:
							iParent.INFO_PRINTF1(_L("\tResult:\tENoCertificate"));
							break;
						case ECertificateValidationError:
							iParent.INFO_PRINTF1(_L("\tResult:\tECertificateValidationError"));
					      break;
						case ESignatureNotPresent:
							iParent.INFO_PRINTF1(_L("\tResult:\tESignatureNotPresent"));
					      break;
						case ESignatureCouldNotBeValidated:
							iParent.INFO_PRINTF1(_L("\tResult:\tESignatureCouldNotBeValidated"));
					      break;
						case ENoCodeSigningExtension:
							iParent.INFO_PRINTF1(_L("\tResult:\tENoCodeSigningExtension"));
					      break;
					 	case ENoSupportedPolicyExtension:
							iParent.INFO_PRINTF1(_L("\tResult:\tENoSupportedPolicyExtension"));
					      break;
						case EMandatorySignatureMissing:
							iParent.INFO_PRINTF1(_L("\tResult:\tEMandatorySignatureMissing"));
					      break;
					 	default:
					 		iParent.INFO_PRINTF2(_L("\tResult:\tUnknown (%d)"), iResult);
					      break;
					 		};


						iParent.SetTestStepResult(iResult);	
						CActiveScheduler::Stop();					
						if (iController)
							{
							delete iController;      // Make heap marks happy	
							iController = NULL;				
							}
						if (iControllerData)
							{
							delete iControllerData;					
							iControllerData = NULL;
							}
						if (iContents)
							{
							delete iContents;
							iContents = NULL;
							}							
						}
						break;

					default:
						{
						iParent.ERR_PRINTF1(_L("Reached Illegal State!"));										
						iParent.SetTestStepResult(EFail);						
						}
					}		  		
		  		}

			void CVerifySignatureActiveStep::DoCancel()
				{
				
				}

			TInt CVerifySignatureActiveStep::RunError(TInt aError)
				{
				iParent.ERR_PRINTF2(_L("An error occured while verifying the signature: %d"), aError);
				iParent.SetTestStepResult(aError);
				CActiveScheduler::Stop();
				return KErrNone;				
				}
			
		} // namespace Test
	} // namespace Sis
} // namespace Swi
