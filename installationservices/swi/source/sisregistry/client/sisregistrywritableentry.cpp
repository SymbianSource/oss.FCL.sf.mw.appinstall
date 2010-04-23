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
* sisregistry - client interface implementation
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#include<s32mem.h>
#include "sisregistrydependency.h"
#include "sisregistrywritableentry.h"
#include "sisregistryclientserver.h"
#include "sisregistrypackage.h"
#include "sisregistryfiledescription.h"
#include "arrayutils.h"

using namespace Swi;


EXPORT_C TChar RSisRegistryWritableEntry::SelectedDriveL()
	{
	// Avoids code duplication.
	return RSisRegistryEntry::SelectedDriveL();
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
//
// An optimized version of API which improves performance when a large number of file descriptions are retrieved
//
EXPORT_C void RSisRegistryWritableEntry::FileDescriptionsL(RPointerArray<CSisRegistryFileDescription>& aFileDescriptions)
	{
	// First retrieve the number of files
	TPckgBuf<TInt> filesCount;
	TStubExtractionMode operationMode = EGetCount;
	TPckgC<TStubExtractionMode> packageMode(operationMode);
	TInt result = SendReceive(EFiles, TIpcArgs(&packageMode, &filesCount));
	User::LeaveIfError(result);	

	// Now retrieve the files descriptions in chunks
	TInt totalFilesCount = filesCount();

	HBufC8* buffer = HBufC8::NewLC(KDefaultBufferSize);
	TPtr8 pOutput(buffer->Des());
	TInt startingFile = 0;
	while (startingFile < totalFilesCount)
		{	
		// Fetch the files.	
		User::LeaveIfError(SendReceive(EFileDescriptions, TIpcArgs(&pOutput)));
		// Create a stream based on the buffer
		RDesReadStream stream(*buffer);
		CleanupClosePushL(stream);
		
		// Reassemble the array from the stream
		InternalizePointerArrayL(aFileDescriptions, stream);
		
		CleanupStack::PopAndDestroy(&stream);  //stream
		startingFile = aFileDescriptions.Count();	
		}
	
	CleanupStack::PopAndDestroy(buffer);
	}
#else
EXPORT_C void RSisRegistryWritableEntry::FileDescriptionsL(RPointerArray<CSisRegistryFileDescription>& aFileDescriptions)
	{
	HBufC8* buffer = SendReceiveBufferLC(EFileDescriptions);
	
	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// reassemble the array from the stream
	InternalizePointerArrayL(aFileDescriptions, stream);
	
	CleanupStack::PopAndDestroy(2, buffer); // buffer, stream
	}
#endif

EXPORT_C Sis::TInstallType RSisRegistryWritableEntry::InstallTypeL()
	{
	Sis::TInstallType installType;
	TPckg<Sis::TInstallType> packageInstallType(installType);
	
	User::LeaveIfError(SendReceive(EInstallType, TIpcArgs(&packageInstallType)));
	return installType;
	}

//
// API to retrieve the matching supported languages
//
EXPORT_C void RSisRegistryWritableEntry::GetMatchingSupportedLanguagesL(RArray<TLanguage>& aMatchingSupportedLanguages)
	{
	
	HBufC8* buffer = SendReceiveBufferLC(EGetMatchingSupportedLanguages);
	
	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	// reassemble the array from the stream
	InternalizeArrayL(aMatchingSupportedLanguages, stream);

	CleanupStack::PopAndDestroy(2, buffer);

	}

