/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef R_NCDDATABASEITEMS_H
#define R_NCDDATABASEITEMS_H


#include <bamdesca.h>
#include "ncdpanics.h"
#include "catalogsdebug.h"

/**
 * Contains several database UIDs of one type
 */
class RNcdDatabaseItems
    {
public:

    RNcdDatabaseItems( TInt aType, MDesCArray* aUids ) :
        iType( aType ), 
        iUids( aUids )
        {        
        NCD_ASSERT_ALWAYS( aUids, ENcdPanicInvalidArgument );
        } 
            
    void Close() 
        {
        delete iUids;
        iUids = NULL;
        }
                
    const MDesCArray& Uids() const 
        {
        return *iUids;
        }
        
public:
    TInt iType;
    MDesCArray* iUids;    
    
    };

#endif // R_NCDUNREMOVABLEDATABASEITEM_H
