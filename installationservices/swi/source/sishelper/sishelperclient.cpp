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


#include <s32mem.h>
#include <caf/caf.h>
#include <bautils.h>
#include "sishelperclient.h"
#include "log.h"

using namespace Swi;

//
// RSisHelper
//

EXPORT_C TInt RSisHelper::Connect()
	{
	return CreateSession(KSisHelperServerName, 
		TVersion(KSisHelperVersionMajor, 
			KSisHelperVersionMinor, 
			KSisHelperVersionBuild));
	}

EXPORT_C void RSisHelper::Close()
	{
	RSessionBase::Close();
	}

// Request SISController data from SISHelper
EXPORT_C HBufC8* RSisHelper::SisControllerLC()
	{
	// Allocate a buffer
	const TInt KDefaultControllerBuffer=1024; // 1KB
	HBufC8* controllerData=HBufC8::NewMaxLC(KDefaultControllerBuffer);
	TPtr8 des=controllerData->Des();
	
	TInt err=SendReceive(ESisHelperGetController, TIpcArgs(&des));
	if (err == KErrOverflow)
		{
		// is there enough space for an integer?
		if(des.Length() != sizeof(TInt))
		   {
			User::Leave(KErrCorrupt);				
		   }
		
		// Get needed size
		TInt neededSize=0;
		TPckg<TInt> neededSizePackage(neededSize);
		neededSizePackage.Copy(des);
		
		// Re-allocate buffer
		CleanupStack::PopAndDestroy(controllerData);
		controllerData=HBufC8::NewMaxLC(neededSize);
		TPtr8 neededDes=controllerData->Des();
		
		// Rerequest
		err=SendReceive(ESisHelperGetController, TIpcArgs(&neededDes));
		}

	User::LeaveIfError(err);
	return controllerData;
	}

EXPORT_C TInt RSisHelper::ExtractFileL(RFs& /*aFs*/, RFile& aFile, TInt aFileIndex,
	TInt aDataUnit, RUiHandler& /*aUiHandler*/)
	{		
	TIpcArgs args;
	aFile.TransferToServer(args, 0, 1);
	args.Set(2, aFileIndex);
	args.Set(3, aDataUnit);
	return SendReceive(ESisHelperExtractFile, args);
	}

void RSisHelper::SetupAsyncExtractionL(RFs& aFs, RFile& aFile, TInt aFileIndex,TInt aDataUnit)
	{
	User::LeaveIfError(SendReceive(ESisHelperSetupAsyncExtraction, TIpcArgs(aFs, aFile.SubSessionHandle(), aFileIndex, aDataUnit)));
	}

void RSisHelper::AsyncExtractionL(TInt64 length, TRequestStatus& aStatus)
	{
	SendReceive(ESisHelperAsyncExtraction, TIpcArgs(I64HIGH(length), I64LOW(length)), aStatus);
	}

void RSisHelper::EndAsyncExtractionL()
	{
	User::LeaveIfError(SendReceive(ESisHelperEndAsyncExtraction, TIpcArgs()));
	}

EXPORT_C void RSisHelper::FillDrivesAndSpacesL(RArray<TChar>& aDriveLetters, 
	RArray<TInt64>& aDriveSpaces)
	{
	// calculate the likely size of the data transfer buffer
	const TInt KMaxBufSize=
		sizeof(TInt)+                 // number of entries
		KMaxDrives*sizeof(TUint)+  // drive letters stored as TUints
		KMaxDrives*sizeof(TInt64); // drive free spaces
	
	// allocate buffer for the returned arrays
	HBufC8* buf=HBufC8::NewMaxLC(KMaxBufSize);
	TPtr8 pBuf=buf->Des();
	User::LeaveIfError(SendReceive(ESisHelperFillDrivesAndSpaces, 
		TIpcArgs(&pBuf)));
	
	// got the buffer, internalise the arrays
	RDesReadStream ins(*buf);
	CleanupClosePushL(ins);

	// first comes the number of entries (TInt)
	TInt count=ins.ReadInt32L();
	
	// then drive letters
	TInt i;
	for (i = 0; i < count; ++i)
		{
		TUint chVal=ins.ReadUint32L();
		TChar ch(chVal);
		aDriveLetters.AppendL(ch);
		}
	
	// then drive spaces
	for (i = 0; i < count; ++i)
		{
		TInt32 l=ins.ReadInt32L();
		TInt32 h=ins.ReadInt32L();
		TInt64 space=MAKE_TINT64(h, l);
		aDriveSpaces.AppendL(space);
		}
	
	// cleanup
	CleanupStack::PopAndDestroy(2, buf); // ins, buf
	}

EXPORT_C void RSisHelper::OpenDrmContentL(ContentAccess::TIntent aIntent)
	{
	TInt intent = static_cast<TInt>(aIntent);
	User::LeaveIfError(SendReceive(ESisHelperOpenDrmContent, TIpcArgs(intent)));
	}
	
EXPORT_C void RSisHelper::ExecuteDrmIntentL(ContentAccess::TIntent aIntent)
	{
	TInt intent = static_cast<TInt>(aIntent);
	User::LeaveIfError(SendReceive(ESisHelperExecuteDrmIntent, TIpcArgs(intent)));
	}

TInt RSisHelper::CreateSisStub(RFile& aFile)
	{
	TIpcArgs ipcArgs;
	aFile.TransferToServer(ipcArgs, 0, 1);
	return SendReceive(ESisHelperCreateSisStub, ipcArgs);
	}
	
TBool RSisHelper::IsStubL()
	{
	TBool stub = EFalse;
	TPckg<TBool> stubPckg(stub);
	User::LeaveIfError(SendReceive(ESisHelperIsStub, TIpcArgs(&stubPckg)));
	return stub;
	}

TChar RSisHelper::GetSisFileDriveL()
	{
	TChar drive(0);
	TPckg<TChar> drivePckg(drive);
	User::LeaveIfError(SendReceive(ESisHelperGetSisFileDrive, TIpcArgs(&drivePckg)));
	return drive;
	}

TBool RSisHelper::IsSisFileReadOnlyL()
	{
	TBool readOnly = EFalse;
	TPckg<TBool> readOnlyPckg(readOnly);
	User::LeaveIfError(SendReceive(ESisHelperIsSisFileReadOnly, TIpcArgs(&readOnlyPckg)));
	return readOnly;
	}

// Request SIS file name from SISHelper
void RSisHelper::GetSisFileNameL(TDes& aFileName)
	{
	TInt err = SendReceive(ESisHelperGetSisFileName, TIpcArgs(&aFileName));
	User::LeaveIfError(err);
	}

// Retrieve Controller from the specified SIS file
EXPORT_C HBufC8* RSisHelper::GetControllerFromSisFileLC(const TDesC& aFileName)
	{
	// Open the file and pass the handle securely to SisHelper since
	// it does not have the capability to read files from private folders
	// of other processes
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.ShareProtected());
	RFile file;
	User::LeaveIfError(file.Open(fs, aFileName, EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(file);

	// Allocate a buffer
	const TInt KDefaultControllerBuffer = 1024; // 1KB
	HBufC8* controllerData = HBufC8::NewMaxLC(KDefaultControllerBuffer);
	TPtr8 ptr(controllerData->Des());

	// Store the RFs handle in message slot 1 and the RFile handle in slot 2
	TIpcArgs ipcArgs;
	User::LeaveIfError(file.TransferToServer(ipcArgs, 1, 2));

	ipcArgs.Set(0, &ptr);
	TInt err = SendReceive(ESisHelperGetControllerFromSis, ipcArgs);
	if (err == KErrOverflow)
		{
		// is there enough space for an integer?
		if(ptr.Length() != sizeof(TInt))
		   {
			User::Leave(KErrCorrupt);				
		   }
		
		// Get needed size
		TInt neededSize = 0;
		TPckg<TInt> neededSizePackage(neededSize);
		neededSizePackage.Copy(ptr);

		// Re-allocate buffer
		CleanupStack::PopAndDestroy(controllerData);
		controllerData = HBufC8::NewMaxLC(neededSize);
		TPtr8 neededPtr(controllerData->Des());

		// Rerequest
		User::LeaveIfError(file.TransferToServer(ipcArgs, 1, 2));
		ipcArgs.Set(0, &neededPtr);
		err = SendReceive(ESisHelperGetControllerFromSis, ipcArgs);
		}

	User::LeaveIfError(err);

	CleanupStack::Pop(controllerData);
	CleanupStack::PopAndDestroy(2, &fs);
	CleanupStack::PushL(controllerData);
	return controllerData;
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
TBool RSisHelper::IsDrmProtectedL()
	{
	TPckgBuf<TBool> isContentProtected;
	TInt returnCode = SendReceive(ESisHelperIsDrmProtected, TIpcArgs(&isContentProtected));
	User::LeaveIfError(returnCode);
		
	return isContentProtected();
	}
#endif
void RSisHelper::GetEquivalentLanguageListL(TLanguage aLangId,RArray<TLanguage>& aEquivLangs)
	{
	// calculate the likely size of the data transfer buffer
	const TInt KMaxBufSize=
		sizeof(TInt)+                 // number of entries
		(KMaxDowngradeLanguages+1)*sizeof(TLanguage);  // Languages IDs stored as TLanguage
	
	// allocate buffer for the returned arrays
	HBufC8* buf=HBufC8::NewMaxLC(KMaxBufSize);
	TPtr8 pBuf=buf->Des();
	User::LeaveIfError(SendReceive(ESisHelperGetEquivalentLanguages, 
		TIpcArgs(&pBuf,aLangId)));
	
	// got the buffer, internalise the arrays
	RDesReadStream ins(*buf);
	CleanupClosePushL(ins);

	// first comes the number of entries (TInt)
	TInt count=ins.ReadInt32L();
	
	// then language ID's
	TInt i;
	for (i = 0; i < count; ++i)
		{
		TLanguage langId=(TLanguage)ins.ReadInt32L();
		aEquivLangs.AppendL(langId);
		}
	// cleanup
	CleanupStack::PopAndDestroy(2, buf); // buf
	}
