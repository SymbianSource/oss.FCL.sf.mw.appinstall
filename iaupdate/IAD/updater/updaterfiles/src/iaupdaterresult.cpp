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
* Description:   CIAUpdaterResult
*
*/




#include "iaupdaterresult.h"
#include "iaupdateridentifier.h"


// This constant will be used to check if the IAD application
// and iaupdater are using the same version of the file. If the versions
// differ, then this may require additional checking in the future versions
// when data is internalized. For example, IAD may give old version data 
// for iaupdater during self update just after iaupdater has been updated
// and IAD is itself updated after that.
const TInt KVersion( 1 );


// -----------------------------------------------------------------------------
// CIAUpdaterResult::NewL
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdaterResult* CIAUpdaterResult::NewL()
    {
    CIAUpdaterResult* self =
        CIAUpdaterResult::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResult::NewLC
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdaterResult* CIAUpdaterResult::NewLC()
    {
    CIAUpdaterResult* self =
        new( ELeave) CIAUpdaterResult();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResult::CIAUpdaterResult
//
// -----------------------------------------------------------------------------
//
CIAUpdaterResult::CIAUpdaterResult()
: CBase(),
  iErrorCode( KErrNone )
    {
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResult::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIAUpdaterResult::ConstructL()
    {
    iIdentifier = CIAUpdaterIdentifier::NewL();
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResult::~CIAUpdaterResult
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdaterResult::~CIAUpdaterResult()
    {
    delete iIdentifier;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResult::Identifier
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdaterIdentifier& CIAUpdaterResult::Identifier()
    {
    return *iIdentifier;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResult::ErrorCode
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIAUpdaterResult::ErrorCode() const
    {
    return iErrorCode;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResult::SetErrorCode
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterResult::SetErrorCode( const TInt aErrorCode )
    {
    iErrorCode = aErrorCode;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResult::Hidden
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIAUpdaterResult::Hidden() const
    {
    return iHidden;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResult::SetHidden
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterResult::SetHidden( TBool aHidden )
    {
    iHidden = aHidden;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterResult::InternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdaterResult::InternalizeL( RReadStream& aStream )
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

    Identifier().InternalizeL( aStream );

    TInt tmpErrorCode( aStream.ReadInt32L() );
    TBool tmpHidden( aStream.ReadInt32L() );
       
    iErrorCode = tmpErrorCode;
    iHidden = tmpHidden;
	}


// -----------------------------------------------------------------------------
// CIAUpdaterResult::ExternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdaterResult::ExternalizeL( RWriteStream& aStream )
	{
	// If you make changes here, 
	// remember to update InternalizeL accordingly!!!

    aStream.WriteInt32L( KVersion );

    Identifier().ExternalizeL( aStream );

	aStream.WriteInt32L( iErrorCode );
	aStream.WriteInt32L( iHidden );
	}
