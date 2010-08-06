/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tsisstep.h"
#include "filesisdataprovider.h"
#include "dessisdataprovider.h"

#include "siscontents.h"
#include "siscontroller.h"
#include "sisparser.h"
#include "sisptrprovider.h"

_LIT(KFileNameKey, "filename");
_LIT(KExpectedResultKeyFormat, "result%d");
const TInt KResultKeyMaxLength = 8;
const TInt KResultKeyMaxNum = 99;
const TInt KErrMaxLength = 256; // equivalent to KEikErrorResolverMaxTextLength = 256;
using namespace Swi;
using namespace Swi::Sis;


_LIT(KTestDataDir, "z:\\tswi\\tsis\\data\\");

namespace Swi
{
  namespace Sis
  { 
    namespace Test
    { 
    	CParserTestStep::~CParserTestStep()
    		{
    		if (iDataProvider)
    			{
    			delete iDataProvider;
    			}
    		iExpectedResults.Close();
    		iFs.Close();
    		delete iControllerData;
    		}
    		
		void CParserTestStep::SetTestStepResult(TInt result)
			{
			TBool isExpected = EFalse;
			
			for (TInt k = 0; k < iExpectedResults.Count(); k++)
				{
				if (result == iExpectedResults[k])
					{
					isExpected = ETrue;
					}
				
				}
			
			if (!isExpected)
				{
				INFO_PRINTF2(_L("return set to %d"), result);				
				CTestStep::SetTestStepResult(EFail);
				}
			else 
				{
				CTestStep::SetTestStepResult(EPass);								
				}
								
			switch (result)
				{
				case EInconclusive :
					{
					CTestStep::SetTestStepResult(EInconclusive);									
					}
					break;
														
				case EAbort :
					{
					CTestStep::SetTestStepResult(EAbort);				
					}
					break;					
					
				default :
					{
					// Nothing 
					}
				}
			}
    		
    		
		CContents* CParserTestStep::GetContentsL()
			{
			HBufC* fileNameBuf = HBufC::NewMaxLC(KMaxFileName);
			TPtrC fileNamePtr(fileNameBuf->Des());
			TBool found = GetStringFromConfig(ConfigSection(), KFileNameKey, fileNamePtr);
	
			if (!found)
				{				
				ERR_PRINTF1(_L("Missing file name key in .ini file!"));	
				SetTestStepResult(EInconclusive);
				User::Leave(KErrNotFound);				
				}
			
			TFileName fileName(KTestDataDir);

			fileName.Append(fileNamePtr);
			CleanupStack::PopAndDestroy(fileNameBuf);
				
			TInt err = iFs.Connect();
			if (err != KErrNone)
				{
				ERR_PRINTF1(_L("Failed to open a file session"));
				SetTestStepResult(EInconclusive);	
				return NULL;				
				}
			iDataProvider = CFileSisDataProvider::NewL(iFs, fileName);
			
			CContents* contents = NULL;
			TRAP(err, contents = Parser::ContentsL(*iDataProvider));
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
				User::Leave(err);
				}
#ifdef SIS_CRC_CHECK_ENABLED			
			TRAP(err, contents->CheckCrcL());
			if (err == KErrCorrupt)
				{
				ERR_PRINTF1(_L("Crc values in SIS file are incorrect"));
				SetTestStepResult(err);		
				User::Leave(err);
				}
#endif  			
			CleanupStack::Pop(contents);	
			return contents;	
			
			}		
	CController* CParserTestStep::GetControllerL(CContents& contents, TBool aUseInPlaceParse)
		{
		TRAPD(err, iControllerData = contents.ReadControllerL());

		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Failed to read controller data: error %d"), err);
			SetTestStepResult(err);
			User::Leave(err);
			}
		
		CController* controller = NULL;
		
		if (aUseInPlaceParse)
			{
			TPtrProvider controllerProvider(iControllerData->Des());
			TRAP(err, controller = CController::NewL(controllerProvider));
			}
		else
			{

			CDesDataProvider* controllerProvider= CDesDataProvider::NewLC(*iControllerData);
			TRAP(err, controller = CController::NewL(*controllerProvider));
			CleanupStack::PopAndDestroy(controllerProvider);
			}
			
		CleanupStack::PushL(controller);
			
		if (err != KErrNone)
			{
			if (err == KErrCorrupt)
				{
				ERR_PRINTF1(_L("Failed to parse controller data: KErrCorrupt"));
				}
			else
				{
				ERR_PRINTF2(_L("Failed to parse controller data: error %d"), err);
				}				
			SetTestStepResult(err);
			User::Leave(err);
			}
			
		CleanupStack::Pop(controller);			
		return controller;
		}
				
	EXPORT_C enum TVerdict CParserTestStep::doTestStepPreambleL()
		{
		SetExpectedResult();
		return TestStepResult();		
		}


	void CParserTestStep::SetExpectedResult()
		{
		TInt result = KErrNone;
		TBuf<KResultKeyMaxLength> resultKey;
		TBool found = ETrue;
		
		for (TInt i = 1; i <= KResultKeyMaxNum && found; i++)
			{
			resultKey.Format(KExpectedResultKeyFormat, i);
			found = GetIntFromConfig(ConfigSection(), resultKey, result);
			if (found)
				User::LeaveIfError(iExpectedResults.Append(result));
			}
	
		if (iExpectedResults.Count() == 0)
			{				
			WARN_PRINTF1(_L("Missing Expected result defaulting to KErrNone"));	
			}		
		else
			{
			TBuf<128> mymsg;

			TBuf<KErrMaxLength> errText;
			for (TInt k = 0; k < iExpectedResults.Count(); k++)
				{
				if (iExpectedResults[k] != KErrNone)
					{
					errText.Format(_L("%d"), iExpectedResults[k]);
					}
				else
					{
					errText.Format(_L("KErrNone"));
					}
					
				mymsg.Format(_L("Expected result: %S (%d)"), &errText, iExpectedResults[k]);

				INFO_PRINTF1(mymsg);				
				}
			}
		}
	} // namespace Test
	  
  } // namespace Sis
	
} // namespace Swi
