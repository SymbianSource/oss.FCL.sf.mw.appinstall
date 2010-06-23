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
* Description:   CIAUpdaterFileList
*
*/




#include "iaupdaterfilelist.h"
#include "iaupdaterfileinfo.h"
#include "iaupdateridentifier.h"


// This constant will be used to check if the IAD application
// and iaupdater are using the same version of the file. If the versions
// differ, then this may require additional checking in the future versions
// when data is internalized. For example, IAD may give old version data 
// for iaupdater during self update just after iaupdater has been updated
// and IAD is itself updated after that.
const TInt KVersion( 1 );


// -----------------------------------------------------------------------------
// CIAUpdaterFileList::NewL
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdaterFileList* CIAUpdaterFileList::NewL()
    {
    CIAUpdaterFileList* self =
        CIAUpdaterFileList::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileList::NewLC
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdaterFileList* CIAUpdaterFileList::NewLC()
    {
    CIAUpdaterFileList* self =
        new( ELeave) CIAUpdaterFileList();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileList::CIAUpdaterFileList
//
// -----------------------------------------------------------------------------
//
CIAUpdaterFileList::CIAUpdaterFileList()
: CBase()
    {
    
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileList::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIAUpdaterFileList::ConstructL()
    {
    iIdentifier = CIAUpdaterIdentifier::NewL();
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileList::~CIAUpdaterFileList
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdaterFileList::~CIAUpdaterFileList()
    {
    Reset();

    delete iIdentifier;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileList::Reset
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterFileList::Reset()
    {
    iIdentifier->Reset();

    delete iBundleName;
    iBundleName = NULL;

    iHidden = EFalse;    

    iFileInfos.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileList::Identifier
//
// -----------------------------------------------------------------------------
//
EXPORT_C CIAUpdaterIdentifier& CIAUpdaterFileList::Identifier()
    {
    return *iIdentifier;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileList::BundleName
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIAUpdaterFileList::BundleName() const
    {
    if ( !iBundleName )
        {
        return KNullDesC();
        }
        
    return *iBundleName;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileList::SetBundleName
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterFileList::SetBundleNameL( const TDesC& aName )
    {
    HBufC* tmp( aName.AllocL() );
    delete iBundleName;
    iBundleName = tmp;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileList::Hidden
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIAUpdaterFileList::Hidden() const
    {
    return iHidden;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileList::SetHidden
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterFileList::SetHidden( TBool aHidden )
    {
    iHidden = aHidden;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileList::FileList
//
// -----------------------------------------------------------------------------
//
EXPORT_C const RPointerArray< CIAUpdaterFileInfo >& CIAUpdaterFileList::FileInfos() const
    {
    return iFileInfos;
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileList::AddFileInfoL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterFileList::AddFileInfoL( CIAUpdaterFileInfo* aInfo )
    {
    iFileInfos.AppendL( aInfo );
    }

// -----------------------------------------------------------------------------
// CIAUpdaterFileList::RemoveFileInfo
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIAUpdaterFileList::RemoveFileInfo( TInt aIndex )
    {
    delete iFileInfos[ aIndex ];
    iFileInfos.Remove( aIndex );
    }


// -----------------------------------------------------------------------------
// CIAUpdaterFileList::InternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdaterFileList::InternalizeL( RReadStream& aStream )
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
    HBufC* tmpBundleName( NULL );
	if ( length > 0 )
	    {
    	tmpBundleName = HBufC::NewLC( length );
    	TPtr bundleNamePtr( tmpBundleName->Des() );
        aStream.ReadL( bundleNamePtr, length );
	    }

    TBool tmpHidden( aStream.ReadInt32L() );

    Identifier().InternalizeL( aStream );
		
	CleanupStack::Pop( tmpBundleName );	    

    delete iBundleName;
	iBundleName = tmpBundleName;
    
    iHidden = tmpHidden;

    iFileInfos.ResetAndDestroy();
	TInt count( aStream.ReadInt32L() );
	for( TInt i = 0; i < count; ++i )
	    {
	    CIAUpdaterFileInfo* info( CIAUpdaterFileInfo::NewLC() );
	    info->InternalizeL( aStream );
	    iFileInfos.AppendL( info );
	    CleanupStack::Pop( info );
	    }	
	}


// -----------------------------------------------------------------------------
// CIAUpdaterFileList::ExternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdaterFileList::ExternalizeL( RWriteStream& aStream )
	{
	// If you make changes here, 
	// remember to update InternalizeL accordingly!!!

    aStream.WriteInt32L( KVersion );

    TInt length = BundleName().Length();
    aStream.WriteInt32L( length );
    if ( length > 0 )
        {
        aStream.WriteL( BundleName(), length );        
        }
    
    aStream.WriteInt32L( Hidden() );

    Identifier().ExternalizeL( aStream );

    TInt count( FileInfos().Count() );
	aStream.WriteInt32L( count );
	
	for( TInt i = 0; i < count; ++i )
	    {
	    CIAUpdaterFileInfo* info( FileInfos()[ i ] );
	    info->ExternalizeL( aStream );
	    }
	}
