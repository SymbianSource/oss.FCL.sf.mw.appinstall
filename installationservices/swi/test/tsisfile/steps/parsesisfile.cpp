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
* @file parsesisfile.cpp
*
*/


#include "parsesisfile.h"
#include "filesisdataprovider.h"
#include "dessisdataprovider.h"

#include "tsis.h"

#include "siscontents.h"
#include "siscontroller.h"
#include "sisparser.h"


using namespace Swi;
using namespace Swi::Sis;


//_LIT(KFileNameKey, "filename");
//_LIT(KTestDataDir, "z:\\tswi\\tsis\\data\\");

namespace Swi
{
  namespace Sis
  { 
    namespace Test
    {     
	   CParseStep::CParseStep(CTestParserServer& aParent) : iParent(aParent)
	   	{
	   	
	   	}
    

		EXPORT_C TVerdict CParseStep::doTestStepL()
			{
			__UHEAP_MARK;
								
			CContents* contents = NULL;
			TRAPD(err, contents = GetContentsL());
			CleanupStack::PushL(contents);
			
			if (err != KErrNone)
				{
				if (err == KErrCorrupt)
					{
					ERR_PRINTF1(_L("Failed to parse contents data: KErrCorrupt"));
					}
				else
					{
					ERR_PRINTF2(_L("Failed to read contents data: error %d"), err);					
					}
				SetTestStepResult(err);
				
				CleanupStack::PopAndDestroy(contents);
	
				return TestStepResult();				
				}	
				
			CController* controller = NULL;
			TRAP(err, controller = GetControllerL(*contents));
			if (err != KErrNone)
				{
				SetTestStepResult(err);
				delete controller;
				CleanupStack::PopAndDestroy(contents);
				return TestStepResult();
				}
			delete controller;
			CleanupStack::PopAndDestroy(contents);

			SetTestStepResult(KErrNone);
			
			
			delete iControllerData;
			iControllerData = NULL;
			delete iDataProvider; // To make the heap check pass :(
			iDataProvider = NULL;
			
			
			iExpectedResults.Close();				
				
			__UHEAP_MARKEND;
					
			return TestStepResult();
			}	
	} // namespace Test
	  
  } // namespace Sis
	
} // namespace Swi
