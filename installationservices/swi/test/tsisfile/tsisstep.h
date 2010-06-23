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


#ifndef __TSISSTEP_H__
#define __TSISSTEP_H__

#include <test/testexecutestepbase.h>

#include "siscontents.h"
#include "siscontroller.h"

namespace Swi
{
  class CFileSisDataProvider;
  
  namespace Sis
  { 

    namespace Test
    {

      class CParserTestStep : public CTestStep
      {
      public:
      	virtual ~CParserTestStep();
      	
      public:
		IMPORT_C enum TVerdict	doTestStepPreambleL();

		void SetTestStepResult(TInt result);
      
      	CContents* GetContentsL();       	
		CController* GetControllerL(CContents& contents, TBool aUseInPlaceParse = EFalse);
		void SetExpectedResult();
      	
      protected:
      	CFileSisDataProvider* iDataProvider;
      	RFs iFs;
      	RArray<TInt> iExpectedResults;
      	HBufC8* iControllerData;
      };

    } // namespace Test

  } // namespace Sis

} // namespace Swi


#endif // __TSISSTEP_H__
