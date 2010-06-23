/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* swiutils.h
*/

/**
 @file 
 @publishedAll
 @released
*/


#ifndef __SWIUTILS_H__
#define __SWIUTILS_H__

#include <e32def.h> 
#include <e32cmn.h> 

namespace Swi
    {
    const TInt KMaxUidCount = 16; //One more than the maximum number of Uids that the array, publishing the Uids, holds
    
    /**
     * Returns an array of package Uids published by the installer.
     * @param aUidList Array which holds a list of Uids that is published.
     * @return KErrNone in case of success otherwise one of the system-wide error codes.
     */
    IMPORT_C TInt GetAllUids(RArray<TUid>& aUidList);
    } //end of namespace SWI

#endif
