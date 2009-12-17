/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of CNcdServerAddress
*
*/


#include <e32math.h>

#include "ncdserveraddress.h"

#include "catalogsdebug.h"
#include "catalogsutils.h"
#include "ncdproviderdefines.h"

    
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
// 
CNcdServerAddress* CNcdServerAddress::NewL( const TDesC& aAddress, 
    const TInt64& aValidity )
    {
    CNcdServerAddress* self = CNcdServerAddress::NewLC( aAddress, aValidity );
    CleanupStack::Pop( self );
    return self;
    }
    
    
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//     
CNcdServerAddress* CNcdServerAddress::NewL( RReadStream& aStream )
    {
    CNcdServerAddress* self = new ( ELeave ) CNcdServerAddress( 0 );
    CleanupStack::PushL( self );
    self->InternalizeL( aStream );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
// 
CNcdServerAddress* CNcdServerAddress::NewLC( const TDesC& aAddress, 
    const TInt64& aValidity )
    {
    CNcdServerAddress* self = new( ELeave ) CNcdServerAddress( aValidity );
    CleanupStack::PushL( self );
    self->ConstructL( aAddress );        
    return self;        
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
// 
CNcdServerAddress::~CNcdServerAddress()
    {
    delete iAddress;
    }


// ---------------------------------------------------------------------------
// Address getter
// ---------------------------------------------------------------------------
// 
const TDesC& CNcdServerAddress::Address() const
    {
    return *iAddress;
    }
    
    
// ---------------------------------------------------------------------------
// Validity as TInt64
// ---------------------------------------------------------------------------
// 
TInt64 CNcdServerAddress::Validity() const
    {
    return iValidity;
    }
    
    
// ---------------------------------------------------------------------------
// Validity checker
// ---------------------------------------------------------------------------
// 
TBool CNcdServerAddress::IsValid() const
    {
    DLTRACEIN((""));
    if ( iUseValidity ) 
        {
        
        TTime validTime( iValidity );

        TTime currentTime;
        currentTime.HomeTime();
        DLTRACEOUT(("cur: %Li, validity: %Li, Is valid: %d", currentTime.Int64(),
            validTime.Int64(), (currentTime < validTime) ));
        return currentTime < validTime;
        }
    // Always valid if not using validity
    return ETrue;
    }


// ---------------------------------------------------------------------------
// Set use validity
// ---------------------------------------------------------------------------
// 
void CNcdServerAddress::SetUseValidity( TBool aUseValidity )
    {
    iUseValidity = aUseValidity;
    }
    
// ---------------------------------------------------------------------------
// ExternalizeL
// ---------------------------------------------------------------------------
// 
void CNcdServerAddress::ExternalizeL( RWriteStream& aStream )
    {
    ExternalizeDesL( *iAddress, aStream );
    aStream << iValidity;    
    aStream.WriteInt32L( iUseValidity );
    }


// ---------------------------------------------------------------------------
// InternalizeL
// ---------------------------------------------------------------------------
// 
void CNcdServerAddress::InternalizeL( RReadStream& aStream )
    {
    InternalizeDesL( iAddress, aStream );
    aStream >> iValidity;    
    iUseValidity = aStream.ReadInt32L();
    }



// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// 
CNcdServerAddress::CNcdServerAddress( const TInt64& aValidity )
    : iValidity( aValidity )
    {
    // Enable validity check if validity != 0
    if ( iValidity ) 
        {
        iUseValidity = ETrue;
        }
    }
    
    
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
// 
void CNcdServerAddress::ConstructL( const TDesC& aAddress )
    {
    iAddress = aAddress.AllocL();
    }
