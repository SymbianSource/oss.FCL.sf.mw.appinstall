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
* @file recognize.cpp
*
*/


#include "recognize.h"
#include "filesisdataprovider.h"
#include "dessisdataprovider.h"
#include <e32property.h>
#include <e32uid.h>

#include "siscontents.h"
#include "siscontroller.h"

#ifndef SWI_TEXTSHELL_ROM
	#include "recsisx.h"
	#include <apmrec.h>
	#include <apgcli.h>
#endif

_LIT(KFileNameKey, "filename");

using namespace Swi;
using namespace Swi::Sis;

_LIT(KTestDataDir, "z:\\tswi\\tsis\\data\\");

namespace Swi
{
  namespace Sis
  { 
    namespace Test
    { 

		EXPORT_C TVerdict CRecognizeStep::doTestStepL()
			{
			__UHEAP_MARK;
			
			SetTestStepResult(EFail);				
			
			HBufC* fileNameBuf = HBufC::NewMaxLC(KMaxFileName);
			TPtrC fileNamePtr(fileNameBuf->Des());
			TBool found = GetStringFromConfig(ConfigSection(), KFileNameKey, fileNamePtr);
	
			if (!found)
				{				
				ERR_PRINTF1(_L("Missing file name key in .ini file!"));	
				SetTestStepResult(EInconclusive);
				return TestStepResult();								
				}
			
			TFileName fileName(KTestDataDir);

			fileName.Append(fileNamePtr);
				
			RFs fileSession;
			CleanupClosePushL(fileSession);
			TInt err = fileSession.Connect();
			if (err != KErrNone)
				{
				ERR_PRINTF1(_L("Failed to open a file session"));
				SetTestStepResult(EInconclusive);				
				return TestStepResult();				
				}
			CFileSisDataProvider* dataProvider = CFileSisDataProvider::NewLC(fileSession, fileName);
	
			TInt connectErr = KErrNotFound;
		#ifndef SWI_TEXTSHELL_ROM
			//CApaSisxRecognizer* rec= new (ELeave) CApaSisxRecognizer;
			//CleanupStack::PushL(rec);			
			RApaLsSession apaSession;
			connectErr = apaSession.Connect();
			if (connectErr != KErrNotFound)
				{
				User::LeaveIfError(connectErr);
				CleanupClosePushL(apaSession);
			
				TBuf8<64> buffer;
				dataProvider->Read(buffer);
			
				TDataRecognitionResult result;
			
				//rec->RecognizeL(fileName, buffer);
				User::LeaveIfError(apaSession.RecognizeData(fileName, buffer, result));
			
				if (result.iDataType.Des8() != KDataTypeSisx())
					{
					ERR_PRINTF1(_L("Recognised data incorrectly"));
					SetTestStepResult(EFail);				
					}
				else
					{
					TInt confidence = result.iConfidence;
					if ((confidence == CApaDataRecognizerType::EProbable) || (confidence == CApaDataRecognizerType::ECertain))
						{
						SetTestStepResult(EPass);				
						}
					else
						{
						SetTestStepResult(KErrCorrupt);
						}		
					}					
				CleanupStack::PopAndDestroy(&apaSession);	
				}
		#endif
			if (connectErr == KErrNotFound)
				{
				// emulator tests running in textshell or in textshell ROM (#def SWI_TEXTSHELL_ROM)
				TRAPD(error, RecognizeSisxL(fileName));
				if (error != KErrNone)
					{
					ERR_PRINTF1(_L("Recognised data incorrectly"));
					SetTestStepResult(EFail);				
					}
				else
					{
					SetTestStepResult(EPass);
					}
				}
			//CleanupStack::PopAndDestroy(rec);
			CleanupStack::PopAndDestroy(dataProvider);
			CleanupStack::PopAndDestroy(&fileSession);
			CleanupStack::PopAndDestroy(fileNameBuf);

			__UHEAP_MARKEND;

			return TestStepResult();
			}
		
	void CRecognizeStep::RecognizeSisxL(TDesC& aFileName)
		{
		RFile file;
		User::LeaveIfError(file.Open(iFs, aFileName, EFileRead | EFileStream | EFileShareReadersOnly));
		CleanupClosePushL(file);
		TUid uid1, uid2, uid3;
		// This will leave if the file is not a proper sis file
		ReadSymbianHeaderL(file, uid1, uid2, uid3);
		// A proper sisx file had this uid
		const TUid KUidSisxFile = {0x10201A7A};
		if (uid1 != KUidSisxFile)
			{
			User::Leave(KErrCorrupt);
			}
		CleanupStack::PopAndDestroy(&file);
		}
	
	void CRecognizeStep::ReadSymbianHeaderL(RFile& aFile, TUid& aUid1, TUid& aUid2, TUid& aUid3)
		{
		TInt uidLen =  sizeof(TInt32);
		
		TPckg<TInt32> uid1(aUid1.iUid);
		User::LeaveIfError(aFile.Read(uid1, uidLen));
		if (uid1.Length() != uidLen)
			{
			User::Leave(KErrUnderflow);
			}
			
		TPckg<TInt32> uid2(aUid2.iUid);
		User::LeaveIfError(aFile.Read(uid2, uidLen));
		if (uid1.Length() != uidLen)
			{
			User::Leave(KErrUnderflow);
			}
			
		TPckg<TInt32> uid3(aUid3.iUid);
		User::LeaveIfError(aFile.Read(uid3, uidLen));
		if (uid1.Length() != uidLen)
			{
			User::Leave(KErrUnderflow);
			}
	
		}
	
	} // namespace Test
	  
  } // namespace Sis
	
} // namespace Swi
