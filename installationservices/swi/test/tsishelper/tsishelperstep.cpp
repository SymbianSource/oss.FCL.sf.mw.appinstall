/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file tsishelperstep.cpp
*/
#include "tsishelperstep.h"
#include <test/testexecutelog.h>
#include "sishelperclient.h"
#include "swi/launcher.h"

namespace Swi
{
class RUiHandler;
}

CTSISHelperStepController::CTSISHelperStepController()
	{
	SetTestStepName(KTSISHelperStepController);
	}

TVerdict CTSISHelperStepController::doTestStepL()
	{
	SetTestStepResult(EFail);
	TPtrC sisFileName;
	GetStringFromConfig(ConfigSection(),_L("SIS"), sisFileName);
	Swi::TSisHelperStartParams params(sisFileName);
	User::LeaveIfError(startSisHelper(params));
	Swi::RSisHelper server;
	User::LeaveIfError(server.Connect());
	server.OpenDrmContentL(ContentAccess::EExecute);
	HBufC8* controller = server.SisControllerLC();
	TPtrC expectedController;
	GetStringFromConfig(ConfigSection(),_L("ExpectedController"), expectedController);

	RFile file;
	User::LeaveIfError(file.Open(iFs, expectedController, EFileRead));
	TInt pos=0;
	User::LeaveIfError(file.Seek(ESeekEnd,pos));
	TInt expectedLength = pos;
	pos = 0;

	User::LeaveIfError(file.Seek(ESeekStart, pos));

	if (expectedLength == controller->Des().Length())
		{
		HBufC8* expectedBuffer = HBufC8::NewLC(expectedLength);
		TPtr8 expected(expectedBuffer->Des());
		file.Read(expected);
		if (expected.Compare(controller->Des()) == 0)
			{
			SetTestStepResult(EPass);
			}
		CleanupStack::PopAndDestroy(expectedBuffer);	
		}
	
	CleanupStack::PopAndDestroy(controller);
	file.Close();
	server.Close();
	return TestStepResult();
	}


CTSISHelperStepData::CTSISHelperStepData()
	{
	SetTestStepName(KTSISHelperStepData);
	}

TVerdict CTSISHelperStepData::doTestStepL()
	{
	SetTestStepResult(EFail);
		
	// Start the SISHelper
	TPtrC sisFileName;
	GetStringFromConfig(ConfigSection(),_L("SIS"), sisFileName);
	Swi::TSisHelperStartParams params(sisFileName);
	User::LeaveIfError(startSisHelper(params));
	Swi::RSisHelper server;
	User::LeaveIfError(server.Connect());
	server.OpenDrmContentL(ContentAccess::EExecute);

	TInt dataUnit = 0;
	GetIntFromConfig(ConfigSection(),_L("DataUnit"), dataUnit);
	TInt fileNumber = 0;
	GetIntFromConfig(ConfigSection(),_L("FileNumber"), fileNumber);
	
	// Create a file handle to pass
	RFs sharedFs;
	RFile sharedFile;
	User::LeaveIfError(sharedFs.Connect());
	User::LeaveIfError(sharedFs.ShareProtected());
	_LIT(KTempFileName,"\\tsishelper.temp");
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<64> tempFileName (sysDrive.Name());
	tempFileName.Append(KTempFileName);
	
	User::LeaveIfError(sharedFile.Replace (sharedFs, tempFileName, EFileWrite));

	TInt fakeUiHandler=0;
	Swi::RUiHandler& uiHandler=reinterpret_cast<Swi::RUiHandler&>(fakeUiHandler);
	User::LeaveIfError(server.ExtractFileL(sharedFs, sharedFile, fileNumber, dataUnit, uiHandler));
	sharedFile.Close();
	sharedFs.Close();
	
	TPtrC expectedData;
	GetStringFromConfig(ConfigSection(),_L("ExpectedData"), expectedData);
	RFile oldFile;
	User::LeaveIfError(oldFile.Open(iFs, expectedData, EFileRead));
	RFile newFile;
	User::LeaveIfError(newFile.Open(iFs, tempFileName, EFileRead));  // KErrBusy means SISHelper didn't close!
	
	TInt expectedLength(0);
	oldFile.Seek(ESeekEnd,expectedLength);
	TInt actualLength(0);
	newFile.Seek(ESeekEnd,actualLength);
	
	if (expectedLength == actualLength)
		{
		TInt pos = 0;
		oldFile.Seek(ESeekStart, pos);
		newFile.Seek(ESeekStart, pos);
		SetTestStepResult(EPass);
		for (TInt i=0; i<expectedLength; i++)
			{
			TInt8 oldChar;
			TInt8 newChar;
			TPckg<TInt8> oldPkg(oldChar);
			TPckg<TInt8> newPkg(newChar);
			oldFile.Read(oldPkg);
			newFile.Read(newPkg);
			if (oldChar != newChar)
				{
				SetTestStepResult(EFail);
				break;
				}
			}
		}	
	oldFile.Close();
	newFile.Close();
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	// Delete the temp file	
	TInt error = fs.Delete(tempFileName);
	if (error != KErrNone && error != KErrNotFound)
		{
		User::Leave(error);
		} 
	fs.Close(); 
	server.Close();
	return TestStepResult();
	}
