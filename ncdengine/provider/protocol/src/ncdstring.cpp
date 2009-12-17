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
* Description:   CNcdString implementation
*
*/


#include "ncdstring.h"
#include "catalogsutils.h"

CNcdString* CNcdString::NewLC( const TDesC& aKey,
                               const TDesC& aData )
    {
    CNcdString* self = new(ELeave) CNcdString;
    CleanupStack::PushL( self );
    self->ConstructL( aKey, aData );
    return self;
    }

CNcdString* CNcdString::NewL( const TDesC& aKey,
                              const TDesC& aData )
    {
    CNcdString* self = NewLC( aKey, aData );
    CleanupStack::Pop( self );
    return self;
    }

CNcdString* CNcdString::NewLC( RReadStream& aStream )
    {
    CNcdString* self = new(ELeave) CNcdString;
    CleanupStack::PushL( self );
    self->ConstructL( aStream );
    return self;
    }

CNcdString* CNcdString::NewL( RReadStream& aStream )
    {
    CNcdString* self = NewLC( aStream );
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdString* CNcdString::NewL( const CNcdString& aString ) 
    {
    CNcdString* self = NewLC( aString );
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdString* CNcdString::NewLC( const CNcdString& aString )
    {
    CNcdString* self = new( ELeave ) CNcdString;
    CleanupStack::PushL( self );
    self->ConstructL( aString );
    return self;
    }


CNcdString::CNcdString()
    {
    }

void CNcdString::ConstructL( const TDesC& aKey, const TDesC& aData )
    {
    SetKeyL( aKey );
    SetDataL( aData );
    }

void CNcdString::ConstructL( RReadStream& aStream )
    {
    InternalizeL( aStream );
    }
    
void CNcdString::ConstructL( const CNcdString& aString ) 
    {
    SetKeyL( aString.Key() );
    SetDataL( aString.Data() );
    }

CNcdString::~CNcdString()
    {
    delete iKey;
    delete iData;
    }


const TDesC& CNcdString::Key() const
    {
    return *iKey;
    }

const TDesC& CNcdString::Data() const
    {
    return *iData;
    }

void CNcdString::SetDataL( const TDesC8& aData )
    {
    AssignDesL( iData, aData );
    }

void CNcdString::SetKeyL( const TDesC8& aKey )
    {
    AssignDesL( iKey, aKey );
    }

void CNcdString::SetDataL( const TDesC16& aData )
    {
    AssignDesL( iData, aData );
    }

void CNcdString::SetKeyL( const TDesC16& aKey )
    {
    AssignDesL( iKey, aKey );
    }


void CNcdString::ExternalizeL( RWriteStream& aStream ) const
    {
    ExternalizeDesL( *iKey, aStream );
    ExternalizeDesL( *iData, aStream );
    } 

void CNcdString::InternalizeL( RReadStream& aStream )
    {
    InternalizeDesL( iKey, aStream );
    InternalizeDesL( iData, aStream );
    }
