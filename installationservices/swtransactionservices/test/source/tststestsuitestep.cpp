/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* tststestsuitestep.h
*
*/


/**
 @file
 @test
 @internalComponent
*/
#include "tststestsuitestep.h"
#include "tststestsuiteserver.h"
#include <s32file.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "stsrecovery.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS


_LIT(KEFileShareExclusive, 				"EFileShareExclusive");
_LIT(KEFileShareReadersOnly,			"EFileShareReadersOnly");
_LIT(KEFileShareAny,					"EFileShareAny");
_LIT(KEFileShareReadersOrWriters, 		"EFileShareReadersOrWriters");
_LIT(KEFileStream, 						"EFileStream");
_LIT(KEFileStreamText,					"EFileStreamText");
_LIT(KEFileRead,						"EFileRead");
_LIT(KEFileWrite,						"EFileWrite");
_LIT(KEFileReadAsyncAll,				"EFileReadAsyncAll");
_LIT(KEFileWriteBuffered,				"EFileWriteBuffered");
_LIT(KEFileWriteDirectIO,				"EFileWriteDirectIO");
_LIT(KEFileReadBuffered,				"EFileReadBuffered");
_LIT(KEFileReadDirectIO,				"EFileReadDirectIO");
_LIT(KEFileReadAheadOn,					"EFileReadAheadOn");
_LIT(KEFileReadAheadOff,				"EFileReadAheadOff");
_LIT(KORSign,							"|");


CStsBaseTestStep::CStsBaseTestStep(CStsTestServer &aStsTestServer): iStsTestServer(aStsTestServer)
	{
	}

CStsTestServer& CStsBaseTestStep::Server()
	{
	return iStsTestServer;
	}

TInt CStsBaseTestStep::GetIntFromConfigL(const TDesC& aName)
	{
	TInt ret(0);
	if(!GetIntFromConfig(ConfigSection(), aName, ret))
		{
		ERR_PRINTF2(_L("%S is not found in config!"), &aName);
		User::Leave(KErrNotFound);
		}
	return ret;
	}

TInt CStsBaseTestStep::GetTransactionSlotFromConfigL()
/**
 Reads the target file's path from the ini file
 */
	{
	return GetIntFromConfigL(KTransactionSlot);
	}

TInt CStsBaseTestStep::GetTransactionSlotToReuseTransactionIdFromConfigL()
	{
	return GetIntFromConfigL(KTransactionSlotID);
	}

TInt CStsBaseTestStep::GetTransactionIdFromConfigL()
	{
	return GetIntFromConfigL(KTransactionID);
	}

TPtrC CStsBaseTestStep::GetStringFromConfigL(const TDesC& aName)
	{
	TPtrC ret;
	if(!GetStringFromConfig(ConfigSection(), aName, ret))
		{
		ERR_PRINTF2(_L("%S is not found in config!"), &aName);
		User::Leave(KErrNotFound);
		}
	return ret;
	}

TPtrC CStsBaseTestStep::GetTargetFilePathFromConfigL()
	{
	return GetStringFromConfigL(KTargetFile);
	}

void CStsBaseTestStep::WriteToFileL(RFile &aFile, TDesC& aString)
	{
	RFileWriteStream stream;
	CleanupClosePushL(stream);
	stream.Attach(aFile);
	TRAPD(err,
		stream.WriteL(aString);
		stream.CommitL();
		); //catch the error so that it can be reported
	if(err!=KErrNone)
		{
		ERR_PRINTF2(_L("CStsBaseTestStep::WriteToFileL() Writing to file failed! Error code: %d"), err);
		User::Leave(err);
		}
	stream.Close();
	CleanupStack::PopAndDestroy(&stream);
	}

TBool CStsBaseTestStep::FileExistsL(const TDesC &aFileName)
	{
	INFO_PRINTF2(_L("CStsBaseTestStep::FileExistsL() %S"), &aFileName);
	TBool ret=EFalse;
	TRAPD(err, ret=Server().GetTestUtil().FileExistsL(aFileName)); //trap leaving fucntion to log error
	if(err!=KErrNone)
		{
		ERR_PRINTF3(_L("CStsBaseTestStep::FileExistsL() File existence check failed for %S Error code: %d"), &aFileName, err);
		User::Leave(err);
		}
	return ret;
	}

void CStsBaseTestStep::DeleteFileL(const TDesC &aFileName)	
	{
	TInt err=Server().GetTestUtil().Delete(aFileName); 
	if(err!=KErrNone)
		{
		ERR_PRINTF3(_L("DeleteFileL() Deletion failed for %S Error code: %d"), &aFileName, err);
		User::Leave(err);
		}
	}
void CStsBaseTestStep::CopyFileL(const TDesC& aSourceFile, const TDesC& aDestinationFile)
	{
	TInt err=Server().GetTestUtil().Copy(aSourceFile, aDestinationFile); 
	if(err!=KErrNone)
		{
		ERR_PRINTF4(_L("CStsBaseTestStep::CopyFileL() Copying %S to %S failed. Error code: %d"), &aSourceFile, &aDestinationFile, err);
		User::Leave(err);
		}
	}

void CStsBaseTestStep::RmDirL(const TDesC& aFileName)
	{
	TInt err=Server().GetTestUtil().RmDir(aFileName); 
	if(err!=KErrNone)
		{
		ERR_PRINTF3(_L("CStsBaseTestStep::RmDirL() Deleting dir %S failed. Error code: %d"), &aFileName, err);
		User::Leave(err);
		}
	}

void CStsBaseTestStep::MkDirAllL(const TDesC& aFileName)
	{
	TInt err=Server().GetTestUtil().MkDirAll(aFileName); 
	if(err!=KErrNone)
		{
		ERR_PRINTF3(_L("CStsBaseTestStep::MkDirAllL() Creating dir %S failed. Error code: %d"), &aFileName, err);
		User::Leave(err);
		}
	}

void CStsBaseTestStep::DeleteL(const TDesC& aFileName)
	{
	TInt err=Server().GetTestUtil().Delete(aFileName); 
	if(err!=KErrNone)
		{
		ERR_PRINTF3(_L("CStsBaseTestStep::DeleteL() Deleting %S failed. Error code: %d"), &aFileName, err);
		User::Leave(err);
		}
	}

void CStsBaseTestStep::SetReadOnlyL(const TDesC& aFileName, TInt aSetReadOnly)
	{
	TInt err=Server().GetTestUtil().SetReadOnly(aFileName, aSetReadOnly);
	if(err!=KErrNone)
		{
		ERR_PRINTF4(_L("CStsBaseTestStep::SetReadOnly() Trying to set \"readonly\" flag on %S failed to %d. Error code: %d"), &aFileName, aSetReadOnly, err);
		User::Leave(err);
		}
	}

HBufC* CStsBaseTestStep::CreateDataForNewFileCreatedByStsServerLC(const TDesC& aTestStepName, TNewFileType aType)
	{
	HBufC *buf = HBufC::NewLC(200);
	TPtr bufPtr = buf->Des();

	bufPtr.Append(KFileData1);
	switch(aType)
		{
		case ENewTemporaryFile:
			bufPtr.Append(KFileData2);
			break;
		case ENewPermanentFile:
			bufPtr.Append(KFileData3);
			break;
		default:
			bufPtr.Append(KFileData4);
			break;
		}
	bufPtr.Append(KFileData5);
	bufPtr.Append(KFileData7);
	bufPtr.Append(aTestStepName);
	bufPtr.Append(KFileData6);
	return buf;
	}

//----------------------------------------

CCreateTransactionStep::CCreateTransactionStep(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCreateTransactionStep);
	}

void CCreateTransactionStep::ImplTestStepPreambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CCreateTransactionStep Preamble"));
	SetTestStepResult(EPass);
	}

void CCreateTransactionStep::ImplTestStepL()
/**
 @return - void code
 Override of base class pure virtual
 */
	{
	INFO_PRINTF1(_L("CCreateTransactionStep in test step"));

	TInt trSlot = GetTransactionSlotFromConfigL();
	TInt trID = Server().CreateTransactionL(trSlot);
	SetTestStepResult(EPass);
	}

void CCreateTransactionStep::ImplTestStepPostambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CCreateTransactionStep Postamble"));
	}

//----------------------------------------

COpenTransactionStep::COpenTransactionStep(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KOpenTransactionStep);
	}

void COpenTransactionStep::ImplTestStepPreambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("COpenTransactionStep Preamble"));
	SetTestStepResult(EPass);
	
	}

void COpenTransactionStep::ImplTestStepL()
/**
 @return - void code
 Override of base class pure virtual
 */
	{
	INFO_PRINTF1(_L("COpenTransactionStep in test step"));

	
	TInt trSlot = GetTransactionSlotFromConfigL(); //get required transaction slot from ini
	TInt requiredSlotToTakeIdFrom(0);
	TStsTransactionId trID(0);
	TRAPD(err, requiredSlotToTakeIdFrom = GetTransactionSlotToReuseTransactionIdFromConfigL()); //get transaction id from ini to reuse its id
	if(err == KErrNone)
		{
		trID = Server().GetTransactionIDL(requiredSlotToTakeIdFrom); //get ID from server from the requested slot
		}
	else
		{
		if(err == KErrNotFound)
			{
			trID =GetTransactionIdFromConfigL();
			}
		else
			{
			User::Leave(err);
			}
		}
	//open transaction
	Server().OpenTransactionL(trSlot, trID);

	SetTestStepResult(EPass);
	
	}

void COpenTransactionStep::ImplTestStepPostambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("COpenTransactionStep Postamble"));
	
	}



//----------------------------------------
		
CCloseTransactionStep::CCloseTransactionStep(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCloseTransactionStep);
	}

void CCloseTransactionStep::ImplTestStepPreambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CCloseTransactionStep Preamble"));
	SetTestStepResult(EPass);
	
	}

void CCloseTransactionStep::ImplTestStepL()
/**
 @return - void code
 Override of base class pure virtual
 */
	{
	INFO_PRINTF1(_L("CCloseTransactionStep in test step"));
	
	//get transaction slot from ini
	TInt trSlot = GetTransactionSlotFromConfigL();

	//close transaction
	Server().CloseTransactionL(trSlot);

	SetTestStepResult(EPass);
	
	}

void CCloseTransactionStep::ImplTestStepPostambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CCloseTransactionStep Postamble"));
	
	}

//----------------------------------------

CRegisterNewFileStep::CRegisterNewFileStep(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KRegisterNewFileStep);
	}

void CRegisterNewFileStep::ImplTestStepPreambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CRegisterNewFileStep Preamble"));
	SetTestStepResult(EPass);
	
	}

void CRegisterNewFileStep::ImplTestStepL()
/**
 @return - void code
 Override of base class pure virtual
 */
	{
	INFO_PRINTF1(_L("CRegisterNewFileStep in test step"));
	
	//get transaction slot from ini
	TInt trSlot = GetTransactionSlotFromConfigL();

	//get the target file path from ini
	TPtrC filePathInIni = GetTargetFilePathFromConfigL();

	Server().RegisterNewL(trSlot, filePathInIni);

	SetTestStepResult(EPass);
	
	}

void CRegisterNewFileStep::ImplTestStepPostambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CRegisterNewFileStep Postamble"));
	
	}


//----------------------------------------

CCreateNewFileStep::CCreateNewFileStep(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCreateNewFileStep);
	}

void CCreateNewFileStep::ImplTestStepPreambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CCreateNewFileStep Preamble"));
	SetTestStepResult(EPass);
	
	}

void CCreateNewFileStep::ImplTestStepL()
/**
 @return - void code
 Override of base class pure virtual
 */
	{
	INFO_PRINTF1(_L("CCreateNewFileStep in test step"));
	
	TFileMode fileMode(static_cast<TFileMode>(EFileShareExclusive|EFileWrite));
	
	//get transaction slot from ini
	TInt trSlot = GetTransactionSlotFromConfigL();

	//get the target file path from ini
	TPtrC filePathInIni = GetTargetFilePathFromConfigL();
	
	//the handle to the newly created file
	RFile newFile;
	CleanupClosePushL(newFile);
	//register newly created file
	Server().CreateNewL(trSlot, filePathInIni, newFile, fileMode);

	HBufC *bufPtr=CreateDataForNewFileCreatedByStsServerLC(KCreateNewFileStep, ENewPermanentFile);
	WriteToFileL(newFile, *bufPtr);
	
	CleanupStack::PopAndDestroy(2, &newFile);

	SetTestStepResult(EPass);
	
	}

void CCreateNewFileStep::ImplTestStepPostambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CCreateNewFileStep Postamble"));
	
	}

//----------------------------------------

CRemoveStep::CRemoveStep(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KRemoveStep);
	}

void CRemoveStep::ImplTestStepPreambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CRemoveStep Preamble"));
	SetTestStepResult(EPass);
	
	}

void CRemoveStep::ImplTestStepL()
/**
 @return - void code
 Override of base class pure virtual
 */
	{
	INFO_PRINTF1(_L("CRemoveStep in test step"));
	
	//get transaction slot from ini
	TInt trSlot = GetTransactionSlotFromConfigL();

	//get the target file path from ini
	TPtrC filePathInIni = GetTargetFilePathFromConfigL();

	Server().RemoveL(trSlot, filePathInIni);

	SetTestStepResult(EPass);
	
	}

void CRemoveStep::ImplTestStepPostambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CRemoveStep Postamble"));
	
	}

//----------------------------------------
	
CRegisterTemporaryStep::CRegisterTemporaryStep(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KRegisterTemporaryStep);
	}

void CRegisterTemporaryStep::ImplTestStepPreambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CRegisterTemporaryStep Preamble"));
	SetTestStepResult(EPass);
	
	}

void CRegisterTemporaryStep::ImplTestStepL()
/**
 @return - void code
 Override of base class pure virtual
 */
	{
	INFO_PRINTF1(_L("CRegisterTemporaryStep in test step"));
	
	//get transaction slot from ini
	TInt trSlot = GetTransactionSlotFromConfigL();

	//get the target file path from ini
	TPtrC filePathInIni = GetTargetFilePathFromConfigL();

	Server().RegisterTemporaryL(trSlot, filePathInIni);

	SetTestStepResult(EPass);
	
	}

void CRegisterTemporaryStep::ImplTestStepPostambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CRegisterTemporaryStep Postamble"));
	
	}

//----------------------------------------
	
CCreateTemporaryStep::CCreateTemporaryStep(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCreateTemporaryStep);
	}

void CCreateTemporaryStep::ImplTestStepPreambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CCreateTemporaryStep Preamble"));
	SetTestStepResult(EPass);
	
	}

void CCreateTemporaryStep::ImplTestStepL()
/**
 @return - void code
 Override of base class pure virtual
 */
	{
	INFO_PRINTF1(_L("CCreateTemporaryStep in test step"));
	
	TFileMode fileMode(static_cast<TFileMode>(EFileShareExclusive|EFileWrite));
	
	//get transaction slot from ini
	TInt trSlot = GetTransactionSlotFromConfigL();

	//get the target file path from ini
	TPtrC filePathInIni = GetTargetFilePathFromConfigL();
	
	//the handle to the newly created file
	RFile newTempFile;
	CleanupClosePushL(newTempFile);
	//register newly created file
	Server().CreateTemporaryL(trSlot, filePathInIni, newTempFile, fileMode);

	HBufC* bufPtr=CreateDataForNewFileCreatedByStsServerLC(KCreateTemporaryStep, ENewTemporaryFile);
	WriteToFileL(newTempFile, *bufPtr);

	
	CleanupStack::PopAndDestroy(2, &newTempFile);

	SetTestStepResult(EPass);
	
	}

void CCreateTemporaryStep::ImplTestStepPostambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CCreateTemporaryStep Postamble"));
	
	}

//----------------------------------------
		
COverwriteStep::COverwriteStep(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KOverwriteStep);
	}

void COverwriteStep::ImplTestStepPreambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("COverwriteStep Preamble"));
	SetTestStepResult(EPass);
	
	}

void COverwriteStep::ImplTestStepL()
/**
 @return - void code
 Override of base class pure virtual
 */
	{
	INFO_PRINTF1(_L("COverwriteStep in test step"));
	
	TFileMode fileMode(static_cast<TFileMode>(EFileShareExclusive|EFileWrite));
	
	//get transaction slot from ini
	TInt trSlot = GetTransactionSlotFromConfigL();

	//get the target file path from ini
	TPtrC filePathInIni = GetTargetFilePathFromConfigL();
	
	//the handle to the newly created file
	RFile newOverwriteFile;
	CleanupClosePushL(newOverwriteFile);

	//register newly created file
	Server().OverwriteL(trSlot, filePathInIni, newOverwriteFile, fileMode);

	HBufC *bufPtr=CreateDataForNewFileCreatedByStsServerLC(KOverwriteStep, ENewPermanentFile);
	WriteToFileL(newOverwriteFile, *bufPtr);
	
	CleanupStack::PopAndDestroy(2, &newOverwriteFile);

	SetTestStepResult(EPass);
	
	}

void COverwriteStep::ImplTestStepPostambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("COverwriteStep Postamble"));
	
	}

//----------------------------------------
		
CCommitStep::CCommitStep(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCommitStep);
	}

void CCommitStep::ImplTestStepPreambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CCommitStep Preamble"));
	SetTestStepResult(EPass);
	
	}

void CCommitStep::ImplTestStepL()
/**
 @return - void code
 Override of base class pure virtual
 */
	{
	INFO_PRINTF1(_L("CCommitStep in test step"));
	
	//get transaction slot from ini
	TInt trSlot = GetTransactionSlotFromConfigL();

	//commit transaction
	Server().CommitL(trSlot);

	SetTestStepResult(EPass);
	
	}

void CCommitStep::ImplTestStepPostambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CCommitStep Postamble"));
	
	}

//----------------------------------------
		
CRollbackStep::CRollbackStep(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KRollbackStep);
	}

void CRollbackStep::ImplTestStepPreambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CRollbackStep Preamble"));
	SetTestStepResult(EPass);
	
	}

void CRollbackStep::ImplTestStepL()
/**
 @return - void code
 Override of base class pure virtual
 */
	{
	INFO_PRINTF1(_L("CRollbackStep in test step"));
	
	//get transaction slot from ini
	TInt trSlot = GetTransactionSlotFromConfigL();

	//roolback transaction
	Server().RollBackL(trSlot);
	
	SetTestStepResult(EPass);
	
	}

void CRollbackStep::ImplTestStepPostambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CRollbackStep Postamble"));
	
	}

//----------------------------------------
		
CRollbackAllPendingStep::CRollbackAllPendingStep(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KRollbackAllPendingStep);
	}

void CRollbackAllPendingStep::ImplTestStepPreambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CRollbackAllPendingStep Preamble"));
	SetTestStepResult(EPass);
	
	}

void CRollbackAllPendingStep::ImplTestStepL()
/**
 @return - void code
 Override of base class pure virtual
 */
	{
	INFO_PRINTF1(_L("CRollbackAllPendingStep in test step"));
	
	RStsRecoverySession session;
	session.RollbackAllPendingL();
	session.Close();
	SetTestStepResult(EPass);
	
	}

void CRollbackAllPendingStep::ImplTestStepPostambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CRollbackAllPendingStep Postamble"));
	
	}

//----------------------------------------
			
CCheckFilesStep::CCheckFilesStep(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCheckFilesStep);
	}

void CCheckFilesStep::ImplTestStepPreambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CCheckFilesStep Preamble"));
	SetTestStepResult(EPass);
	
	}

void CCheckFilesStep::ImplTestStepL()
/**
 @return - void code
 Override of base class pure virtual
 */
	{
	INFO_PRINTF1(_L("CCheckFilesStep in test step"));
	SetTestStepResult(EPass);
	
	RArray<TPtrC> fileNumExist;
	RArray<TPtrC> fileNumNonExist;
	CleanupClosePushL(fileNumExist);
	CleanupClosePushL(fileNumNonExist);
	GetFileNamesForCheckL(fileNumExist, fileNumNonExist);
	CheckIfFilesExistL(fileNumExist);
	CheckIfFilesNotExistL(fileNumNonExist);
	CleanupStack::PopAndDestroy(2, &fileNumExist);
	}

void CCheckFilesStep::ImplTestStepPostambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CCheckFilesStep Postamble"));
	
	}


void CCheckFilesStep::GetFileNamesForCheckL(RArray<TPtrC>& aFileNumExist,RArray<TPtrC>& aFileNumNonExist)
	{
	TInt entriesNumExist=0;
	TInt entriesNumNonExist=0;
	
	GetIntFromConfig(ConfigSection(), KNumExist, entriesNumExist);
	GetIntFromConfig(ConfigSection(), KNumNonExist, entriesNumNonExist);
	
	ExtractFileNameL(entriesNumExist, KExistBase, aFileNumExist);
	ExtractFileNameL(entriesNumNonExist, KNonExistBase, aFileNumNonExist);
	}
		
void CCheckFilesStep::ExtractFileNameL(TInt aEntries, const TDesC& aEntryBase, RArray<TPtrC>& aFileArray)
	{
	TPtrC fname;
	const TInt KKeyBufSize =64;
	
	for(TInt i=0; i<aEntries; i++)
		{
		//construct name of the key
		TBuf<KKeyBufSize> keyBuf(aEntryBase);
		keyBuf.AppendNum(i);
		
		if(!GetStringFromConfig(ConfigSection(),keyBuf,fname))
			{
			ERR_PRINTF2(_L("%S is not found in config!"), &keyBuf);
			User::Leave(KErrNotFound);
			}
		aFileArray.Insert(fname, i);
		}
	}

void CCheckFilesStep::CheckIfFilesExistL(const RArray<TPtrC>& aFileArray)
	{
	TInt nErr =0;
	TInt fileCount=aFileArray.Count();
	for(TInt i =0; i< fileCount; i++)
		{
		if(!FileExistsL(aFileArray[i]))
			{
			ERR_PRINTF2(_L("File missing: %S"), &aFileArray[i]);
			nErr++;
			}
		}
	if(nErr)
		{
		SetTestStepResult(EFail);
		}
	}

void CCheckFilesStep::CheckIfFilesNotExistL(const RArray<TPtrC>& aFileArray)
	{
	TInt nErr =0;
	TInt fileCount=aFileArray.Count();
	
	for(TInt i =0; i< fileCount; i++)
		{
		if(FileExistsL(aFileArray[i]))
			{
			ERR_PRINTF2(_L("File exists (but shouldn't): %S"), &aFileArray[i]);
			nErr++;
			}
		}
	if(nErr)
		{
		SetTestStepResult(EFail);
		}
	}
	

//----------------------------------------
			
CheckFileModeChangeStep::CheckFileModeChangeStep(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCheckFileModeChangeStep);
	}

void CheckFileModeChangeStep::ImplTestStepPreambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CheckFileModeChangeStep Preamble"));
	SetTestStepResult(EPass);
	
	}

void CheckFileModeChangeStep::ImplTestStepL()
/**
 @return - void code
 Override of base class pure virtual
 */
	{
	INFO_PRINTF1(_L("CheckFileModeChangeStep in test step"));
	SetTestStepResult(EPass);
	
	TFileMode fileMode(static_cast<TFileMode>(EFileShareExclusive|EFileWrite));
	
	//get transaction slot from ini
	TInt trSlot = GetTransactionSlotFromConfigL();

	//get the target file path from ini
	TPtrC filePathInIni = GetTargetFilePathFromConfigL();
	
	//the handle to the newly created file
	RFile newFile;
	CleanupClosePushL(newFile);
	//register newly created file
	Server().CreateNewL(trSlot, filePathInIni, newFile, fileMode);


	TFileMode fileModes[] = {
			EFileShareReadersOnly,
			EFileShareAny,
			EFileShareReadersOrWriters,
			EFileShareExclusive
			};

	const TDesC* fileModeStrings[] = {
				&KEFileShareReadersOnly,
				&KEFileShareAny,
				&KEFileShareReadersOrWriters,
				&KEFileShareExclusive
				};
	
	TFileMode fileSubModes[] = {
			EFileStream,
			EFileStreamText,
			EFileRead,
			EFileWrite,
			EFileReadAsyncAll,
			EFileWriteBuffered,
			EFileWriteDirectIO,
			EFileReadBuffered,
			EFileReadDirectIO,
			EFileReadAheadOn,
			EFileReadAheadOff
			};

	const TDesC* fileSubModeStrings[] = {
			&KEFileStream,
			&KEFileStreamText,
			&KEFileRead,
			&KEFileWrite,
			&KEFileReadAsyncAll,
			&KEFileWriteBuffered,
			&KEFileWriteDirectIO,
			&KEFileReadBuffered,
			&KEFileReadDirectIO,
			&KEFileReadAheadOn,
			&KEFileReadAheadOff
			};

	
	INFO_PRINTF4(_L("CheckFileModeChangeStep::ImplTestStepL() returned file handle mode is: %S%S%S"), &KEFileShareExclusive, &KORSign, &KEFileWrite);

	for(TInt mode=0; mode<sizeof(fileModes)/sizeof(TFileMode); ++mode)
		{
		for(TInt submode=0; submode<sizeof(fileSubModes)/sizeof(TFileMode); ++submode)
			{
			TInt err=newFile.ChangeMode(static_cast<TFileMode>(fileModes[mode]|fileSubModes[submode]));
			if(err==KErrNone)
				{
				INFO_PRINTF4(_L("<font color=green>CheckFileModeChangeStep::ImplTestStepL() setting file mode to %S%S%S was successful</font>"),fileModeStrings[mode], &KORSign, fileSubModeStrings[submode]);
				}
			else if(err==KErrArgument)
				{
				ERR_PRINTF4(_L("<font color=red>CheckFileModeChangeStep::ImplTestStepL() setting file mode to %S%S%S was unsuccessful. KErrArgument is returned!</font>"),fileModeStrings[mode], &KORSign, fileSubModeStrings[submode]);
				}
			else if(err==KErrAccessDenied)
				{
				ERR_PRINTF4(_L("<font color=red>CheckFileModeChangeStep::ImplTestStepL() setting file mode to %S%S%S was unsuccessful. KErrAccessDenied is returned!</font>"),fileModeStrings[mode], &KORSign, fileSubModeStrings[submode]);
				}
			else
				{
				ERR_PRINTF5(_L("<font color=red>CheckFileModeChangeStep::ImplTestStepL() setting file mode to %S%S%S was unsuccessful. %d is returned!</font>"),fileModeStrings[mode], &KORSign, fileSubModeStrings[submode], err);
				}
			}
		
		}
	
	CleanupStack::PopAndDestroy(&newFile);	
	}

void CheckFileModeChangeStep::ImplTestStepPostambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CheckFileModeChangeStep Postamble"));
	
	}

//----------------------------------------
			
CFileOperationsStep::CFileOperationsStep(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCheckFilesStep);
	}

void CFileOperationsStep::ImplTestStepPreambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CFileOperationsStep Preamble"));
	SetTestStepResult(EPass);
	
	}

void CFileOperationsStep::ImplTestStepL()
/**
 @return - void code
 Override of base class pure virtual
 */
	{
	INFO_PRINTF1(_L("CFileOperationsStep in test step"));
	ProcessFileOperationsL();
	SetTestStepResult(EPass);
	
	}

void CFileOperationsStep::ImplTestStepPostambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CFileOperationsStep Postamble"));
	
	}
		
void CFileOperationsStep::ProcessFileOperationsL()
	{
	TInt numOfOps=GetIntFromConfigL(KNumOfOperations);
	const TInt buffsize(30);
	
	for(TInt i=0; i<numOfOps; i++)
		{
		//construct name of the action (ie: action0, action1, ...)
		TBuf<buffsize> opBuf(KOpNameBase);
		opBuf.AppendNum(i);
		TBool ignoreErrors=EFalse;
		TPtrC operationLinePtr=GetStringFromConfigL(opBuf);
		TInt nextParamStart(0);
		TInt err=KErrNone;
		
		TPtrC command(GetNextCommandLineParameterL(operationLinePtr, nextParamStart));
		TInt tempNextParamStart=nextParamStart;
		TPtrC ignoreErrorsSwitch(GetNextCommandLineParameterL(operationLinePtr, nextParamStart));
		if(ignoreErrorsSwitch == KIgnoreErrorsSwitch)
			{
			//"-i" switch is present
			ignoreErrors=ETrue;
			}
		else
			{
			//no "-i" switch go back to original position
			nextParamStart=tempNextParamStart;
			}
		if(command == KMkDirAllOp) //mkdirall
			{
			TPtrC target(GetNextCommandLineParameterL(operationLinePtr, nextParamStart));
			TRAP(err, MkDirAllL(target));
			}
		else if(command == KRmDirOp) //rmdir
			{
			TPtrC target(GetNextCommandLineParameterL(operationLinePtr, nextParamStart));
			TRAP(err, RmDirL(target));
			}
		else if(command == KCopyOp) //copy
			{
			TPtrC source(GetNextCommandLineParameterL(operationLinePtr, nextParamStart));
			TPtrC dest(GetNextCommandLineParameterL(operationLinePtr, nextParamStart));
			TRAP(err, CopyFileL(source, dest));
			}
		else if(command == KDeleteOp) //delete
			{
			TPtrC target(GetNextCommandLineParameterL(operationLinePtr, nextParamStart));
			TRAP(err, DeleteL(target));
			}
		else if(command == KAttribOp) //attrib
			{
			enum TAttribOperation{
			Unset=0x00,
			SetReadOnly=0x01}; //later more operations can be handled

			TPtrC attribSwitch(GetNextCommandLineParameterL(operationLinePtr, nextParamStart));
			TInt operation=SetReadOnly*(attribSwitch[0] == TChar('r') || attribSwitch[0] == TChar('R'));
			TInt flag=0;
			if(attribSwitch[1] == TChar('+'))
				flag=1;
			else if(attribSwitch[1] == TChar('-'))
				flag=-1;
			if(attribSwitch.Length() == 2 && operation && flag)
				{
				TPtrC target(GetNextCommandLineParameterL(operationLinePtr, nextParamStart));
				switch(operation) //later more operations can be handled
					{
					case SetReadOnly:
						TRAP(err, SetReadOnlyL(target, (flag>0? 1 : 0)));
						break;
					default:
						break;
					}
				}
			else
				{
				ERR_PRINTF2(_L("<font color=red>CFileOperationsStep::ProcessFileOperationsL() Unknown command or corrupt command line %S </font>"), &operationLinePtr);
				User::Leave(KErrArgument);
				}
			}
		else //unknown command or corrupt command line (ie: missing command)
			{
			ERR_PRINTF2(_L("<font color=red>CFileOperationsStep::ProcessFileOperationsL() Unknown command or corrupt command line %S </font>"), &operationLinePtr);
			User::Leave(KErrArgument);
			}
		
		if(!ignoreErrors)
			{
			User::LeaveIfError(err);
			}
		}
	}

TPtrC CFileOperationsStep::GetNextCommandLineParameterL(const TDesC& aLine, TInt& aNextAvailablePos)
	{
	TInt lineLength=aLine.Length();
	TInt startpos=0, stoppos=0;
	TUint spaceChar=' ', tabChar='\t';
	
	if(lineLength==0 || aNextAvailablePos>=lineLength) //empty command line or next position is beyond the end of line
		{
		User::Leave(KErrArgument);
		}

	for(startpos=aNextAvailablePos; startpos<lineLength; ++startpos)
		{
		if(aLine[startpos]!=spaceChar && aLine[startpos]!=tabChar) //skip initial spaces
			break;
		}
	
	if(startpos==lineLength) //line contains only spaces
		{
		User::Leave(KErrArgument);
		}
	
	for(stoppos=startpos; stoppos<lineLength; ++stoppos)
		{
		if(aLine[stoppos]==spaceChar || aLine[startpos]==tabChar) //skip all non space characters (ie: find the end of string)
			break;
		}
	aNextAvailablePos=stoppos;
	return TPtrC(aLine.Mid(startpos, stoppos-startpos));
	
	}

//----------------------------------------
			
CFileModeTestStep::CFileModeTestStep(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer)
	{
	SetTestStepName(KFileModeTestStep);
	}

void CFileModeTestStep::ImplTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CFileModeTestStep Preamble"));
	SetTestStepResult(EPass);
	
	}

void CFileModeTestStep::ImplTestStepL()
	{
	INFO_PRINTF1(_L("CFileModeTestStep in test step"));
	TestCase1L();
	TestCase2L();
	TestCase3L();
	}

void CFileModeTestStep::TestCase1L()
	{
	INFO_PRINTF1(_L("CFileModeTestStep test case 1"));
	
/*
 This tescase tests that the file -that's created by the server and whose handle is returned to the client- is writable by the client 
 */	
	TFileMode fileModes[] = {
			EFileShareAny,
			EFileShareReadersOrWriters,
			EFileShareExclusive
			};

	const TDesC* fileModeStrings[] = {
				&KEFileShareAny,
				&KEFileShareReadersOrWriters,
				&KEFileShareExclusive
				};
	
	TFileMode fileSubModes[] = {
			EFileStream,
			EFileStreamText,
			EFileWrite,
			EFileWriteBuffered,
			EFileWriteDirectIO,
			};

	const TDesC* fileSubModeStrings[] = {
			&KEFileStream,
			&KEFileStreamText,
			&KEFileWrite,
			&KEFileWriteBuffered,
			&KEFileWriteDirectIO,
			};

	//get transaction slot from ini
	TInt trSlot = GetTransactionSlotFromConfigL();

	//get the target file path from ini
	TPtrC filePathInIni = GetTargetFilePathFromConfigL();
	TParsePtrC parse(filePathInIni);
	
	//the handle to the newly created file
	RFile newFile;
	//register newly created file
	HBufC *bufPtr1=CreateDataForNewFileCreatedByStsServerLC(KCreateNewFileStep, ENewPermanentFile);
	HBufC *bufPtr2=CreateDataForNewFileCreatedByStsServerLC(KCreateTemporaryStep, ENewTemporaryFile);
	HBufC *bufPtr3=CreateDataForNewFileCreatedByStsServerLC(KOverwriteStep, ENewPermanentFile);
	
	HBufC* targetFileName = HBufC::NewLC(KMaxFileName);
	TPtr fileNameBasePtr(targetFileName->Des());
	fileNameBasePtr.Append(parse.DriveAndPath());
	fileNameBasePtr.Append(parse.Name());
	TPtr fileNameFullPtr(fileNameBasePtr);
	
	TInt counter(0);
	for(TInt mode=0; mode<sizeof(fileModes)/sizeof(TFileMode); ++mode)
		{
		for(TInt submode=0; submode<sizeof(fileSubModes)/sizeof(TFileMode); ++submode, ++counter)
			{
			TInt err=KErrNone;
			CleanupClosePushL(newFile);

			TFileMode fileMode(static_cast<TFileMode>(fileModes[mode]|fileSubModes[submode]));
			
			//test RStsSession::CreateNewL()
			fileNameFullPtr.Set(fileNameBasePtr);
			fileNameFullPtr.Append(_L("_createnew"));
			fileNameFullPtr.AppendNum(counter);
			fileNameFullPtr.Append(parse.Ext());
			TRAP(err,
					Server().CreateNewL(trSlot, fileNameFullPtr, newFile, fileMode);
					WriteToFileL(newFile, *bufPtr1);
					);
			newFile.Close();
			if(err!=KErrNone)
				{
				ERR_PRINTF4(_L("The file -created by RStsSession::CreateNewL()- cannot be written into when the creation mode is set to  %S %S %S"), &fileModeStrings[mode], &KORSign, &fileSubModeStrings[submode]);
				}
	
			//test RStsSession::CreateTemporaryL()
			fileNameFullPtr.Set(fileNameBasePtr);
			fileNameFullPtr.Append(_L("_createtemp"));
			fileNameFullPtr.AppendNum(counter);
			fileNameFullPtr.Append(parse.Ext());
			TRAP(err,
					Server().CreateTemporaryL(trSlot, fileNameFullPtr, newFile, fileMode);
					WriteToFileL(newFile, *bufPtr2);
					);
			newFile.Close();
			if(err!=KErrNone)
				{
				ERR_PRINTF4(_L("The file -created by RStsSession::CreateTemporaryL()- cannot be written into when the creation mode is set to  %S %S %S"), &fileModeStrings[mode], &KORSign, &fileSubModeStrings[submode]);
				}
			
			//test RStsSession::OverwriteL()
			fileNameFullPtr.Set(fileNameBasePtr);
			fileNameFullPtr.Append(_L("_overwrite"));
			fileNameFullPtr.AppendNum(counter);
			fileNameFullPtr.Append(parse.Ext());
			TRAP(err,
					Server().OverwriteL(trSlot, fileNameFullPtr, newFile, fileMode);
					WriteToFileL(newFile, *bufPtr3);
					);
			newFile.Close();
			if(err!=KErrNone)
				{
				ERR_PRINTF4(_L("The file -created by RStsSession::OverwriteL()- cannot be written into when the creation mode is set to  %S %S %S"), &fileModeStrings[mode], &KORSign, &fileSubModeStrings[submode]);
				}

			CleanupStack::PopAndDestroy(); //newFile
			}//inner for
		}//outer for
	CleanupStack::PopAndDestroy(4, bufPtr1); //bufPtr
	}

void CFileModeTestStep::TestCase2L()
	{
	INFO_PRINTF1(_L("CFileModeTestStep test case 2"));
/*
 According to the TFileMode documentation a file cannot be opened for writing if using a share mode flag of EFileShareReadersOnly
 */
	//get transaction slot from ini
	TInt trSlot = GetTransactionSlotFromConfigL();

	//get the target file path from ini
	TPtrC filePathInIni = GetTargetFilePathFromConfigL();
	
	
	TFileMode fileMode(static_cast<TFileMode>(EFileShareReadersOnly|EFileWrite));
	RFile newFile;
	TInt err;

	TRAP(err, Server().CreateNewL(trSlot, filePathInIni, newFile, fileMode));
	newFile.Close();
	if(err!=KErrArgument )
		{
		ERR_PRINTF1(_L("RStsSession::CreateNewL() returned KErrNone when an error condition was set (ie: newFileMode = EFileShareReadersOnly|EFileWrite) and KErrArgument was expected"));
		SetTestStepResult(EFail);
		}
	TRAP(err, Server().CreateTemporaryL(trSlot, filePathInIni, newFile, fileMode));
	newFile.Close();
	if(err!=KErrArgument )
		{
		ERR_PRINTF1(_L("RStsSession::CreateTemporaryL() returned KErrNone when an error condition was set (ie: newFileMode = EFileShareReadersOnly|EFileWrite) and KErrArgument was expected"));
		SetTestStepResult(EFail);
		}
	TRAP(err, Server().OverwriteL(trSlot, filePathInIni, newFile, fileMode));
	newFile.Close();
	if(err!=KErrArgument )
		{
		ERR_PRINTF1(_L("RStsSession::OverwriteL() returned KErrNone when an error condition was set (ie: newFileMode = EFileShareReadersOnly|EFileWrite) and KErrArgument was expected"));
		SetTestStepResult(EFail);
		}
	}

void CFileModeTestStep::TestCase3L()
	{
	INFO_PRINTF1(_L("CFileModeTestStep test case 3"));
	
/*
 This tescase tests that the file -that's created by the server and whose handle is returned to the client- is writable by others (obviously depending on the share mode) 
 */	
	TFileMode fileModes[] = {
			EFileShareAny,
			EFileShareReadersOrWriters,
			EFileShareExclusive
			};

	const TDesC* fileModeStrings[] = {
				&KEFileShareAny,
				&KEFileShareReadersOrWriters,
				&KEFileShareExclusive
				};
	
	TFileMode fileSubModes[] = {
			EFileStream,
			EFileStreamText,
			EFileWrite,
			EFileWriteBuffered,
			EFileWriteDirectIO,
			};

	const TDesC* fileSubModeStrings[] = {
			&KEFileStream,
			&KEFileStreamText,
			&KEFileWrite,
			&KEFileWriteBuffered,
			&KEFileWriteDirectIO,
			};

	//get transaction slot from ini
	TInt trSlot = GetTransactionSlotFromConfigL();

	//get the target file path from ini
	TPtrC filePathInIni = GetTargetFilePathFromConfigL();
	TParsePtrC parse(filePathInIni);
	
	//register newly created file
	HBufC *bufPtr1=CreateDataForNewFileCreatedByStsServerLC(KCreateNewFileStep, ENewPermanentFile);
	HBufC *bufPtr2=CreateDataForNewFileCreatedByStsServerLC(KCreateTemporaryStep, ENewTemporaryFile);
	HBufC *bufPtr3=CreateDataForNewFileCreatedByStsServerLC(KOverwriteStep, ENewPermanentFile);
	
	HBufC* targetFileName = HBufC::NewLC(KMaxFileName);
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	TPtr fileNameBasePtr(targetFileName->Des());
	fileNameBasePtr.Append(parse.DriveAndPath());
	fileNameBasePtr.Append(parse.Name());
	TPtr fileNameFullPtr(fileNameBasePtr);
	
	TInt counter(100);
	for(TInt mode=0; mode<sizeof(fileModes)/sizeof(TFileMode); ++mode)
		{
		for(TInt submode=0; submode<sizeof(fileSubModes)/sizeof(TFileMode); ++submode, ++counter)
			{
			//the handle to the newly created file
			RFile newFile;
			CleanupClosePushL(newFile);
			//handle repsresting some other process/application/etc.. who wants to have access to the file
			RFile otherFileClient;
			CleanupClosePushL(otherFileClient);

			TFileMode fileMode(static_cast<TFileMode>(fileModes[mode]|fileSubModes[submode]));
			TInt otherFileClientError=KErrNone;
			
			//create new file interface
			fileNameFullPtr.Set(fileNameBasePtr);
			fileNameFullPtr.Append(_L("_createnew"));
			fileNameFullPtr.AppendNum(counter);
			fileNameFullPtr.Append(parse.Ext());
			Server().CreateNewL(trSlot, fileNameFullPtr, newFile, fileMode); //create new file
			otherFileClientError=otherFileClient.Open(fs, fileNameFullPtr, EFileShareAny|EFileWrite); //test if others can have acces to the file
			if(otherFileClientError==KErrNone)
				TRAP(otherFileClientError, WriteToFileL(otherFileClient, *bufPtr1));
			if(fileModes[mode]!=EFileShareExclusive && otherFileClientError!=KErrNone)
				{
				ERR_PRINTF3(_L("File %S created by RStsSession::CreateNewL() cannot be written by other clients using EFileShareAny|EFileWrite filemode. Error code: %d"), &fileNameFullPtr, otherFileClientError);
				SetTestStepResult(EFail);//original share mode is other than EFileShareExclusive so there should be no error reported when writing to the file by others
				}
			else if(fileModes[mode]==EFileShareExclusive && otherFileClientError==KErrNone)
				{
				ERR_PRINTF2(_L("File %S -created by RStsSession::CreateNewL() using EFileShareExclusive share mode- can be written by other clients using EFileShareAny|EFileWrite filemode."), &fileNameFullPtr);
				SetTestStepResult(EFail);//original share mode is EFileShareExclusive so others should not be allowed to have access to it
				}
			WriteToFileL(newFile, *bufPtr1);
			newFile.Close();
			otherFileClient.Close();
			

			//create new temp file interface
			fileNameFullPtr.Set(fileNameBasePtr);
			fileNameFullPtr.Append(_L("_createtemp"));
			fileNameFullPtr.AppendNum(counter);
			fileNameFullPtr.Append(parse.Ext());
			Server().CreateTemporaryL(trSlot, fileNameFullPtr, newFile, fileMode);
			otherFileClientError=otherFileClient.Open(fs, fileNameFullPtr, EFileShareAny|EFileWrite); //test if others can have acces to the file
			if(otherFileClientError==KErrNone)
				TRAP(otherFileClientError, WriteToFileL(otherFileClient, *bufPtr1));
			if(fileModes[mode]!=EFileShareExclusive && otherFileClientError!=KErrNone)
				{
				ERR_PRINTF3(_L("File %S created by RStsSession::CreateTemporaryL() cannot be written by other clients using EFileShareAny|EFileWrite filemode. Error code: %d"), &fileNameFullPtr, otherFileClientError);
				SetTestStepResult(EFail);//original share mode is other than EFileShareExclusive so there should be no error reported when writing to the file by others
				}
			else if(fileModes[mode]==EFileShareExclusive && otherFileClientError==KErrNone)
				{
				ERR_PRINTF2(_L("File %S -created by RStsSession::CreateTemporaryL() using EFileShareExclusive share mode- can be written by other clients using EFileShareAny|EFileWrite filemode."), &fileNameFullPtr);
				SetTestStepResult(EFail);//original share mode is EFileShareExclusive so others should not be allowed to have access to it
				}
			WriteToFileL(newFile, *bufPtr2);
			newFile.Close();
			otherFileClient.Close();
			
			//overwrite file interface
			fileNameFullPtr.Set(fileNameBasePtr);
			fileNameFullPtr.Append(_L("_overwrite"));
			fileNameFullPtr.AppendNum(counter);
			fileNameFullPtr.Append(parse.Ext());
			Server().OverwriteL(trSlot, fileNameFullPtr, newFile, fileMode);
			otherFileClientError=otherFileClient.Open(fs, fileNameFullPtr, EFileShareAny|EFileWrite); //test if others can have acces to the file
			if(otherFileClientError==KErrNone)
				TRAP(otherFileClientError, WriteToFileL(otherFileClient, *bufPtr1));
			if(fileModes[mode]!=EFileShareExclusive && otherFileClientError!=KErrNone)
				{
				ERR_PRINTF3(_L("File %S created by RStsSession::OverwriteL() cannot be written by other clients using EFileShareAny|EFileWrite filemode. Error code: %d"), &fileNameFullPtr, otherFileClientError);
				SetTestStepResult(EFail);//original share mode is other than EFileShareExclusive so there should be no error reported when writing to the file by others
				}
			else if(fileModes[mode]==EFileShareExclusive && otherFileClientError==KErrNone)
				{
				ERR_PRINTF2(_L("File %S -created by RStsSession::OverwriteL() using EFileShareExclusive share mode- can be written by other clients using EFileShareAny|EFileWrite filemode."), &fileNameFullPtr);
				SetTestStepResult(EFail);//original share mode is EFileShareExclusive so others should not be allowed to have access to it
				}
			WriteToFileL(newFile, *bufPtr3);
			newFile.Close();
			otherFileClient.Close();

			CleanupStack::PopAndDestroy(2, &newFile); //newFile, otherFileClient
			}//inner for
		}//outer for
	CleanupStack::PopAndDestroy(5, bufPtr1); //bufPtr1, bufPtr2, bufPtr3, targetFileName, fs
	}

void CFileModeTestStep::ImplTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CFileModeTestStep Postamble"));
	
	}
//----------------------------------------

//----------------------------------------

CCreateLongFileNameTestStep::CCreateLongFileNameTestStep(CStsTestServer &aStsTestServer): CStsBaseTestStep(aStsTestServer)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCreateLongFileNameTestStep);
	}

void CCreateLongFileNameTestStep::ImplTestStepPreambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CCreateLongFileNameTestStep Preamble"));
	SetTestStepResult(EPass);
	}

void CCreateLongFileNameTestStep::ImplTestStepL()
/**
 @return - void code
 Override of base class pure virtual
 */
	{
	INFO_PRINTF1(_L("CCreateLongFileNameTestStep in test step"));

	//get transaction slot from ini
	TInt trSlot = GetTransactionSlotFromConfigL();
	
	//get the target file path from ini
	TPtrC filePathInIni = GetTargetFilePathFromConfigL();
	
	TFileMode fileMode(static_cast<TFileMode>(EFileShareExclusive|EFileWrite));
	
	//the handle to the newly created file
	// The file handle doesn't get actually created as it is too long. Hence
	// pushing onto the closecleanup stack is not necessary.
	RFile newFile;

	//register newly created file
	TRAP_IGNORE(Server().CreateNewL(trSlot, filePathInIni, newFile, fileMode));

	//roolback transaction
	Server().RollBackL(trSlot);
	
	TBuf<KBufSize> opBuf(GetStringFromConfigL(KBaseDir));
	// checking for the existence of the base directory. If the base directory
	// is existing then it implies that rollback was not successful. Hence return
	// failure. Otherwise return success.
	if (FileExistsL(opBuf))
		{
		SetTestStepResult(EFail);
		}
	else
		{
		SetTestStepResult(EPass);
		}
	}

void CCreateLongFileNameTestStep::ImplTestStepPostambleL()
/**
 @return - void code
 Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CCreateLongFileNameTestStep Postamble"));
	}

//----------------------------------------
