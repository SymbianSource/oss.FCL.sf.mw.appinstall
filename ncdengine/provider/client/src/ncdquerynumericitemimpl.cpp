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
* Description:  
*
*/


#include <s32strm.h>

#include "ncdquerynumericitemimpl.h"
#include "ncd_cp_queryelement.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"


// ======== MEMBER FUNCTIONS ========

CNcdQueryNumericItem* CNcdQueryNumericItem::NewL(
    RReadStream& aReadStream, CNcdQuery& aParent )
    {
    CNcdQueryNumericItem* self =
        CNcdQueryNumericItem::NewLC( aReadStream, aParent );
    CleanupStack::Pop( self );
    return self;
    }

CNcdQueryNumericItem* CNcdQueryNumericItem::NewLC(
    RReadStream& aReadStream, CNcdQuery& aParent )
    {
    CNcdQueryNumericItem* self = new ( ELeave ) CNcdQueryNumericItem( aParent );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );

    self->ConstructL();
    self->InternalizeL( aReadStream );
    return self;
    }
    
CNcdQueryNumericItem* CNcdQueryNumericItem::NewL(
    const MNcdConfigurationProtocolQueryElement& aQueryElement,
    CNcdQuery& aParent )
    {
    CNcdQueryNumericItem* self = 
        CNcdQueryNumericItem::NewLC( aQueryElement, aParent );
    CleanupStack::Pop( self );
    return self;
    }

CNcdQueryNumericItem* CNcdQueryNumericItem::NewLC(
    const MNcdConfigurationProtocolQueryElement& aQueryElement,
    CNcdQuery& aParent )
    {
    CNcdQueryNumericItem* self = 
        new ( ELeave ) CNcdQueryNumericItem( aParent );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );

    self->ConstructL();
    self->InternalizeL( aQueryElement );
    return self;
    }
    
void CNcdQueryNumericItem::InternalizeL( RReadStream& aReadStream )
    {
    CNcdQueryItem::InternalizeL( aReadStream );
    InternalizeDesL( iValue, aReadStream  );
    }
    
void CNcdQueryNumericItem::InternalizeL(
    const MNcdConfigurationProtocolQueryElement& aQueryElement )
    {
    CNcdQueryItem::InternalizeL( aQueryElement );
    }
        
void CNcdQueryNumericItem::ExternalizeL( RWriteStream& aWriteStream ) const
    {
    CNcdQueryItem::ExternalizeL( aWriteStream );
    ExternalizeDesL( *iValue, aWriteStream );
    }
    
    
void CNcdQueryNumericItem::SetValueL( const TDesC& aValue )
    {
    AssignDesL( iValue, aValue );
    iIsSet = ETrue;
    }

TNcdInterfaceId CNcdQueryNumericItem::Type() const
    {
    return static_cast<TNcdInterfaceId>(MNcdQueryNumericItem::KInterfaceUid);
    }

const TDesC& CNcdQueryNumericItem::ValueL()
    {
    if ( ! iIsSet )
        {
        User::Leave( KErrArgument );
        }
    
    return *iValue;
    }

CNcdQueryNumericItem::~CNcdQueryNumericItem()
    {    
    }
    
CNcdQueryNumericItem::CNcdQueryNumericItem( CNcdQuery& aParent )
    : CNcdQueryItem( aParent )
    {
    }
    
void CNcdQueryNumericItem::ConstructL()
    {
    CNcdQueryItem::ConstructL();
    // Register the interfaces of this object
    MNcdQueryNumericItem* queryItem( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL(
            queryItem, this, MNcdQueryNumericItem::KInterfaceUid ) );
    }
