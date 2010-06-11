/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* SWIS test step implementation
*
*/


/**
 @file
*/


#include <e32std.h>
#include "tswisstep.h"
#include "testutilclientswi.h"
#include <test/testexecutelog.h>
#include <swi/launcher.h>
#include "sisregistrywritableentry.h"
#include "swi/sisregistrysession.h"
#include "swi/sisrevocationentry.h"
#include "swi/sisregistryentry.h"
#include "sisregistrywritablesession.h"
#include "swi/sisregistrypackage.h"
#include "cleanuputils.h"
#include "dessisdataprovider.h"
#include "cafsisdataprovider.h"
#include "swispubsubdefs.h"
#include "pkgremover.h"
#include "pkgremovererrors.h"
#include "sisregistryaccess_client.h"
#include <swi/swiutils.h>

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <usif/scr/scr.h>
#include "installclientserver.h"
#endif

using namespace Swi;
using namespace Swi::Test;

//
// CSwisTestStep
//
_LIT(KExpectedHash, "hash");
_LIT(KExpectedDisplayText, "displaytext");
_LIT(KExpectedResultKeyFormat, "expectedresult%d");
const TInt KResultKeyMaxLength = 16;
const TInt KResultKeyMaxNum = 99;
const TInt KSetBufSize=64;
_LIT(KExpectedDialog, "dialog");
_LIT(KErrCode, "errorcode");


void CSwisTestStep::GetFileNamesForCheck(RArray<TPtrC>& aFileNumExist,RArray<TPtrC>& aFileNumNonExist,TReadType aReadType)
	{
	_LIT(KNumExistCancel, "numexistcancel"); // this specifies how many files to check for
	_LIT(KExistBaseCancel, "existcancel"); // + number (0-based) = file to check for
	_LIT(KNumNonExistCancel, "numnonexistcancel"); // this specifies how many files to check for
	_LIT(KNonExistBaseCancel, "nonexistcancel"); // + number (0-based) = file to check for
	
	_LIT(KNumExist, "numexist"); // this specifies how many files to check for
	_LIT(KExistBase, "exist"); // + number (0-based) = file to check for
	_LIT(KNumNonExist, "numnonexist"); // this specifies how many files to check for
	_LIT(KNonExistBase, "nonexist"); // + number (0-based) = file to check for
	
	_LIT(KNumRegistered, "numregistered");
	_LIT(KRegisteredBase, "registered");

	TInt entriesNumExist=0;
	TInt entriesNumNonExist=0;
	
	switch(aReadType)
		{
		case ESimpleType:
			{
			GetIntFromConfig(ConfigSection(), KNumExist, entriesNumExist);
			GetIntFromConfig(ConfigSection(), KNumNonExist, entriesNumNonExist);
			ExtractFileName(entriesNumExist, KExistBase, aFileNumExist);
			ExtractFileName(entriesNumNonExist, KNonExistBase, aFileNumNonExist);
			break;
			}
		
		case ECancelType:
			{
			GetIntFromConfig(ConfigSection(), KNumExistCancel, entriesNumExist);
			GetIntFromConfig(ConfigSection(), KNumNonExistCancel, entriesNumNonExist);
			ExtractFileName(entriesNumExist, KExistBaseCancel, aFileNumExist);
			ExtractFileName(entriesNumNonExist, KNonExistBaseCancel, aFileNumNonExist);
			break;
			}
		
		case ERegisteredType:
			{
			GetIntFromConfig(ConfigSection(), KNumRegistered, entriesNumExist);
			ExtractFileName(entriesNumExist, KRegisteredBase, aFileNumExist);
			break;
			}
		}
	}
		
void CSwisTestStep::ExtractFileName(TInt aEntries, const TDesC& aEntryBase, RArray<TPtrC>& aFileArray)
	{
	TPtrC fname;
	const TInt KKeyBufSize =64;
	
	for(TInt i=0; i<aEntries; i++)
		{
		//construct name of the key
		TBuf<KKeyBufSize> keyBuf(aEntryBase);
		keyBuf.AppendNum(i);
		
		if(GetStringFromConfig(ConfigSection(),keyBuf,fname))
			{
			aFileArray.Insert(fname, i);
			}
		else
			{
			ERR_PRINTF2(_L("Config - filename item missing: %S"), &keyBuf);
			}
		}
	}

void CSwisTestStep::CheckIfFilesExist(TBool aCheckExist, const RArray<TPtrC>& aFileArray, RTestUtilSessionSwi& aTestUtil, TInt aMsecTimeout)
	{
	TInt nErr =0;
	if(aCheckExist)
		{
		for(TInt i =0; i< aFileArray.Count(); i++)
			{
			if(!aTestUtil.FileExistsL(aFileArray[i], aMsecTimeout))
				{
				ERR_PRINTF2(_L("File missing: %S"), &aFileArray[i]);
				nErr++;
				}
			}
		}
	else
		{
		for(TInt i =0; i< aFileArray.Count(); i++)
			{
			if(aTestUtil.FileExistsL(aFileArray[i], aMsecTimeout))
				{
				ERR_PRINTF2(_L("File exists (but shouldn't): %S"), &aFileArray[i]);
				nErr++;
				}
			}
		}
	// DEF118994 - If any files are incorrect the test result should be failure.
	if(nErr)
		{
		SetTestStepResult(EFail);
		}
	}
	
void CSwisTestStep::CheckRegistryEntry(RArray<TPtrC>& aFileArray)
	{
	TInt nErr=0;
	RPointerArray<HBufC> regFiles;
	
	TInt uid;
	if (GetHexFromConfig(ConfigSection(), _L("uid"), uid))
		{
		// trap this whole section
		enum { EConnectStage=1, EOpenStage, EEntryStage } stage = EConnectStage;
			
		TRAPD(err,
			{ 
			
			TUid regUid = {uid};
			
			RSisRegistrySession session;
			User::LeaveIfError(session.Connect());
			CleanupClosePushL(session);

			stage = EOpenStage;
			RSisRegistryEntry entry;
			User::LeaveIfError(entry.Open(session, regUid));
			CleanupClosePushL(entry);

			stage = EEntryStage;
			entry.FilesL(regFiles);
			
			CleanupStack::PopAndDestroy(2, &session);
			});
				
		if (err != KErrNone)
			{
			ERR_PRINTF3(_L("Couldn't query the registry at stage %d failed with error %d"), stage, err);
			}
		}

	for(TInt i = 0; i< aFileArray.Count(); i++)
		{
		TBool found = EFalse;
		for(TInt j = 0; j < regFiles.Count(); j++)
			{
			// Find this file name in the reg
			if (regFiles[j]->CompareF(aFileArray[i]) == 0)
				{
				found = ETrue;
				break;
				}
			}
		if (!found)
			{
			ERR_PRINTF2(_L("File not registered: %S"), &aFileArray[i]);
			nErr++;
			}
		}
		
	regFiles.ResetAndDestroy();
	
	if (nErr && (TestStepResult() == EPass))
		{
		SetTestStepResult(EFail);			
		}	
	}
	
void CSwisTestStep::CompareFilesL()
	{
	_LIT(KCompareFileAFormat, "comparefilea%d");
	_LIT(KCompareFileBFormat, "comparefileb%d");
	const TInt KKeyMaxLength = 15;
	const TInt KCompareFileMaxNum = 99;
	const TInt KExpectedHashLength = 160;

	RTestUtilSessionSwi testutil;
	User::LeaveIfError(testutil.Connect());
	CleanupClosePushL(testutil);
	TBool failedComparison(EFalse);
	
	TPtrC fileNameA;
	TPtrC fileNameB;
	TBuf<KKeyMaxLength> CompareFileAKey;
	TBuf<KKeyMaxLength> CompareFileBKey;

	HBufC8* hashBufA = HBufC8::NewLC(KExpectedHashLength);
	TPtr8 hashBufAPtr = hashBufA->Des();
	HBufC8* hashBufB = HBufC8::NewLC(KExpectedHashLength);
	TPtr8 hashBufBPtr = hashBufB->Des();

	for (TInt i = 1; i <= KCompareFileMaxNum; i++)
		{
		CompareFileAKey.Format(KCompareFileAFormat, i);
		CompareFileBKey.Format(KCompareFileBFormat, i);
		if (GetStringFromConfig(ConfigSection(), CompareFileAKey, fileNameA) &&
				 GetStringFromConfig(ConfigSection(), CompareFileBKey, fileNameB))
			{
			TInt err = testutil.GetFileHash(fileNameA, hashBufAPtr);
			if (err == KErrNone)
				{
				err = testutil.GetFileHash(fileNameB, hashBufBPtr);
				if (err != KErrNone)
					{
					ERR_PRINTF3(_L("Attempt to get hash for file %S returned %d"), &fileNameB, err);
					failedComparison = ETrue;
					}
				}
			else 
				{
				ERR_PRINTF3(_L("Attempt to get hash for file %S returned %d"), &fileNameA, err);
				failedComparison = ETrue;
				}
			if (err == KErrNone)
				{
				if (hashBufAPtr == hashBufBPtr)
					{
					INFO_PRINTF3(_L("Hashes for %S and %S match"), &fileNameA, &fileNameB);
					}
				else
					{
					ERR_PRINTF3(_L("Hashes for %S and %S do not match"), &fileNameA, &fileNameB);
					failedComparison = ETrue;
					}
				}
			}
		else
			{
			break;	
			}
		}
	CleanupStack::PopAndDestroy(3, &testutil);
	if (failedComparison && (TestStepResult() == EPass))
		{
		SetTestStepResult(EFail);			
		}
	}

void CSwisTestStep::CompareFileWithBufferL(const Swi::Test::CUIScriptAdaptor& ui)
	{
	_LIT(KExpectedDisplayTextFile,"ExpectedDisplayTextFile");
	TBool failedComparison(EFalse);
	TPtrC compareFileName;
	if (GetStringFromConfig(ConfigSection(), KExpectedDisplayTextFile, compareFileName))
		{
		const TDesC& actualDisplayText = ui.DisplayedTextActual();

		// Convert buffer from 16 bit to 8 bit
		HBufC8 *bufferText8 = HBufC8::NewLC(actualDisplayText.Size());
		bufferText8->Des().Copy(reinterpret_cast<const TUint8 *>(actualDisplayText.Ptr()), actualDisplayText.Size());

		// Read file contents into a buffer
		RFs fs;
		RFile file;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		User::LeaveIfError(file.Open(fs, compareFileName, EFileRead));
		CleanupClosePushL(file);
		TInt seekPos = 0;
		file.Seek(ESeekStart, seekPos);
		HBufC8* bufferFile8 = HBufC8::NewLC(bufferText8->Size() + 1);
		TPtr8 bufferFilePtr(bufferFile8->Des());
		User::LeaveIfError(file.Read(bufferFilePtr, bufferText8->Size() + 1));

		if ((bufferFile8->Size() != bufferText8->Size()) ||
			(*bufferFile8 != *bufferText8))
			{
			failedComparison = ETrue;
			}

		if (failedComparison == EFalse)
			{
			INFO_PRINTF1(_L("File compares correctly with expected display text"));
			}
			else
			{
			ERR_PRINTF1(_L("File does NOT compare correctly with expected display text"));
			}

		CleanupStack::PopAndDestroy(4, bufferText8);	// fs, file, bufferFile8
		}

	if ((TestStepResult() == EPass) && failedComparison)
		{
		SetTestStepResult(EFail);			
		}
	}

void CSwisTestStep::CopyFilesL(const TDesC& aNumEntries, const TDesC& aFrom, const TDesC& aTo)
	{
	RTestUtilSessionSwi testutil;
	User::LeaveIfError(testutil.Connect());
	CleanupClosePushL(testutil);

	TInt numEntries=0;
	if (GetIntFromConfig(ConfigSection(), aNumEntries, numEntries) && numEntries!=0)
		{
		for (TInt i=0; i<numEntries; i++)
			{
			TPtrC ptr;

			// construct name of "from" key
			TBuf<64> fromBuf(aFrom);
			fromBuf.AppendNum(i);
			// Read "from" key
			User::LeaveIfError(GetStringFromConfig(ConfigSection(), fromBuf, ptr));
			TFileName fromFile = ptr;

			// construct name of "to" key
			TBuf<64> toBuf(aTo);
			toBuf.AppendNum(i);
			// Read "to" key
			User::LeaveIfError(GetStringFromConfig(ConfigSection(), toBuf, ptr));
			TFileName toFile = ptr;
				
			User::LeaveIfError(testutil.Copy(fromFile, toFile));
			}
		}
	
	CleanupStack::PopAndDestroy(&testutil);
	}

void CSwisTestStep::DeleteFilesL(const TDesC& aNumEntries,
 								const TDesC& aDeleteKeyBase)
 	{
 	TInt numEntries=0;
 	if (GetIntFromConfig(ConfigSection(), aNumEntries, numEntries) && numEntries!=0)
 		{
		TBool ignoreErrors = EFalse;
		GetBoolFromConfig(ConfigSection(), _L("deleteignoreerrors"), ignoreErrors);

 		RTestUtilSessionSwi testutil;
 		User::LeaveIfError(testutil.Connect());
 		CleanupClosePushL(testutil);
 		
 		for (TInt i=0; i<numEntries; i++)
 			{
 			TPtrC ptr;
 
 			// construct name of delete file key
 			TBuf<64> deleteKey(aDeleteKeyBase);
 			deleteKey.AppendNum(i);
 			// Read delete file name
 			User::LeaveIfError(GetStringFromConfig(ConfigSection(), deleteKey, ptr));
 			TFileName deleteFile = ptr;
 			
			TInt err = testutil.Delete(deleteFile);
			if(ignoreErrors && (err < 0))
				{
				INFO_PRINTF2(_L("Ignoring delete error code %d\n"), err);
				}
			else
				{
				User::LeaveIfError(err);
				}
 			}
 		CleanupStack::PopAndDestroy(&testutil);
 		}
 	}
 

void CSwisTestStep::SetClearReadOnlyFilesL(const TDesC& aNumEntries,
										   const TDesC& aFileKeyBase,
										   const TDesC& aSetClearOpBase)
	{
	TInt numEntries=0;
	if (GetIntFromConfig(ConfigSection(), aNumEntries, numEntries) && numEntries!=0)
		{
		RTestUtilSessionSwi testutil;
		User::LeaveIfError(testutil.Connect());
		CleanupClosePushL(testutil);
		
		for (TInt i=0; i<numEntries; i++)
			{
			TPtrC ptr;

			// construct name of file key
			TBuf<64> key(aFileKeyBase);
			key.AppendNum(i);
			// Read file name
			User::LeaveIfError(GetStringFromConfig(ConfigSection(), key, ptr));
			TFileName filename = ptr;

			// construct name of operation key
			key.Copy(aSetClearOpBase);
			key.AppendNum(i);
			// Read operation (zero means clear read only attribute, non-zero means set)
			TInt operation(1);
			GetIntFromConfig(ConfigSection(), key, operation);
			
			User::LeaveIfError(testutil.SetReadOnly(filename, operation));
			}
		CleanupStack::PopAndDestroy(&testutil);
		}
	
	}

void CSwisTestStep::CheckPubSubStatus(TDesC& aPubSubStatus)
	{
	if (aPubSubStatus.Length() > 0)
		{
		TInt property = 0;
 		if (RProperty::Get(KUidSystemCategory, KUidSoftwareInstallKey, property) == KErrNone)
			{
			TSwisOperationStatus pubsubValue = TSwisOperationStatus(property & KSwisOperationStatusMask);
			TFileName swisStatus;
			
			if (pubsubValue == ESwisStatusAborted)
				{
				swisStatus.Copy(_L("ESwisStatusAborted"));
				}
			else if (pubsubValue == ESwisStatusSuccess)
				{
				swisStatus.Copy(_L("ESwisStatusSuccess"));
				}
			else if (pubsubValue == ESwisStatusNone)
				{
				swisStatus.Copy(_L("ESwisStatusNone"));
				}
			else
				{
				ERR_PRINTF2(_L("Pub & Sub status %d is not supported!"), pubsubValue);
				SetTestStepResult(EFail);
				return;
				}
			
			if(aPubSubStatus.Compare(swisStatus) != KErrNone)
 				{
				TSwisOperation opValue = TSwisOperation(property & KSwisOperationMask);

 				if ((pubsubValue == ESwisStatusNone) && (opValue == ESwisNone))
 					{
 					// Occationally this is the case for Functional-DP, as it misses the small window of time
 					// before ~CInstallSession() reseting KUidSoftwareInstallKey
 					// PDEF104559: INFO_PRINTF2(_L("WARNING: Pub & Sub Status is [%S] which was not as expected"), &swisStatus);	
 					}
  				else
  					{
  					INFO_PRINTF2(_L("Pub & Sub Status is [%S] which was not as expected"), &swisStatus);
            		SetTestStepResult(EFail);
  					}
 				}
 			else
 				{
				INFO_PRINTF2(_L("Pub & Sub Status is [%S] which was as expected"), &swisStatus);
            	}	
 			}
		}
	}

void CSwisTestStep::CheckPubSubStatusIdle()
	{
	TInt property = 0;
	TInt err = RProperty::Get(KUidSystemCategory, KUidSoftwareInstallKey, property);
	if (err == KErrNone)
		{
		TInt pubsubValue = property & KSwisOperationStatusMask;

		// Always check that the status returns to ESwisStatusNone, waiting
		// up to 5 seconds.
		TInt delaytime = 5;
		while (delaytime-- && err == KErrNone && pubsubValue != ESwisStatusNone)
			{
			User::After(1000000); // wait a second until the next test
			err = RProperty::Get(KUidSystemCategory, KUidSoftwareInstallKey, property);
			if (err == KErrNone)
				{
				pubsubValue = property & KSwisOperationStatusMask;
				}
			}

		if (pubsubValue != ESwisStatusNone)
			{
			ERR_PRINTF1(_L("Pub & Sub status has not returned to ESwisStatusNone."));
			SetTestStepResult(EFail);
			}
		}

	// Check if there was a problem on the last RProperty::Get()
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("RProperty::Get() for the software install key returned %d"), err);
		SetTestStepResult(EFail);
		}
	}

void CSwisTestStep::StoreExpectedInstallSizesL(TInt aNumExpectedSizeEntries, RArray<TTableOfInstallSizes>& aExpectedValue)
	{
	_LIT(KCorePkgName, "corePkgName");
	
	TPtrC corePackageName;
	
	if (!GetStringFromConfig(ConfigSection(), KCorePkgName, corePackageName))
		{
		ERR_PRINTF1(_L("Missing the core package file name"));
		SetTestStepResult(EFail);
		}
	else
		{
		_LIT(KExpectedInstallSize, "expectedInstallSize");
		
		const TInt KInstNameBufSize=64;
		TBuf<KInstNameBufSize> instNameBuffer(KExpectedInstallSize);
		instNameBuffer.Append(corePackageName);
		
		TInt expectedSize = 0;
	
		for (TInt i=0; i<aNumExpectedSizeEntries; i++)
			{
			const TInt KKeyBufSize=64;
			TBuf<KKeyBufSize> keyBuf(instNameBuffer);
			keyBuf.AppendNum(i);
			GetIntFromConfig(ConfigSection(), keyBuf, expectedSize);
		
			const TInt KPkgNameBufSize=64;
			TBuf<KPkgNameBufSize> pkgNameBuffer(corePackageName);
			pkgNameBuffer.AppendNum(i);

			// Add install size to the list of expected install sizes.			
			TTableOfInstallSizes tableElement;
			tableElement.iPkgFileName = pkgNameBuffer;
			tableElement.iInstallSize = expectedSize;
			aExpectedValue.AppendL(tableElement);			
			
			INFO_PRINTF3(_L("Value added  is: aExpectedValue[%D].iInstallSize = %D"), i, aExpectedValue[i].iInstallSize);
			INFO_PRINTF3(_L("Package name is: aExpectedValue[%D].iPkgFileName = %S"), i, &aExpectedValue[i].iPkgFileName);
			}
		}
	}

void CSwisTestStep::CheckDialogsCalled(const Swi::Test::CUIScriptAdaptor& ui)
	{
	_LIT(KGrantUserCapsDialogExpected,"GrantUserCapsDialogExpected");
	TBool grantUserCapsExpected=EFalse;
	TBool dialogCheckOkay = ETrue;

	if (GetBoolFromConfig(ConfigSection(),KGrantUserCapsDialogExpected, grantUserCapsExpected))
		{
		INFO_PRINTF2(_L("Expect GrantUserCaps dialog = %D"), grantUserCapsExpected);
		if (!ui.GrantUserCapsDialogCalled() && grantUserCapsExpected)
			{
			// Dialog not called when it should have been
			ERR_PRINTF1(_L("User Grantable Capabilities dialog not called when it should have been"));
			dialogCheckOkay = EFalse;
			}
		if (ui.GrantUserCapsDialogCalled() && !grantUserCapsExpected)
			{
			// Dialog called when it should not have been
			ERR_PRINTF1(_L("User Grantable Capabilities dialog called when it should not have been"));
			dialogCheckOkay = EFalse;
			}
		}
	_LIT(KDisplayDriveDialogExpected,"DisplayDriveDialogExpected");
 	TBool displayDriveExpected=EFalse;
 
 	if (GetBoolFromConfig(ConfigSection(),KDisplayDriveDialogExpected, displayDriveExpected))
 		{
 		INFO_PRINTF2(_L("Expect DisplayDrive dialog = %D"), displayDriveExpected);
 		if (!ui.DisplayDriveDialogCalled() && displayDriveExpected)
 			{
 			// Dialog not called when it should have been
 			ERR_PRINTF1(_L("Display drive dialog not called when it should have been"));
			dialogCheckOkay = EFalse;
 			}
 		if (ui.DisplayDriveDialogCalled() && !displayDriveExpected)
 			{
 			// Dialog called when it should not have been
 			ERR_PRINTF1(_L("Display drive dialog called when it should not have been"));
			dialogCheckOkay = EFalse;
 			}
 		}		

	_LIT(KDisplayCannotOverwriteFileExpected,"DisplayCannotOverwriteFileExpected");
 	TBool displayCannotOverwriteFileExpected = EFalse;
 
 	if (GetBoolFromConfig(ConfigSection(),KDisplayCannotOverwriteFileExpected, displayCannotOverwriteFileExpected))
 		{
 		INFO_PRINTF2(_L("Expect DisplayCannotOverwriteFile dialog = %D"), displayCannotOverwriteFileExpected);
 		if (!ui.CannotOverwriteFileDialogCalled() && displayCannotOverwriteFileExpected)
 			{
 			// Dialog not called when it should have been
 			ERR_PRINTF1(_L("Display CannotOverwriteFile dialog not called when it should have been"));
			dialogCheckOkay = EFalse;
 			}
 		if (ui.CannotOverwriteFileDialogCalled() && !displayCannotOverwriteFileExpected)
 			{
 			// Dialog called when it should not have been
 			ERR_PRINTF1(_L("Display CannotOverwriteFile dialog called when it should not have been"));
			dialogCheckOkay = EFalse;
 			}
 		if(dialogCheckOkay)
 			{
			_LIT(KExpectedFileOwner,"ExpectedFileOwner");
 			TPtrC expectedFileOwner;
 			if (GetStringFromConfig(ConfigSection(),KExpectedFileOwner, expectedFileOwner))
 				{
		 		INFO_PRINTF2(_L("Expect Owner of the File = %S"), &expectedFileOwner);
		 		INFO_PRINTF2(_L("Actual Owner of the File = %S"), &ui.FileOwner());
		 		if (expectedFileOwner.CompareF(ui.FileOwner()) != 0)
 					{
		 			// Expected Owner name is different
					ERR_PRINTF1(_L("Actual Owner of the file is different"));
					dialogCheckOkay = EFalse;
 					}
 				}
 			}
 		}		

	_LIT(KExpectedDisplayText,"ExpectedDisplayText");
	TPtrC expDtScript;
	if (GetStringFromConfig(ConfigSection(), KExpectedDisplayText, expDtScript))
		{
		TPtrC expDisplayText(expDtScript.Left(Min(KMaxDisplayText, expDtScript.Length())));
		const TDesC& actualDisplayText = ui.DisplayedText();
		
		INFO_PRINTF2(_L("Expected display text = \"%S\""), &expDisplayText);
		INFO_PRINTF2(_L("Actual display text = \"%S\""), &actualDisplayText);
		
		if (expDisplayText != actualDisplayText)
			{
			dialogCheckOkay = EFalse;
			}
		}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	_LIT(KExpectedSecurityWarningDialogsCount,"ExpectedSecurityWarningDialogsCount");		
	TInt expectedSecurityWarningDialogs(0);
	if (GetIntFromConfig(ConfigSection(), KExpectedSecurityWarningDialogsCount, expectedSecurityWarningDialogs))
		{
		if (expectedSecurityWarningDialogs != ui.NumberOfSecurityDialogsInvoked())
			{
			ERR_PRINTF3(_L("The number of expected security warning dialogs did not match the actual one. Expected: %d, encountered: %d"), 
					expectedSecurityWarningDialogs, ui.NumberOfSecurityDialogsInvoked());
			dialogCheckOkay = EFalse;
			}
		}
#endif

	if (TestStepResult() == EPass && !dialogCheckOkay)
		{
		SetTestStepResult(EFail);
		}
	}

void CSwisTestStep::CheckFatalOcspResponse(const Swi::Test::CUIScriptAdaptor& ui)
	{
	_LIT(KFatalOcspResponseExpected,"FatalOcspResponseExpected");
	TBool fatalOcspResponseExpected=EFalse;

	if (GetBoolFromConfig(ConfigSection(),KFatalOcspResponseExpected, fatalOcspResponseExpected))
		{
		TBool ocspFatalCheckOkay = ETrue;
		if (!ui.FatalOcspResponseEncountered() && fatalOcspResponseExpected)
			{			
			ERR_PRINTF1(_L("A fatal OCSP response was not encountered when it should have been"));
			ocspFatalCheckOkay = EFalse;
			}
		if (ui.FatalOcspResponseEncountered() && !fatalOcspResponseExpected)
			{			
			ERR_PRINTF1(_L("A fatal OCSP response was encountered when it should not have been"));
			ocspFatalCheckOkay = EFalse;
			}
		if (TestStepResult() == EPass && !ocspFatalCheckOkay)
			{
			SetTestStepResult(EFail);
			}
		}
	}


void CSwisTestStep::CheckExpectedOcspOutcomes(const Swi::Test::CUIScriptAdaptor& ui)
     {
     TInt num;
 	_LIT(KNumOutcomes,"NumOutcomes");
 	if(GetIntFromConfig(ConfigSection(),KNumOutcomes,num) && num!=0)
     	{
       	TBool ocspCheckOkay(ETrue);

		const RPointerArray<TOCSPOutcome>& obs = ui.ObservedOcspOutcomes();
 		if (obs.Count() != num)
			{
 			INFO_PRINTF3(_L("Expected %d OCSP outcomes but observed %d"),
 						num, obs.Count());
 			ocspCheckOkay = EFalse;
 			}
        else
 			{
 			_LIT(KExpectedOcspStatus,"ExpectedOcspStatus");
 			_LIT(KExpectedOcspResult,"ExpectedOcspResult");
 
 			for (TInt j=0 ; j < num; j++)     
 				{
 				TBuf<30> statusKey(KExpectedOcspStatus);
 				statusKey.AppendNum(j);
 				TBuf<30> resultKey(KExpectedOcspResult);
 				resultKey.AppendNum(j);
 								
 				TInt expectedStatus;
 				TInt expectedResult;
 				if(GetIntFromConfig(ConfigSection(),statusKey,expectedStatus))
 					{
 					if(GetIntFromConfig(ConfigSection(),resultKey,expectedResult))
 						{
 						TOCSPOutcome expectedOutcome((OCSP::TStatus)expectedStatus,
 													 (OCSP::TResult)expectedResult);
 						if (*obs[j] == expectedOutcome)
 							{
 							INFO_PRINTF4(_L("Expected OCSP outcome at position %d, with status %d, result %d matches observed outcome."),
 										j, expectedStatus, expectedResult);
 							}
 						else
 							{
 							INFO_PRINTF6(_L("Expected OCSP outcome at position %d, with status %d, result %d does not match observed outcome (%d,%d)."),
 										j, expectedStatus, expectedResult, obs[j]->iStatus, obs[j]->iResult);
 							ocspCheckOkay = EFalse;
 							}
 						}
 					}
 
 				// It's okay to check the expected number of OCSP outcomes
 				// but not specify all expected values (if not all values are
 				// important for the test step.)  The test will skip checking for
 				// outcomes where only an expected status or result is specified,
 				// but not both.
 				}
 			}
 		if (ocspCheckOkay)
 			{
 			INFO_PRINTF1(_L("OCSP outcome check successful."));
 			}
 		else
 			{
 			INFO_PRINTF1(_L("OCSP outcome check failed."));
 			if (TestStepResult() == EPass)
 				{
 				SetTestStepResult(EFail); 				
	 			}
 			}
 		}
 	}

void CSwisTestStep::CheckExpectedResultCodesL()
	{
	RArray<TInt> expectedResultCodes;
	CleanupClosePushL(expectedResultCodes);
	TInt result = KErrNone;
	TBuf<KResultKeyMaxLength> resultKey;
	TBool found = ETrue;

	for (TInt i = 1; i <= KResultKeyMaxNum && found; i++)
		{
		resultKey.Format(KExpectedResultKeyFormat, i);
		found = GetIntFromConfig(ConfigSection(), resultKey, result);
		if (found)
			{
			INFO_PRINTF2(_L("Acceptable result code %d"), result);
			TInt err = expectedResultCodes.InsertInOrder(result);
			if (err != KErrNone && err != KErrAlreadyExists)
				{
				User::Leave(err);
				}
			}
		}

	if (expectedResultCodes.Count() > 0)
		{
		// A result code matching one of the possible expected results will
		// be converted to EPass, otherwise the result will be EFail.
		if (expectedResultCodes.FindInOrder(TestStepResult()) == KErrNotFound)
			{
			ERR_PRINTF2(_L("Result code %d is not expected"), TestStepResult());
			SetTestStepResult(EFail);
			}
		else
			{
			INFO_PRINTF2(_L("Result code %d is in accepted list"), TestStepResult());
			SetTestStepResult(EPass);
			}
		}
	CleanupStack::PopAndDestroy(&expectedResultCodes);
	}


void CSwisTestStep::GetDevSupportedLanguagesL(RArray<TInt>& aDeviceLanguages)
	{
	_LIT(KDeviceLanguage, "devicelanguage");
	
	// Check for device supported languages
	const TInt maxSupportedLanguages = 16;
	for (TInt i=0; i<maxSupportedLanguages; ++i)
		{
		HBufC* devLangPattern = HBufC::NewLC(KDeviceLanguage.iTypeLength+2);
		TPtr devLangPatternPtr(devLangPattern->Des());
		devLangPatternPtr.Copy(KDeviceLanguage);
		devLangPatternPtr.AppendNum(i);
		
		TInt devLanguage;
		if (!GetIntFromConfig(ConfigSection(), *devLangPattern, devLanguage))
			{
			CleanupStack::PopAndDestroy(devLangPattern);
			break;
			}
		// Add the supported language to the list
		aDeviceLanguages.Append(devLanguage);		
				
		CleanupStack::PopAndDestroy(devLangPattern);
		}
	}


//
// CSwisInstallStep
//

CSwisInstallStep::~CSwisInstallStep()
	{
	delete iUi;
	delete iFileToWatch;

	for (TInt i=0; i < iFilesHeldOpen.Count(); i++)
		{
		iFilesHeldOpen[i]->Close();
		}
	iFilesHeldOpen.ResetAndDestroy();
	iDeviceLanguages.Close();

	}
	

CSwisInstallStep::CSwisInstallStep(TInstallType aInstallType, TBool aDoCancelTest)
	: iInstallType(aInstallType), iDoCancelTest(aDoCancelTest), iExpectedUserDrive(-1)
	{
	// Call base class method to set up the human readable name for logging
	
	switch (aInstallType)
		{
		case EUseFileHandle:
			SetTestStepName(KSwisInstallFHStep);
			break;
		
		case EUseMemory:
			SetTestStepName(KSwisInstallMemStep);
			break;

		case EUseFileName:
			SetTestStepName(KSwisInstallStep);
			break;
			
		case ECheckInstallPerformance:
		    SetTestStepName(KSwisInstallPerformanceStep);
		    break;
		
		case EUseCAF:
			SetTestStepName(KSwisInstallCAFStep);
			break;

		case EUseOpenFileName:
			SetTestStepName(KSwisInstallOpenFileStep);
			break;
			
		case ECheckExitValue:
			SetTestStepName(KSwisCheckedInstallStep);
			break; 	
			
		}
	}

/**
 * Override of base class virtual. Prepares for the test run of SWIS
 * @return TVerdict code
 */
TVerdict CSwisInstallStep::doTestStepPreambleL()
	{
	// get step parameters
	TPtrC str;
	if (!GetStringFromConfig(ConfigSection(), _L("sis"), str))
		{
		ERR_PRINTF1(_L("Missing SIS file name"));
		SetTestStepResult(EFail);
		}
	else
		{
		iSisFileName.Copy(str);
		if (!GetStringFromConfig(ConfigSection(), _L("script"), str))
			{
			ERR_PRINTF1(_L("Missing XML file name"));
			SetTestStepResult(EFail);
			}
		else
			{
			iXmlFileName.Copy(str);
			INFO_PRINTF3(_L("Installing '%S' using script '%S'"), 
				&iSisFileName, &iXmlFileName);
			TPtrC expectedText;
			// Check if the test section lists expected installation size(s).
			TInt numExpectedSizes = 0;
			
			if(GetIntFromConfig(ConfigSection(),KExpectedDialog,iExpectedDialog) && GetIntFromConfig(ConfigSection(), KErrCode,iKErrCode ) && GetStringFromConfig(ConfigSection(),KExpectedHash,iExpectedHash))
				{
				iUi = CUIScriptAdaptor::NewL(iXmlFileName, Logger(),_L("NULL"),iExpectedHash, iExpectedDialog, iKErrCode);				
				}
			else if (GetStringFromConfig(ConfigSection(),KExpectedHash,iExpectedHash))	
		        {
		        iUi = CUIScriptAdaptor::NewL(iXmlFileName, Logger(),_L("NULL"),iExpectedHash, NULL, NULL);
		        }
		    else if (GetIntFromConfig(ConfigSection(),KExpectedDialog,iExpectedDialog) && GetIntFromConfig(ConfigSection(), KErrCode,iKErrCode ))
		   	 	{
		    	iUi = CUIScriptAdaptor::NewL(iXmlFileName, Logger(),_L("NULL"),_L("NULL"), iExpectedDialog, iKErrCode);
		    	}
			else if ((GetIntFromConfig(ConfigSection(), KNumExpectedSizes, numExpectedSizes)) && numExpectedSizes!=0)
				{
				INFO_PRINTF3(_L("Installing '%S'; it has %D install size(s)"),
								&iSisFileName, numExpectedSizes);
								
				// Store the value(s) for expectedInstallSize.
 				StoreExpectedInstallSizesL(numExpectedSizes, iExpectedValue);

				// create UI handler and populate the answers from XML file; also pass in
				// information about any expected install sizes the test specifies
				iUi = CUIScriptAdaptor::NewL(iXmlFileName, Logger(), &iExpectedValue);

				}
			// Check if expected display text is listed in the config section.
			else if (GetStringFromConfig(ConfigSection(),KExpectedDisplayText, expectedText))
		        {
		        iUi = CUIScriptAdaptor::NewL(iXmlFileName, Logger(), expectedText,_L("NULL"),NULL, NULL);
		        }
			else
				{
				// create UI handler and populate the answers from XML file
				iUi = CUIScriptAdaptor::NewL(iXmlFileName, Logger());
				}
			}
		}

	// Read optional expectedUserDrive
	if (GetIntFromConfig(ConfigSection(), _L("expectedUserDrive"), iExpectedUserDrive))
		{
		TInt uid;
		if (!GetHexFromConfig(ConfigSection(), _L("uid"), uid))
			{
			// If expectedUserDrive we need a uid to query
			ERR_PRINTF1(_L("Missing uid"));
			SetTestStepResult(EFail);
			return EFail;
			}
		iUid.iUid=uid;
		}
	
	// Read optional ExpectedPubSubStatus
	GetStringFromConfig(ConfigSection(), _L("pubsubstatus"), iPubSubStatus);

	TPtrC watchFilePtr;
	if (GetStringFromConfig(ConfigSection(), _L("watchfilename"), watchFilePtr))
		{
		delete iFileToWatch;
		iFileToWatch = NULL;
		iFileToWatch = watchFilePtr.AllocL();
		GetBoolFromConfig(ConfigSection(), _L("expectfilechange"), iExpectFileChange);
		if (iExpectFileChange)
			{
			INFO_PRINTF2(_L("Expecting file %S to change in this test."), iFileToWatch);
			}
		else
			{
			INFO_PRINTF2(_L("Expecting no changes to file %S in this test."), iFileToWatch);
			}
		}


	TInt dialogDelay;
	if (GetIntFromConfig(ConfigSection(), _L("dialogdelay"), dialogDelay) && dialogDelay > 0)
		{
		iUi->SetDialogDelay(dialogDelay);
		TInt delayDialogNumber(-1);
		if (GetIntFromConfig(ConfigSection(), _L("delaydialognumber"), delayDialogNumber)
			&& delayDialogNumber >= 0)
			{
			INFO_PRINTF3(_L("Test will delay %d microseconds at dialog %d."),
						dialogDelay, delayDialogNumber);
			iUi->SetDelayDialogNumber(delayDialogNumber);
			}
		else
			{
			INFO_PRINTF2(_L("Test will delay %d microseconds at each dialog."),
						dialogDelay);		
			}
		}

	GetFilesToHoldOpenL();

	GetDevSupportedLanguagesL(iDeviceLanguages);
	if (iDeviceLanguages.Count() > 0)
		iUseDeviceLanguages = ETrue;

	return TestStepResult();
	}
	
/**
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini file section
 * @return TVerdict code
 */
TVerdict CSwisInstallStep::doTestStepL()
	{
	INFO_PRINTF1(KSwisInstallStep);

	// launch the installation
	CInstallPrefs* prefs = CInstallPrefs::NewLC();
	
	// get OCSP responder URI from INI file
	TPtrC uri;
	if (GetStringFromConfig(ConfigSection(), _L("ocsp_uri"), uri))
		{
		const TUint KMaxUriLength=512;
		TBuf8<KMaxUriLength> temp;
		temp.Copy(uri);
		prefs->SetRevocationServerUriL(temp);
		}
	
	TInt ocspFlag=0;
	if (GetIntFromConfig(ConfigSection(), _L("ocsp_flag"), ocspFlag))
		{
		prefs->SetPerformRevocationCheck(ocspFlag ? ETrue : EFalse);
		}

	TRequestStatus fileWatchStatus(KErrNone);
	
	RTestUtilSessionSwi util;
	User::LeaveIfError(util.Connect());
	CleanupClosePushL(util);
	
	if (iFileToWatch)
		{
		util.WatchFile(*iFileToWatch, fileWatchStatus);
		}
	
	if (iDoCancelTest)
		{
		// Cancel testing
		TBool cancelled = EFalse;
		TInt dialogNumber = 0;
		iUi->SetCancelDialogTesting(ECancelFromDialog);

		RArray<TPtrC> fileNumExist;
		RArray<TPtrC> fileNumNonExist;
		GetFileNamesForCheck(fileNumExist, fileNumNonExist,ECancelType);
	
		while (ETrue)
			{
			iUi->ResetCurrentDialogNumber();
			iUi->SetCancelDialogNumber(dialogNumber);
			TInt error = KErrNone;
			TRAPD(leaveErr, error=DoInstallL(*prefs));
			
			if (leaveErr != KErrNone)
				{
				ERR_PRINTF2(_L("DoInstallL left with %d."), leaveErr);
				// Can't set error to KErrServerBusy, or Testexecute will retry.
				SetTestStepResult((leaveErr != KErrServerBusy) ? static_cast<TVerdict>(leaveErr) : EFail);
				}
			else if (iUi->InstallationCancelled())
				{
				cancelled=ETrue;
				
				// Error ought to be KErrCancel
				if (error != KErrCancel)
					{
					ERR_PRINTF3(_L("Installation returned %d, KErrCancel expected, at dialog %d"), error, dialogNumber);
					SetTestStepResult(EFail);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Installation cancellation at dialog %d successful"), dialogNumber);
					
					// Files which are labelled as existing should not exist since we cancelled.
					CheckIfFilesExist(ETrue,fileNumExist,util);
					CheckIfFilesExist(EFalse,fileNumNonExist,util);
	
					// we might have failed the test already so stop.
					if (TestStepResult()!=EPass)
						{
						break;
						}				
					}
				}
			else
				{
				if (error != KErrNone)
					{
					ERR_PRINTF2(_L("DoInstallL returned %d."), error);
					// Can't set error to KErrServerBusy, or Testexecute will retry.
					SetTestStepResult((error != KErrServerBusy)? static_cast<TVerdict>(error) : EFail);
					}
				// Must have finished installation
				break;
				}
			
			// fail at next dialog
			++dialogNumber;
			}

		// Only check files if this is the last step and we didn't cancel
		if (!cancelled)
			{
			// Files which are labelled as existing should exist since we didn't cancel.
			CheckIfFilesExist(EFalse,fileNumExist,util);
			CheckIfFilesExist(ETrue,fileNumNonExist,util);
			}
		}
		else
			{
			TInt err = DoInstallL(*prefs);
			CheckPubSubStatus(iPubSubStatus);
			INFO_PRINTF2(_L("DoInstallL returned %d"), err);
			if (err != KErrNone)
				{
				// Can't set error to KErrServerBusy, or Testexecute will retry.
				SetTestStepResult((err != KErrServerBusy)? static_cast<TVerdict>(err) : EFail);
				}			
			}

	if (iFileToWatch)
		{
		// Cancel file watch if still pending
		if (fileWatchStatus.Int() == KRequestPending)
			{
			util.WatchFileCancelL();
			}

		// Catch request status change.
		User::WaitForRequest(fileWatchStatus);

		TInt completionCode = fileWatchStatus.Int();
		TBool fileChanged = (completionCode == KErrNone) ? ETrue : EFalse;
		if (fileChanged || completionCode == KErrCancel)
			{
			INFO_PRINTF2(_L("Expected file change: %d"), iExpectFileChange);
			INFO_PRINTF2(_L("File changed: %d"), fileChanged);
			if (fileChanged != iExpectFileChange)
				{
				SetTestStepResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF2(_L("FS completed file watch status with %d"), completionCode);
			SetTestStepResult(EFail);
			}
		}
	CleanupStack::PopAndDestroy(&util);
	CleanupStack::PopAndDestroy(prefs);
	return TestStepResult();
	}

void CSwisInstallStep::PrintPerformanceLog(TTime aTime)
    {
    _LIT(KPerformanceTestInfo, "PERFORMANCE_LOG_INFORMATION");
    TDateTime timer = aTime.DateTime();
    INFO_PRINTF6(_L("%S,%d:%d:%d:%d"), &KPerformanceTestInfo(), timer.Hour(), timer.Minute(), timer.Second(), timer.MicroSecond());
    }

TInt CSwisInstallStep::DoInstallL(CInstallPrefs& aInstallPrefs)
	{
	switch (iInstallType)
		{
		case EUseFileName:
			{
			TInt error;
			if (iUseDeviceLanguages)
				error = Launcher::Install(*iUi, iSisFileName, aInstallPrefs, iDeviceLanguages);
			else
				error = Launcher::Install(*iUi, iSisFileName, aInstallPrefs);
			return error;
			}

		case ECheckInstallPerformance:
		    {
		    _LIT(KMaxDurationName, "MaxDuration");
            _LIT(KMaxTestCaseDuration, "TEST_CASE_MAXIMUM_ALLOWED_DURATION");
            _LIT(KActualTestCaseDuration, "TEST_CASE_ACTUAL_DURATION");
            
            TInt maxDuration = 0;
            if(!GetIntFromConfig(ConfigSection(), KMaxDurationName, maxDuration))
                {
                ERR_PRINTF2(_L("%S could not be found in configuration."), &KMaxDurationName());
                User::Leave(KErrNotFound);
                }
            
		    TInt error;
		    TTime startTime, endTime;
		    startTime.HomeTime(); // Set the start time
		    PrintPerformanceLog(startTime);
		    
            if (iUseDeviceLanguages)
                error = Launcher::Install(*iUi, iSisFileName, aInstallPrefs, iDeviceLanguages);
            else
                error = Launcher::Install(*iUi, iSisFileName, aInstallPrefs);
            
            // Calculate the time taken for installation in milliseconds
            endTime.HomeTime();
            PrintPerformanceLog(endTime);            
            TTimeIntervalMicroSeconds duration = endTime.MicroSecondsFrom(startTime);
            TInt actualDuration = I64INT(duration.Int64())/1000;
            
            INFO_PRINTF3(_L("%S,%d"), &KMaxTestCaseDuration(), maxDuration);
            INFO_PRINTF3(_L("%S,%d"), &KActualTestCaseDuration(), actualDuration);
                        
            if(actualDuration <= maxDuration)
                {
                INFO_PRINTF2(_L("This test meets performance requirement (Duration=%d)."), actualDuration);
                }
            else
                {
                ERR_PRINTF2(_L("This test does not meet performance requirement (Duration=%d)."), actualDuration);
                error = KErrGeneral;
                SetTestStepResult(EFail);
                }
                
            return error;
		    }

		case EUseOpenFileName:
		// open the file as a shared for readers only
			{
			RFs fs;
			User::LeaveIfError(fs.Connect());
			fs.ShareProtected();
			CleanupClosePushL(fs);
			RFile file;
			User::LeaveIfError(file.Open(fs, iSisFileName, EFileShareReadersOnly));
			CleanupClosePushL(file);
			TInt error;
			if (iUseDeviceLanguages)
				error = Launcher::Install(*iUi, iSisFileName, aInstallPrefs, iDeviceLanguages);
			else
				error = Launcher::Install(*iUi, iSisFileName, aInstallPrefs);
			CleanupStack::PopAndDestroy(2, &fs);
			return error;			
			}

		case EUseFileHandle:
			{
			RFs fs;
			User::LeaveIfError(fs.Connect());
			fs.ShareProtected();
			CleanupClosePushL(fs);
			RFile file;
			CleanupClosePushL(file);
			// File is in a private directory, so we need to use the
			// test util server to open it.
			RTestUtilSessionSwi testutil;
			User::LeaveIfError(testutil.Connect());
			CleanupClosePushL(testutil);
			_LIT(KPrivatePathPattern, "?:\\private\\*");
			if (iSisFileName.MatchF(KPrivatePathPattern) == KErrNotFound)
				{
				User::LeaveIfError(file.Open(fs, iSisFileName, 0));
				}
			else
				{
				RFile tempFile;
				CleanupClosePushL(tempFile);
				User::LeaveIfError(testutil.GetFileHandle(iSisFileName, tempFile));
				User::LeaveIfError(file.Duplicate(tempFile));
				CleanupStack::PopAndDestroy(&tempFile);
				}
			TInt error;
			if (iUseDeviceLanguages)
				error = Launcher::Install(*iUi, file, aInstallPrefs, iDeviceLanguages);
			else
				error=Launcher::Install(*iUi, file, aInstallPrefs);
			CleanupStack::PopAndDestroy(3, &fs);
			return error;
			}

		case EUseMemory:
			{
			RFs fs;
			User::LeaveIfError(fs.Connect());
			fs.ShareProtected();
			CleanupClosePushL(fs);
			RFile file;
			User::LeaveIfError(file.Open(fs, iSisFileName, 0));
			CleanupClosePushL(file);
			
			TInt fileSize=0;
			User::LeaveIfError(file.Size(fileSize));
			HBufC8* buffer=HBufC8::NewLC(fileSize);
			TPtr8 pBuffer(buffer->Des());
			User::LeaveIfError(file.Read(pBuffer));
			CDesDataProvider* dataProvider = CDesDataProvider::NewLC(*buffer);
			TInt error;
			if (iUseDeviceLanguages)
				error = Launcher::Install(*iUi, *dataProvider, aInstallPrefs, iDeviceLanguages);
			else
				error=Launcher::Install(*iUi, *dataProvider, aInstallPrefs);
			CleanupStack::PopAndDestroy(4, &fs);
			return error;
			}		
			
		case EUseCAF:
			{
			CCafSisDataProvider* dataProvider = CCafSisDataProvider::NewLC(iSisFileName);
			TInt error;
			if (iUseDeviceLanguages)
				error = Launcher::Install(*iUi, *dataProvider, aInstallPrefs, iDeviceLanguages);
			else
				error=Launcher::Install(*iUi, *dataProvider, aInstallPrefs);
			CleanupStack::PopAndDestroy(dataProvider);
			return error;
			}		
			
		
		case ECheckExitValue:
			{
			// This test case does an install and checks for pass or failure
			// TInt err = Launcher::Install(*iUi, iSisFileName, *prefs);
			TInt err;
			if (iUseDeviceLanguages)
				err = Launcher::Install(*iUi, iSisFileName, aInstallPrefs, iDeviceLanguages);
			else
				err = Launcher::Install(*iUi, iSisFileName, aInstallPrefs);
			INFO_PRINTF2(_L("Install return code was %d"), err);
			
			TPtrC expected;
			if (!GetStringFromConfig(ConfigSection(), _L("result"), expected))
				{
				return ETestSuiteError;
				}
			else 
				{
				
				_LIT(KSucess, "sucess");
				_LIT(KFailure, "failure");
				
				TVerdict result;
				
				if (expected.CompareF(KSucess) == 0)
					{
					result = (err == KErrNone ? EPass : EFail);
					}
				else if (expected.CompareF(KFailure) == 0)
					{
					result = (err != KErrNone ? EPass : EFail);
					}
				else 
					{
					result = ETestSuiteError;
					}
					
				return result;
								
				}
			}
		}
			
	// Shouldn't get here
	return KErrGeneral;
	}

/**
 * Override of base class virtual
 * @return TVerdict code
 */
TVerdict CSwisInstallStep::doTestStepPostambleL()
	{
	if (!iDoCancelTest)
		{
		CheckExpectedResultCodesL();

		RArray<TPtrC> fileNumExist;
		RArray<TPtrC> fileNumNonExist;
		GetFileNamesForCheck(fileNumExist, fileNumNonExist, ESimpleType);
		
		RTestUtilSessionSwi util;
		User::LeaveIfError(util.Connect());
		CleanupClosePushL(util);
		
		CheckIfFilesExist(ETrue,fileNumExist,util);
		CheckIfFilesExist(EFalse,fileNumNonExist,util);

		RArray<TPtrC> fileNumRegistered;
		GetFileNamesForCheck(fileNumRegistered, fileNumRegistered, ERegisteredType);
		CheckRegistryEntry(fileNumRegistered);		
		
		CompareFilesL();
		CheckDialogsCalled(*iUi);
		CompareFileWithBufferL(*iUi);
		CheckFatalOcspResponse(*iUi);
        CheckExpectedOcspOutcomes(*iUi);
		if((TestStepResult() == EPass) && (iExpectedUserDrive != -1))
			{
			RSisRegistrySession registrySession;
			User::LeaveIfError(registrySession.Connect());
			CleanupClosePushL(registrySession);

			RSisRegistryWritableEntry registryEntry;
			TInt error=registryEntry.Open(registrySession, iUid);
			if(error != KErrNone)
				{
				ERR_PRINTF2(_L("RSisRegistrySession::Open failed error=%d\n"), error);
				SetTestStepResult(EFail);
				}
			else
				{
				CleanupClosePushL(registryEntry);
				
				TInt userDrive =0;
				TRAP(error, userDrive = registryEntry.SelectedDriveL());
				if((error != KErrNone) || (userDrive != iExpectedUserDrive))
					{
					ERR_PRINTF4(_L("SelectedDriveL err=%d, userDrive expected %d got %d\n"),
								error, iExpectedUserDrive, userDrive);
					SetTestStepResult(EFail);
					}
				else
					{
					INFO_PRINTF2(_L("SelectedDriveL userDrive = %d OK\n"), userDrive);
					}
				
				CleanupStack::PopAndDestroy(&registryEntry);
				}
			CleanupStack::PopAndDestroy(&registrySession);
			}
		CleanupStack::PopAndDestroy(&util);	
		}

	CheckPubSubStatusIdle();

	return TestStepResult();
	}

void CSwisInstallStep::GetFilesToHoldOpenL()
	{
	_LIT(KHeldFileNameFormat, "holdfile%d");
	const TInt KKeyMaxLength = 10;
	const TInt KHeldFileMaxNum = 99;

	TPtrC holdFilePtr;
	TBuf<KKeyMaxLength> holdFileKey;
	TBool found = ETrue;

	RTestUtilSessionSwi testutil;
	User::LeaveIfError(testutil.Connect());
	CleanupClosePushL(testutil);

	for (TInt i = 1; i <= KHeldFileMaxNum && found; i++)
		{
		holdFileKey.Format(KHeldFileNameFormat, i);
		found = GetStringFromConfig(ConfigSection(), holdFileKey, holdFilePtr);
		if (found)
			{
			RFile* holdFileHandle = new (ELeave) RFile;
			CleanupStack::PushL(holdFileHandle);
			User::LeaveIfError(testutil.GetFileHandle(holdFilePtr, *holdFileHandle));
			CleanupClosePushL(*holdFileHandle);
			iFilesHeldOpen.AppendL(holdFileHandle);
			CleanupStack::Pop(2, holdFileHandle);
			INFO_PRINTF2(_L("Holding file %S open for duration of test"), &holdFilePtr);
			}
		}
	CleanupStack::PopAndDestroy(&testutil);
	}

//
// CSwisUninstallStep
//

CSwisUninstallStep::~CSwisUninstallStep()
	{
	delete iUi;
	}

CSwisUninstallStep::CSwisUninstallStep(TUninstallType aType, TBool aDoCancelTest)
	: iType(aType), iDoCancelTest(aDoCancelTest)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSwisUninstallStep);
	}

	
	
	
/**
 * Override of base class virtual. Prepares for the test run of SWIS
 * @return TVerdict code
 */
TVerdict CSwisUninstallStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CSwisUninstallStep::doTestStepPreambleL"));
	// get step parameters
	TInt uid=0;

	TPtrC str;
	if (!GetStringFromConfig(ConfigSection(), _L("script"), str))
		{
		ERR_PRINTF1(_L("Missing XML file name"));
		SetTestStepResult(EFail);
		return EFail;
		}

	if (!GetHexFromConfig(ConfigSection(), _L("uid"), uid))
		{
		ERR_PRINTF1(_L("Missing uid"));
		SetTestStepResult(EFail);
		return EFail;
		}

	iUid.iUid=uid;
		
	if (iType == EByPackage)
		{
		TPtrC vendorName;
		if (!GetStringFromConfig(ConfigSection(), _L("vendorName"), vendorName))
			{
			ERR_PRINTF1(_L("Missing Vendor Name"));
			SetTestStepResult(EFail);
			return EFail;
			}
		iVendorName.Set(vendorName);
		
		TPtrC packageName;
		if (!GetStringFromConfig(ConfigSection(), _L("packageName"), packageName))
			{
			ERR_PRINTF1(_L("Missing Package Name"));
			SetTestStepResult(EFail);
			return EFail;
			}
		iPackageName.Set(packageName);
			
		iXmlFileName.Copy(str);
		INFO_PRINTF5(_L("Uninstalling %D, %S, %S  using script '%S'"), 
			iUid.iUid, &iPackageName, &iVendorName, &iXmlFileName);
		// create UI handler and populate the answers from XML file
		if (GetIntFromConfig(ConfigSection(),KExpectedDialog,iExpectedDialog) && GetIntFromConfig(ConfigSection(), KErrCode,iKErrCode ))
		    {
		    iUi = CUIScriptAdaptor::NewL(iXmlFileName, Logger(),_L("NULL"),_L("NULL"),iExpectedDialog,iKErrCode);
		    }
		else
			{
			iUi = CUIScriptAdaptor::NewL(iXmlFileName, Logger());	
			}    
		}
	else if (iType== EByUid)
		{
		iXmlFileName.Copy(str);
		INFO_PRINTF3(_L("Uninstalling '%D' using script '%S'"), 
			iUid.iUid, &iXmlFileName);
		// create UI handler and populate the answers from XML file
		if (GetIntFromConfig(ConfigSection(),KExpectedDialog,iExpectedDialog) && GetIntFromConfig(ConfigSection(), KErrCode,iKErrCode ))
		    {
		    iUi = CUIScriptAdaptor::NewL(iXmlFileName, Logger(),_L("NULL"),_L("NULL"),iExpectedDialog,iKErrCode);
		    }
		else
			{
			iUi = CUIScriptAdaptor::NewL(iXmlFileName, Logger());	
			}    
		}
	
	// Read optional ExpectedPubSubStatus
	GetStringFromConfig(ConfigSection(), _L("pubsubstatus"), iPubSubStatus);
	
	GetBoolFromConfig(ConfigSection(), _L("safeMode"), iIsSafeMode);

	TInt dialogDelay;
	if (GetIntFromConfig(ConfigSection(), _L("dialogdelay"), dialogDelay) && dialogDelay > 0)
		{
		iUi->SetDialogDelay(dialogDelay);
		TInt delayDialogNumber(-1);
		if (GetIntFromConfig(ConfigSection(), _L("delaydialognumber"), delayDialogNumber)
			&& delayDialogNumber >= 0)
			{
			INFO_PRINTF3(_L("Test will delay %d microseconds at dialog %d."),
						dialogDelay, delayDialogNumber);
			iUi->SetDelayDialogNumber(delayDialogNumber);
			}
		else
			{
			INFO_PRINTF2(_L("Test will delay %d microseconds at each dialog."),
						dialogDelay);		
			}
		}

	return TestStepResult();
	}

/**
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini file section
 * @return TVerdict code
 */

TInt CSwisUninstallStep::DoUninstallL()
	{
	TInt error = KErrNotFound;

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	if (iIsSafeMode)
		{
		TInt err = RProperty::Define(KUidInstallServerCategory, KUidSafeModeUninstallKey, RProperty::EInt);
		if (err != KErrNone && err != KErrAlreadyExists)
			User::Leave(err);
		User::LeaveIfError(RProperty::Set(KUidInstallServerCategory, KUidSafeModeUninstallKey, KSwisSafeModeUninstallEnabled));
		}
#endif	

	if (iType == EByUid)
		{
		// launch the installation
		return Launcher::Uninstall(*iUi, iUid); 
		}
	else if (iType == EByPackage)
		{
		
		TInt err = KErrNotFound;
		// Provisional fix to work around registry problem		
		// Go through list of packages from base package to get augmentations.
		CSisRegistryPackage* uninstallPackage=CSisRegistryPackage::NewLC(iUid, iPackageName, iVendorName);
		
		RSisRegistrySession registrySession;
		User::LeaveIfError(registrySession.Connect());
		CleanupClosePushL(registrySession);

		RSisRegistryEntry registryEntry;
	
		User::LeaveIfError(registryEntry.Open(registrySession, iUid));
		CleanupClosePushL(registryEntry);

		CSisRegistryPackage* package=registryEntry.PackageL();
		CleanupStack::PushL(package);
		
		if (*package == *uninstallPackage)
			{
				err = -1;
				err=Launcher::Uninstall(*iUi, *package); 
			}
		else
			{
			// check augmenations
			RPointerArray<CSisRegistryPackage> augmentationPackages;
			CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(augmentationPackages);

			registryEntry.AugmentationsL(augmentationPackages);
			for (TInt i=0; i < augmentationPackages.Count(); ++i)
				{
				if (*augmentationPackages[i] == *uninstallPackage)
					{
					err=User::LeaveIfError(Launcher::Uninstall(*iUi, *augmentationPackages[i])); //FIX-ME
					break;
					}
				}
			CleanupStack::PopAndDestroy(&augmentationPackages);
			}	
		
		CleanupStack::PopAndDestroy(3, &registrySession);
		CleanupStack::PopAndDestroy(uninstallPackage);
		error = err;
		}
	return error;
	}

TVerdict CSwisUninstallStep::doTestStepL()
	{
	INFO_PRINTF1(KSwisUninstallStep);

	if (iDoCancelTest)
		{
		// Cancel testing FIX-ME
		TBool cancelled = EFalse;
		TInt dialogNumber = 0;
		iUi->SetCancelDialogTesting(ECancelFromDialog);
		
		RArray<TPtrC> fileNumExist;
		RArray<TPtrC> fileNumNonExist;
		GetFileNamesForCheck(fileNumExist, fileNumNonExist,ECancelType);
				
		RTestUtilSessionSwi util;
		User::LeaveIfError(util.Connect());
		CleanupClosePushL(util);	
		
		while (ETrue)
			{
			iUi->ResetCurrentDialogNumber();
			iUi->SetCancelDialogNumber(dialogNumber);
			
			TInt error=DoUninstallL();
			
			if (iUi->InstallationCancelled())
				{
				cancelled=ETrue;
				
				// Error ought to be KErrCancel
				if (error != KErrCancel)
					{
					ERR_PRINTF3(_L("Installation returned %d, KErrCancel expected, at dialog %d"), error, dialogNumber);
					SetTestStepResult(EFail);
					break;
					}
				else
					{
					INFO_PRINTF2(_L("Installation cancellation at dialog %d successful"), dialogNumber);
					
					// Files which are labelled as existing should not exist since we cancelled.
					CheckIfFilesExist(ETrue,fileNumExist,util);
					CheckIfFilesExist(EFalse,fileNumNonExist,util);

					// we might have failed the test already so stop.
					if (TestStepResult()!=EPass)
						{
						break;
						}
					}
				}
			else
				{
				User::LeaveIfError(error);	
				// No Error, so must have finished installation
				break;
				}
			
			// fail at next dialog
			++dialogNumber;
			}

		// Only check files if this is the last step and we didn't cancel
		if (!cancelled)
			{
			// Files which are labelled as existing should exist since we didn't cancel.
			CheckIfFilesExist(EFalse,fileNumExist,util);
			CheckIfFilesExist(ETrue,fileNumNonExist,util);
			}
		CleanupStack::PopAndDestroy(&util);	
		}
	else
		{		
		TInt err = DoUninstallL();
		CheckPubSubStatus(iPubSubStatus);
		INFO_PRINTF2(_L("DoUninstallL returned %d"), err);
		if (err != KErrNone)
			{
			// Can't set error to KErrServerBusy, or Testexecute will retry.
			SetTestStepResult((err != KErrServerBusy)? static_cast<TVerdict>(err) : EFail);
			}
		}

	return TestStepResult();
	}

/**
 * Override of base class virtual
 * @return TVerdict code
 */
TVerdict CSwisUninstallStep::doTestStepPostambleL()
	{
	CheckExpectedResultCodesL();

	RArray<TPtrC> fileNumExist;
	RArray<TPtrC> fileNumNonExist;
	GetFileNamesForCheck(fileNumExist, fileNumNonExist, ESimpleType);
		
	RTestUtilSessionSwi util;
	User::LeaveIfError(util.Connect());
	CleanupClosePushL(util);
		
	CheckIfFilesExist(ETrue,fileNumExist,util);
	CheckIfFilesExist(EFalse,fileNumNonExist,util);
	CleanupStack::PopAndDestroy(&util);
	
	CompareFilesL();
	CheckPubSubStatusIdle();
	return TestStepResult();
	}


/////////////////////////////////////////////////////////////////////
// CPostInstallRevocationCheck
///////////////////////////////////////////////////////////////////// 
CPostInstallRevocationCheckStep::CPostInstallRevocationCheckStep()
	{
	SetTestStepName(KPirCheck);
	}

TVerdict CPostInstallRevocationCheckStep::doTestStepL()
    {
    RSisRegistryWritableSession session;
    User::LeaveIfError(session.Connect());
    CleanupClosePushL(session);

    SetTestStepResult(EFail);
	
	TInt uid;
	TBool async = EFalse;
    TPtrC uri;
	
    if (GetHexFromConfig(ConfigSection(), _L("uid"), uid))
		{
		WARN_PRINTF2(_L("Checking revocation status of package with UID = %d"), uid);
		}
	else 
	    {
		WARN_PRINTF2(_L("No uid found in config section : %S "), &ConfigSection());
		return TestStepResult();
		}
   	if (!GetStringFromConfig(ConfigSection(), _L("ocsp_uri"), uri))
	    {
	    
	    WARN_PRINTF2(_L("No uid found in config section : %S "), &ConfigSection());
		return TestStepResult();
	    }
    if (!GetBoolFromConfig(ConfigSection(), _L("async"), async))
   	    {
   	    async = EFalse;
   	    }
	
	RSisRevocationEntry entry;
    User::LeaveIfError(entry.Open(session, TUid::Uid(uid)));
    CleanupClosePushL(entry);
	
    if (async)
        {
         WARN_PRINTF2(_L("Async check not yet implemented : %S"), &ConfigSection());
        }
    else
        {   
        const TUint KMaxUriLength=512;
		TBuf8<KMaxUriLength> temp;
		temp.Copy(uri);
		
		TRequestStatus stat;
		
		entry.CheckRevocationStatus(temp, stat);
		TTime t;
		t.UniversalTime();
		INFO_PRINTF2(_L("Called check @ %d"), t.Int64());
        User::After(700000);
		entry.CancelRevocationStatusL();
		t.UniversalTime();
		INFO_PRINTF2(_L("Called cancelled @ %d"), t.Int64());
        User::WaitForRequest(stat);
		INFO_PRINTF2(_L("Cancelled result == %d"), stat.Int());
        
        }
    SetTestStepResult(EPass);
    CleanupStack::PopAndDestroy(2,&session);
    return TestStepResult();
    }

/////////////////////////////////////////////////////////////////////
// CTrustStatusStep
///////////////////////////////////////////////////////////////////// 
CTrustStatusStep::CTrustStatusStep()
	{
	SetTestStepName(KTrustStatus);
	}
	
TPtrC* CTrustStatusStep::GetValidationName(Swi::TValidationStatus aStatus)
    {
    static TPtrC res;
  
    switch (aStatus)
        {
        case Swi::EUnknown:
            res.Set(_L("Unknown"));
            break;          
        case EExpired:     
            res.Set(_L("Expired"));
            break;      
        case EInvalid:                
            res.Set(_L("Invalid"));
            break;
        case EUnsigned:
            res.Set(_L("Unsigned"));
            break;               
        case EValidated:             
            res.Set(_L("Validated"));
            break; 
        case EValidatedToAnchor:
            res.Set(_L("ValidatedToAnchor"));
            break;
        case EPackageInRom:
            res.Set(_L("PackageInRom"));
            break;
        };
    return &res;    
       
    }
   
TPtrC* CTrustStatusStep::GetRevocationName(Swi::TRevocationStatus aStatus)
    {
    static TPtrC res;
   
    switch (aStatus)
        {
        case EUnknown2:
            res.Set(_L("Unknown"));
            break;
        case EOcspNotPerformed:  
            res.Set(_L("NotPerformed"));
            break;
        case EOcspRevoked:        
            res.Set(_L("OcspRevoked"));
            break;
        case EOcspUnknown:        
            res.Set(_L("OcspUnknown"));
            break;
        case EOcspTransient:      	
            res.Set(_L("OcspTransient"));
            break;
        case EOcspGood:
            res.Set(_L("OcspGood"));
            break;
        }
    return &res;
    }
namespace 
{
    
TBuf<256>* Date2Str(TBuf<256>& aBuffer, const TTime& aTime)
    {
    _LIT(KFormatTxt,"%H%:1%T%:2%S on %1%/1%2%/2%3");
    aTime.FormatL(aBuffer, KFormatTxt);
   
    return &aBuffer;
    }
}
TVerdict CTrustStatusStep::doTestStepL()
	{
	
    RSisRegistrySession session;
	User::LeaveIfError(session.Connect());
    CleanupClosePushL(session);

    SetTestStepResult(EFail);
	
	TInt uid;
	TInt expectedValid;
	TInt expectedRevoked;
	TInt expectedTrusted;
	
	if (GetHexFromConfig(ConfigSection(), _L("uid"), uid))
		{
		WARN_PRINTF2(_L("Retrieving Package with UID = %d"), uid);
		}
	else 
	    {
		WARN_PRINTF2(_L("No uid found in config section : %S "), &ConfigSection());
		return TestStepResult();
		}
	if (!GetIntFromConfig(ConfigSection(), _L("valid_status"), expectedValid))
	    {
	    WARN_PRINTF2(_L("No valid_status found in config section : %S "), &ConfigSection());
		return TestStepResult();
	    }
	if (!GetIntFromConfig(ConfigSection(), _L("rev_status"), expectedRevoked))
	    {
        WARN_PRINTF2(_L("No rev_status found in config section : %S "), &ConfigSection());
		return TestStepResult();
		
	    }
	if (!GetIntFromConfig(ConfigSection(), _L("trusted"), expectedTrusted))
	    {
	    WARN_PRINTF2(_L("No trusted found in config section : %S "), &ConfigSection());
		return TestStepResult();
		}
		
    Swi::RSisRegistryEntry entry;
	User::LeaveIfError(entry.Open(session, TUid::Uid(uid)));
	CleanupClosePushL(entry);
	
	TSisTrustStatus trustStatus = entry.TrustStatusL();

	SetTestStepResult(EPass);
	
	if (expectedTrusted != trustStatus.IsTrusted())
	    {
	    WARN_PRINTF3(_L("Trust mismatch: Expected %d,  Got %d"), 
	        expectedTrusted, trustStatus.IsTrusted());
	    SetTestStepResult(EFail);  
		}
	if (expectedValid != trustStatus.ValidationStatus())
	    {
	    WARN_PRINTF3(_L("Validation mismatch: Expected %d,  Got %d"), 
	        expectedValid, trustStatus.ValidationStatus());
	    SetTestStepResult(EFail);  
		}
	if (expectedRevoked != trustStatus.RevocationStatus())
	    {
	    WARN_PRINTF3(_L("Revocation mismatch: Expected %d,  Got %d"), 
	        expectedRevoked, trustStatus.RevocationStatus());
	    SetTestStepResult(EFail);  
		}
			
	TPtrC trusted(trustStatus.IsTrusted() ? _L("True") : _L("False"));
	INFO_PRINTF2(_L("Trusted = %S"), &trusted);	
    INFO_PRINTF2(_L("Validation = %S"),GetValidationName(trustStatus.ValidationStatus()));
    INFO_PRINTF2(_L("Revocation = %S"),GetRevocationName(trustStatus.RevocationStatus()));
    TBuf<256> buffer;
    INFO_PRINTF2(_L("Result Date= %S"),Date2Str(buffer, trustStatus.ResultDate()));
    INFO_PRINTF2(_L("Attempt Date= %S"),Date2Str(buffer, trustStatus.LastCheckDate()));
    
    //INFO_PRINTF2(_L("Num Chains checked = %d"),entry.EntryObject().CertChainIndices());
        
	CleanupStack::PopAndDestroy(2,&session);
	return TestStepResult();
	}

//
// CSwisMmcStep - see tswistep.h for descriptions
//

CSwisMmcStep::CSwisMmcStep(TMmcOperation aOperation)
	: iOperation(aOperation)
	{
	}

CSwisMmcStep::~CSwisMmcStep()
	{
	#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	#ifndef SWI_TEXTSHELL_ROM
	if (iBootMode != KTextShell)
		{
		delete iSwiSidChecker;
		}
	#endif
	#endif
	}

TVerdict CSwisMmcStep::doTestStepPreambleL()
	{
	// Get the System Startup Mode
	User::LeaveIfError(RProperty::Get(KUidSystemCategory, KSystemStartupModeKey, iBootMode));
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
#ifndef SWI_TEXTSHELL_ROM
	if (iBootMode != KTextShell)
		{
		if(!iSwiSidChecker && iOperation == EQuerySidViaApparcPlugin)
			{
			iSwiSidChecker = CAppSidChecker::CheckerForAppType(TUid::Uid(0));
			if(!iSwiSidChecker)
				{
				User::Leave(KErrNotSupported);
				}
			}
		}
#else
	// Since in Textshell ROM SSCForStartupMode1_target.rsc was replaces as SSCForStartupMode0.rsc
	// we always get the boot mode as 0 , reset the value to 1.
	iBootMode = KTextShell;
#endif
#else
	iBootMode = KTextShell;
#endif

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	if((iOperation == EMount) || 
	   (iOperation == EUnMount) ||
	   (iOperation == EFormat))
		{
		// get step parameters
		TPtrC str;
		if (!GetStringFromConfig(ConfigSection(), _L("drivechar"), str))
			{
			ERR_PRINTF1(_L("Missing drivechar setting"));
			SetTestStepResult(EFail);
			}
		else
			{
			User::LeaveIfError(fs.CharToDrive(str[0], iDrive));
			iDriveChar = str[0];
			}
		}
	CleanupStack::PopAndDestroy(&fs);
	return TestStepResult();
	}

TVerdict CSwisMmcStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CSwisMmcStep::doTestStepL()
	{
	RTestUtilSessionSwi testutil;
	User::LeaveIfError(testutil.Connect());
	CleanupClosePushL(testutil);

	switch(iOperation)
		{
		case EFormat:
			{
			TBool formatFatTableOnly = EFalse;
   			GetBoolFromConfig(ConfigSection(), _L("formatFatTableOnly"), formatFatTableOnly);
			User::LeaveIfError(testutil.FormatDrive(iDrive, formatFatTableOnly));
			break;
			}
		case EMount:
			User::LeaveIfError(testutil.MountDrive(iDrive));
			break;
		case EUnMount:
			User::LeaveIfError(testutil.UnMountDrive(iDrive));
			break;
		case ECopyFiles:
			{
			_LIT(KCopyNum, "copynum"); // this specifies how many files to copy
			_LIT(KCopyFrom, "copyfrom"); // + number (0-based) = file to copy from
			_LIT(KCopyTo, "copyto"); // + number (0-based) = file to copy to
			CopyFilesL(KCopyNum, KCopyFrom, KCopyTo);

			break;
			}
		case ECheckFiles:
			{
			RArray<TPtrC> fileNumExist;
			RArray<TPtrC> fileNumNonExist;
			GetFileNamesForCheck(fileNumExist, fileNumNonExist, ESimpleType);
		
			RTestUtilSessionSwi util;
			User::LeaveIfError(util.Connect());
			CleanupClosePushL(util);
		
			CheckIfFilesExist(ETrue,fileNumExist,util,20000);
			CheckIfFilesExist(EFalse,fileNumNonExist,util,100);
			CleanupStack::PopAndDestroy(&util);
					
			CompareFilesL();
			break;
			}
		case EResetTags:
			{
			RFs fs;
			User::LeaveIfError(fs.Connect());
			CleanupClosePushL(fs);
			
			CFileMan *fileMan = CFileMan::NewL(fs);
			CleanupStack::PushL(fileMan);
			//User::LeaveIfError(fileMan->RmDir(KTAGDIR));
			// On the H4 the following call works correctly, BUT returns -12!!!
			// Even if it failed, the CheckFilesStep would detect it.
			TDriveUnit sysDrive (fs.GetSystemDrive());
			TBuf<128> tagDir (sysDrive.Name());
			tagDir.Append(_L("\\dummy-swi-plugin\\"));
			(void) fileMan->RmDir(tagDir);
			CleanupStack::PopAndDestroy(fileMan);
			CleanupStack::PopAndDestroy(&fs);
			break;
			}
		case EDeleteFiles:
 			{
 			_LIT(KDeleteNum, "deletenum"); // this specifies how many files to copy
 			_LIT(KDeleteFileBase, "deletefile"); // + number (0-based) = file to copy from
 			DeleteFilesL(KDeleteNum, KDeleteFileBase);
 			break;
 			}
		case EGetNumFiles:
			{
			TPtrC fname;
			TInt numFiles=0;
			TInt retNum;
			
			if (!GetStringFromConfig(ConfigSection(), _L("dirpath"), fname))
				{
				// the string must exist, otherwise the config is invalid
                ERR_PRINTF1(_L("Missing dirpath"));
                SetTestStepResult(EFail);
                break;
				}
				
			if (!GetIntFromConfig(ConfigSection(), _L("numfiles"), numFiles))
   				{
				// the string must exist, otherwise the config is invalid
                ERR_PRINTF1(_L("Missing numfiles"));
                SetTestStepResult(EFail);
                break;
				}
			
			retNum = testutil.GetNumFilesL(fname);
			
			if (numFiles != retNum)
				{
				ERR_PRINTF3(_L("numFiles=%d NOT match returned number-of-files(%d)"), numFiles, retNum);
				SetTestStepResult(EFail);
				}
			break;
			}
		case EAskReg:
			{
			TInt uidValue;
			if (!GetHexFromConfig(ConfigSection(), _L("uid"), uidValue))
				{
				ERR_PRINTF1(_L("Missing uid"));
				SetTestStepResult(EFail);
				break;
				}

			TBool expectInstalled;
			// Default behaviour is to expect the app with the specified UID
			// to be installed.
			if (!GetBoolFromConfig(ConfigSection(), _L("expectinstalled"), expectInstalled))
				{
				expectInstalled = ETrue;
				}

			TBool expectPresent;
			// Default behaviour is to expect the app with the specified UID
			// to be present.
			if(!GetBoolFromConfig(ConfigSection(), _L("expectpresent"), expectPresent))
				{
				expectPresent = ETrue;
				}
			
			TBool checkAug = EFalse;
			TPtrC augName;
			if(GetStringFromConfig(ConfigSection(), _L("packageName"), augName))
				{
				checkAug = ETrue;
				}

			TUid uid;
			uid.iUid = uidValue;

			Swi::RSisRegistrySession session;
			CleanupClosePushL(session);
			User::LeaveIfError(session.Connect());

			Swi::RSisRegistryEntry entry;
			CleanupClosePushL(entry);

			if (entry.Open(session, uid) == KErrNotFound)
				{
				if (expectInstalled)
					{
					if(checkAug)
						{
						WARN_PRINTF2(_L("Failed to find base package for patch, UID = 0x%x"), uid);
						}
					else
						{
						WARN_PRINTF2(_L("Failed to find package, UID = 0x%x"), uid);
						}
					
					SetTestStepResult(EFail);
					}
				}
			else
				{
				if(!checkAug)
					{
					TBool present = entry.IsPresentL();
					if(present != expectPresent)
						{
						WARN_PRINTF4(_L("Presence mismatch for package, UID = 0x%x - expected %d got %d"), 
									 uid, expectPresent, present);
						SetTestStepResult(EFail);
						}
					}
				else
					{
					// Search for aug
					RPointerArray<CSisRegistryPackage> augmentationPackages;
					CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(augmentationPackages);
					entry.AugmentationsL(augmentationPackages);
					TBool found = EFalse;
					for (TInt i=0; i < augmentationPackages.Count(); ++i)
						{
						if (augmentationPackages[i]->Name() == augName)
							{
							found = ETrue;
							RSisRegistrySession registrySession;
							User::LeaveIfError(registrySession.Connect());
							CleanupClosePushL(registrySession);
							RSisRegistryEntry registryEntry;
							const CSisRegistryPackage &augPackage = *augmentationPackages[i];
							User::LeaveIfError(registryEntry.OpenL(registrySession, augPackage));
							CleanupClosePushL(registryEntry);
							TBool present = registryEntry.IsPresentL();
							CleanupStack::PopAndDestroy(&registryEntry);
							CleanupStack::PopAndDestroy(&registrySession);

							if(present != expectPresent)
								{
								WARN_PRINTF4(_L("Presence mismatch for patch, UID = 0x%x - expected %d got %d"), 
											 uid, expectPresent, present);
								SetTestStepResult(EFail);
								break;
								}
							}
						}
					CleanupStack::PopAndDestroy(&augmentationPackages);
					if(!found)
						{
						WARN_PRINTF3(_L("Failed to registry entry for patch, UID = 0x%x %S"), uid, &augName);
						SetTestStepResult(EFail);
						}
					}
				}
			CleanupStack::PopAndDestroy(2, &session);
			break;
			}
		case ESetClearReadOnly:
			{
			_LIT(KSetClearNum, "setclearnum"); // this specifies how many files to copy
			_LIT(KSetFileBase, "setclearfile"); // + number (0-based) = file to copy from
			_LIT(KSetOpBase, "setclearoperation"); // + number (0-based) = flag to set (non-zero) or clear (zero)
			SetClearReadOnlyFilesL(KSetClearNum, KSetFileBase, KSetOpBase);
			break;
			}
		case EQuerySidViaApparcPlugin:
			{
			if (iBootMode == KTextShell) 
				{
				// emulator tests running in textshell or in textshell ROM (#def SWI_TEXTSHELL_ROM)
				WARN_PRINTF1(_L("Sid Checking using Apparc not supported in TEXTSHELL"));			
				SetTestStepResult(EFail);	
				}
			else
				{
				#ifndef SWI_TEXTSHELL_ROM
					TInt sidValue;
					if (!GetHexFromConfig(ConfigSection(), _L("sid"), sidValue))
						{
						ERR_PRINTF1(_L("Missing sid"));
						SetTestStepResult(EFail);
						break;
						}
					TUid sid;
					sid.iUid = sidValue;
		
					TInt expectMatchingDrives;
					// expectMatchingDrives is defined for this test as a bit mask map of all the drives that return 'true' in the function "AppRegisteredAt" 
					// The least significant bit will be matched to drive A. Drive B will be matched to second bit and so on...
						
					// Default behaviour is to expect an exe with the specified SID on drive C ,Just for convinience.
					// Drive C is 4th bit in drive bit mask.
					if(!GetHexFromConfig(ConfigSection(), _L("expectMatchingDrives"), expectMatchingDrives))
						{
						expectMatchingDrives = 4;
						}
			
					TInt presentMatchingDrives = 0;
					#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
					for(TInt drive=EDriveA; drive<=EDriveZ; ++drive)
						{
							if(iSwiSidChecker->AppRegisteredAt(sid, drive))
								{
								// Adds new bit to bit mask if return is true.
								presentMatchingDrives = presentMatchingDrives + (1 << (drive - EDriveA));
								INFO_PRINTF2(_L("AppRegisteredAt returned true for drive: %d"), drive);
								}  
						}
					#endif
					if(presentMatchingDrives != expectMatchingDrives)
						{
						WARN_PRINTF4(_L("Matching drives mismatch for exe, SID = 0x%x - expected %d got %d"), 
									sid, expectMatchingDrives, presentMatchingDrives);
						SetTestStepResult(EFail);
						}
					
				#endif
				}
			break;
			}

		default:
			SetTestStepResult(EFail);
			break;
		}
	
	CleanupStack::PopAndDestroy(&testutil);
	return TestStepResult();
	}

//
// CSwisUninstallAppsStep - see tswistep.h for descriptions
//

CSwisUninstallPkgsStep::CSwisUninstallPkgsStep(TUnInstallAppType aUnInstallAppType)
	: iUnInstallAppType(aUnInstallAppType)
	{
	}

CSwisUninstallPkgsStep::~CSwisUninstallPkgsStep()
	{
	}

TVerdict CSwisUninstallPkgsStep::doTestStepPreambleL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	TPtrC str;
	if (!GetStringFromConfig(ConfigSection(), _L("drivechar"), str))
		{
		ERR_PRINTF1(_L("Missing drivechar setting"));
		SetTestStepResult(EFail);
		}
	else
		{
		User::LeaveIfError(fs.CharToDrive(str[0], iDrive));
		iDriveChar = str[0];
		}
	CleanupStack::PopAndDestroy(&fs);
 
	return TestStepResult();
	}

TVerdict CSwisUninstallPkgsStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CSwisUninstallPkgsStep::doTestStepL()
	{
	RPointerArray<CUninstalledPackageEntry> uninstalledPkgEntry;
	CleanupResetAndDestroy<RPointerArray<CUninstalledPackageEntry> >::PushL(uninstalledPkgEntry);

	TDriveNumber drive = TDriveNumber(iDrive);
	_LIT(KPkgName, "pkgname");
	TBuf<KSetBufSize> pkgBufferName(KPkgName);
	_LIT(KPkgVendor, "pkgvendor");
	TBuf<KSetBufSize> pkgBufferVendor(KPkgVendor);
	_LIT(KPkgUid, "pkguid");
	TBuf<KSetBufSize> pkgBufferUid(KPkgUid);
	
	_LIT(KPkgMajor, "pkgmajor");
	TBuf<KSetBufSize> pkgBufferMajor(KPkgMajor);
	_LIT(KPkgMinor, "pkgminor");
	TBuf<KSetBufSize> pkgBufferMinor(KPkgMinor);
	_LIT(KPkgBuild, "pkgbuild");
	TBuf<KSetBufSize> pkgBufferBuild(KPkgBuild);
	
	_LIT(KPkgType, "pkgtype");
	TBuf<KSetBufSize> pkgBufferType(KPkgType);
	
			
	switch(iUnInstallAppType)
		{
		case EListPkgs:
			{
			TRAPD(err,UninstalledSisPackages::ListL(drive, uninstalledPkgEntry));
			if(err!=KErrNone)
				{
				INFO_PRINTF1(_L("Error while listing"));
				SetTestStepError(err);
				CleanupStack::PopAndDestroy(&uninstalledPkgEntry);
				return TestStepResult();
				}
			TInt noOfEntries;
			GetIntFromConfig(ConfigSection(),_L("count"),noOfEntries);
			if(noOfEntries!=uninstalledPkgEntry.Count())
				{
				INFO_PRINTF3(_L("ListL Failed----Expected %d Got %d "), noOfEntries, uninstalledPkgEntry.Count());
				SetTestStepResult(EFail);
				}
				
			for(TInt loopCnt=0;loopCnt<uninstalledPkgEntry.Count();loopCnt++)	
				{
				CUninstalledPackageEntry* uninstallPkg= uninstalledPkgEntry[loopCnt];
				TPtrC pkgName = uninstallPkg->Name();
				TPtrC expectedPkgName;
				TBuf<KSetBufSize> pkgBufNames(pkgBufferName);
				pkgBufNames.AppendNum(loopCnt);
				GetStringFromConfig(ConfigSection(),pkgBufNames,expectedPkgName);
				if(expectedPkgName.Compare(pkgName)!=0)
					{
					INFO_PRINTF1(_L("ListL PackageName Failed"));
					SetTestStepResult(EFail);
					}
				}
			}
			break;
		case ERemovePkgs:
			{
			TInt noOfEntries,noOfPkgsToBeDeleted;
			TPtrC expectedPkgName;
			GetIntFromConfig(ConfigSection(),_L("count"),noOfEntries);
			TRAPD(err,UninstalledSisPackages::ListL(drive, uninstalledPkgEntry));
		
			if(err == KErrServerBusy)
				{
				INFO_PRINTF1(_L("Server Busy"));
				CleanupStack::PopAndDestroy(&uninstalledPkgEntry);
				return TestStepResult();
				}
			if(noOfEntries!=uninstalledPkgEntry.Count())
				{
				INFO_PRINTF3(_L("ListL Failed----Expected %d Got %d "), noOfEntries, uninstalledPkgEntry.Count());
				SetTestStepResult(EFail);
				CleanupStack::PopAndDestroy(&uninstalledPkgEntry);
				return TestStepResult();
				}
			TBool delFile = EFalse;
			if((GetBoolFromConfig(ConfigSection(),_L("deletefile"),delFile)))
				{
				if(delFile)
					{
					_LIT(KDeleteNum, "deletenum"); // this specifies how many files to copy
	 				_LIT(KDeleteFileBase, "deletefile"); // + number (0-based) = file to copy from
	 				DeleteFilesL(KDeleteNum, KDeleteFileBase);
	 				}
				}
			if(!(GetIntFromConfig(ConfigSection(),_L("noOfPkgToBeDeleted"),noOfPkgsToBeDeleted)))
				{
					for(TInt loopCnt=0;loopCnt<noOfEntries;loopCnt++)
						{
						CUninstalledPackageEntry* uninstallPkg= uninstalledPkgEntry[loopCnt];
						UninstalledSisPackages::RemoveL(*uninstallPkg);	
						}
				}
			else
				{
				RArray<TPtrC> pkgDelete;
				for(TInt cnt=0;cnt<noOfPkgsToBeDeleted;cnt++)
					{
					TBuf<KSetBufSize> pkgBufNames(pkgBufferName);
					pkgBufNames.AppendNum(cnt);
					GetStringFromConfig(ConfigSection(),pkgBufNames,expectedPkgName);
					pkgDelete.Append(expectedPkgName);
					}
				for(TInt loopCnt=0;loopCnt<noOfEntries;loopCnt++)
					{
					CUninstalledPackageEntry* uninstallPkg= uninstalledPkgEntry[loopCnt];
					if(pkgDelete.Find(uninstallPkg->Name())!=KErrNone)
						{
						UninstalledSisPackages::RemoveL(*uninstallPkg);		
						}
					}
				}
				
			}
			break;
		case EPkgDetails:
			{
			TInt noOfEntries;
			GetIntFromConfig(ConfigSection(),_L("count"),noOfEntries);
			UninstalledSisPackages::ListL(drive, uninstalledPkgEntry);
			if(noOfEntries!=uninstalledPkgEntry.Count())
				{
				INFO_PRINTF1(_L("ListL Failed"));
				SetTestStepResult(EFail);
				CleanupStack::PopAndDestroy(&uninstalledPkgEntry);
				return TestStepResult();
				}
			for(TInt loopCnt=0;loopCnt<noOfEntries;loopCnt++)	
				{
				CUninstalledPackageEntry* uninstallPkg= uninstalledPkgEntry[loopCnt];
				TPtrC pkgName = uninstallPkg->Name();
				TPtrC expectedPkgName;
				TBuf<KSetBufSize> pkgBufNames(pkgBufferName);
				pkgBufNames.AppendNum(loopCnt);
				GetStringFromConfig(ConfigSection(),pkgBufNames,expectedPkgName);
				if(expectedPkgName.Compare(pkgName)!=0)
					{
					INFO_PRINTF1(_L("ListL PackageName Failed"));
					SetTestStepResult(EFail);
					CleanupStack::PopAndDestroy(&uninstalledPkgEntry);
					return TestStepResult();
					}
					
					
				TBuf<KSetBufSize> pkgBufVendor(pkgBufferVendor);
				pkgBufVendor.AppendNum(loopCnt);
				TPtrC expectedPkgVendor;
				TPtrC pkgVendor = uninstallPkg->Vendor();
				GetStringFromConfig(ConfigSection(),pkgBufVendor,expectedPkgVendor);
				if(expectedPkgVendor.Compare(pkgVendor )!=0)
					{
					INFO_PRINTF1(_L("ListL PackageVendor Failed"));
					SetTestStepResult(EFail);
					CleanupStack::PopAndDestroy(&uninstalledPkgEntry);
					return TestStepResult();
					}
				
				
				TBuf<KSetBufSize> pkgBufUid(pkgBufferUid);
				pkgBufUid.AppendNum(loopCnt);
				TInt expectedPkgUidTemp;
				TUid pkgUid = uninstallPkg->Uid();
				GetHexFromConfig(ConfigSection(),pkgBufUid,expectedPkgUidTemp);
				TUid expectedPkgUid = TUid::Uid(expectedPkgUidTemp);
				if(expectedPkgUid!=pkgUid)
					{
					INFO_PRINTF1(_L("ListL uid Failed"));
					SetTestStepResult(EFail);
					CleanupStack::PopAndDestroy(&uninstalledPkgEntry);
					return TestStepResult();
					}
				
				
				
				TVersion pkgVersion = uninstallPkg->Version();
				TBuf<KSetBufSize> pkgBufMajor(pkgBufferMajor);
				pkgBufMajor.AppendNum(loopCnt);
				TBuf<KSetBufSize> pkgBufMinor(pkgBufferMinor);
				pkgBufMinor.AppendNum(loopCnt);
				TBuf<KSetBufSize> pkgBufBuild(pkgBufferBuild);
				pkgBufBuild.AppendNum(loopCnt);
				TInt pkgMajor,pkgMinor,pkgBuild;
				GetIntFromConfig(ConfigSection(),pkgBufMajor,pkgMajor);
				GetIntFromConfig(ConfigSection(),pkgBufMinor,pkgMinor);
				GetIntFromConfig(ConfigSection(),pkgBufBuild,pkgBuild);
				TVersion expectedPkgVersion(pkgMajor,pkgMinor,pkgBuild);
				if(expectedPkgVersion.Name()!=pkgVersion.Name())
					{
					INFO_PRINTF1(_L("Version not matched"));
					CleanupStack::PopAndDestroy(&uninstalledPkgEntry);
					return TestStepResult();
					}
				
				TInt expectedPkgType= uninstallPkg->PackageType();
				TBuf<KSetBufSize> pkgBufType(pkgBufferType);
				pkgBufType.AppendNum(loopCnt);
				TInt pkgType;
				GetIntFromConfig(ConfigSection(),pkgBufType,pkgType);
				if(expectedPkgType!=pkgType)
					{
					INFO_PRINTF1(_L("Package Type Not Matched"));
					CleanupStack::PopAndDestroy(&uninstalledPkgEntry);
					return TestStepResult();
					}
				
				}
			}
		}
	CleanupStack::PopAndDestroy(&uninstalledPkgEntry);
	return TestStepResult();
	}


//
// CSwisCheckPublishUidStep
//

CSwisCheckPublishUidStep::CSwisCheckPublishUidStep()
    {
    // Call base class method to set up the human readable name for logging
    SetTestStepName(KSwisGetPublishedUidArrayStep);
    }
      
/**
 * Override of base class virtual. Prepares for the test run of SWIS
 * @return TVerdict code
 */
TVerdict CSwisCheckPublishUidStep::doTestStepPreambleL()
    {
    INFO_PRINTF1(_L("CSwisCheckPublishUidStep::doTestStepPreambleL"));
    _LIT(KDefineProperty, "DefineProperty");
    iJustDefineProperty=EFalse;
    if (GetBoolFromConfig(ConfigSection(),KDefineProperty,iJustDefineProperty) )
        {
        if(iJustDefineProperty)
            {
            INFO_PRINTF1(_L("CSwisCheckPublishUidStep is running in define Mode..just define the property"));
            return TestStepResult();
            }
        }
    
	_LIT(KPublishedUidCount, "PublishedUidCount");
	_LIT(KPublishedUidValue, "PublishedUidValue");
	TInt expectedUidCount;
	TBuf<20> publishedUidParam;
	
	if (!GetIntFromConfig(ConfigSection(),KPublishedUidCount,expectedUidCount))
		{
        ERR_PRINTF1(_L("No uid mentioned in .ini"));
		return TestStepResult();
		}

	iExpectedUidList[0].iUid = expectedUidCount;
	if(expectedUidCount>=KTestMaxUidCount-1)
	    {
        ERR_PRINTF1(_L("The buffer available holds upto 15 Uids"));
        return TestStepResult();
	    }
	
	for (TInt i=0; i<iExpectedUidList[0].iUid; i++)
		{
        TInt uid;
		publishedUidParam = KPublishedUidValue;
		GenerateIndexedAttributeNameL(publishedUidParam, i);
		if (!GetHexFromConfig(ConfigSection(), publishedUidParam, uid))
			{
   			ERR_PRINTF1(_L("Missing uid"));
			continue;
   			}
		TUid pkgUid;
		pkgUid.iUid=uid;
		iExpectedUidList[i+1]= pkgUid;
		}
    return TestStepResult();
    }
	
void CSwisCheckPublishUidStep::GenerateIndexedAttributeNameL(TDes& aInitialAttributeName, TInt aIndex)
	{
	const TInt MAX_INT_STR_LEN = 8;
	TBuf<MAX_INT_STR_LEN> integerAppendStr;
	integerAppendStr.Format(_L("%d"), aIndex);
	aInitialAttributeName.Append(integerAppendStr);
	}


TVerdict CSwisCheckPublishUidStep::doTestStepL()
    {
    INFO_PRINTF1(KSwisGetPublishedUidArrayStep);
    TInt err=0;
    if(iJustDefineProperty)
        {
         err = RProperty::Define(KUidSystemCategory, KSWIUidsCurrentlyBeingProcessed, RProperty::EByteArray);
        if (err != KErrNone && err != KErrAlreadyExists)
            User::Leave(err);
        return TestStepResult();
        }
      
    TUid expUid;
    TInt expCount;
    RArray<TUid> retrivedUidList;
    CleanupClosePushL(retrivedUidList);
    expCount=iExpectedUidList[0].iUid;
    if(KErrNone!=GetAllUids(retrivedUidList))
        {
        ERR_PRINTF1(_L("Failed to read all uids "));
        SetTestStepResult(EFail);
        CleanupStack::PopAndDestroy(&retrivedUidList);
        return TestStepResult();
        }
    if (expCount!=retrivedUidList.Count())
        {
        ERR_PRINTF3(_L("expected and read count is not same , expected count is %d  read count is %d"),expCount, retrivedUidList.Count());
        SetTestStepResult(EFail);
        CleanupStack::PopAndDestroy(&retrivedUidList);
        return TestStepResult();
        }
	for(TInt i=0;i<expCount;i++)
        {
        expUid=iExpectedUidList[i+1];
        INFO_PRINTF2(_L("Expected package uid %x"), expUid.iUid);
        if (retrivedUidList.Find(expUid)==KErrNotFound)
            {
            ERR_PRINTF3(_L("Expected uid %x is not found and return code for retrivedUidList.Find is %d "), expUid.iUid,err);
            SetTestStepResult(EFail);
            }
        }
    CleanupStack::PopAndDestroy(&retrivedUidList);
    return TestStepResult();
    }

TVerdict CSwisCheckPublishUidStep::doTestStepPostambleL()
    {
    if(!iJustDefineProperty)
           {
        TInt err = RProperty::Delete(KUidSystemCategory,KSWIUidsCurrentlyBeingProcessed);
        if(err != KErrNone)
            {
            ERR_PRINTF1(_L("Not able to delete property "));
            }
           return TestStepResult();
           }
           
    return TestStepResult();
    }

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
/**
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters from an ini file section
 * @return TVerdict code
 */
TVerdict CCheckScrFieldStep::doTestStepL()
	{
	SetTestStepName(KCheckSCRFieldStep);	
	INFO_PRINTF1(KCheckSCRFieldStep);
	
	// UID of the component to be checked
	TUid uid;
	// Which field of the component to be checked
	TPtrC fieldToCheck;
	// And it's expected value
	TBool expectedValue;

	SetTestStepResult(EPass);
	
	TInt tuid;
	// get step input parameters from the configuration (given in ini file).
	if (!GetStringFromConfig(ConfigSection(), _L("fieldname"), fieldToCheck))
		{
		ERR_PRINTF1(_L("Missing the field name to be checked"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}	
	else if (!GetHexFromConfig(ConfigSection(), _L("uid"), tuid))
		{
		ERR_PRINTF1(_L("Missing UID of the component to be checked"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else if (!GetBoolFromConfig(ConfigSection(), _L("expectedvalue"), expectedValue))
		{
		ERR_PRINTF1(_L("Missing expected value to be compared"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	// If not left in the above condition, all the inputs are available. So, continue testing.
	uid.iUid = tuid;
	
	// Session handle to Software Component Registry
	Usif::RSoftwareComponentRegistry scrSession;	
	User::LeaveIfError(scrSession.Connect());
	CleanupClosePushL(scrSession);
	
	// Get the right component by filtering against the given uid.
	_LIT(KCompUid, "CompUid");
	Usif::CComponentFilter* componentFilter = Usif::CComponentFilter::NewLC();
	componentFilter->SetSoftwareTypeL(Usif::KSoftwareTypeNative);
	componentFilter->AddPropertyL(KCompUid, uid.iUid);
		
	Usif::RSoftwareComponentRegistryView subSession;	
	subSession.OpenViewL(scrSession, componentFilter);
	CleanupClosePushL(subSession);
	
	Usif::CComponentEntry* compEntry = NULL;
	compEntry = subSession.NextComponentL();
	
	// If no component found, fail the step.
	if (compEntry == NULL)
		{
		// Close the SCR sub session and session.
		CleanupStack::PopAndDestroy(3, &scrSession); //subsession, componentFilter, scrSession
		INFO_PRINTF2(_L("Component %x is not found in SCR"), uid.iUid);
		SetTestStepResult(EFail);
		return TestStepResult();
		}		
	CleanupStack::PushL(compEntry);
	
	TBool actualValue(EFalse);
	if (!fieldToCheck.Compare(_L("originverified")))
		{
		actualValue = compEntry->IsOriginVerified();	
		}
	else if(!fieldToCheck.Compare(_L("knownrevoked")))
		{
		actualValue = compEntry->IsKnownRevoked();
		}
	else if(!fieldToCheck.Compare(_L("drmprotected")))
		{
		actualValue = compEntry->IsDrmProtected();		
		}
	else if(!fieldToCheck.Compare(_L("hidden")))
		{
		actualValue = compEntry->IsHidden();
		}

	if (expectedValue != actualValue)
		{
		SetTestStepResult(EFail);
		}
		
	CleanupStack::PopAndDestroy(4, &scrSession); //compEntry, subSession, componentFilter, scrSession
	return TestStepResult();
	}
	
TVerdict CCheckScrCompPropertyStep::doTestStepL()
	{
	SetTestStepName(KCheckSCRCompPropertyStep);
	INFO_PRINTF1(KCheckSCRCompPropertyStep);
	
	// UID of the component to be checked
	TUid uid;
	// Which property of the component to be checked
	TPtrC propertyToCheck;
	// And it's expected value
	TInt expectedValue;

	SetTestStepResult(EPass);
	
	TInt tuid;
	// get step input parameters from the configuration (given in ini file).
	if (!GetStringFromConfig(ConfigSection(), _L("propertyname"), propertyToCheck))
		{
		ERR_PRINTF1(_L("Missing the property name to be checked"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}	
	else if (!GetHexFromConfig(ConfigSection(), _L("uid"), tuid))
		{
		ERR_PRINTF1(_L("Missing UID of the component to be checked"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	else if (!GetIntFromConfig(ConfigSection(), _L("expectedvalue"), expectedValue))
		{
		ERR_PRINTF1(_L("Missing expected value to be compared"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	// If not left in the above condition, all the inputs are available. So, continue testing.
	uid.iUid = tuid;
	
	// Session handle to Software Component Registry
	Usif::RSoftwareComponentRegistry scrSession;	
	User::LeaveIfError(scrSession.Connect());
	CleanupClosePushL(scrSession);
	
	// Get the right component by filtering against the given uid.
	_LIT(KCompUid, "CompUid");
	Usif::CComponentFilter* componentFilter = Usif::CComponentFilter::NewLC();
	componentFilter->SetSoftwareTypeL(Usif::KSoftwareTypeNative);
	componentFilter->AddPropertyL(KCompUid, uid.iUid);

	Usif::RSoftwareComponentRegistryView subSession;	
	subSession.OpenViewL(scrSession, componentFilter);
	CleanupClosePushL(subSession);
	
	Usif::CComponentEntry* compEntry = NULL;
	compEntry = subSession.NextComponentL();
	
	// If no component found, fail the step.
	if (compEntry == NULL)
		{
		// Close the SCR sub session and session.
		CleanupStack::PopAndDestroy(3, &scrSession); //subsession, componentFilter, scrSession
		INFO_PRINTF2(_L("Component %x is not found in SCR"), uid.iUid);
		SetTestStepResult(EFail);
		return TestStepResult();
		}		
	CleanupStack::PushL(compEntry);
	
	TInt actualValue = 0;	
	Usif::CIntPropertyEntry* propertyEntry = NULL;
	
	propertyEntry = static_cast<Usif::CIntPropertyEntry *>(scrSession.GetComponentPropertyL(compEntry->ComponentId(), propertyToCheck));
	
	__ASSERT_ALWAYS(propertyEntry, User::Leave(KErrAbort));
		
	actualValue = propertyEntry->IntValue();
	delete propertyEntry;
			
	if (expectedValue != actualValue)
		{
		SetTestStepResult(EFail);
		}
		
	CleanupStack::PopAndDestroy(4, &scrSession); //compEntry, subSession, componentFilter, scrSession
	return TestStepResult();
	}

#endif

//
// This ECOM loader function in apfile.dll is NOT exported so we need
// to duplicate it here so we can load the apparc plugin to be able to test it.
//
//const TUid KAppSidCheckerInterfaceUid = {0x10281FBB};
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#ifndef SWI_TEXTSHELL_ROM
const TUid KAppSidCheckerInterfaceUidv2 = {0x20007D8C};

CAppSidChecker* CAppSidChecker::CheckerForAppType(TUid aAppTypeUid)
	{
	TEComResolverParams resolverParams;
	TBuf8<KMaxUidName> sidName;
	sidName.Copy(aAppTypeUid.Name());
	resolverParams.SetDataType(sidName);
	TAny* ptr = NULL;
	TRAP_IGNORE( ptr = REComSession::CreateImplementationL(
			KAppSidCheckerInterfaceUidv2,
			_FOFF(CAppSidChecker, iDtor_ID_Key),
			NULL, resolverParams) );
	return reinterpret_cast<CAppSidChecker*>(ptr);
	}
#endif
#endif
/////
//Step to Set RemoveWithLastDependent property
/////

CSwisSetRemoveWithLastDependent::CSwisSetRemoveWithLastDependent()
    {
    }

CSwisSetRemoveWithLastDependent::~CSwisSetRemoveWithLastDependent()
    {
    }

TVerdict CSwisSetRemoveWithLastDependent::doTestStepL()
    {
    RSisRegistrySession registrySession;
    User::LeaveIfError(registrySession.Connect());
    CleanupClosePushL(registrySession);
    
    TInt packageUid = 0;
    GetHexFromConfig(ConfigSection(),_L("packageUid"),packageUid);    
    TUid expectedPkgUid = TUid::Uid(packageUid);
    RSisRegistryEntry regEntry;
    CleanupClosePushL(regEntry);
    User::LeaveIfError(regEntry.Open(registrySession, expectedPkgUid));
    TRAPD(err, regEntry.SetRemoveWithLastDependentL(expectedPkgUid));
    if(KErrNone == err)
        SetTestStepResult(EPass);
    else
        SetTestStepResult(EFail);

    CleanupStack::PopAndDestroy(2, &registrySession);
    return TestStepResult();
    
    }

// End of file
