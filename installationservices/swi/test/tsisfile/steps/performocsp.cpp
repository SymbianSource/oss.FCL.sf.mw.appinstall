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
* @file performocsp.cpp
*
*/


#include "performocsp.h"
#include "tsis.h"
#include "filesisdataprovider.h"

#include "siscontents.h"
#include "sisstring.h"
#include "siscontroller.h"
#include "sisfiledescription.h"
#include "sisinstallblock.h"
#include "sishash.h"
#include "securitymanager.h"


using namespace Swi;
using namespace Swi::Sis;


_LIT(KExpectedValidationResultKey, "validationresult");
//_LIT8(KDefaultOCSPUri, "http:\\cam-ocsptest01:19000");

namespace Swi
{
	namespace Sis
	{ 

		namespace Test
		{ 
			CPerformOCSPStep::CPerformOCSPStep(CTestParserServer& aParent) : CVerifySignatureStep(aParent)
				{	  
				}  
				
			CPerformOCSPStep::~CPerformOCSPStep()
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
		
		
			EXPORT_C enum TVerdict CPerformOCSPStep::doTestStepPreambleL()
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
				
			EXPORT_C enum TVerdict CPerformOCSPStep::doTestStepPostambleL()
				{			
			    INFO_PRINTF1(_L("Uninstalling scheduler"));
		    	CActiveScheduler::Install (NULL);
			
				return TestStepResult();
				}

			EXPORT_C TVerdict CPerformOCSPStep::doTestStepL()
				{	
				__UHEAP_MARK;
															
			    iActiveStep = new (ELeave) CPerformOCSPActiveStep(*this);
			    iActiveStep->doTestStepL();
			    CActiveScheduler::Start();
			    delete iActiveStep;
			    iActiveStep = NULL;
			    delete iDataProvider;
			    iDataProvider = NULL;
				iExpectedResults.Close();			    
			    			    
			    __UHEAP_MARKEND;
			    
				return TestStepResult();			    	    
				}


			//
			// Active Step
			//

			CPerformOCSPActiveStep::CPerformOCSPActiveStep(CPerformOCSPStep& aParent) : CVerifySignatureActiveStep(aParent), iParent(aParent)
				{
				//CActiveScheduler::Add(this);
				}

			CPerformOCSPActiveStep::~CPerformOCSPActiveStep()
				{
				delete iSecMan;		

			    iEndCertificates.ResetAndDestroy();	
			    iOutcome.ResetAndDestroy();	
			    
				iTocspSession.Close();
				}

			void CPerformOCSPActiveStep::doTestStepL()
				{
				iSecMan = CSecurityManager::NewL();
			    SetActive();		
			    TRequestStatus* status = &iStatus;		    
			    User::RequestComplete(status, KErrNone);
			    iState = EVerify;				
				}
				

			void CPerformOCSPActiveStep::DoCheckL()
				{		
				// Start the test OCSP server	
				User::LeaveIfError(iTocspSession.Connect());
				iTocspSession.Check();			

				//SetActive();				
				}				
				
		  	void CPerformOCSPActiveStep::RunL()
		  		{
				if (iStatus.Int() != KErrNone)
					{
					iParent.SetTestStepResult(EFail);
					iParent.ERR_PRINTF2(_L("Cannot complete OCSP check = %d"), iStatus.Int());						

					CActiveScheduler::Stop();
					return;
					}
					
				switch (iState)
					{
					case EVerify:
						{
						iState = EChecking;
						DoVerifyL();
						}
						break;
											
					case EChecking:
						{
						iState = EDone;
						DoCheckL();
						}
						break;

					case EDone:
						{			
						// Check the results								
						//iParent.SetTestStepResult(iResult);	
						CActiveScheduler::Stop();											
						}
						break;

					default:
						{
						iParent.ERR_PRINTF1(_L("Reached Illegal State!"));										
						iParent.SetTestStepResult(EFail);						
						}
					}		  		
		  		}

			void CPerformOCSPActiveStep::DoCancel()
				{
				
				}

			TInt CPerformOCSPActiveStep::RunError(TInt aError)
				{
				if (iState == EVerify)
					{
					iParent.ERR_PRINTF2(_L("An error occured while verifying the signature: %d"), aError);									
					}
				else
					{
					iParent.ERR_PRINTF2(_L("An error occured while performing OCSP checks: %d"), aError);					
					}
				iParent.SetTestStepResult(aError);
				CActiveScheduler::Stop();
				return KErrNone;				
				}
			
		} // namespace Test
	} // namespace Sis
} // namespace Swi
