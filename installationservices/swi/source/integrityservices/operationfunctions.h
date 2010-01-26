/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __OPERATIONFUNCTIONS_H__
#define __OPERATIONFUNCTIONS_H__

#include <e32base.h>
#include <f32file.h>
#include <e32ldr_private.h>

namespace Swi
{
class CIntegrityTreeLeaf;

void IntegrityDeleteFileL(const TDesC& aPath, CIntegrityTreeLeaf* aLeaf, RFs& aFs, RLoader& aLoader, 
						  CFileMan& aFileMan);
void IntegrityRestoreFileL(const TDesC& aPath, CIntegrityTreeLeaf* aLeaf, RFs& aFs, RLoader& aLoader, 
	 					   CFileMan& aFileMan);
void RemoveDirectoryTreeL(RFs& aFs, const TDesC& aFileName);

} // namespace Swi

#endif /* __OPERATIONFUNCTIONS_H__ */
