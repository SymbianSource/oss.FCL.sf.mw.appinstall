/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements CSwiObserver providing management of SWI Observer services.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#include "swiobserver.h"
#include "installclientserver.h"

using namespace Swi;

static const TUint swiObserverRangeCount = 3;

static const TInt swiObserverRanges[swiObserverRangeCount] =
	{
	0,							//Range 0 - 0 to EBaseSession-1. Not used.
	CScsServer::EBaseSession,	//Range 1 - EBaseSession to EBaseMustAllow-1 inclusive.
	CScsServer::EBaseMustAllow
	};

static const TUint8 swiObserverElementsIndex[swiObserverRangeCount] =
	{
	CPolicyServer::ENotSupported, // Range 0 is not supported.
	0,							  // Range 1 must come from the Install Server.
	CPolicyServer::EAlwaysPass
	};							

static const CPolicyServer::TPolicyElement swiObserverElements[] =
	{
	{_INIT_SECURITY_POLICY_S0(KInstallServerUid3.iUid), CPolicyServer::EFailClient},	
	};

static const CPolicyServer::TPolicy swiObserverPolicy =
	{
	CPolicyServer::EAlwaysPass, // Allow all connects
	swiObserverRangeCount,
	swiObserverRanges,
	swiObserverElementsIndex,
	swiObserverElements,
	};

//
//CSwiObserver
//

CSwiObserver::CSwiObserver()
/**
	Intializes the SWI Observer object with its version and policy.
 */
	:	CScsServer(SwiObserverVersion(),swiObserverPolicy)
		{
		//empty
		}

CSwiObserver::~CSwiObserver()
/**
	Destructor. Cleanup the SWI Observer.
 */
	{
	DEBUG_PRINTF(_L8("SWI Observer Shutdown"));
	delete iProcessor;
	delete iPrivateFolder;
	iFs.Close();
	}
	
	
CSwiObserver* CSwiObserver::NewLC()
/**
	Factory function allocates new, initialized instance of CSwiObserver.

	@return		New, initialized instance of CSwiObserver
				which is left on the cleanup stack.
 */
	{
	CSwiObserver *so = new (ELeave) CSwiObserver();
	CleanupStack::PushL(so);
	so->ConstructL();
	return so;
	}


void CSwiObserver::ConstructL()
/**
	Second phase constructor starts the SWI Observer.
 */
	{
	DEBUG_PRINTF(_L8("SWI Observer Created"));
	//SWI Observer is not auto-exit server. So, provide shutdown period as zero.
	CScsServer::ConstructL(0); /*aShutdownPeriodUs=0*/
	
	//Create the processor performing the observation file processing.
	iProcessor = CSwiObserverProcessor::NewL(*this);
	
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareProtected());
	
	//Check and initialize the private folder.
	TBool startProcessor = InitializeLogDirsL();
	
	//If we won't run the processor, enable the shutdown timer.
	if(!startProcessor)
		{
		//Remove the flag file which marks presence of unprocessed logs. If the flag file is not present during boot, then SWI observer won't be loaded, speeding up the boot process		
		TRAP_IGNORE(ManipulateFlagFileL(EDeleteFlagFile)); //flag file operation is not considered fatal, so the function is trapped here
		DEBUG_PRINTF(_L8("Enable Shutdown Timer"));
		EnableShutdownTimerL(KSwiObserverShutdownPeriod);
		}
		
	StartL(KSwiObserverName);
	
	if(startProcessor)
		{
		DEBUG_PRINTF(_L8("Processor is being started to process the log file(s) left from the previous session"));
		//There is at least one log file to be processed,
		//run the processor to process them.
		ProcessL();
		}
	}


CScsSession* CSwiObserver::DoNewSessionL(const RMessage2& aMessage)
/**
	Implement CScsServer by allocating a new instance of CSwiObserverSession.

	@param	aMessage	Standard server-side handle to message.
	@return				New instance of CSwiObserverSession which is owned by the caller.
 */
	{
	DEBUG_PRINTF(_L8("SWI Observer session created"));
	return CSwiObserverSession::NewL(*this, aMessage);
	}


void CSwiObserver::ProcessorCompleted()
/**
	This function is called by CSwiObserverProcessor to notify the server object
	that the process has been completed. A timer is started and if no requests
	come in pre-defined time, the server is shut down.
 */
	{
	DEBUG_PRINTF(_L8("SWI Observer Processor completed"));
	//Remove the flag file which marks presence of unprocessed logs. If the flag file is not present during boot, then SWI observer won't be loaded, speeding up the boot process		
	TRAP_IGNORE(ManipulateFlagFileL(EDeleteFlagFile)); //ProcessorCompleted() can not leave therefore the function call is trapped
	if(iAsyncRequests.Count() > 0)
		{
		DEBUG_PRINTF(_L8("Wake up another waiting client"));
		TRequestStatus *status = &(iAsyncRequests[0]->iStatus);
		User::RequestComplete(status,KErrNone);
		}
	else
		{
		DEBUG_PRINTF(_L8("Enable Shutdown Timer"));
		TRAP_IGNORE(EnableShutdownTimerL(KSwiObserverShutdownPeriod));
		}
	}


HBufC* CSwiObserver::MakeLogDirectoryLC(const TDesC& aLogDir)
/**
	Makes a log directory and returns its full path. If the log directory already exists,
	just returns the path of the log directory.
	The log directory path format is !:\\<private_dir>\\<aLogDir>\\
	
	@param aLogDir	The name of the log directory which will be created in the private directory.
	@return			The full path of the log directory which has been created.
 */
	{
	HBufC* logPath = GetLogDirPathLC(aLogDir);
	
	TInt err = iFs.MkDir(*logPath);
	if(err != KErrNone)
		{
		if(err != KErrAlreadyExists)
			{
			User::LeaveIfError(err);
			}
		}
	return logPath;
	}
	
	
TBool CSwiObserver::InitializeLogDirsL()
/**
	First finds the private directory of the SWI observer. Then checks whether both of log directories
	in the private directory exist. If not, the log directories are created. These directories are:
	\log_swi and \log_obs. 

	The log_swi directory contains log files commited successfully by the SWI Installer. These files
	will be processed by the SWI Observer processor. This directory can not have log files which has
	not been commited yet. Because, uncommitted log files are deleted during either the session close
	or integrity rollback operation.
	
	The log_obs directory contains log files which are being processed by the processor. When a log file
	is started to be processed, it is first moved on that directory. If there is any log file in that
	directory at this point, it means that the processor had failed while it was processing these log
	files. Therefore, these files must be deleted.
	
	The function also finds the highestfile name number by scanning log_swi folder. 
	File name format: a hexadecimal name (8 chars, e.g. E34FA879).
	
	@return ETrue, if there is any file in the swi log directory; otherwise, EFalse.
 */
	{
	TDriveNumber drive = iFs.GetSystemDrive();
	User::LeaveIfError(iFs.CreatePrivatePath(drive));
	User::LeaveIfError(iFs.SetSessionToPrivate(drive));
	
	TFileName logPath;
	User::LeaveIfError(iFs.PrivatePath(logPath));
	iPrivateFolder = logPath.AllocL();
	
	// Create file management object
	CFileMan *fileMan = CFileMan::NewL(iFs);
	CleanupStack::PushL(fileMan); 
	
	//Ensure the observer log directory exist (/private/102836C3/log_obs/)
	HBufC *swiLogDir = MakeLogDirectoryLC(KObserverLogDir);
	//Delete all files in the swi log directory
	TInt err = fileMan->Attribs(*swiLogDir, 0, 
				KEntryAttReadOnly | KEntryAttSystem | KEntryAttHidden, TTime(0));
	if (err != KErrNone)
		{
		DEBUG_PRINTF2(_L("SWI Observer InitializeLogDirsL - attribs failed with error %d"), err);
		}
	
	err = fileMan->Delete(*swiLogDir);
	if(err != KErrNone) 
		{
		if(err != KErrNotFound)
			{
			DEBUG_PRINTF2(_L("SWI Observer InitializeLogDirsL - delete failed with error %d"), err);
			User::Leave(err);
			}	
		}

	CleanupStack::PopAndDestroy(2, fileMan);
			
	//Ensure the swi log directory exist
	HBufC *obsLogDir = MakeLogDirectoryLC(KSwiLogDir);
	
	CDir *entryList;
	//Order the entries of the swi log directory in descending order. 
	//The first element in the list will have the highest hexadecimal name.
	User::LeaveIfError(iFs.GetDir(*obsLogDir,KEntryAttNormal,ESortByName|EDescending|EDirsLast,entryList));
	CleanupStack::PopAndDestroy(obsLogDir);
	CleanupStack::PushL(entryList);
	
	TBool fileExist = (entryList->Count() > 0);
	if(fileExist)
		{
		//Get the first file from the list
		TParsePtrC fpp ((*entryList)[0].iName);
		
		TLex lex(fpp.Name());
		(void)lex.Val(iHighestFileNum,EHex);//If this fails or count==0, iHighestFileNum will be
											//keept as zero which was assigned in the construction.
		}	
	DEBUG_PRINTF2(_L8("The highest SWI Observer log file name is %08x"), iHighestFileNum);
	CleanupStack::PopAndDestroy(entryList);
	return fileExist;
	}


void CSwiObserver::AppendFileNameL(RBuf& aFileName)
/**
	Creates a hexadecimal file name by using the highest available number.
	@return A hexadecimal file name.
 */
	{
	//Integrity service might have deleted the swi log directory, so check its existence.
	//and gets the swi log folder path.
	HBufC *swiLogDir = MakeLogDirectoryLC(KSwiLogDir);
	//Now generate a log file name.
	aFileName.CreateL(swiLogDir->Length() + KMaxLogFileName);
	_LIT(KLogFileNameFormat, "%S%08x"); //<private_swi_log_dir><hex_name>
	aFileName.AppendFormat(KLogFileNameFormat, swiLogDir, ++iHighestFileNum);
	CleanupStack::PopAndDestroy(swiLogDir);
	}
	

void CSwiObserver::ProcessL()
/**	
	This function starts the processor if it has not been started yet.
	The shutdown timer is also disabled. 
 */
	{	
	if(EProcessorIdle == iProcessor->ProgressStatus())
		{
		DisableShutdownTimer();
		iProcessor->StartProcessingL();
		}
	}


HBufC* CSwiObserver::GetLogDirPathLC(const TDesC& aLogDir)
/**
	Generates and returns the full path of the provided log directory.
	The log directory path format: !:\\<private_dir>\\<aLogDir>\\
	
	@param aLogDir The name of the log directory.
	@return		   The full path of the provided log directory.
				   The returned value is left on the cleanup stack.
 */
	{
	HBufC* logDirPath(0);
	_LIT(KLogDirPathFormat, "%S%S");//\\<private_dir>\\<log_dir>
	
	logDirPath = HBufC::NewLC(iPrivateFolder->Length() + aLogDir.Length());
	TPtr ptr(logDirPath->Des());
	ptr.AppendFormat(KLogDirPathFormat, iPrivateFolder, &aLogDir);
	
	return logDirPath;
	}

	
HBufC* CSwiObserver::GetLogFilePathLC(const TDesC& aLogFile, const TDesC& aLogDir)
/**
	Generates and returns the fully qualified path of the provided log file.
	The log file path format: !:\\<private_dir>\\<aLogDir>\\<aLogFile>
	
	@param aLogFile The name of the log file.
	@param aLogDir	The name of the log directory.
	@return			The fully qualified path of the provided log file.
					The returned value is left on the cleanup stack.
 */
	{
	HBufC* logFilePath(0);
	_LIT(KLogFilePathFormat, "%S%S%S");//\\<private_dir>\\<log_dir><file_name>
	
	logFilePath = HBufC::NewLC(iPrivateFolder->Length() + aLogDir.Length() + aLogFile.Length());
	TPtr ptr(logFilePath->Des());
	ptr.AppendFormat(KLogFilePathFormat, iPrivateFolder, &aLogDir, &aLogFile);
	
	return logFilePath;
	}


TProcessorProgress CSwiObserver::ProcessorProgressStatus()
/**
	@return The progress status of the SWI Observer Processor.
 */
	{
	return iProcessor->ProgressStatus();
	}


void CSwiObserver::DoPreHeapMarkOrCheckL()
/**
	Deletes the shutdown timer to make the OOM tests passed.
 */
	{
#ifdef _DEBUG
	DisableShutdownTimer();
#endif
	}
	

void CSwiObserver::DoPostHeapMarkOrCheckL()
/**
	Re-creates the shutdown object after memory checking.
 */
	{
#ifdef _DEBUG
	EnableShutdownTimerL(KSwiObserverShutdownPeriod);
#endif
	}

/*static*/ HBufC* CSwiObserver::CreateFlagFileNameLC()
	{
	HBufC* flagFileNameBufferPtr = HBufC::NewLC(KMaxFileName);
	TPtr ptr(flagFileNameBufferPtr->Des());
	ptr.Append(RFs::GetSystemDriveChar());
	ptr.AppendFormat(KObserverFlagFileNameFormat, KUidSwiObserver.iUid);
	return flagFileNameBufferPtr;
	}

void CSwiObserver::ManipulateFlagFileL(TFlagFileOperation operation)
	{
 	HBufC* flagFileNameBufferPtr=CreateFlagFileNameLC();
 	TInt err(KErrNone);
	switch(operation)
		{
		case EDeleteFlagFile:
			err=iFs.Delete((*flagFileNameBufferPtr));
			if(KErrNone!=err && KErrPathNotFound!=err && KErrNotFound!=err) 
				{
				DEBUG_PRINTF3(_L("CSwiObserver::ManipulateFlagFileL() - Deleting flag file %S returned error code %d"), flagFileNameBufferPtr, err);
				User::Leave(err);
				}
			break;
		case ECreateFlagFile:
			err=iFs.MkDirAll(*flagFileNameBufferPtr); //first we have to create the full directory path otherwise RFs::Create will fail
			if(KErrNone==err || KErrAlreadyExists==err)
				{
				RFile file;
				err=file.Create(iFs, *flagFileNameBufferPtr, TFileMode(EFileShareAny|EFileWrite));
				if(KErrNone==err)
					{
					file.Close();
					}
				else if(KErrAlreadyExists!=err && KErrInUse!=err)
					{
					DEBUG_PRINTF3(_L("CSwiObserver::ManipulateFlagFileL() - Creating flag file %S returned error code %d"), flagFileNameBufferPtr, err);
					User::Leave(err);
					}
				}
			else
				{
				DEBUG_PRINTF3(_L("CSwiObserver::ManipulateFlagFileL() - Creating path for flag file %S returned error code %d"), flagFileNameBufferPtr, err);
				User::Leave(err);
				}
			break;
		default:
			break;
		}
	CleanupStack::PopAndDestroy(flagFileNameBufferPtr);
	}


