/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* verifysiganture.h
*
*/


#ifndef __VERIFYSIGNATURE_H__
#define __VERIFYSIGNATURE_H__


#include <test/testexecutestepbase.h>
#include <swi/msisuihandlers.h>

#include "tsisstep.h"

_LIT(KVerifySignatureStep,"VerifySignatureStep");

class CPKIXValidationResult;

namespace Swi
{
  class CSecurityManager;
  class CController;
  class CContents;
  
  namespace Sis
  {
    namespace Test
    {
      class CVerifySignatureActiveStep;
      class CTestParserServer;
    
      class CVerifySignatureStep : public CParserTestStep
      {
      public:
		CVerifySignatureStep(CTestParserServer& aParent);
		~CVerifySignatureStep();
		
      public: // from CTestStep	
		IMPORT_C enum TVerdict doTestStepL();
		IMPORT_C enum TVerdict doTestStepPreambleL();
		IMPORT_C enum TVerdict doTestStepPostambleL();
		
      private:
		CActiveScheduler*	        iSched;
      	CVerifySignatureActiveStep* iActiveStep;
      };

      class CVerifySignatureActiveStep : public CActive
      {
	  public:
	  	CVerifySignatureActiveStep(CVerifySignatureStep& aParent);
		~CVerifySignatureActiveStep();  

	  public:
		void doTestStepL();
		void DoVerifyL();
	
	  public: // from CActive
	  	void RunL();
		void DoCancel();
		TInt RunError(TInt aError);
		
	  protected:
		TSignatureValidationResult iResult;
		
		CSecurityManager*    iSecMan;
		CController* 		 iController;
		CContents*			 iContents;
		HBufC8* 		 	 iControllerData;
		
		RPointerArray<CX509Certificate> iEndCertificates;
		RPointerArray<CPKIXValidationResultBase> iPkixResults;
		TBool                iAllowUnsigned;
		TCapabilitySet	 	 iCapabilitySet;
		
      	enum TState
      		{
      		EVerify,
      		EDone
      		} iState;
      			  
	  	CVerifySignatureStep& iParent;    	
      };

    
    }// namespace Test

  } // namespace Sis

} // namespace Swi

#endif // __VERIFYSIGNATURE_H__
