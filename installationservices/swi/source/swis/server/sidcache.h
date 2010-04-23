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


#ifndef __SIDCACHE_H__
#define __SIDCACHE_H__

/**
@file
@internalComponent
@released
*/

#include <e32base.h>
#include <f32file.h>

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <usif/sts/sts.h>
#else
#include "integrityservices.h"
#endif

class RWriteStream;
class RReadStream;
class RFs;

namespace Swi
	{
	
	class CSidCache : public CBase
		{
		
	public:
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		static CSidCache* NewL(Usif::RStsSession& aStsSession);
		static CSidCache* NewLC(Usif::RStsSession& aStsSession);
#else
		static CSidCache* NewL(CIntegrityServices& aIntegrityServices);
		static CSidCache* NewLC(CIntegrityServices& aIntegrityServices);
#endif
		
		TBool IsCachedL(TUid& aSid);
		
		~CSidCache();
		
	private:
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		void ConstructL(Usif::RStsSession& aStsSession);
#else
		void ConstructL(CIntegrityServices& aIntegrityServices);
#endif
		
		void ExternalizeL(RWriteStream& aStream);
		void InternalizeL(RReadStream& aStream);
		
		void ScanFileSystemL(RFs& aFs);
		
		/**
		Finds all subdirectories of the directories specified in aSearchDirs and
		appends the fully qualified paths to aSearchDirs; therefore, at least one
		starting directory must be specified.
		
		@param aSearchDirs The array of directories to expand.
		@param aRFs the file server session to use.
		*/
		void FindDirsL(RFs& aFs, RPointerArray<HBufC>& aSearchDirs);
		
	private:
		RArray<TUid> iSidArray;
		TDriveNumber iSystemDrive;		
		};
	
	}

#endif /*__SIDCACHE_H__*/

