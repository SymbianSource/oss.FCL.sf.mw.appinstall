/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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


#ifndef _INSTALLATIONFAILEDAPPSCACHE_H
#define _INSTALLATIONFAILEDAPPSCACHE_H

#include <e32base.h>
#include <e32cmn.h>

// It is assumed there is only ever ONE instance of this class
class CInstallationFailedAppsCache : public CBase
	{
public:

	static CInstallationFailedAppsCache *NewL();
	
	~CInstallationFailedAppsCache();

	/**
	 * Read the cache from disk 
	 */
	void InitFromCacheFileL();
	
	/**
	 * Add new UID to cache.
	 */	
	void AddPackageUID( TUid aUid );  
	
	/**
	 * Check if given UID is found in cache.
	 */	
	TBool HasPreviousInstallationFailed( TUid aUid );

	/**
	 * Flush cache to disk. Create new file if file not found.
	 */
	void FlushToDiskL();
	
private:

	CInstallationFailedAppsCache();
	
	void ConstructL();
	
private:	
	
	RArray<TUid> iUidsArray; // Array containing UIDs	
	RFs          iFs;
	TBool        iNewUID; // Defines if new UID is added to array.
	};

#endif
