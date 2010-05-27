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
* Implements CSwiObserverProcessor processing log files and notifying plug-ins.
*
*/


/**
 @file
 @internalComponent
 @released
*/
 
 #include "swiobserver.h"

using namespace Swi;

//
//CSwiObserverProcessor
//

CSwiObserverProcessor::CSwiObserverProcessor(CSwiObserver& aObserver)
/**
	Initializes and adds the SWI Observer Processor object to the active scheduler.
 */
	:CActive(CActive::EPriorityLow),
	 iObserver(aObserver),
	 iState(EIdle)
		{
		CActiveScheduler::Add(this);
		}

		
CSwiObserverProcessor::~CSwiObserverProcessor()
/**
	Destructor.
 */
	{
	DEBUG_PRINTF(_L8("SWI Observer Processor Destructed"));
	Reset();
	delete iReader;
	
	iImplList.ResetAndDestroy();
	iImplList.Close();
	
	iProperty.Close();
		
	Deque();
	}
	
	
void CSwiObserverProcessor::Reset()
/**
	Resets the SWI Observer processor object.
 */
	{
	DEBUG_PRINTF(_L8("SWI Observer Processor Reset"));
	delete iData;
	iData = 0;
		
	delete iHeader;
	iHeader = 0;	
		
	delete iCurrentFilter;
	iCurrentFilter =0;
	
	delete iPlugin;
	iPlugin = 0;
	iCurrentImpl = 0;
	
	if(iReader)
		{
		iReader->CloseFile();
		}	
		
	iCurrentFileName.Close();		
	
	REComSession::FinalClose();
	}
	
	
CSwiObserverProcessor* CSwiObserverProcessor::NewL(CSwiObserver& aObserver)
/**
	Factory function allocates a new, initialized instance of CSwiObserverProcessor.
 */
	{
	CSwiObserverProcessor *sop = new(ELeave) CSwiObserverProcessor(aObserver);
	CleanupStack::PushL(sop);
	sop->ConstructL();
	CleanupStack::Pop(sop);
	return sop;
	}
	

void CSwiObserverProcessor::ConstructL()
/**
	Second phase constructor.
 */
	{
	DEBUG_PRINTF(_L8("SWI Observer Processor Created"));
	//Create a reader object to read observation files in next states.
	iReader = CSwiObservationReader::NewL();
	//Retrieve the list of all ECOM plug-ins registered to the SWI Observer.
	REComSession::ListImplementationsL(KUidSwiObserverPluginInterface, iImplList);	
	}
	
	
void CSwiObserverProcessor::DoCancel()
//	Implements from CActive.
	{
	BULLSEYE_OFF
	//The SWI Observer processor is cancelled by noone.
	//It might be cancelled by the SWI Observer Server,
	//but it is not because this a background processor.
	DEBUG_PRINTF(_L8("SWI Observer Processor Cancelled"));
	switch(iState)
		{	
		case ERestoreCompleted:
			 iProperty.Cancel();
			 break;
	
		case ENextPlugin:
		case EProcessLogFile:
			 if(iPlugin)
				{
				iPlugin->Cancel();
				}
			 break;
	
		default:
			break;
		}
	BULLSEYE_RESTORE
	}


TInt CSwiObserverProcessor::RunError(TInt aError)
//	Implements from CActive.
	{
	DEBUG_PRINTF2(_L8("SWI Observer Processor failed with %d error number"),aError);
	(void)aError;
	//Reset the processor and leaves it ready for next run.
	Reset();
	//Informs swi observer that the processor has completed.
	iProgress = EProcessorIdle;
	//Initialize the state machine for the next run
	iState = EIdle;
	//Complete the swi observer
	iObserver.ProcessorCompleted();
			
	return KErrNone;
	}

	
void CSwiObserverProcessor::RunL()
//	Implements from CActive.
	{
	
	switch(iState)
		{			
		case ERestoreCompleted:
			 RestoreCompletedL();
			 break;
		
		case ENextPlugin:
			 NextPluginL();
			 break;
		
		case ENextLogFile:
			 NextLogFileL();
			 break;
		
		case EProcessLogFile:
			 ProcessLogFileL();
			 break;
			
		case ECompleteProcess:
			 CompleteProcess();
			 break;
		BULLSEYE_OFF	 
		default://Unexpected state. Should never come here.
			User::Leave(KErrGeneral);
		BULLSEYE_RESTORE	
		}
	}


void CSwiObserverProcessor::StartProcessingL()
/**
	Initiates the SWI Observer processor to investigate log files and notify 
	subscribed ECOM plug-ins. 
	
	If the phone is in normal mode, the next state of the processor will be ECheckPluginList.
	
	If the phone is in backup/restore mode, the processor subscribes to the property
	of the backup&restore and waits until the phone turns back normal mode.
 */
	{
	DEBUG_PRINTF(_L8("SWI Observer Processor Started"));
	//Check whether the phone is in backup/restore mode.
	TInt property = 0;
	TInt err = iProperty.Get(KUidSystemCategory, conn::KUidBackupRestoreKey, property);
	
	if (err != KErrNone && err != KErrNotFound)
		{
		User::Leave(err);
		}
	
	if(property & (conn::KBURPartTypeMask^conn::EBURNormal))
		{//if the phone is in backup/restore mode, subscribe and wait until it turns to normal mode.
		DEBUG_PRINTF(_L8("Backup/Restore is in progress. The processor is waiting until it finishes"));
		User::LeaveIfError(iProperty.Attach(KUidSystemCategory,conn::KUidBackupRestoreKey));
		iProperty.Subscribe(iStatus);
		iProgress = EProcessorSubscribed;
		iState = ERestoreCompleted;
		SetActive();
		}
	else
		{
		//The phone is in normal mode. Carry on the proccess with ENextLogFile state.
		iProgress = EProcessorBusy;
		iState = ENextLogFile;
		SelfComplete();
		}	
	}
	
	
void CSwiObserverProcessor::SelfComplete()
/**
	Generates an event on itself by completing on iStatus.
 */
	{
	TRequestStatus *status = &iStatus;
	User::RequestComplete(status,KErrNone);
	SetActive();
	}


void CSwiObserverProcessor::RestoreCompletedL()
/**
	Gets the backup/restore key property to see whether a restore process is in progress.
	If the phone is in normal mode, the next state is ECheckPluginList.
	If the phone is in bacup/restore mode, this function subscribes the backup/restore
	property key and waits until the backup/restore process completes.
 */
	{
	DEBUG_PRINTF(_L8("SWI Observer Processor - Backup/Restore completed"));
	TInt property = 0;
	TInt err = iProperty.Get(KUidSystemCategory, conn::KUidBackupRestoreKey, property);
	
	if (err != KErrNone && err != KErrNotFound)
		{
		User::Leave(err);
		}
	
	if(property & (conn::KBURPartTypeMask^conn::EBURNormal))
		{
		DEBUG_PRINTF(_L8("Backup/Restore is in progress. The processor is waiting until it finishes"));
		User::LeaveIfError(iProperty.Attach(KUidSystemCategory,conn::KUidBackupRestoreKey));
		iProperty.Subscribe(iStatus);
		iState = ERestoreCompleted;
		SetActive();
		}
	else
		{
		iProgress = EProcessorBusy;
		iState = ENextLogFile;
		SelfComplete();
		}
	}
 
 
 void CSwiObserverProcessor::NextLogFileL()
 /**
 	Gets the first log file from the log_swi private folder and moves it to the log_obs 
 	directory for processing.
 	
    If there is any log file to process, the next state is ENextPlugin.
    If all log files have already been processed, the next state is ECompleteProcess.
  */
 	{
 	//First of all, delete the previous log file, if it exists.
 	if(iCurrentFileName.Length() > 0)
 		{
 		DEBUG_PRINTF2(_L("SWI Observer Processor - Deleting %S log file"),&iCurrentFileName);
 		User::LeaveIfError(iObserver.iFs.Delete(iCurrentFileName));
 		}
 	
 	//Obtain the swi log directory
 	HBufC* swiLogDir = iObserver.GetLogDirPathLC(KSwiLogDir);
 	
 	//Get the list of all files in the swi log directory
 	CDir *entryList(0);
	User::LeaveIfError(iObserver.iFs.GetDir(*swiLogDir,KEntryAttNormal,ESortByExt|EAscending|EDirsLast,entryList));
	CleanupStack::PopAndDestroy(swiLogDir);
	CleanupStack::PushL(entryList);
	
	TBool found = EFalse;
	TInt count = entryList->Count();
	
	for(TInt i=0; i<count; ++i)
		{
		//Get the full path of the current log file file in the list.
		HBufC* swiFilePath = iObserver.GetLogFilePathLC((*entryList)[i].iName, KSwiLogDir);
		DEBUG_PRINTF2(_L("SWI Observer Processor - %S log file will be processed"),swiFilePath);		
		
		//Get the new full path of the log file.
		HBufC* obsFilePath = iObserver.GetLogFilePathLC((*entryList)[i].iName, KObserverLogDir);
		//Update the current file name member.
		iCurrentFileName.Close();
		iCurrentFileName.CreateL(*obsFilePath);
		
		TRAPD(err,
			MoveLogFileL(iObserver.iFs, *swiFilePath, *obsFilePath);
			iReader->OpenFileL(iObserver.iFs, iCurrentFileName));	
		CleanupStack::PopAndDestroy(2,swiFilePath);
					
		if(KErrNone == err)
			{
			found = ETrue;
			break;
			}
		else if(KErrNoMemory == err)
			{
			User::Leave(err);
			}
		else
			{//Delete the file
			DEBUG_PRINTF3(_L("SWI Observer Processor - Deleting %S log file because of a failure %d"),&iCurrentFileName,err);		
			iReader->CloseFile();
			(void)iObserver.iFs.Delete(iCurrentFileName);
			}
		}
		
	CleanupStack::PopAndDestroy(entryList);
	
	if(found)
		{
		DEBUG_PRINTF2(_L("SWI Observer Processor - Starting to process %S log file"),&iCurrentFileName);
		iState = ENextPlugin;
		}
	else
		{
		DEBUG_PRINTF(_L("SWI Observer Processor - No log file to process"));
		iState = ECompleteProcess;
		}
	
	SelfComplete();
 	}
 	
 	
void CSwiObserverProcessor::NextPluginL()
/**
	Instantiates an implementation of the next plug-in from the registered plug-ins list.
	
	if the last plug-in of the list has already been instantiated, the next processor state is ENextLogFile.
	Otherwise, the next state is EProcessLogFile to process the current file for that plugin.
 */
	{	
	//Get the next implementation
	if(iCurrentImpl < iImplList.Count())
		{
		//First unload the previous implementation, if exists.
		delete iPlugin;
		iPlugin = 0;
		
		TUid implUid = iImplList[iCurrentImpl++]->ImplementationUid();
		DEBUG_PRINTF2(_L("SWI Observer Processor - Loading Plugin(%08x)"),implUid);		
		
		TRAPD(err,iPlugin = CSwiObserverPlugin::NewL(implUid));
		if(KErrNone == err)
			{//Ecom plug-in loaded sucessfully.
			//Get the filter of the newly loaded plug-in.
			delete 	iCurrentFilter;
			iCurrentFilter = 0;
			iCurrentFilter = iPlugin->GetFilterL();
			iState = EProcessLogFile;
			}
		else if(KErrNoMemory == err)
			{//No memory, leave.
			User::Leave(err);
			}
		else
			{//if something goes wrong while loading the plug-in, 
			 //try to load the next one if it exits.
			iState = ENextPlugin;
			}
		}
	else
		{
		DEBUG_PRINTF2(_L("SWI Observer Processor - %S log file processed by all plugins"),&iCurrentFileName);
		iCurrentImpl = 0;
		iReader->CloseFile();
		iState = ENextLogFile;
		}
		
	SelfComplete();
	}


TLogFileProcess CSwiObserverProcessor::ProcessNextLineL(TInt& aIndex)
/**
	Reads the record type field from the current log file and determines what data type will 
	be read next.
	
	If the next record type	in the log file is header, the function will read and create a 
	header object.
	
	If the next record type	in the log file is data, the processor state will read and create 
	a data object.
	
	@param	aIndex The index of the filter list matching with that line.
	@return If a header object has been read, EHeaderNotification is returned to notify current
			ecom plugin. If a data object has been read and the data exist in the filter list,
			EDataNotification is returned to notify the plug-in. Otherwise, ENoNotification is
			returned.
 */
	{
	TLogFileProcess ret = ENoNotification;
	
	TUint8 t = iReader->ReadRecordTypeL();
	if(KHeaderField == t)
		{
		if(!iHeader)
			{
			iPlugin->StartL(iObserver.iFs);
			}	
		delete iHeader;
		iHeader = 0;
		iHeader = iReader->ReadHeaderL();
		ret = EHeaderNotification;
		}
	else 
		{// t == KDataField - ReadRecordTypeL leaves if t is different from
		 // KDataField and KHeaderField. Therefore, t must be KDataField.
		delete iData;
		iData = 0;	
		iData = iReader->ReadRecordL();
		
		aIndex = iCurrentFilter->FindF(iData->iFileName);
		if(KErrNotFound != aIndex)
			{
			ret = EDataNotification;
			}
		}
	return ret;
	}


void CSwiObserverProcessor::ProcessLogFileL()
/**
	Processes a log file and makes required plug-in notifications.
	
	Read the log file record-by-record and notifies the current loaded plug-in 
	if the information is related with the plugin. If the function notifes the plug-in, 
	the next state does not change. If it reaches the end of file, the reader cursor 
	is set to the beginning of the file and then the next state is set to ENextPlugin 
	to be able to process the same log file with another plug-in.
 */
	{
	TLogFileProcess result(ENoNotification);
	TInt error(KErrNone);
	TInt index = 0;
	
	while(KErrNone == error)
		{
		TRAP(error, result = ProcessNextLineL(index));
		if(EHeaderNotification == result)
			{
			iPlugin->NextObservedHeaderL(*iHeader,iStatus);
			iState = EProcessLogFile;
			SetActive();
			break;
			}
		else if(EDataNotification == result)
			{
			iPlugin->NextObservedDataL(*iData,index,iStatus);
			iState = EProcessLogFile;
			SetActive();
			break;
			}
		}
		
	if(error != KErrNone && error != KErrEof)
		{
		//close handle to be able to delete the file.
		iReader->CloseFile();
		
		//If there is no memory, no need to try the next one.	
		if(KErrNoMemory == error)
			{
			User::Leave(error);
			}
		//Process the next log file, if exists.
		iState = ENextLogFile;
		//Start from the first plug-in to process the next one.
		iCurrentImpl = 0;	
		SelfComplete();
		}
		
	if(KErrEof == error)
		{//the end of log file has been reached.
		DEBUG_PRINTF3(_L("SWI Observer Processor - Plugin-%d completed processing %S"),iCurrentImpl,&iCurrentFileName);
		iReader->SetStartPosL();
		//Zeros iHeader to be able to send Start notification for the next plugin
		delete iHeader;
		iHeader = 0;
		iState = ENextPlugin;
		iPlugin->FinalizeL(iStatus);
		SetActive();
		}
	}


void CSwiObserverProcessor::CompleteProcess()
/**
	When the processor comes to this point, all registered plug-ins and avaliable 
	files have already been processed. It's time to complete the processor.
	The SWI Observer first notified to allow it to start shutdown period. Then
	the processor object is reset to make it ready for next process if the
	SWI Commit event happens before the SWI Observer is shut down.
 */
	{
	iObserver.ProcessorCompleted();
	Reset();
	iProgress = EProcessorIdle;
	}
	
	
TProcessorProgress CSwiObserverProcessor::ProgressStatus()
/**
	@return Returns the progress status of the processor.
 */
	{
	return iProgress;
	}


void CSwiObserverProcessor::MoveLogFileL(RFs aFs, const TDesC& aFrom, const TDesC& aTo)
/**
	Moves a file from the source directory to the destination one.
	
	@param aFrom		A full path indicating the file to be moved.
						e.g. !:\\<private_dir>\\<log_dir_1>\\<file_name>			
	@param aTo			A full path indicating the directory into which the file is to be moved.
						e.g. !:\\<private_dir>\\<log_dir_2>\\<file_name>			
 */
	{
	// Create file management object to move the file.
	// CFileMan::Move is used to be able to overwrite if the file exists in the destination.
	DEBUG_PRINTF3(_L("SWI Observer Processor - %S moved to %S"),&aFrom,&aTo);
    CFileMan* fileMan = CFileMan::NewL(aFs);
    CleanupStack::PushL(fileMan); 

	User::LeaveIfError(fileMan->Move(aFrom,aTo));
	
	CleanupStack::PopAndDestroy(fileMan);
	}
