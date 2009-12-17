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

#include "ncdquerytextitemimpl.h"
#include "ncd_cp_queryelement.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"

// ======== MEMBER FUNCTIONS ========

CNcdQueryTextItem* CNcdQueryTextItem::NewL(
    RReadStream& aReadStream, CNcdQuery& aParent )
    {
    CNcdQueryTextItem* self = CNcdQueryTextItem::NewLC( aReadStream, aParent );
    CleanupStack::Pop( self );
    return self;
    }

CNcdQueryTextItem* CNcdQueryTextItem::NewLC(
    RReadStream& aReadStream, CNcdQuery& aParent )
    {
    CNcdQueryTextItem* self = new ( ELeave ) CNcdQueryTextItem( aParent );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );

    self->ConstructL();
    self->InternalizeL( aReadStream );
    return self;
    }
    
CNcdQueryTextItem* CNcdQueryTextItem::NewL(
    const MNcdConfigurationProtocolQueryElement& aQueryElement,
    CNcdQuery& aParent )
    {
    CNcdQueryTextItem* self = CNcdQueryTextItem::NewLC( aQueryElement, aParent );
    CleanupStack::Pop( self );
    return self;
    }

CNcdQueryTextItem* CNcdQueryTextItem::NewLC(
    const MNcdConfigurationProtocolQueryElement& aQueryElement,
    CNcdQuery& aParent )
    {
    CNcdQueryTextItem* self = new ( ELeave ) CNcdQueryTextItem( aParent );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );

    self->ConstructL();
    self->InternalizeL( aQueryElement );
    return self;
    }
    
void CNcdQueryTextItem::InternalizeL( RReadStream& aReadStream )
    {
    CNcdQueryItem::InternalizeL( aReadStream );
    delete iText;
    iText = NULL;
    iText = HBufC::NewL( aReadStream, KMaxTInt );
    }

void CNcdQueryTextItem::InternalizeL(
    const MNcdConfigurationProtocolQueryElement& aQueryElement )
    {
    CNcdQueryItem::InternalizeL( aQueryElement );    
    }    
    
void CNcdQueryTextItem::ExternalizeL( RWriteStream& aWriteStream ) const
    {
    CNcdQueryItem::ExternalizeL( aWriteStream );
    aWriteStream << *iText;
    }


const TDesC& CNcdQueryTextItem::Text()
    {
    return *iText;
    }

void CNcdQueryTextItem::SetTextL( const TDesC& aText )
    {
    delete iText;
    iText = NULL;
    iText = aText.AllocL();
    iIsSet = ETrue;
    }

TNcdInterfaceId CNcdQueryTextItem::Type() const
    {
    return static_cast<TNcdInterfaceId>(MNcdQueryTextItem::KInterfaceUid);
    }

const TDesC& CNcdQueryTextItem::ValueL()
    {
    if ( ! iIsSet )
        {
        User::Leave( KErrArgument );
        }
    return *iText;
    }

CNcdQueryTextItem::~CNcdQueryTextItem()
    {
    delete iText;    
    }
    
CNcdQueryTextItem::CNcdQueryTextItem( CNcdQuery& aParent )
    : CNcdQueryItem( aParent )
    {
    }
    
void CNcdQueryTextItem::ConstructL()
    {
    CNcdQueryItem::ConstructL();
    AssignDesL( iText, KNullDesC );
    // Register the interfaces of this object
    MNcdQueryTextItem* queryItem( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL(
            queryItem, this, MNcdQueryTextItem::KInterfaceUid ) );
    }
