/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "mimehandler.h"
#include <apacmdln.h>

const TInt KUidMimeAppValue=0x802EE3EE ; 

void CMimeAppUi::DoExitL()
	{
	
	CVwsSessionWrapper* ses=CVwsSessionWrapper::NewLC();
	ses->EnableServerEventTimeOut(ETrue);
	CleanupStack::PopAndDestroy();
	Exit();
	}

CMimeAppUi::CMimeAppUi()
	{
	RDebug::Printf("CMimeAppUi::CMimeAppUi()");
	}
	
void CMimeAppUi::ConstructL()
    {
	BaseConstructL();
	}

CMimeAppUi::~CMimeAppUi()
	{
	}

TBool CMimeAppUi::ProcessCommandParametersL(TApaCommand aCommand, 
	TFileName& aDocumentName, const TDesC8& /*aTail*/)
	{

	RDebug::Printf("CMimeAppUi::ProcessCommandParametersL by filename %d", aCommand);
	switch (aCommand)
		{
		case EApaCommandOpen:
			{
			//Open the file for read the path and create the file.
			TBuf8 <KMaxFileName> readBuffer;
			RFs fs;
			User::LeaveIfError(fs.Connect());
			CleanupClosePushL(fs);			
			RFile file;
			User::LeaveIfError(file.Open(fs, aDocumentName, EFileStreamText));
			CleanupClosePushL(file);			
			User::LeaveIfError(file.Read(readBuffer));
			RFile newFile;
			TBuf <KMaxFileName> fileName;
			fileName.Copy(readBuffer);	
			_LIT(KFormat, "Creating %S");
			RDebug::Print(KFormat, &fileName);
				
			User::LeaveIfError(newFile.Create(fs,fileName,EFileStreamText|EFileWrite|EFileShareExclusive));
			newFile.Close();
			CleanupStack::PopAndDestroy(2, &fs);
			}
		break;	   
		}
		
	return ETrue;	
	}
	
TBool CMimeAppUi::ProcessCommandParametersL(CApaCommandLine& aCommandLine)
	{
	RDebug::Printf("CMimeAppUi::ProcessCommandParametersL %d", aCommandLine.Command());
		
	switch (aCommandLine.Command())
		{
		case EApaCommandOpen:
			{		
			// Open the file and use the handle to install it
			RFile fileHandle;
			CleanupClosePushL(fileHandle);
			aCommandLine.GetFileByHandleL(fileHandle);
			
			if (fileHandle.SubSessionHandle() != KNullHandle)
				{
				//Open the file for read the path and create the file.
				TBuf8 <KMaxFileName> readBuffer;
				User::LeaveIfError(fileHandle.Read(readBuffer));
				RFile newFile;
				TBuf <KMaxFileName> fileName;
				fileName.Copy(readBuffer);	
				_LIT(KFormat, "Creating %S");
				RDebug::Print(KFormat, &fileName);
				
				RFs fs;
				User::LeaveIfError(fs.Connect());
				CleanupClosePushL(fs);			
				User::LeaveIfError(newFile.Create(fs,fileName,EFileStreamText|EFileWrite|EFileShareExclusive));
				newFile.Close();
				CleanupStack::PopAndDestroy(&fs);
				}
			else
				{
				TFileName name=aCommandLine.DocumentName();
				// No file handle, launch by document name
				ProcessCommandParametersL(aCommandLine.Command(), name, aCommandLine.TailEnd());
				}
			CleanupStack::PopAndDestroy(&fileHandle); // fileHandle
			
			}
			Exit();
		break;	
		}
	return ETrue;	
	}
	
	
CMimeDocument::CMimeDocument(CMimeApplication& aApp)
		: CEikDocument(aApp)
	{
	__DECLARE_NAME(_S("CMimeDocument"));
	}
	
CEikAppUi* CMimeDocument::CreateAppUiL()
	{
	RDebug::Printf("CMimeDocument::CreateAppUiL()");	
    return(new(ELeave) CMimeAppUi);
	} 


	
// ===========================================================================
// CMimeApplication
// Main application class
// ===========================================================================
	
TUid CMimeApplication::AppDllUid() const
	{
	RDebug::Printf("CMimeApplication::AppDllUid()");	
	const TUid KUidMimeApp={KUidMimeAppValue};
	return(KUidMimeApp);
	}
	
CApaDocument* CMimeApplication::CreateDocumentL()
	{
	RDebug::Printf("CMimeApplication::CreateDocumentL()");	
	return(new(ELeave) CMimeDocument(*this));
	}
	

// ===========================================================================
// Main EPOC app startup
// ===========================================================================

EXPORT_C CApaApplication* NewApplication()
	{
	RDebug::Printf("NewApplication()");	
	return(new CMimeApplication);
	}

GLDEF_C TInt E32Main()
	{
	RDebug::Printf("E32Main()");	
	return EikStart::RunApplication(NewApplication);
	}
