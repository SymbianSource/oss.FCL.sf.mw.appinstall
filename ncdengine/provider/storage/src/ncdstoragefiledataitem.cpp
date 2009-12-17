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


#include "ncdstoragefiledataitem.h"

#include <s32file.h>

#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CNcdStorageFileDataItem::ConstructL( const TDesC& aFilename )
    {
    if ( !aFilename.Length() ) 
        {
        User::Leave( KErrArgument );
        }
        
    iFilename.CreateL( aFilename );
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdStorageFileDataItem* CNcdStorageFileDataItem::NewL( 
    const TDesC& aFilename, RFs& aFs )
    {
    CNcdStorageFileDataItem* self = CNcdStorageFileDataItem::NewLC( 
        aFilename, aFs );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CNcdStorageFileDataItem* CNcdStorageFileDataItem::NewLC( const TDesC& 
    aFilename, RFs& aFs )
    {
    CNcdStorageFileDataItem* self = new( ELeave ) CNcdStorageFileDataItem(
        aFs );
    CleanupStack::PushL( self );
    self->ConstructL( aFilename );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdStorageFileDataItem::~CNcdStorageFileDataItem()
    {
    iFilename.Close();
    }


// ---------------------------------------------------------------------------
// ExternalizeL
// ---------------------------------------------------------------------------
//
void CNcdStorageFileDataItem::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN(( _L("Externalizing: %S"), &iFilename ));
        
#ifdef CATALOGS_BUILD_CONFIG_DEBUG
    RFile file;
    CleanupClosePushL( file );
    User::LeaveIfError( file.Open( iFs, iFilename, EFileRead ) );
    TInt fileSize = -1;
    User::LeaveIfError( file.Size( fileSize ) );
    CleanupStack::PopAndDestroy( &file );
    DLINFO(("File size: %d", fileSize ));
#endif
    
    RFileReadStream rstream;
    CleanupClosePushL( rstream );
    
    DLTRACE(("Opening file"));    
    User::LeaveIfError( rstream.Open( iFs, iFilename, 
        EFileRead | EFileStream | EFileShareReadersOnly ) );
    
    DLTRACE(("Externalizing data"));
    aStream.WriteL( rstream );
    CleanupStack::PopAndDestroy( &rstream );
    
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// InternalizeL
// ---------------------------------------------------------------------------
//
void CNcdStorageFileDataItem::InternalizeL( RReadStream& /* aStream */ )
    {
    User::Leave( KErrNotSupported );
    }

    
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdStorageFileDataItem::CNcdStorageFileDataItem( RFs& aFs ) : iFs( aFs )
    {
    }

    
