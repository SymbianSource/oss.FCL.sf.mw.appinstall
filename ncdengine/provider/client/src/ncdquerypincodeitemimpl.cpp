/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include "ncdquerypincodeitemimpl.h"

#include <s32strm.h>

#include "ncd_cp_queryelement.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"


// ======== MEMBER FUNCTIONS ========

CNcdQueryPinCodeItem* CNcdQueryPinCodeItem::NewL(
    RReadStream& aReadStream, CNcdQuery& aParent )
    {
    CNcdQueryPinCodeItem* self =
        CNcdQueryPinCodeItem::NewLC( aReadStream, aParent );
    CleanupStack::Pop( self );
    return self;
    }

CNcdQueryPinCodeItem* CNcdQueryPinCodeItem::NewLC(
    RReadStream& aReadStream, CNcdQuery& aParent )
    {
    CNcdQueryPinCodeItem* self = new ( ELeave ) CNcdQueryPinCodeItem( aParent );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );

    self->ConstructL();
    self->InternalizeL( aReadStream );
    return self;
    }
    
CNcdQueryPinCodeItem* CNcdQueryPinCodeItem::NewL(
    const MNcdConfigurationProtocolQueryElement& aQueryElement,
    CNcdQuery& aParent )
    {
    CNcdQueryPinCodeItem* self = 
        CNcdQueryPinCodeItem::NewLC( aQueryElement, aParent );
    CleanupStack::Pop( self );
    return self;
    }

CNcdQueryPinCodeItem* CNcdQueryPinCodeItem::NewLC(
    const MNcdConfigurationProtocolQueryElement& aQueryElement,
    CNcdQuery& aParent )
    {
    CNcdQueryPinCodeItem* self = new ( ELeave ) CNcdQueryPinCodeItem( aParent );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    self->InternalizeL( aQueryElement );
    return self;
    }
    
void CNcdQueryPinCodeItem::InternalizeL( RReadStream& aReadStream )
    {
    CNcdQueryItem::InternalizeL( aReadStream );
    InternalizeDesL( iPinCode, aReadStream );
    }
    
void CNcdQueryPinCodeItem::InternalizeL(
    const MNcdConfigurationProtocolQueryElement& aQueryElement )
    {
    CNcdQueryItem::InternalizeL( aQueryElement );
    AssignDesL( iPinCode, KNullDesC );
    }
    
void CNcdQueryPinCodeItem::ExternalizeL( RWriteStream& aWriteStream ) const
    {
    CNcdQueryItem::ExternalizeL( aWriteStream );
    ExternalizeDesL( *iPinCode, aWriteStream );    
    }


void CNcdQueryPinCodeItem::SetPinCodeL( const TDesC& aPinCode )
    {
    AssignDesL( iPinCode, aPinCode );
    iIsSet = ETrue;
    }


TNcdInterfaceId CNcdQueryPinCodeItem::Type() const
    {
    return static_cast<TNcdInterfaceId>(MNcdQueryPinCodeItem::KInterfaceUid);
    }

const TDesC& CNcdQueryPinCodeItem::ValueL()
    {
    if ( ! iIsSet )
        {
        User::Leave( KErrArgument );
        }
    return *iPinCode;
    }

CNcdQueryPinCodeItem::~CNcdQueryPinCodeItem()
    {
    delete iPinCode;
    }
    
CNcdQueryPinCodeItem::CNcdQueryPinCodeItem( CNcdQuery& aParent )
    : CNcdQueryItem( aParent )
    {
    }
    
void CNcdQueryPinCodeItem::ConstructL()
    {
    CNcdQueryItem::ConstructL();
    // Register the interfaces of this object
    MNcdQueryPinCodeItem* queryItem( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL(
            queryItem, this, MNcdQueryPinCodeItem::KInterfaceUid ) );
    }
