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
* This file defines the CCafSisDataProvider class, which is used to enable Secure Software Install
* to process DRM encrypted files.
*
*/


/**
 @file
 @internalTechnology
*/
#ifndef __CAFSISDATAPROVIDER_H
#define __CAFSISDATAPROVIDER_H

#include <e32std.h>

#include "sisdataprovider.h"

namespace ContentAccess
	{
	class CContent;
	class CData;
	}

namespace Swi
{
/**
 * CAF enabled version of CFileSisDataProvider. This class provides data to 
 * Software Install, and can be instantiated from either a file name, or a
 * file handle. It will handle DRM encrypted content through CAF, and evaluates
 * and executes DRM rights through CAF also.
 */
class CCafSisDataProvider : public CBase, public MSisDataProvider
	{
public:
	IMPORT_C static CCafSisDataProvider* NewL(const TFileName& aFileName);
	IMPORT_C static CCafSisDataProvider* NewLC(const TFileName& aFileName);

	IMPORT_C static CCafSisDataProvider* NewL(RFile& aFileHandle);
	IMPORT_C static CCafSisDataProvider* NewLC(RFile& aFileHandle);
	
	/** Destructor */
	IMPORT_C virtual ~CCafSisDataProvider();

// From MSisDataProvider
	IMPORT_C virtual TInt Read(TDes8& aDes);
	
	IMPORT_C virtual TInt Read(TDes8& aDes, TInt aLength);
	
	IMPORT_C virtual TInt Seek(TSeek aMode, TInt64& aPos);

 	IMPORT_C virtual void OpenDrmContentL(ContentAccess::TIntent aIntent);
	
	IMPORT_C virtual TInt ExecuteDrmIntent(ContentAccess::TIntent aIntent);
	/**
	* Gives the DRM protection status of the package
	* @return - ETrue, if the content's EIsProtected attribute is TRUE. EFalse, otherwise.
	*/
	IMPORT_C TBool IsContentProtected();

private:
	/** Constructor */
	CCafSisDataProvider();

	/** Second phase constructor */
	void ConstructL(const TFileName& aFileName);

	/** Second phase constructor */
	void ConstructL(RFile& aFileHandle);
	
	ContentAccess::CData& Data() const;

	ContentAccess::CContent* iContent;
	ContentAccess::CData* iData;
	};

} // namespace Swi

#endif // #ifndef __CAFSISDATAPROVIDER_H
