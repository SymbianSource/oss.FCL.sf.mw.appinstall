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
*
*/


/**
 @file
 @test
 @internalComponent
*/

#ifndef __TSTSTESTSUITESTEPCOMMON_H__
#define __TSTSTESTSUITESTEPCOMMON_H__
#include <e32cmn.h>
#include <e32def.h>
#include <f32file.h>

//ini item names
/**
 Name of the item that defines which transaction slot is to be used in CStsTestServer
 Value can range from 0 to CStsTestServer::maxNumberOfTransactions-1
 Usage: <KTransactionSlot>=<slot>   e.g: TrSlot=0 or TrSlot=1 etc...
 */
_LIT(KTransactionSlot,	"trslot");

/**
 Name of the item that defines the fully qualified name of the target file
 Usage: <KTargetFile>=<path>   e.g: target=c:\sys\bin\sts\server\config.ini
 Can be used with the all commands that require a file path as input. 
 */
_LIT(KTargetFile,	"target");

/**
 This item has the followin role:
 a) Name of the item that defines which transaction id is to be reused (from which slot) when issuing an open transaction command
    Usage: <KTransactionSlotID>=<slot>   e.g: TrSlotID=0 or TrSlot=1 etc...
 
 Usage:
    [testopentransactionsection]
    TrSlot=1
    TrSlotID=0  ==> reuses transaction ID of transaction in slot 0            

 Important note: a section that is intended to be used with open transaction command should contain either 
                 KTransactionSlotID or KTransactionID out of which KTransactionSlotID is searched for first.
                 (That's if an ini section conatins both then KTransactionID is simply ignored)
 */
_LIT(KTransactionSlotID,	"trslotid");

/**
 An arbitrary transaction ID (4byte) can be defined in the ini file that is used when opening a transaction. 

 Usage:
    [testopentransactionsection]
    trslot=1
    trid=0xABCDEF  ==> this vvalue is used when openeing a transaction
 Important note: a section that is intended to be used with open transaction command should contain either 
                 KTransactionSlotID or KTransactionID out of which KTransactionSlotID is searched for first.
                 (That's if an ini section conatins both then KTransactionID is simply ignored)
 */
_LIT(KTransactionID,	"trid");

/**
 This item is for CheckFilesStep.
 It's used to show how many subsequent file name/path items in the ini file are to be extracted 
 for checking wheter the corresponding files exist or not.
 Usage:
   numexist=xx eg: numexist=2
*/
_LIT(KNumExist, "numexist"); 

/**
 This item is for CheckFilesStep.
 It's used to define a particular file's name/path that is to be checked for.
 Usage:
   exist<x>=<path> where x starts from 0. eg: exist0=d:\myfolder\mysubfolder\myfile.txt
*/
_LIT(KExistBase, "exist"); // + number (0-based) = file to check for

/**
 This item is for CheckFilesStep.
 It's used to show how many subsequent file name/path items in the ini file are to be extracted 
 for checking wheter the corresponding files don't exist.
 Usage:
   numnonexist=xx eg: numnonexist=2
*/
_LIT(KNumNonExist, "numnonexist"); // this specifies how many files to check for

/**
 This item is for CheckFilesStep.
 It's used to define a particular file's name/path that is to be checked for.
 Usage:
   nonexist<x>=<path> where x starts from 0. eg: nonexist0=d:\myfolder\mysubfolder\myfile.txt
*/
_LIT(KNonExistBase, "nonexist"); // + number (0-based) = file to check for

/**
 This item is for FileOperationsStep.
 It's used to specify the number of subsequent file operations in the ini file.
 Usage:
   operations=xx eg: operations=2
*/
_LIT(KNumOfOperations, "operations"); // this specifies how many fileoperations are to be executed

_LIT(KOpNameBase, 	   "operation"); 

/**
 This item is for CreateLargeFileNameStep.
 It's used to specify the base directory.
 Usage:
   basedir=xx 
*/
_LIT(KBaseDir, 	   "basedir");

/**
 This item is for CreateLargeFileNameStep.
 It's used to specify the base directory 
 buffer size.
*/
const TInt KBufSize = 30; 

/**
 Thess items are for FileOperationsStep.
 They denote file commands accepted by FileOperationsStep.
 Usage:
   operationxx=<command> <switch> <source> <destination>
 eg: 
   operation0=mkdirall -i c:\sys\bin\dummydir\    //-i suppresses all errors during the operation
   operation1=copy z:\sys\bin\dummydir\dummyfile.txt z:\sys\bin\dummydir\dummyfileoncdrive.txt
*/
_LIT(KMkDirAllOp, 	   "mkdirall"); 
_LIT(KRmDirOp, 	   	   "rmdir"); 
_LIT(KCopyOp, 	   	   "copy"); 
_LIT(KDeleteOp,	   	   "delete"); 
_LIT(KAttribOp,		   "attrib"); 
_LIT(KIgnoreErrorsSwitch, "-i"); 

//test step names
_LIT(KCreateTransactionStep,	"CreateTransactionStep");
_LIT(KOpenTransactionStep,		"OpenTransactionStep");
_LIT(KCloseTransactionStep,		"CloseTransactionStep");
_LIT(KRegisterNewFileStep,		"RegisterNewFileStep");
_LIT(KCreateNewFileStep,		"CreateNewFileStep");
_LIT(KRemoveStep,				"RemoveStep");
_LIT(KRegisterTemporaryStep,	"RegisterTemporaryStep");
_LIT(KCreateTemporaryStep,		"CreateTemporaryStep");
_LIT(KOverwriteStep,			"OverwriteStep");
_LIT(KCommitStep,				"CommitStep");
_LIT(KRollbackStep,				"RollbackStep");
_LIT(KRollbackAllPendingStep,	"RollbackAllPendingStep");
_LIT(KCheckFilesStep,			"CheckFilesStep");
_LIT(KNegativeTestsStep,		"NegativeTestsStep");
_LIT(KCheckFileModeChangeStep,	"CheckFileModeChangeStep");
_LIT(KJournalFileUnitTest,	    "JournalFileUnitTest");
_LIT(KJournalFileUnitTestAdd,  	"JournalFileUnitTestAdd");
_LIT(KJournalFileUnitTestTemp,  	"JournalFileUnitTestTemp");
_LIT(KJournalFileUnitTestRemove, "JournalFileUnitTestRemove");
_LIT(KJournalFileUnitTestRead,  	"JournalFileUnitTestRead");
_LIT(KJournalFileUnitTestEvent,  "JournalFileUnitTestEvent");
_LIT(KJournalUnitTest,	    		"JournalUnitTest");
_LIT(KJournalUnitTestInstall,	"JournalUnitTestInstall");
_LIT(KJournalUnitTestRollback,	"JournalUnitTestRollback");
_LIT(KIntegrityservicesUnitTestInstall,	"IntegrityservicesUnitTestInstall");
_LIT(KIntegrityservicesUnitTestRollback,	"IntegrityservicesUnitTestRollback");
_LIT(KFileOperationsStep,					"FileOperationsStep");
_LIT(KFileModeTestStep,					"FileModeTestStep");
_LIT(KCreateLongFileNameTestStep,			"CreateLongFileNameTestStep");

//test step names for legacy integrityservices testcode (see ../tintegrityservices/source/tintegrityservicesstep.cpp)
_LIT(KInstall, 					"Install");
_LIT(KRecover, 					"Recover");
_LIT(KCleanup, 					"Cleanup");
_LIT(KCheck, 					"Check");


const TInt KDefaultFileCreationMode = EFileShareExclusive|EFileWrite;

enum TNewFileType
	{
	ENewTemporaryFile,
	ENewPermanentFile
	};
	
//predefined elements used to create file content when creating new files	
_LIT(KFileData1, "This ");
_LIT(KFileData2, "temporary ");
_LIT(KFileData3, "permanent ");
_LIT(KFileData4, "unknown type ");
_LIT(KFileData5, "file was created by ");
_LIT(KFileData6, " command");
_LIT(KFileData7, "the STS server when instructed by ");
_LIT(KFileData8, " @ ");
_LIT(KTimeFormatString, "%H%:%T%:%S%.%C  %D%/%M%/%Y");


//script and ini file item used by legacy integrityservices testcode (see ../tintegrityservices/source/tintegrityservicesstep.cpp)
_LIT(KRemoveFile,	 	"removefile");
_LIT(KTempFile, 		"temporaryfile");
_LIT(KAddFile, 			"addfile");
_LIT(KFailType, 		"failtype");
_LIT(KFailPosition, 	"failposition");
_LIT(KFailFileName, 	"failfilename");
_LIT(KPresent, 			"present");
_LIT(KAbsent, 			"absent");
_LIT(KCleanupFile, 		"cleanupfile");
_LIT(KCleanupDirectory, "cleanupdirectory");
_LIT(KArbitraryOpFile, 	"arbitraryopfile");
_LIT(KOperation, 		"operation");
#endif

