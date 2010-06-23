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
* Description:   CIAUpdaterFileInfo
*
*/




#include "iaupdaterfileinfo.h"


// This constant will be used to check if the IAD application
// and iaupdater are using the same version of the file. If the versions
// differ, then this may require additional checking in the future versions
// when data is internalized. For example, IAD may give old version data 
// for iaupdater during self update just after iaupdater has been updated
// and IAD is itself updated after that.
const TInt KVersion( 1 );


// -----------------------------------------------------------------------------
// CIAUpdaterFileInfo::NewL
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdaterFileInfo* CIAUpdaterFileInfo::NewL()
    {
    CIAUpdaterFileInfo* self =
        CIAUpdaterFileInfo::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileInfo::NewLC
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdaterFileInfo* CIAUpdaterFileInfo::NewLC()
    {
    CIAUpdaterFileInfo* self =
        new( ELeave) CIAUpdaterFileInfo();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileInfo::CIAUpdaterFileInfo
//
// -----------------------------------------------------------------------------
//
CIAUpdaterFileInfo::CIAUpdaterFileInfo()
: CBase()
    {
    
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileInfo::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIAUpdaterFileInfo::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileInfo::~CIAUpdaterFileInfo
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdaterFileInfo::~CIAUpdaterFileInfo()
    {
    delete iFilePath;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileInfo::FilePath
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIAUpdaterFileInfo::FilePath() const
    {
    if ( !iFilePath )
        {
        return KNullDesC();
        }
        
    return *iFilePath;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileInfo::SetFilePathL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterFileInfo::SetFilePathL( const TDesC& aPath )
    {
    HBufC* tmp( aPath.AllocL() );
    delete iFilePath;
    iFilePath = tmp;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileInfo::InternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdaterFileInfo::InternalizeL( RReadStream& aStream )
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
    HBufC* tmpPath( NULL );     
    if ( length > 0 )
        {
        // Only create new path if it has some data.
        // Otherwise, just delete it later.
        tmpPath = HBufC::NewLC( length );
        TPtr ptrPath( tmpPath->Des() );
        aStream.ReadL( ptrPath, length );
        CleanupStack::Pop( tmpPath );        
        }

    delete iFilePath;
    iFilePath = tmpPath;
	}


// -----------------------------------------------------------------------------
// CIAUpdaterFileInfo::ExternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdaterFileInfo::ExternalizeL( RWriteStream& aStream )
	{
	// If you make changes here, 
	// remember to update InternalizeL accordingly!!!

    aStream.WriteInt32L( KVersion );

    TInt length( FilePath().Length() );
    aStream.WriteInt32L( length );
    if ( length > 0 )
        {
        aStream.WriteL( FilePath(), length );        
        }
	}
