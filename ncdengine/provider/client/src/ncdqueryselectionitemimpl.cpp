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
#include <badesca.h>

#include "ncdqueryselectionitemimpl.h"
#include "ncd_cp_queryelement.h"
#include "ncdqueryoptionimpl.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsconstants.h"
#include "catalogsdebug.h"
#include "ncdqueryimpl.h"
#include "ncdlocalizerutils.h"


// ======== MEMBER FUNCTIONS ========

CNcdQuerySelectionItem* CNcdQuerySelectionItem::NewL(
    RReadStream& aReadStream, CNcdQuery& aParent )
    {
    CNcdQuerySelectionItem* self = CNcdQuerySelectionItem::NewLC( aReadStream, aParent );
    CleanupStack::Pop( self );
    return self;
    }

CNcdQuerySelectionItem* CNcdQuerySelectionItem::NewLC(
    RReadStream& aReadStream, CNcdQuery& aParent )
    {
    CNcdQuerySelectionItem* self = new ( ELeave ) CNcdQuerySelectionItem( aParent );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );

    self->ConstructL();
    self->InternalizeL( aReadStream );
    self->CreateSelectionTextsL();
    return self;
    }
    
CNcdQuerySelectionItem* CNcdQuerySelectionItem::NewL(
    const MNcdConfigurationProtocolQueryElement& aQueryElement,
    CNcdQuery& aParent )
    {
    CNcdQuerySelectionItem* self = CNcdQuerySelectionItem::NewLC( aQueryElement, aParent );
    CleanupStack::Pop( self );
    return self;
    }

CNcdQuerySelectionItem* CNcdQuerySelectionItem::NewLC(
    const MNcdConfigurationProtocolQueryElement& aQueryElement,
    CNcdQuery& aParent )
    {
    CNcdQuerySelectionItem* self = new ( ELeave ) CNcdQuerySelectionItem( aParent );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );

    self->ConstructL();
    self->InternalizeL( aQueryElement );
    self->CreateSelectionTextsL();
    return self;
    }

CNcdQuerySelectionItem* CNcdQuerySelectionItem::NewL(
    const RPointerArray<CNcdString>& aPaymentMethodNames,
    const MDesC8Array& aPaymentMethodTypes,
    CNcdQuery& aParent )
    {
    CNcdQuerySelectionItem* self = 
        CNcdQuerySelectionItem::NewLC( aPaymentMethodNames, aPaymentMethodTypes, aParent );
    CleanupStack::Pop( self );
    return self;
    }

CNcdQuerySelectionItem* CNcdQuerySelectionItem::NewLC(
    const RPointerArray<CNcdString>& aPaymentMethodNames,
    const MDesC8Array& aPaymentMethodTypes,
    CNcdQuery& aParent )
    {
    CNcdQuerySelectionItem* self = new ( ELeave ) CNcdQuerySelectionItem( aParent );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );

    self->ConstructL( aPaymentMethodNames, aPaymentMethodTypes );
    self->CreateSelectionTextsL();
    return self;
    }
    
void CNcdQuerySelectionItem::InternalizeL( RReadStream& aReadStream )
    {
    CNcdQueryItem::InternalizeL( aReadStream );
    iSelection = aReadStream.ReadInt32L();
    TInt selectionCount = aReadStream.ReadInt32L();

    for ( TInt i = 0 ; i < selectionCount ; i++ )
        {
        CNcdQueryOption* option = CNcdQueryOption::NewLC( aReadStream );
        iOptions.AppendL( option );
        CleanupStack::Pop( option );
        }

    delete iSelectionTexts;
    iSelectionTexts = NULL;

    delete iSelectionData;
    iSelectionData = NULL;
    iSelectionData = new (ELeave) CDesC8ArrayFlat( KListGranularity );
    
    for ( TInt i = 0 ; i < iOptions.Count() ; i++ )
        {
        iSelectionData->AppendL( iOptions[i]->Data() );
        }
    }
    
void CNcdQuerySelectionItem::InternalizeL(
    const MNcdConfigurationProtocolQueryElement& aQueryElement )
    {
    CNcdQueryItem::InternalizeL( aQueryElement );
    for ( TInt i = 0 ; i < aQueryElement.OptionCount() ; i++ )
        {
        CNcdQueryOption* option = CNcdQueryOption::NewLC(
            aQueryElement.Option( i ) );
        iOptions.AppendL( option );
        CleanupStack::Pop( option );
        }

    delete iSelectionTexts;
    iSelectionTexts = NULL;
    delete iSelectionData;
    iSelectionData = NULL;
    iSelectionData = new (ELeave) CDesC8ArrayFlat( KListGranularity );

    for ( TInt i = 0 ; i < iOptions.Count() ; i++ )
        {
        iSelectionData->AppendL( iOptions[i]->Data() );
        }

    }
    
void CNcdQuerySelectionItem::ExternalizeL( RWriteStream& aWriteStream ) const
    {
    CNcdQueryItem::ExternalizeL( aWriteStream );
    aWriteStream.WriteInt32L( iSelection );
    aWriteStream.WriteInt32L( iOptions.Count() );
    for ( TInt i = 0 ; i < iOptions.Count() ; i++ )
        {
        iOptions[i]->ExternalizeL( aWriteStream );
        }
    }

    
TInt CNcdQuerySelectionItem::Selection() const
    {
    return iSelection;
    }
    
const MDesCArray& CNcdQuerySelectionItem::SelectionTexts() const
    {
    DLTRACEIN((""));
    return *iSelectionTexts;
    }
    
const MDesC8Array& CNcdQuerySelectionItem::SelectionData() const
    {
    return *iSelectionData;
    }

void CNcdQuerySelectionItem::SetSelectionL( TInt aIndex )
    {
    if ( aIndex < 0 || aIndex > iOptions.Count() )
        {
        User::Leave( KErrArgument );
        }
    iSelection = aIndex;
    iIsSet = ETrue;
    }

TNcdInterfaceId CNcdQuerySelectionItem::Type() const
    {
    return static_cast<TNcdInterfaceId>(MNcdQuerySelectionItem::KInterfaceUid);
    }

const TDesC& CNcdQuerySelectionItem::ValueL()
    {    
    if ( iSelection < 0 || iSelection > iOptions.Count() || ! iIsSet )
        {
        User::Leave( KErrArgument );
        }
    return iOptions[iSelection]->Value();
    }

CNcdQuerySelectionItem::~CNcdQuerySelectionItem()
    {
    DLTRACEIN((""));
    delete iSelectionTexts;
    delete iSelectionData;
    iOptions.ResetAndDestroy();
    }
    
CNcdQuerySelectionItem::CNcdQuerySelectionItem( CNcdQuery& aParent )
    : CNcdQueryItem( aParent )
    {
    }
    
void CNcdQuerySelectionItem::ConstructL()
    {
    CNcdQueryItem::ConstructL();
    // Register the interfaces of this object
    MNcdQuerySelectionItem* queryItem( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL(
            queryItem, this, MNcdQuerySelectionItem::KInterfaceUid ) );
    }

void CNcdQuerySelectionItem::ConstructL( const RPointerArray<CNcdString>& aPaymentMethodNames,
                                         const MDesC8Array& aPaymentMethodTypes )
    {
    ConstructL();

    DASSERT( aPaymentMethodNames.Count() == aPaymentMethodTypes.MdcaCount() );

    TInt count = aPaymentMethodNames.Count();
    CNcdQueryOption* option;
    for( TInt i = 0; i < count; i++ )
        {
        option = CNcdQueryOption::NewLC( *aPaymentMethodNames[ i ], aPaymentMethodTypes.MdcaPoint( i ) );
        iOptions.AppendL( option );
        CleanupStack::Pop( option );
        }
    }
    
void CNcdQuerySelectionItem::CreateSelectionTextsL()
    {
    delete iSelectionTexts;
    iSelectionTexts = NULL;
    iSelectionTexts = new (ELeave) CDesC16ArrayFlat( KListGranularity );
    // Use the string localizer to create selection texts.
    for ( TInt i = 0; i < iOptions.Count(); i++ )
        {
        const CNcdString& string = iOptions[i]->Name();
        HBufC* stringBuf( NULL );
        // returns KNullDesC if localizer not available
        const TDesC& localizedString = CNcdLocalizerUtils::LocalizedString(
            string, iParentQuery.ClientLocalizer(), stringBuf );
        if ( stringBuf ) 
            {
            CleanupStack::PushL( stringBuf );
            iSelectionTexts->AppendL( *stringBuf );
            CleanupStack::PopAndDestroy( stringBuf );
            }
        else
            {
            iSelectionTexts->AppendL( localizedString );
            }
        }
    }
