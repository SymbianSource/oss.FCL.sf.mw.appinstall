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
* @file verifyintegrity.cpp
*
*/


#include "verifyintegrity.h"
#include "tsis.h"
#include "filesisdataprovider.h"
#include "hashcontainer.h"

#include "siscontents.h"
#include "sisstring.h"
#include "siscontroller.h"
#include "sisfiledescription.h"
#include "sisinstallblock.h"
#include "sishash.h"
#include "siselseif.h"
#include "sislogo.h"
#include "securitymanager.h"
#include "sisinfo.h"


using namespace Swi;
using namespace Swi::Sis;


//_LIT(KFileNameKey, "filename");
//_LIT(KTestDataDir, "z:\\tswi\\tsis\\data\\");
_LIT(KIntegrityFile, "\\tswi\\integritytemp.bin");

namespace Swi
{
  namespace Sis
  { 

    namespace Test
    { 
	   CVerifyIntegrityStep::CVerifyIntegrityStep(CTestParserServer& aParent, TBool aInPlace) 
	   	: iParent(aParent), iInPlace(aInPlace)
	   	{	   	
	   	}    

		EXPORT_C enum TVerdict CVerifyIntegrityStep::doTestStepPreambleL()
			{
			CParserTestStep::doTestStepPreambleL();
				
			if (CActiveScheduler::Current() == NULL)
			    {
			    INFO_PRINTF1(_L("Installing scheduler"));
			    //iSched = new (ELeave) CActiveScheduler();
			    CActiveScheduler::Install (&iSched);
			    }		
		    				    
			return TestStepResult();		
			}
		
		EXPORT_C TVerdict CVerifyIntegrityStep::doTestStepL()
			{
			__UHEAP_MARK;
					
			CContents* contents = NULL;
			CController* controller = NULL;
			
			TRAPD(err, contents = GetContentsL());
					
			if (err != KErrNone)
				{
				ERR_PRINTF1(_L("The contents could not be read!"));				
				SetTestStepResult(err);				
				return TestStepResult();
				}
				
			CleanupStack::PushL(contents);

			TRAP(err, controller = GetControllerL(*contents, iInPlace));

			if (controller == NULL)
				{
				ERR_PRINTF1(_L("The controller could not be read!"));				
				SetTestStepResult(err);				
				User::Leave(err);
				}		
			CleanupStack::PushL(controller);					

			TBool resLogo = ETrue;
			
			if (controller->Logo())
				{
				resLogo = CheckFileDescriptionL(controller->Logo()->FileDescription(), contents, 0);

				if (!resLogo)
					{
					SetTestStepResult(KErrBadHash);				
					}
				}

			const CInstallBlock& installBlock = controller->InstallBlock();						
			TBool res = EFalse;
				
			// Only do test on non pre-installed files since pre-installed do not have any files. 
			if (controller->Info().InstallType() != EInstPreInstalledApp
				&& controller->Info().InstallType() != EInstPreInstalledPatch)
				{
				res = CheckInstallBlockL(installBlock, contents, 0);
				}
			else
				{
				res=ETrue;
				}

			CleanupStack::PopAndDestroy(controller);
			CleanupStack::PopAndDestroy(contents);
			
			if (res && resLogo)
				{
				SetTestStepResult(KErrNone);				
				}
			
			delete iControllerData;
			iControllerData = NULL;
			delete iDataProvider; // To make the heap check pass :(
			iDataProvider = NULL;

			iExpectedResults.Close();	
			
			
			CSecurityPolicy::ReleaseResource();			
				
			__UHEAP_MARKEND;			

			return TestStepResult();
			}	

      EXPORT_C enum TVerdict CVerifyIntegrityStep::doTestStepPostambleL()
      	{
      	return TestStepResult();
      	}

	  TBool CVerifyIntegrityStep::CheckFileDescriptionL(const CFileDescription& aFileDescription, 
	  													CContents* aContents,
	  													TUint32 aDataIndex)
	  	{
		RFile temp;
		RFs fs;
		User::LeaveIfError(fs.Connect());
		TDriveUnit sysDrive (fs.GetSystemDrive());
		TBuf<128> integrityFile (sysDrive.Name());
		integrityFile.Append(KIntegrityFile);
	  	
		if (aFileDescription.Operation() != EOpNull)
			{
			TUint32 idx = aFileDescription.Index();				
 			User::LeaveIfError(temp.Replace(iFs, integrityFile, EFileWrite));
			aContents->ReadDataL(temp, idx, aDataIndex); 
			temp.Close();
 			CFileSisDataProvider* dataProvider = CFileSisDataProvider::NewLC(iFs, integrityFile);
			CSecurityManager* secman = CSecurityManager::NewLC();
			CHashContainer* hash = CHashContainer::NewLC(aFileDescription.Hash());
			TBool result=EFalse;
			TRAPD(err, result = secman->VerifyFileHashL(*dataProvider, *hash));
			
			CleanupStack::PopAndDestroy(hash);
			CleanupStack::PopAndDestroy(secman);
			CleanupStack::PopAndDestroy(dataProvider);
			
			if (err != KErrNone)
				{
				SetTestStepResult(err);
				User::Leave(err);
				}

			// Delete the file	
			TInt error = fs.Delete(integrityFile);
			if (error != KErrNone && error != KErrNotFound)
				{
				User::Leave(error);
				} 
			fs.Close(); 	
			
			if (!result)
				{
				ERR_PRINTF1(_L("The hash could not be verified!"));
				ERR_PRINTF1(aFileDescription.Target().Data());
				return EFalse;
				}
			}
		return ETrue;
	  	}


	  TBool CVerifyIntegrityStep::CheckInstallBlockL(const CInstallBlock& aInstallBlock, CContents* aContents, TUint32 aDataIndex)
	  	{
		RPointerArray<CFileDescription> fileDescriptions = aInstallBlock.FileDescriptions();
		
		TInt err = iFs.Connect();
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Failed to open a file session"));
			SetTestStepResult(err);				
			User::Leave(err);
			}
			
		RFile temp;
		
		for (TInt k = 0; k < fileDescriptions.Count(); k++)
			{
			TBool res = CheckFileDescriptionL(*(fileDescriptions[k]), aContents, aDataIndex);

			if (!res)
				{
				SetTestStepResult(KErrBadHash);				
				return EFalse;
				}
			}
			
		TBool res = ETrue;
		
		for (TInt i = 0; i < aInstallBlock.IfStatements().Count() ; i++)
			{			
			res = CheckInstallBlockL((aInstallBlock.IfStatements()[i])->InstallBlock(), 
									 aContents,
									 aDataIndex);
			if (!res)
				{
				SetTestStepResult(KErrBadHash);
				return EFalse;
				}

			for (TInt j = 0; j < aInstallBlock.IfStatements()[i]->ElseIfs().Count(); j++)
				{
				res = CheckInstallBlockL(aInstallBlock.IfStatements()[i]->ElseIfs()[j]->InstallBlock(), 
										 aContents, 
										 aDataIndex);
				if (!res)
					{
					SetTestStepResult(KErrBadHash);
					return EFalse;
					}
				}			
			}
			
		for (TInt l = 0; l < aInstallBlock.EmbeddedControllers().Count(); l++)
			{
			const CInstallBlock& installBlock = aInstallBlock.EmbeddedControllers()[l]->InstallBlock();
			TUint32 dataIndex = aInstallBlock.EmbeddedControllers()[l]->DataIndex();

			res = CheckInstallBlockL(installBlock, 
									 aContents, 
									 aDataIndex + dataIndex);
			if (!res)
				{
				SetTestStepResult(KErrBadHash);
				return EFalse;
				}			
			}

		return ETrue;			 	
	  	}
	  
			
	} // namespace Test
  } // namespace Sis
} // namespace Swi
