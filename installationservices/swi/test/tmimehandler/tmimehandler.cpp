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


#include "tmimehandler.h"
#include <apacmdln.h>

const TInt KUidMimeAppValue=0x1021234d ; 

void CMimeAppUi::DoExitL()
	{
	
	CVwsSessionWrapper* ses=CVwsSessionWrapper::NewLC();
	ses->EnableServerEventTimeOut(ETrue);
	CleanupStack::PopAndDestroy();
	Exit();
	}

CMimeAppUi::CMimeAppUi()
	{
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
			User::LeaveIfError(file.Read(readBuffer));
			file.Close();
			HBufC* buf1 = HBufC::NewLC(KMaxFileName);
			TPtr tptr = buf1->Des();
			tptr.Copy(readBuffer);
			TInt pos = tptr.Locate(' ');
			TInt count1 = tptr.Length();
			TInt count2 = count1-pos;
			tptr.Delete(pos,count2);
			User::LeaveIfError(file.Replace(fs,tptr,EFileWrite));
			CleanupStack::PopAndDestroy(2, &fs);
			}
		break;	   
		}
		
	return ETrue;	
	}
	
TBool CMimeAppUi::ProcessCommandParametersL(CApaCommandLine& aCommandLine)
	{
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
				HBufC8* buf1 = HBufC8::NewLC(KMaxFileName);
				TPtr8 tptr = buf1->Des();
				fileHandle.Read(tptr);
				TInt pos = tptr.Locate(' ');
				TInt count1 = tptr.Length();
				TInt count2 = count1-pos;
				tptr.Delete(pos,count2);
				RFs fs;
				User::LeaveIfError(fs.Connect());
				CleanupClosePushL(fs);
				RFile file;
				CleanupClosePushL(file);
				TBuf <KMaxFileName> fileName;
				fileName.Copy(tptr);				
				User::LeaveIfError(file.Replace(fs,fileName,EFileWrite));
				CleanupStack::PopAndDestroy(3, buf1); // file, fs, buf1
				}
			else
				{
				TFileName name=aCommandLine.DocumentName();
				// No file handle, launch by document name
				ProcessCommandParametersL(aCommandLine.Command(), name, aCommandLine.TailEnd());
				}
			CleanupStack::PopAndDestroy(&fileHandle); // fileHandle
			
			Exit();
			}
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
    return(new(ELeave) CMimeAppUi);
	} 


	
// ===========================================================================
// CMimeApplication
// Main application class
// ===========================================================================
	
TUid CMimeApplication::AppDllUid() const
	{
	const TUid KUidMimeApp={KUidMimeAppValue};
	return(KUidMimeApp);
	}
	
CApaDocument* CMimeApplication::CreateDocumentL()
	{
	return(new(ELeave) CMimeDocument(*this));
	}
	

// ===========================================================================
// Main EPOC app startup
// ===========================================================================

EXPORT_C CApaApplication* NewApplication()
	{
	return(new CMimeApplication);
	}

GLDEF_C TInt E32Main()
	{
	// Wait 1 second on startup for the benefit of synchronous filerun tests.
	// Some tests depend on tmimehandler not completing before an install
	// test step finishes.
	User::After(1000000);
	return EikStart::RunApplication(NewApplication);
	}
