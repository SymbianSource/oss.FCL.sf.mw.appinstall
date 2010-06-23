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
* SCR Accessor - client interface
*
*/


/**
 @file 
 @test
 @internalComponent
*/

#ifndef TSCRACCESSOR_CLIENT_H
#define TSCRACCESSOR_CLIENT_H

#include <e32std.h>
#include <usif/scr/scr.h>


class RScrAccessor : public RSessionBase
	{
public:
	enum TAccessorOperationType
		{
		EAddNonLocalizedSoftwareType,
		EAddLocalizedSoftwareType,
		EAddSofwtareTypeWithoutMimeTypes,
		EAddLocalizedSoftwareTypeWithDifferentPluginUid,
		EAddLocalizedSoftwareTypeWithExtraName,
		EAddLocalizedSoftwareTypeWithMissingName,
		EAddLocalizedSoftwareTypeWithExtraMime,
		EAddLocalizedSoftwareTypeWithMissingMime,
		EAddMultipleSidWithLauncherExecutable
		};
	
	enum TAccessorOperationResult
		{
		EOpSucessful,
		EOpFailSwTypeNotFound,
		EOpFailUnexpectedPluginUid,
		EOpFailUnexpectedMimeTypeNum,
		EOpFailReturnedUnexpectedMimeType,
		EOpFailSwTypeStillExists
		};
public: 
	TInt Connect();

	/**
	 	Adds a new software type name to the SCR database. 
	 	The properties of the software type are chosen by the server.
		
		@param aOpType The type of the software type adding operation. For example, add a software type with localized names.
		@param aOpResult Specific information about the result of the operation.
		@param aSpentTimeInMillisecond The time spent to perform this operation.
	 */
	void AddSoftwareTypeL(TAccessorOperationType aOpType, TAccessorOperationResult& aOpResult, TInt& aSpentTimeInMillisecond);
	
	/**
		Deletes a sofware type name entry and all its mime types from the registry.
		The server decides which software type will be deleted.
		
		@param aOpResult Specific information about the result of the operation.				
		@param aSpentTimeInMillisecond The time spent to perform this operation.
	 */
	void DeleteSoftwareTypeL(TAccessorOperationResult& aOpResult, TInt& aSpentTimeInMillisecond);
	
	/**
		Deletes a given file from the system.
		This function is intended to mainly delete TCB files.
		
		@param aFilePath The full path of the file being deleted.
		@return KErrNone if successful, otherwise one of the other system-wide error codes.
	 */
	TInt DeleteFile(const TDesC& aFilePath);
	
	/**
		Copies a given file into a destination file.
		This function is intended to mainly copy TCB files.
			
		@param aSourceFilePath Path indicating the file to be copied.
		@param aDestinationFilePath Path indicating the directory into which the file is to be copied.
		@return KErrNone if successful, otherwise one of the other system-wide error codes.
	 */
	TInt CopyFile(const TDesC& aSourceFilePath, const TDesC& aDestinationFilePath);
	};

#endif	// TSCRACCESSOR_CLIENT_H
