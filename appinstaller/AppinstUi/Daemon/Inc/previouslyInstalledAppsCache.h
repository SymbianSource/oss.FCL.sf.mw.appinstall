/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _PREVIOUSLYINSTALLEDAPPSCACHE_H
#define _PREVIOUSLYINSTALLEDAPPSCACHE_H
#include <e32base.h>
#include <e32cmn.h>

// @released
// @publishedPartner

//class RFs;
// It is assumed there is only ever ONE instance of this class
class CPreviouslyInstalledAppsCache : public CBase
	{
public:
	static CPreviouslyInstalledAppsCache *NewL();
	~CPreviouslyInstalledAppsCache();

	/**
	 * Read the cache from disk 
	 *
	 * nb. Cache file absense is not counted as an error.
	 */
	void InitFromCacheFileL();

	/**
	 * Query the SIS registry and make sure ALL listed packages (not
	 * augmentations) are in our cache. Does not flush to disk
	 *
	 * This needs ReadUserData
	 */
	void UpdateAllL();

	/**
	 * Query the SIS registry to check if the specified package is
	 * registered add it to the cache.  Does not flush to disk
	 *
	 * This does not need any capabilities.
	 */
	void UpdateAddL(TUid aUid);

	
	/**
	 * Query if the specified package has ever been cached as installed.
	 *
	 * @param UID of package to query
	 * @retval TBool ETrue if previously cached as installed
	 */
	TBool HasBeenPreviouslyInstalled(TUid aPackageId) const;

	/**
	 * Flush cache to disk.
	 */
	void FlushToDiskL();
private:
	CPreviouslyInstalledAppsCache();
	void ConstructL();
	RArray<TUid> iPrevPkgUids;
	RFs iFs;
	};

#endif
