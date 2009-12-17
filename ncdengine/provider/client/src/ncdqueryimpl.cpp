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
#include <bamdesca.h>

#include "ncdqueryimpl.h"
#include "ncdqueryitemimpl.h"
#include "ncdquerytextitemimpl.h"
#include "ncdquerynumericitemimpl.h"
#include "ncdquerypincodeitemimpl.h"
#include "ncdqueryselectionitemimpl.h"
#include "ncd_cp_query.h"
#include "ncd_cp_queryelement.h"
#include "catalogsarray.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsconstants.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncdstring.h"
#include "ncdclientlocalizer.h"
#include "ncdlocalizerutils.h"

// ======== MEMBER FUNCTIONS ========

CNcdQuery* CNcdQuery::NewL( RReadStream& aReadStream )
    {
    CNcdQuery* self = CNcdQuery::NewLC( aReadStream );
    CleanupStack::Pop( self );
    return self;
    }

CNcdQuery* CNcdQuery::NewLC( RReadStream& aReadStream )
    {
    CNcdQuery* self = new ( ELeave ) CNcdQuery( EFalse );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );

    self->ConstructL();
    self->InternalizeL( aReadStream );
    self->InternalAddRef();
    return self;
    }
    
CNcdQuery* CNcdQuery::NewL( const MNcdConfigurationProtocolQuery& aQuery,
    TBool aIsSecureConnection )
    {
    CNcdQuery* self = CNcdQuery::NewLC( aQuery, aIsSecureConnection );
    CleanupStack::Pop( self );
    return self;
    }

CNcdQuery* CNcdQuery::NewLC( const MNcdConfigurationProtocolQuery& aQuery,
    TBool aIsSecureConnection )
    {
    CNcdQuery* self = new ( ELeave ) CNcdQuery( aIsSecureConnection );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );

    self->ConstructL();
    self->InternalizeL( aQuery );
    self->InternalAddRef();
    return self;
    }

CNcdQuery* CNcdQuery::NewL( const RPointerArray<CNcdString>& aPaymentMethodNames,
                            const MDesC8Array& aPaymentMethodTypes )
    {
    CNcdQuery* self = CNcdQuery::NewLC( aPaymentMethodNames,
                                        aPaymentMethodTypes );
    CleanupStack::Pop( self );
    return self;
    }

CNcdQuery* CNcdQuery::NewLC( const RPointerArray<CNcdString>& aPaymentMethodNames,
                             const MDesC8Array& aPaymentMethodTypes )
    {
    CNcdQuery* self = new ( ELeave ) CNcdQuery( EFalse );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL( aPaymentMethodNames, aPaymentMethodTypes );
    self->InternalAddRef();
    return self;
    }

void CNcdQuery::SetClientLocalizer( MNcdClientLocalizer* aLocalizer ) 
    {
    iClientLocalizer = aLocalizer;
    }
    
MNcdClientLocalizer* CNcdQuery::ClientLocalizer() const 
    {
    return iClientLocalizer;
    }

    
void CNcdQuery::InternalizeL( RReadStream& aReadStream )
    {
    delete iId;
    iId = NULL;
    iId = HBufC::NewL( aReadStream, KMaxTInt );
    iIsOptional = aReadStream.ReadInt32L();
    iSemantics = static_cast<MNcdQuery::TSemantics>(
        aReadStream.ReadInt32L());
    delete iTitle;
    iTitle = NULL;
    iTitle = CNcdString::NewL( aReadStream );
    delete iBody;
    iBody = NULL;
    iBody = CNcdString::NewL( aReadStream );
    iResponse = static_cast<TResponse>( aReadStream.ReadInt32L() );
    iIsSecureConnection = aReadStream.ReadInt32L();
    TInt itemCount( aReadStream.ReadInt32L() );
    iItems.ResetAndRelease();
    for ( TInt i = 0 ; i < itemCount ; i++ )
        {
        TNcdInterfaceId interfaceId = static_cast<TNcdInterfaceId>(aReadStream.ReadInt32L());
        CNcdQueryItem* item = NULL;
        switch ( interfaceId )
            {
            case ENcdQueryTextItemUid:
                {
                item = CNcdQueryTextItem::NewL( aReadStream, *this );
                break;
                }
            case ENcdQueryNumericItemUid:
                {
                item = CNcdQueryNumericItem::NewL( aReadStream, *this );
                break;
                }
            case ENcdQueryPinCodeItemUid:
                {
                item = CNcdQueryPinCodeItem::NewL( aReadStream, *this );
                break;
                }
            case ENcdQuerySelectionItemUid:
                {
                item = CNcdQuerySelectionItem::NewL( aReadStream, *this );
                break;
                }
            default:
                {
                User::Leave( KErrCorrupt );
                break;
                }
            }

        // Object's refcount must be at least 1 for Release() to work correctly
        item->AddRef();
        CleanupReleasePushL( *item );
        iItems.AppendL( item );        
        CleanupStack::Pop( item );
        }
    }
    
void CNcdQuery::InternalizeL( const MNcdConfigurationProtocolQuery& aQuery )
    {
    delete iId;
    iId = NULL;
    iId = aQuery.Id().AllocL();
    iIsOptional = aQuery.Optional();
    iSemantics = aQuery.Semantics();
    delete iTitle;
    iTitle = NULL;
    iTitle = CNcdString::NewL( aQuery.Title().Key(), aQuery.Title().Data() );
    delete iBody;
    iBody = NULL;
    iBody = CNcdString::NewL( aQuery.BodyText().Key(), aQuery.BodyText().Data() );
        
    iItems.ResetAndRelease();    
    for ( TInt i = 0 ; i < aQuery.QueryElementCount() ; i++ )
        {
        const MNcdConfigurationProtocolQueryElement& queryElement =
            aQuery.QueryElementL( i );
        CNcdQueryItem* item = NULL;
        switch ( queryElement.Type() )
            {
            case MNcdConfigurationProtocolQueryElement::EFreeText:
            case MNcdConfigurationProtocolQueryElement::EFile:
            case MNcdConfigurationProtocolQueryElement::EGpsLocation:
            case MNcdConfigurationProtocolQueryElement::EConfiguration:
                {
                item = CNcdQueryTextItem::NewL( queryElement, *this );                
                break;
                }
            case MNcdConfigurationProtocolQueryElement::ENumeric:
                {
                if ( queryElement.Semantics() ==
                    MNcdQueryItem::ESemanticsPinCode )
                    {
                    item = CNcdQueryPinCodeItem::NewL( queryElement, *this );
                    }
                else
                    {
                    item = CNcdQueryNumericItem::NewL( queryElement, *this );
                    }
                break;
                }            
            case MNcdConfigurationProtocolQueryElement::ESingleSelect:
            case MNcdConfigurationProtocolQueryElement::EMultiSelect:
                {
                item = CNcdQuerySelectionItem::NewL( queryElement, *this );                
                break;
                }
            default:
                {
                User::Leave( KErrCorrupt );
                break;
                }
            }
        
        // Object's refcount must be at least 1 for Release() to work correctly
        item->AddRef();
        CleanupReleasePushL( *item );		
        iItems.AppendL( item );
        CleanupStack::Pop( item );        
        }
    }
    
void CNcdQuery::ExternalizeL( RWriteStream& aWriteStream ) const
    {
    aWriteStream << *iId;
    aWriteStream.WriteInt32L( iIsOptional );
    aWriteStream.WriteInt32L( iSemantics );
    iTitle->ExternalizeL( aWriteStream );
    iBody->ExternalizeL( aWriteStream );
    aWriteStream.WriteInt32L( iResponse );
    aWriteStream.WriteInt32L( iIsSecureConnection );
    aWriteStream.WriteInt32L( iItems.Count() );
    for ( TInt i = 0 ; i < iItems.Count() ; i++ )
        {
        aWriteStream.WriteInt32L( iItems[i]->Type() );
        iItems[i]->ExternalizeL( aWriteStream );
        }
    }
    
    
TInt CNcdQuery::ItemCount() const
    {
    return iItems.Count();
    }

CNcdQueryItem& CNcdQuery::QueryItemL( TInt aIndex )
    {
    DLTRACEIN((""));
    if ( aIndex < 0 || aIndex >= iItems.Count() )
        {
        User::Leave( KErrArgument );
        }
    DLTRACEOUT((""));
    return *iItems[aIndex];
    }

const TDesC& CNcdQuery::Id() const
    {
    return *iId;
    }

TBool CNcdQuery::AllItemsSet() const
    {
    DLTRACEIN((""));
    for ( TInt i = 0 ; i < iItems.Count() ; i++ )
        {
        if( !iItems[i]->IsSet() )
            {
            DLTRACEOUT(("EFalse"));
            return EFalse;
            }
        }
    DLTRACEOUT(("ETrue"));
    return ETrue;
    }

TBool CNcdQuery::IsOptional() const
    {
    return iIsOptional;
    }
    
MNcdQuery::TSemantics CNcdQuery::Semantics() const
    {
    return iSemantics;
    }
    
const TDesC& CNcdQuery::MessageTitle() const
    {
    DLTRACEIN((""));
    return CNcdLocalizerUtils::LocalizedString(
        *iTitle, iClientLocalizer, iLocalizedTitle );
    }
    
const TDesC& CNcdQuery::MessageBody() const
    {
    DLTRACEIN((""))
    return CNcdLocalizerUtils::LocalizedString(
        *iBody, iClientLocalizer, iLocalizedBody );
    }
    
RCatalogsArray< MNcdQueryItem > CNcdQuery::QueryItemsL()
    {
    DLTRACEIN((""));
    RCatalogsArray< MNcdQueryItem > array;
    TRAPD( err,
        {
        for ( TInt i = 0 ; i < iItems.Count() ; i++ )
            {
            // don't show invisible items
            if( ! iItems[i]->IsInvisible() )
                {
                DLTRACE((_L("Adding item: id=%S description=%S message=%S semantics=%d"),
                     &iItems[i]->Id(), &iItems[i]->Description(),
                     &iItems[i]->Message(), iItems[i]->Semantics() ));
                array.AppendL( iItems[i] );
                iItems[i]->AddRef();
                }
            }
        });
    if ( err != KErrNone )
        {        
        array.ResetAndRelease();
        User::Leave( err );
        }   
    return array;
    }

void CNcdQuery::SetResponseL( TResponse aResponse )
    {
    iResponse = aResponse;
    }

MNcdQuery::TResponse CNcdQuery::Response()
    {
    return iResponse;
    }
    
TBool CNcdQuery::IsSecureConnection() const
    {
    return iIsSecureConnection;
    }
    
CNcdQuery::~CNcdQuery()
    {
    DLTRACEIN((""));
    delete iId;
    delete iTitle;
    delete iBody;
    delete iLocalizedTitle;
    delete iLocalizedBody;
    iItems.ResetAndRelease();
    }
    
CNcdQuery::CNcdQuery( TBool aIsSecureConnection )
    : CCatalogsInterfaceBase( NULL ),
      iIsSecureConnection( aIsSecureConnection )
    {    
    }
    
void CNcdQuery::ConstructL()
    {
    
    iTitle = CNcdString::NewL( KNullDesC, KNullDesC );
    iBody = CNcdString::NewL( KNullDesC, KNullDesC );
    AssignDesL( iId, KNullDesC );
        
    // Register the interfaces of this object
    MNcdQuery* query( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( query, this, MNcdQuery::KInterfaceUid ) );
    }

void CNcdQuery::ConstructL( const RPointerArray<CNcdString>& aPaymentMethodNames,
                            const MDesC8Array& aPaymentMethodTypes )
    {
    ConstructL();

    AssignDesL( iId, KQueryIdPaymentMethod() );
    iSemantics = MNcdQuery::ESemanticsPaymentMethodSelectionQuery;
    
    CNcdQuerySelectionItem* item = 
        CNcdQuerySelectionItem::NewLC( aPaymentMethodNames, aPaymentMethodTypes, *this );
	item->AddRef();
    iItems.AppendL( item );
    CleanupStack::Pop( item );

    }
