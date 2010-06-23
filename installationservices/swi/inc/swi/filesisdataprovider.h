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
* Definition of the CFileSisDataProvider
*
*/




/**
 @file
 @internalTechnology
*/
 
#ifndef __FILESISDATAPROVIDER_H
#define __FILESISDATAPROVIDER_H

#include <e32std.h>

#include "sisdataprovider.h"

class RFs;
class RFile;

namespace Swi
{

class CFileSisDataProvider : public CBase, public MSisDataProvider
	{
public:
	IMPORT_C static CFileSisDataProvider* NewL(RFs& aFs, 
		const TFileName& aFileName, 
		TInt aFileMode = EFileRead | EFileStream | EFileShareReadersOnly);
	IMPORT_C static CFileSisDataProvider* NewLC(RFs& aFs, 
		const TFileName& aFileName, 
		TInt aFileMode = EFileRead | EFileStream | EFileShareReadersOnly);

	IMPORT_C static CFileSisDataProvider* NewL(RFile& aFileHandle);

	IMPORT_C static CFileSisDataProvider* NewLC(RFile& aFileHandle);


// From MSisDataProvider
	IMPORT_C virtual TInt Read(TDes8& aDes);
	
	IMPORT_C virtual TInt Read(TDes8& aDes, TInt aLength);
	
	IMPORT_C virtual TInt Seek(TSeek aMode, TInt64& aPos);

	/** Destructor */
	IMPORT_C virtual ~CFileSisDataProvider();

 private:
	/** Constructor */
	CFileSisDataProvider(RFs& aFs);

	/** Constructor */
	CFileSisDataProvider(RFile& aFileHandle);

	/** Second phase constructor */
	void ConstructL(const TFileName& aFileName, TInt aFileMode);

	/** File to read the data from */
	RFile iFile;

	/** File session handle */
	RFs iFs;
	
	/** Whether to close the file handle after we've finished **/
	TBool iCloseHandle;
	};

} // namespace Swi

#endif // #ifndef __FILESISDATAPROVIDER_H
