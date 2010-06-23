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
* fileextractor.cpp
* Component which handles extracting of files asynchronously
*
*/


/**
 @file
 @internalTechnology
*/

#ifndef __FILEEXTRACTOR_H__
#define __FILEEXTRACTOR_H__
 
#include <e32base.h>

#include "appinfo.h"

class RFs;
class RFile;

namespace Swi
{
class CSisRegistryFileDescription;
class RSisHelper;
class RUiHandler;

/**
 * This class is an active object, which manages file extraction in chunks, using SISHelper
 */
class CFileExtractor : public CActive
	{
public:
	static CFileExtractor* NewLC(RFs& aFs, RSisHelper& aSisHelper, RUiHandler& aUiHandler, const TAppInfo& aAppInfo);
	
	static CFileExtractor* NewL(RFs& aFs, RSisHelper& aSisHelper, RUiHandler& aUiHandler, const TAppInfo& aAppInfo);
	
	/**
	 * Extract the file, using provided file handle which must be set up correctly.
	 */	
	void ExtractFileL(TInt aDataUnit, const CSisRegistryFileDescription& aFileDescription, RFile& aFile, TRequestStatus& aStatus);

	/**
	 * Extract the file, using provided file handle which must be set up correctly.
	 */	
	void ExtractFileL(TInt aDataUnit, const CSisRegistryFileDescription& aFileDescription, const TDesC& aFileName, TRequestStatus& aStatus);
		
	~CFileExtractor();
	
private:
	CFileExtractor(RFs& aFs, RSisHelper& aSisHelper, RUiHandler& aUiHandler, const TAppInfo& aAppInfo);	
	void ConstructL();
	
// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	

	/// Sets up the file handle, owned by the file extractor for the duration of the extraction
	void SetupFileHandleL(const TDesC& aFileName);

	/// finishes the extraction and notifies the client of the result
	TInt FinishExtraction(TInt aResult);	

private:	
	RFs&	iFs;
	RSisHelper& iSisHelper;
	RUiHandler& iUiHandler;
	const TAppInfo& iAppInfo;
	
	TRequestStatus* iClientStatus;

	TBool iCancelled;
	TInt iBytesToExtract;
	RFile* iCurrentFile;
	CSisRegistryFileDescription* iFileDescription;
	TInt32 iDataUnit;
	TBool iManagedFileHandle; // whether we manage the file handle used for file extraction.
	};
	
}

#endif // #ifndef __FILEEXTRACTOR_H__
