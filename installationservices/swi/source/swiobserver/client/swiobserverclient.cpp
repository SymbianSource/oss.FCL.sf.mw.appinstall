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
* RSwiObserverSession implementation.	 See class and function definitions
* for more detail.
*
*/


/**
 @file
 @internalComponent
 @released
*/


#include "swiobserverclient.h"

using namespace Swi;


EXPORT_C RSwiObserverSession::RSwiObserverSession()
/**
	This constructor provides a single point of definition from
	which the superclass constructor is called.
 */
	: RScsClientBase(),
	  iFile(),iStream(),iClientTid() //added to make lint happy!
		{
		//empty
		}
		
	
EXPORT_C TInt RSwiObserverSession::Connect()
/**
	Connect to the SWI Observer.

	@return		Symbian OS error code where KErrNone indicates
				success and any other value indicates failure.
 */
	{
	TVersion v = SwiObserverVersion();
	TUidType soFullUid = SwiObserverImageFullUid();
	
	return RScsClientBase::Connect(KSwiObserverName, v, KSwiObserverImageName, soFullUid);
	}
	
	
EXPORT_C void RSwiObserverSession::Close()
/**
Closes the SWI Observer connection.
*/
	{
	iStream.Close();
	RScsClientBase::Close();
	}
    

EXPORT_C void RSwiObserverSession::ProcessLogsL(RFs& aFs)
/**
	Checks the SWI Observer's flag file to find out if there are some log files to process.
	If that's the case, it starts up the SWI Observer, if not, then it returns without doing anything.
	
	@param aFs The file server session handle.
 */
	{
	TBool b;
	TInt err;
	RBuf flagFileName;
	flagFileName.CreateL(KMaxFileName);
	flagFileName.CleanupClosePushL();
	flagFileName.Append(aFs.GetSystemDriveChar());
	flagFileName.AppendFormat(KObserverFlagFileNameFormat, KUidSwiObserver.iUid);
	if((err=aFs.IsFileOpen(flagFileName, b))== KErrNone) //returned error code shows whether the file exists or not, the bool value is ignored
		{
		//flag exists ==> start up swiobserver so that it can process the observer logs
		User::LeaveIfError(Connect());
		}
	else
		{
		if(err!=KErrNotFound && err!=KErrPathNotFound)
			{
			User::Leave(err);
			}
		}
	CleanupStack::PopAndDestroy(&flagFileName);
	}


EXPORT_C void RSwiObserverSession::Register(TRequestStatus& aStatus)
/**
	Registers to the SWI Observer. If the SWI Observer processor is busy,
	the server completes the client when the processor becomes idle.
	
	@param	aStatus		The SWI Observer completes this request object when
						the SWI Observer Processor becomes idle.
 */
	{
	TIpcArgs args;
	CallSessionFunction(ERegisterToObserver, args, aStatus);
	}


EXPORT_C void RSwiObserverSession::CancelRegistration()
/**
	Cancels the registration request.
	This function has no effect if there is no outstanding request.
 */
	{
	CancelSessionFunction(ERegisterToObserver);
	}


EXPORT_C void RSwiObserverSession::GetFileHandleL(TDes& aFileName)
/**
	Requests a file handle from the SWI Observer. Having created a log file (e.g. 1.swi)
	under its own private folder, the SWI Observer shares the file handle with the process
	where an instance of this class is working.
	
	@param  aFileName The fully qualified name of the file whose handle was sent
					  by the SWI Observer.
 */
	{
	TInt fsh;			//session (RFs) handle
	TPckgBuf<TInt> fh;	//subsession (RFile) handle
	TIpcArgs args(&fh);
	
	//The message is completed with the file-server (RFs) session handle
	//Pointer to the file subsession handle in slot 0
	fsh = CallSessionFunction(ETransferFileHandle, args);
	User::LeaveIfError(fsh);
	
	// Adopt the file using the returned handles
	User::LeaveIfError(iFile.AdoptFromServer(fsh,fh()));
	iFile.FullName(aFileName);
	
	//Associate this stream with the received file 
	iStream.Attach(iFile);
	}

	
EXPORT_C void RSwiObserverSession::CommitL()
/**
	Notifies the SWI Observer that the SWI operation has been finished successfully.
 */
	{
	//Release this handle to be able to allow the server to change the filename.
	iStream.Release();	
									 
	User::LeaveIfError(CallSessionFunction(ESwiCommit)); 	
	}								 	
	

EXPORT_C void RSwiObserverSession::AddEventL(const CObservationData& aData)
/**
	Writes an observed SWI event into the shared log file about the file changed by the SWI operation.
	
	@param aData 	The observed SWI event.
	@return			Symbian OS error code where KErrNone indicates success and 
					any other value indicates failure.
 */
	{
	aData.ExternalizeL(iStream);
	//Commit the changes
	iStream.CommitL();
	}


EXPORT_C void RSwiObserverSession::AddHeaderL(const CObservationHeader& aHeader)
/**
	Writes a SWI observation header into the shared log file about the file changed by the SWI operation.
	
	@param aHeader		The observation header.
	@return				Symbian OS error code where KErrNone indicates
						success and any other value indicates failure.
 */
	{
	aHeader.ExternalizeL(iStream);
	//Commit the changes
	iStream.CommitL();
	}
