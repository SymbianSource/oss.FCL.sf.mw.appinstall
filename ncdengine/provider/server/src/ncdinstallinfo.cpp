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
* Description:   CNcdInstallInfo implementation
*
*/


#include <s32strm.h>

#include "ncdinstallinfo.h"
#include "ncdfileinfo.h"
#include "catalogsdebug.h"
#include "catalogsutils.h"
#include "ncdpanics.h"


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdInstallInfo* CNcdInstallInfo::NewL( CNcdFileInfo* aInfo,
    TNcdInstallType aType )    
    {
    CNcdInstallInfo* self = NewLC( aInfo, aType );
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdInstallInfo* CNcdInstallInfo::NewLC( CNcdFileInfo* aInfo,
    TNcdInstallType aType )
    {    
    CNcdInstallInfo* self = new(ELeave) CNcdInstallInfo( aType );
    CleanupStack::PushL( self );
    self->ConstructL( aInfo );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdInstallInfo* CNcdInstallInfo::NewLC( RReadStream& aStream )
    {    
    CNcdInstallInfo* self = new(ELeave) CNcdInstallInfo( ENcdInstallNormal );
    CleanupStack::PushL( self );
    self->InternalizeL( aStream );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdInstallInfo::~CNcdInstallInfo()
    {
    iFiles.ResetAndDestroy();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdInstallInfo::AddFileInfoL( CNcdFileInfo* aInfo )
    {
    DLTRACEIN((""));
    NCD_ASSERT_ALWAYS( aInfo, ENcdPanicInvalidArgument );
    iFiles.AppendL( aInfo );
    DLTRACEOUT((""));
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CNcdInstallInfo::FileInfoCount() const
    {
    return iFiles.Count();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdFileInfo& CNcdInstallInfo::FileInfo( TInt aIndex )
    {
    NCD_ASSERT( aIndex >= 0 && aIndex < FileInfoCount(), 
        ENcdPanicIndexOutOfRange ); 
    return *iFiles[aIndex];
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const CNcdFileInfo& CNcdInstallInfo::FileInfo( TInt aIndex ) const
    {
    NCD_ASSERT( aIndex >= 0 && aIndex < FileInfoCount(), 
        ENcdPanicIndexOutOfRange ); 
    return *iFiles[aIndex];
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdInstallInfo::TNcdInstallType CNcdInstallInfo::InstallType() const
    {
    return iType;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt32 CNcdInstallInfo::Index() const
    {
    DLTRACEIN(("Index: %d", iIndex ));
    return iIndex;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdInstallInfo::SetIndex( TInt32 aIndex )
    {
    DLTRACEIN(("index: %d", aIndex));
    iIndex = aIndex;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdInstallInfo::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    
    // read type
    iType = static_cast<TNcdInstallType>( aStream.ReadInt32L() );
    iIndex = aStream.ReadInt32L();
    TInt count = aStream.ReadInt32L();
    iFiles.ResetAndDestroy();
    iFiles.ReserveL( count );
    DLINFO(( "File count: %d", count ));
    while ( count )
        {
        DLTRACE(("Internalizing fileinfo"));
        CNcdFileInfo* info = CNcdFileInfo::NewLC( aStream );
        iFiles.AppendL( info );
        CleanupStack::Pop( info );
        --count;
        }
    DLTRACEOUT((""));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdInstallInfo::ExternalizeL( RWriteStream& aStream ) const
    {
    DLTRACEIN((""));
    // write type
    aStream.WriteInt32L( iType );
    aStream.WriteInt32L( iIndex );
    
    TInt count = FileInfoCount();
    DLINFO(( "File count: %d", count ));
    // write file infos
    aStream.WriteInt32L( count );
    
    for ( TInt i = 0; i < count; ++i )
        {
        iFiles[i]->ExternalizeL( aStream );        
        }    
    
    DLTRACEOUT((""));
    }

 
 
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdInstallInfo::ExternalizeWithoutFilenamesL( 
    RWriteStream& aStream ) const
    {
    DLTRACEIN((""));
    // write type
    aStream.WriteInt32L( iType );
    aStream.WriteInt32L( iIndex );
    
    TInt count = FileInfoCount();
    DLINFO(( "File count: %d", count ));
    // write file infos
    aStream.WriteInt32L( count );
    
    for ( TInt i = 0; i < count; ++i )
        {
        iFiles[i]->ExternalizeWithoutFilenamesL( aStream );        
        }    
    
    DLTRACEOUT((""));
    
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdInstallInfo::CNcdInstallInfo( TNcdInstallType aType ) : iType( aType )
    {
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CNcdInstallInfo::ConstructL( CNcdFileInfo* aInfo )
    {        
    if ( aInfo ) 
        {        
        AddFileInfoL( aInfo );    
        }
    }
    
    
