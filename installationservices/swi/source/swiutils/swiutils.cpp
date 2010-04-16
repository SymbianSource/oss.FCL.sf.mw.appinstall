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
* swiutils.cpp
*/

#include <e32def.h>
#include <e32cmn.h>
#include <e32property.h> 
#include <sacls.h>
#include "swiutils.h"
#include "log.h"

namespace Swi{

EXPORT_C TInt GetAllUids(RArray<TUid>& aUidList)
    {
    aUidList.Close();
    
    TBuf<KMaxUidCount*sizeof(TUid)> buf;
    TInt err = RProperty::Get(KUidSystemCategory, KSWIUidsCurrentlyBeingProcessed, buf);
    if(err != KErrNone)
        {
        DEBUG_PRINTF2(_L("RProperty::Get returned with err %d."), err);
        return err;
        }
    
	TInt count = 0;
    TInt *bufPtr = (TInt*) buf.Ptr();
    count = *bufPtr++;

    TUid uid;    
    for(TInt i = 0; i < count; ++i)
        {
        uid.iUid = *bufPtr++;
        aUidList.Append(uid);
        }

    return KErrNone;
    }
}
