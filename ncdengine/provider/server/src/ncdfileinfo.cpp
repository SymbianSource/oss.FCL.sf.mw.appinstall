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
* Description:   CNcdFileInfo implementation
*
*/


#include "ncdfileinfo.h"
#include "catalogsdebug.h"
#include "catalogsutils.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdFileInfo* CNcdFileInfo::NewLC( 
    const TDesC& aFilePath,
    const TDesC& aMimeType, 
    TNcdItemPurpose aPurpose )
    {
    CNcdFileInfo* self = new(ELeave) CNcdFileInfo( aPurpose );
    CleanupStack::PushL( self );  
    self->ConstructL( aFilePath, aMimeType );
    return self;
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdFileInfo* CNcdFileInfo::NewLC( RReadStream& aStream )
    {
    CNcdFileInfo* self = new(ELeave) CNcdFileInfo( 
        ENcdItemPurposeUnknown );
    CleanupStack::PushL( self );  
    self->InternalizeL( aStream );
    return self;
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdFileInfo::~CNcdFileInfo()
    {
    DLTRACEIN((""));
    delete iFilePath;
    delete iMimeType;
    delete iData;
    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC& CNcdFileInfo::FilePath() const
    {
    return *iFilePath;
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC& CNcdFileInfo::MimeType() const
    {
    return *iMimeType;
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdFileInfo::SetMimeTypeL( const TDesC& aMime )
    {
    delete iMimeType;
    iMimeType = NULL;
    iMimeType = aMime.AllocL();
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TNcdItemPurpose CNcdFileInfo::Purpose() const
    {
    return iPurpose;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdFileInfo::SetDataL( HBufC8* aData )
    {
    delete iData;
    iData = aData;
    if ( !iData ) 
        {
        iData = KNullDesC8().AllocL();
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC8& CNcdFileInfo::Data() const
    {    
    return *iData;
    }
        
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdFileInfo::ExternalizeL( RWriteStream& aStream ) const
    {
    DLTRACEIN((""));
    
    DLINFO(( "Purpose: %d", iPurpose ));
    DLINFO(( _L("Filepath: %S"), iFilePath ));
    DLINFO(( _L("Mimetype: %S"), iMimeType));
    aStream.WriteInt32L( iPurpose );
    ExternalizeDesL( *iFilePath, aStream );
    ExternalizeDesL( *iMimeType, aStream );
    ExternalizeDesL( *iData, aStream );
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdFileInfo::ExternalizeWithoutFilenamesL( 
    RWriteStream& aStream ) const
    {
    DLTRACEIN((""));
    aStream.WriteInt32L( iPurpose );
    ExternalizeDesL( KNullDesC, aStream );
    ExternalizeDesL( *iMimeType, aStream );
    ExternalizeDesL( KNullDesC8, aStream );
    DLTRACEOUT((""));
    }



    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdFileInfo::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    iPurpose = static_cast<TNcdItemPurpose>( aStream.ReadInt32L() );
    InternalizeDesL( iFilePath, aStream );
    InternalizeDesL( iMimeType, aStream );
    InternalizeDesL( iData, aStream );

    DLINFO(( "Purpose: %d", iPurpose ));
    DLINFO(( _L("Filepath: %S"), iFilePath ));
    DLINFO(( _L("Mimetype: %S"), iMimeType));

    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdFileInfo::CNcdFileInfo( TNcdItemPurpose aPurpose ) :
    iPurpose( aPurpose )
    {
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdFileInfo::ConstructL( const TDesC& aFilePath,
    const TDesC& aMimeType )
    {
    iFilePath = aFilePath.AllocL();
    iMimeType = aMimeType.AllocL();
    iData = KNullDesC8().AllocL();
    }
