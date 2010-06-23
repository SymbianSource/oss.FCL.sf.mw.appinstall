/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include "ncdconnectionmethod.h"

#include <s32strm.h>

EXPORT_C void TNcdConnectionMethod::ExternalizeL(     
    RWriteStream& aStream ) const
    {
    aStream.WriteUint32L( iId );
    aStream.WriteInt8L( iType );
    }
    
EXPORT_C void TNcdConnectionMethod::InternalizeL( 
    RReadStream& aStream )
    {
    TNcdConnectionMethod method;
    method.iId = aStream.ReadUint32L();
    method.iType = static_cast<TNcdConnectionMethodType>( 
        aStream.ReadInt8L() );  
    *this = method;  
    }
