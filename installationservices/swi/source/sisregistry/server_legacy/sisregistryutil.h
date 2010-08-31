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
* SisRegistryUtil a utility class used by variety classes 
* implemented as static functions
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#ifndef __SISREGISTRYUTIL_H__
#define __SISREGISTRYUTIL_H__

#include <f32file.h>
#include "hashcontainer.h"

namespace Swi
{

static TBool operator==(const CHashContainer& a, const CHashContainer& b);
			
class SisRegistryUtil
	{
public:
	static TInt NextAvailableIndexL(RFs& aFs, TUid aUid);
	static TBool FileExistsL(RFs& aFs, const TDesC& aFullName);	
	static HBufC* BuildEntryFileNameLC(TUid aUid, TInt aIndex);
 	static HBufC* BuildUninstallLogPathLC(TUid aUid, TInt aIndex, TChar aDriveLetter);
 	static HBufC* BuildControllerFileNameLC(TUid aUid, TInt aIndex, TInt aController);
 	static HBufC* BuildControllerFileNameLC(TUid aUid, TInt aIndex, TInt aController, TInt aDrive);
	static void EnsureDirExistsL(RFs& aFs, const TDesC& aPath);
	static TBool RomBasedPackageL(const TUid& aUid);
	static HBufC* BuildLogFileNameLC();
	
private:
	static TBool CheckIfMatchingStubPUidL(const TUid& aUid, RFs& aFileServer, const TFileName& aStubFileName);
	};	

inline TBool operator==(const CHashContainer& a, const CHashContainer& b)
	{
	return (a.Algorithm() == b.Algorithm()) && (a.Data() == b.Data());
	}	
	
inline TDriveNumber SystemRomDrive()
	{
	return EDriveZ;
	}

			
} //namespace
#endif //__SISREGISTRYUTIL_H__
