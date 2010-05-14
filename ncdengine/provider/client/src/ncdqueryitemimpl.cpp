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

#include "ncdqueryitemimpl.h"
#include "ncd_cp_queryelement.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"
#include "ncdstring.h"
#include "ncdlocalizerutils.h"
#include "ncdqueryimpl.h"

// ======== MEMBER FUNCTIONS ========

void CNcdQueryItem::InternalizeL( RReadStream& aReadStream )
    {
    delete iId;
    iId = NULL;
    iId = HBufC::NewL( aReadStream, KMaxTInt );
    iSemantics = static_cast<MNcdQueryItem::TSemantics>(
        aReadStream.ReadInt32L());
    delete iLabel;
    iLabel = NULL;
    iLabel = CNcdString::NewL( aReadStream );
    delete iMessage;
    iMessage = NULL;
    iMessage = CNcdString::NewL( aReadStream );
    delete iDescription;
    iDescription = NULL;
    iDescription = CNcdString::NewL( aReadStream );
    iIsOptional = aReadStream.ReadInt32L();
    iIsSet = aReadStream.ReadInt32L();
    iIsInvisible = aReadStream.ReadInt32L();
    }
    
void CNcdQueryItem::InternalizeL( const MNcdConfigurationProtocolQueryElement& aQueryElement )
    {
    delete iId;
    iId = NULL;
    iId = aQueryElement.Id().AllocL();    
    iSemantics = static_cast<MNcdQueryItem::TSemantics>(
        aQueryElement.Semantics());
    delete iLabel;
    iLabel = NULL;
    iLabel = CNcdString::NewL( aQueryElement.Label().Key(), aQueryElement.Label().Data() );
    delete iMessage;
    iMessage = NULL;
    iMessage = CNcdString::NewL( aQueryElement.Message().Key(), aQueryElement.Message().Data() );
    delete iDescription;
    iDescription = NULL;
    iDescription = CNcdString::NewL( aQueryElement.Description().Key(), aQueryElement.Description().Data() );
    iIsOptional = aQueryElement.Optional();
    iIsSet = EFalse;
    if( aQueryElement.Type() == 
        MNcdConfigurationProtocolQueryElement::EConfiguration )
        {
        iIsInvisible = ETrue;
        }
    }
    
void CNcdQueryItem::ExternalizeL( RWriteStream& aWriteStream ) const
    {
    aWriteStream << *iId;
    aWriteStream.WriteInt32L( iSemantics );
    iLabel->ExternalizeL( aWriteStream );
    iMessage->ExternalizeL( aWriteStream );
    iDescription->ExternalizeL( aWriteStream );
    aWriteStream.WriteInt32L( iIsOptional );
    aWriteStream.WriteInt32L( iIsSet );
    aWriteStream.WriteInt32L( iIsInvisible );
    }
    
const TDesC& CNcdQueryItem::Id() const
    {
    return *iId;
    }

TBool CNcdQueryItem::IsSet() const
    {
    return iIsSet;
    }

TBool CNcdQueryItem::IsInvisible() const
    {
    return iIsInvisible;
    }

TNcdInterfaceId CNcdQueryItem::Type() const
    {
    return static_cast<TNcdInterfaceId>(MNcdQueryItem::KInterfaceUid);
    }
    
MNcdQueryItem::TSemantics CNcdQueryItem::Semantics() const
    {
    return iSemantics;
    }

const TDesC& CNcdQueryItem::Label() const
    {
    DLTRACEIN((""));
    MNcdClientLocalizer* localizer = iParentQuery.ClientLocalizer();
    return CNcdLocalizerUtils::LocalizedString(
        *iLabel, localizer, iLocalizedLabel );
    }
    
const TDesC& CNcdQueryItem::Message() const
    {
    DLTRACEIN((""));
    MNcdClientLocalizer* localizer = iParentQuery.ClientLocalizer();
    return CNcdLocalizerUtils::LocalizedString(
        *iMessage, localizer, iLocalizedMessage );
    }
    
const TDesC& CNcdQueryItem::Description() const
    {
    DLTRACEIN((""));
    MNcdClientLocalizer* localizer = iParentQuery.ClientLocalizer();
    return CNcdLocalizerUtils::LocalizedString(
        *iDescription, localizer, iLocalizedDescription );
    }
    
TBool CNcdQueryItem::IsOptional() const
    {
    return iIsOptional;
    }

CNcdQueryItem::CNcdQueryItem( CNcdQuery& aParent ) : 
    CCatalogsInterfaceBase( NULL ), iParentQuery( aParent )
    {
    }

CNcdQueryItem::~CNcdQueryItem()
    {
    delete iId;
    delete iLabel;
    delete iDescription;
    delete iLocalizedLabel;
    delete iLocalizedMessage;
    delete iLocalizedDescription;
    delete iValue;
    delete iMessage;
    }
    
void CNcdQueryItem::ConstructL()
    {
	AssignDesL( iId, KNullDesC );
	AssignDesL( iValue, KNullDesC );
	iLabel = CNcdString::NewL();
	iDescription = CNcdString::NewL();
	iMessage = CNcdString::NewL();
	
    // Register the interfaces of this object
    MNcdQueryItem* queryItem( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL(
            queryItem, this, MNcdQueryItem::KInterfaceUid ) );
    }
        
    
