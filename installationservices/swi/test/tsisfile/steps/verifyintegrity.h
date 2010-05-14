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


#ifndef __VERIFYINTEGRITY_H__
#define __VERIFYINTEGRITY_H__


#include <test/testexecutestepbase.h>
#include "tsisstep.h"

_LIT(KVerifyIntegrityStep,"VerifyIntegrityStep");
_LIT(KNewVerifyIntegrityStep,"VerifyIntegrityStepNewParser");

namespace Swi
{
 namespace Sis
  {
  class CInstallBlock;
  class CFileDescription;
  
   namespace Test
   {
    class CTestParserServer;
    
    class CVerifyIntegrityStep : public CParserTestStep
    {
	public:
	   CVerifyIntegrityStep(CTestParserServer& aParent, TBool aInPlace);
		
    public: // from CTestStep	
      IMPORT_C enum TVerdict	doTestStepL();
      IMPORT_C enum TVerdict	doTestStepPreambleL();
      IMPORT_C enum TVerdict	doTestStepPostambleL();
    
    protected:
	  TBool CheckFileDescriptionL(const CFileDescription& aFileDescription, 
	  							  CContents* aContents,
	  							  TUint32 aDataIndex);
    
   	  TBool CheckInstallBlockL(const CInstallBlock& aInstallBlock, 
   	  						   CContents* aContents, 
   	  						   TUint32 aParentDataIndex);    
      
    private:
    	CTestParserServer& iParent;
		CActiveScheduler	 iSched;
		TBool iInPlace;  	
    };
    
   }// namespace Test

 } // namespace Sis

} // namespace Swi

#endif // __VERIFYINTEGRITY_H__
