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
* CScrRepository implementation.
*
*/

#include "scrrepository.h"
using namespace Usif;

//Initialize static variables
bool CScrRepository::iInstanceFlag = EFalse;
CScrRepository* CScrRepository::iInstance = NULL;
CRepository* CScrRepository::iRepository = NULL;

CScrRepository* CScrRepository :: GetRepositoryInstanceL()
    {   
    if(!iInstanceFlag)
        {        
        iInstance = new (ELeave)CScrRepository;
        iInstance->ConstructL();
        iInstanceFlag = ETrue;
        }
    return iInstance;
    }

CScrRepository :: ~CScrRepository()
    {
    delete iRepository;    
    }


void CScrRepository :: DeleteRepositoryInstance()
    {
    if(iInstanceFlag)
        {
        delete iInstance;
        iInstanceFlag = EFalse;
        }
        
    }
void CScrRepository :: ConstructL()
    {
    iRepository = CRepository::NewL(KUidScrServerRepository);
    }



TInt CScrRepository :: AppUidRangeCountL()
    {  
    //Read the range count value from the cenrep file.
    TInt rangeCount(0);
    User::LeaveIfError(iRepository->Get(KScrAppUidRangeCount, rangeCount));
    
    //If the range count is not set.
    if(rangeCount <= 0)
        {   
        User::Leave(KErrArgument);
        }
    
    return rangeCount;    
    }


void CScrRepository :: GetAppUidRangeL(TInt aNum, TUid& aRangeBegin, TUid& aRangeEnd)
    {
    TInt rangeBegin(0);
    TInt rangeEnd(0);
    User::LeaveIfError(iRepository->Get(KScrAppUidRanges+((2*aNum)-1), rangeBegin));
    User::LeaveIfError(iRepository->Get(KScrAppUidRanges+(2*aNum), rangeEnd));
    
    aRangeBegin = TUid::Uid(rangeBegin);
    aRangeEnd = TUid::Uid(rangeEnd);
    }


