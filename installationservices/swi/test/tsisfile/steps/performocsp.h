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
*
*/


#ifndef __PERFORMOCSP_H__
#define __PERFORMOCSP_H__

#include <test/testexecutestepbase.h>
#include <swi/msisuihandlers.h>

#include "tsisstep.h"
#include "verifysignature.h"
#include "tocspclientserver.h"


_LIT(KPerformOCSPStep,"PerformOCSPStep");

namespace Swi
{
  class CSecurityManager;
  class CController;
  class CContents;
  
  namespace Sis
  {
    namespace Test
    {
      class CPerformOCSPActiveStep;
      class CTestParserServer;
    
      class CPerformOCSPStep : public CVerifySignatureStep
      {
      public:
		CPerformOCSPStep(CTestParserServer& aParent);
		~CPerformOCSPStep();
		
      public: // from CTestStep	
		IMPORT_C enum TVerdict doTestStepL();
		IMPORT_C enum TVerdict doTestStepPreambleL();
		IMPORT_C enum TVerdict doTestStepPostambleL();
		
      private:
		CActiveScheduler*	       		iSched;
      	CPerformOCSPActiveStep* 		iActiveStep;
      };

      class CPerformOCSPActiveStep : public CVerifySignatureActiveStep
      {
	  public:
	  	CPerformOCSPActiveStep(CPerformOCSPStep& aParent);
		~CPerformOCSPActiveStep();  

	  public:
		void doTestStepL();
		void DoCheckL();
	
	  public: // from CActive
	  	void RunL();
		void DoCancel();
		TInt RunError(TInt aError);
		
	  private:	
		TRevocationDialogMessage 		iRevocationMsg;
		RPointerArray<TOCSPOutcome>		iOutcome;
		Swi::Test::ROcspServerSession 	iTocspSession; 		
      	
      	enum TState
      		{
      		EVerify,
      		EChecking,
      		EDone
      		} iState;
      			  
	  	CPerformOCSPStep& iParent;    	
      };

    
    }// namespace Test

  } // namespace Sis

} // namespace Swi


#endif // __PERFORMOCSP_H__
