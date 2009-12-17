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
* Description:   Implementation of CNcdStorageDescriptorDataItem class.
*
*/


#include "ncdstoragedescriptordataitem.h"
#include "catalogsutils.h"

CNcdStorageDescriptorDataItem* CNcdStorageDescriptorDataItem::NewL(
    const TDesC8& aData) 
    {
    CNcdStorageDescriptorDataItem* self = NewLC( aData );
    CleanupStack::Pop(self);
    return self;
    }

CNcdStorageDescriptorDataItem* CNcdStorageDescriptorDataItem::NewLC(
    const TDesC8& aData) 
    {
    CNcdStorageDescriptorDataItem* self = 
        new (ELeave) CNcdStorageDescriptorDataItem();
    CleanupStack::PushL( self );
    self->ConstructL( aData );
    return self;
    }
    
CNcdStorageDescriptorDataItem::CNcdStorageDescriptorDataItem() 
    {
    }
    
CNcdStorageDescriptorDataItem::~CNcdStorageDescriptorDataItem() 
    {
    delete iData;
    }
    
void CNcdStorageDescriptorDataItem::ConstructL( const TDesC8& aData ) 
    {
    iData = aData.AllocL();
    }
    
void CNcdStorageDescriptorDataItem::ExternalizeL( RWriteStream& aStream ) 
    {
    aStream.WriteL( *iData );
    }
    
void CNcdStorageDescriptorDataItem::InternalizeL( RReadStream& /*aStream*/ ) 
    {
    DLERROR(("NOT SUPPORTED"));
    DASSERT(EFalse);
    }
