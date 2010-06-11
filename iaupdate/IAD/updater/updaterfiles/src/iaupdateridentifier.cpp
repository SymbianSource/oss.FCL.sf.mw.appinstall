/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CIAUpdaterIdentifier
*
*/




#include "iaupdateridentifier.h"


// This constant will be used to check if the IAD application
// and iaupdater are using the same version of the file. If the versions
// differ, then this may require additional checking in the future versions
// when data is internalized. For example, IAD may give old version data 
// for iaupdater during self update just after iaupdater has been updated
// and IAD is itself updated after that.
const TInt KVersion( 1 );


// -----------------------------------------------------------------------------
// CIAUpdaterIdentifier::NewL
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdaterIdentifier* CIAUpdaterIdentifier::NewL()
    {
    CIAUpdaterIdentifier* self =
        CIAUpdaterIdentifier::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterIdentifier::NewLC
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdaterIdentifier* CIAUpdaterIdentifier::NewLC()
    {
    CIAUpdaterIdentifier* self =
        new( ELeave) CIAUpdaterIdentifier();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


// -----------------------------------------------------------------------------
// CIAUpdaterIdentifier::CIAUpdaterIdentifier
//
// -----------------------------------------------------------------------------
//
CIAUpdaterIdentifier::CIAUpdaterIdentifier()
: CBase(),
  iUid( TUid::Null() )
    {
    }


// -----------------------------------------------------------------------------
// CIAUpdaterIdentifier::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIAUpdaterIdentifier::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// CIAUpdaterIdentifier::~CIAUpdaterIdentifier
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdaterIdentifier::~CIAUpdaterIdentifier()
    {
    Reset();
    }


// -----------------------------------------------------------------------------
// CIAUpdaterIdentifier::Reset
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterIdentifier::Reset()
    {
    iUid = TUid::Null();
    
    delete iId;
    iId = NULL;
    
    delete iNamespace;
    iNamespace = NULL;    
    }


// -----------------------------------------------------------------------------
// CIAUpdaterIdentifier::Id
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIAUpdaterIdentifier::Id() const
    {
    if ( !iId )
        {
        return KNullDesC;
        }
        
    return *iId;
    }
    

// -----------------------------------------------------------------------------
// CIAUpdaterIdentifier::SetIdL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterIdentifier::SetIdL( const TDesC& aId )
    {
    HBufC* tmp( aId.AllocL() );
    delete iId;
    iId = tmp;
    }
    
    
// -----------------------------------------------------------------------------
// CIAUpdaterIdentifier::Namespace
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIAUpdaterIdentifier::Namespace() const
    {
    if ( !iNamespace )
        {
        return KNullDesC;
        }
    
    return *iNamespace;
    }
    

// -----------------------------------------------------------------------------
// CIAUpdaterIdentifier::SetNamespaceL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterIdentifier::SetNamespaceL( const TDesC& aNamespace )
    {
    HBufC* tmp( aNamespace.AllocL() );
    delete iNamespace;
    iNamespace = tmp;
    }
    

// -----------------------------------------------------------------------------
// CIAUpdaterIdentifier::Uid
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TUid& CIAUpdaterIdentifier::Uid() const
    {
    return iUid;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterIdentifier::SetUid
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterIdentifier::SetUid( const TUid& aUid )
    {
    iUid.iUid = aUid.iUid;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterIdentifier::SetUid
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterIdentifier::SetUid( const TInt aUid )
    {
    iUid.iUid = aUid;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterIdentifier::InternalizeL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterIdentifier::InternalizeL( RReadStream& aStream )
	{
	// If you make changes here, 
	// remember to update ExternalizeL accordingly!!!

    TInt version( aStream.ReadInt32L() );

    // Notice! In the future, some checking maybe required here
    // to be sure that file version is correct and the data can be
    // internalized correctly between different versions.
    if ( version != KVersion )
        {
        // For now, just leave with the corrupt error.
        User::Leave( KErrCorrupt );
        }
	
    TInt length( aStream.ReadInt32L() );
    HBufC* tmpId( NULL );
    if ( length > 0 )
        {
        tmpId = HBufC::NewLC( length );
        TPtr idPtr( tmpId->Des() );
        aStream.ReadL( idPtr, length );        
        }

    length = aStream.ReadInt32L();
    HBufC* tmpNamespace( NULL );
    if ( length > 0 )
        {
        tmpNamespace = HBufC::NewLC( length );
        TPtr namespacePtr( tmpNamespace->Des() );
        aStream.ReadL( namespacePtr, length );        
        }

    TInt tmpUid( aStream.ReadInt32L() );

    if ( tmpNamespace )
        {
        CleanupStack::Pop( tmpNamespace );        
        }

    if ( tmpId )
        {
        CleanupStack::Pop( tmpId );        
        }
    
    delete iId;
    iId = tmpId;

    delete iNamespace;
    iNamespace = tmpNamespace;   

    iUid.iUid = tmpUid;
	}


// -----------------------------------------------------------------------------
// CIAUpdaterIdentifier::ExternalizeL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterIdentifier::ExternalizeL( RWriteStream& aStream )
	{
	// If you make changes here, 
	// remember to update InternalizeL accordingly!!!

    aStream.WriteInt32L( KVersion );

    TInt length( Id().Length() );
    aStream.WriteInt32L( length );
    if ( length > 0 )
        {
        aStream.WriteL( Id(), length );        
        }

    length = Namespace().Length();
    aStream.WriteInt32L( length );
    if ( length > 0 )
        {
        aStream.WriteL( Namespace(), length );        
        }
    
	aStream.WriteInt32L( iUid.iUid );
	}
