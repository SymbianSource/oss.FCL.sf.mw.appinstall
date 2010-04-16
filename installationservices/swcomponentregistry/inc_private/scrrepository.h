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
* SCR Repository definition.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SCRREPOSITORY_H
#define SCRREPOSITORY_H

#include <centralrepository.h>

namespace Usif {

/** The UID of the SCR repository. */
const TUid KUidScrServerRepository = {0x10285bc0};  //271080384

/** The key for the AppUid range count. */
const TUint KScrAppUidRangeCount = {0x00000000};

/** The key for the AppUid ranges. */
const TUint KScrAppUidRanges = {0x00000100};

/** Singleton class controlling access to the repository. */
class CScrRepository : public CBase
    {
public:

    static CScrRepository* GetRepositoryInstanceL();

    TInt AppUidRangeCountL();

    void GetAppUidRangeL(TInt aNum, TUid& aRangeBegin, TUid& aRangeEnd);

    static void DeleteRepositoryInstance();
    
private:
    static void ConstructL();
    ~CScrRepository();
    
private:
    static CScrRepository* iInstance;
    static CRepository* iRepository;
    static bool iInstanceFlag;
    };
}

#endif /* SCRREPOSITORY_H */
