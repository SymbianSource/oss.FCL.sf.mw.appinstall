/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of CNcdKeyValuePair
*
*/


#include "ncdutils.h"

#include <s32strm.h>

#include "catalogsutils.h"
#include "catalogsconstants.h"
#include "ncdproviderdefines.h"

#include "catalogsdebug.h"

EXPORT_C CNcdSearchFilter *CNcdSearchFilter::NewL()
    {
    CNcdSearchFilter *self = CNcdSearchFilter::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
    
EXPORT_C CNcdSearchFilter *CNcdSearchFilter::NewLC()
    {
    CNcdSearchFilter *self = new (ELeave) CNcdSearchFilter;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

EXPORT_C CNcdSearchFilter *CNcdSearchFilter::NewL(
    const CNcdSearchFilter& aFilter )
    {
    CNcdSearchFilter *self = CNcdSearchFilter::NewLC( aFilter );
    CleanupStack::Pop( self );
    return self;
    }
    
EXPORT_C CNcdSearchFilter *CNcdSearchFilter::NewLC(
    const CNcdSearchFilter& aFilter )
    {
    CNcdSearchFilter *self = CNcdSearchFilter::NewLC();
    for ( TInt i = 0 ; i < aFilter.Keywords().MdcaCount() ; i++ )
        {
        self->AddKeywordL( aFilter.Keywords().MdcaPoint( i ) );
        }
    self->SetContentPurposes( aFilter.ContentPurposes() );
    self->SetSearchMode( aFilter.SearchMode() );
    self->SetRecursionDepthL( aFilter.RecursionDepth() );
    
    return self;
    }

EXPORT_C CNcdSearchFilter *CNcdSearchFilter::NewL(
    RReadStream& aStream )
    {
    CNcdSearchFilter *self = CNcdSearchFilter::NewLC( aStream );
    CleanupStack::Pop( self );
    return self;
    }
    
EXPORT_C CNcdSearchFilter *CNcdSearchFilter::NewLC(
    RReadStream& aStream )
    {
    CNcdSearchFilter *self = new (ELeave) CNcdSearchFilter;
    CleanupStack::PushL( self );
    self->InternalizeL( aStream );
    return self;
    }

CNcdSearchFilter::~CNcdSearchFilter()
    {
    delete iKeywords;
    }
        
EXPORT_C void CNcdSearchFilter::AddKeywordL( const TDesC &aKeyword )
    {
    iKeywords->AppendL( aKeyword );
    }
    
EXPORT_C void CNcdSearchFilter::SetContentPurposes( TUint aFlags )
    {
    iPurposes = aFlags;
    }


EXPORT_C void CNcdSearchFilter::SetSearchMode( 
    MNcdSearchFilter::TSearchMode aMode )
    {
    iSearchMode = aMode;
    }


EXPORT_C void CNcdSearchFilter::SetRecursionDepthL( TUint aRecursionDepth )
    {
    if ( aRecursionDepth > NcdProviderDefines::KNcdMaxSearchRecursionDepth ) 
        {
        User::Leave( KErrArgument );
        }
    
    iRecursionDepth = aRecursionDepth;
    }
    

void CNcdSearchFilter::InternalizeL( RReadStream& aStream )
    {
    delete iKeywords;
    iKeywords = NULL;
    iKeywords = new (ELeave) CDesCArrayFlat( KListGranularity );
    TInt keywordCount = aStream.ReadInt32L();
    for ( TInt i = 0 ; i < keywordCount ; i++ )
        {
        HBufC* keyword = NULL;
        InternalizeDesL( keyword, aStream );
        CleanupStack::PushL( keyword );
        iKeywords->AppendL( *keyword );
        CleanupStack::PopAndDestroy( keyword );
        }
    iPurposes = aStream.ReadUint32L();
    InternalizeEnumL( iSearchMode, aStream );
    iRecursionDepth = aStream.ReadUint8L();
    }
    
void CNcdSearchFilter::ExternalizeL( RWriteStream& aStream )
    {
    aStream.WriteInt32L( iKeywords->Count() );
    for ( TInt i = 0 ; i < iKeywords->Count() ; i++ )
        {
        ExternalizeDesL( (*iKeywords)[i], aStream );
        }
    aStream.WriteUint32L( iPurposes );
    ExternalizeEnumL( iSearchMode, aStream );
    aStream.WriteUint8L( iRecursionDepth );
    }

const MDesCArray& CNcdSearchFilter::Keywords() const
    {
    return *iKeywords;
    }

TUint CNcdSearchFilter::ContentPurposes() const
    {
    return iPurposes;
    }

MNcdSearchFilter::TSearchMode CNcdSearchFilter::SearchMode() const
    {
    return iSearchMode;
    }
    
TUint CNcdSearchFilter::RecursionDepth() const
    {
    return iRecursionDepth;
    }
    
CNcdSearchFilter::CNcdSearchFilter()
    : iRecursionDepth( NcdProviderDefines::KNcdDefaultSearchRecursionDepth )
    {
    }
    
void CNcdSearchFilter::ConstructL()
    {
    iKeywords = new (ELeave) CDesCArrayFlat(KListGranularity);
    }
    
// End of file

