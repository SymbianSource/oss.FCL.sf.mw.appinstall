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


#include <s32strm.h>


inline TBool TCatalogsConnectionMethod::operator==( 
    const TCatalogsConnectionMethod& aMethod ) const
    {
    return iId == aMethod.iId && 
           iType == aMethod.iType &&
           iApnId == aMethod.iApnId;
    }    

inline TBool TCatalogsConnectionMethod::operator!=( 
    const TCatalogsConnectionMethod& aMethod ) const
    {
    return !operator==( aMethod );
           
    }    


inline TBool TCatalogsConnectionMethod::Match( 
    const TCatalogsConnectionMethod& aMethod ) const
    {
    return iId == aMethod.iId && 
           iType == aMethod.iType;
    }
    
    
inline TUint32 TCatalogsConnectionMethod::CurrentApnId() const
    {
    if ( iApnId ) 
        {
        return iApnId;
        }
    return iId;
    
    }
    
inline void TCatalogsConnectionMethod::ExternalizeL(     
    RWriteStream& aStream ) const
    {
    aStream.WriteUint32L( iId );
    aStream.WriteInt8L( iType );
    aStream.WriteUint32L( iApnId );
    }
    
    
inline void TCatalogsConnectionMethod::InternalizeL( 
    RReadStream& aStream )
    {
    TCatalogsConnectionMethod method;
    method.iId = aStream.ReadUint32L();
    method.iType = static_cast<TCatalogsConnectionMethodType>( 
        aStream.ReadInt8L() );
    method.iApnId = aStream.ReadUint32L();
    
    *this = method;
    }
