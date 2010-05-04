/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES

#include "iaupdatequeryhistory.h"
#include "iaupdatedebug.h"

#include <bautils.h>  // bafl.lib 
#include <s32file.h>  // estor.lib
#include <sysutil.h>

// CONSTANTS
const TInt KHistoryDrive( EDriveC );
const TInt KHistoryItemSize( 12 );

// MACROS
_LIT( KHistoryFile, "IADQueryHistory" );


/*******************************************************************************
 * class TIAUpdateQueryHistoryItem
 *******************************************************************************/


// -----------------------------------------------------------------------------
// TIAUpdateQueryHistoryItem::Time
//
// -----------------------------------------------------------------------------
//
TTime TIAUpdateQueryHistoryItem::Time()
	{
	return iTime;
	}


// -----------------------------------------------------------------------------
// TIAUpdateQueryHistoryItem::SetTime
//
// -----------------------------------------------------------------------------
//
void TIAUpdateQueryHistoryItem::SetTime( TTime aTime )
	{
	iTime = aTime;
	}


// -----------------------------------------------------------------------------
// TIAUpdateQueryHistoryItem::InternalizeL
//
// -----------------------------------------------------------------------------
//
void TIAUpdateQueryHistoryItem::InternalizeL( RReadStream& aStream )
	{
	iUid = aStream.ReadUint32L();

	TInt64 temp = 0;
	aStream >> temp;
	
	if ( temp <= 0 )
	    {
	    User::Leave( KErrCorrupt );
	    }
	TTime time( temp );
	iTime = time;
	}


// -----------------------------------------------------------------------------
// TIAUpdateQueryHistoryItem::ExternalizeL
//
// -----------------------------------------------------------------------------
//
void TIAUpdateQueryHistoryItem::ExternalizeL( RWriteStream& aStream )
	{
	aStream.WriteUint32L( iUid );
	aStream << iTime.Int64();
	}




	
/*******************************************************************************
 * class CIAUpdateQueryHistory
 *******************************************************************************/


// -----------------------------------------------------------------------------
// CIAUpdateQueryHistory::NewL
//
// -----------------------------------------------------------------------------
//
CIAUpdateQueryHistory* CIAUpdateQueryHistory::NewL()
    {
    CIAUpdateQueryHistory* self = new (ELeave) CIAUpdateQueryHistory();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return(self);
    }


// -----------------------------------------------------------------------------
// CIAUpdateQueryHistory::CIAUpdateQueryHistory
// 
// -----------------------------------------------------------------------------
//
CIAUpdateQueryHistory::CIAUpdateQueryHistory()
: iDelay( KDefaultDelayHours )
	{
    }


// ----------------------------------------------------------------------------
// Destructor
//
// ----------------------------------------------------------------------------
//
CIAUpdateQueryHistory::~CIAUpdateQueryHistory()
    {
    iList.Close();
    }


// -----------------------------------------------------------------------------
// CIAUpdateQueryHistory::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateQueryHistory::ConstructL()
    {
    } 

// -----------------------------------------------------------------------------
// CIAUpdateQueryHistory::SetTimeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateQueryHistory::SetTimeL( TUint aUid )
    {
    
    
    RFs fsSession;
    HBufC* path = HBufC::NewLC( KMaxFileName );
    TPtr pathPtr = path->Des();
    
    ConnectLC( fsSession, pathPtr );
    
    ReadHistoryL( fsSession, *path );
    
    TTime currentTime;
    currentTime.UniversalTime();
    
    TIAUpdateQueryHistoryItem item;
    item.iUid = aUid;
    item.SetTime( currentTime );
    AddItemL( item );
    
    WriteHistoryL( fsSession, *path );
      
    CleanupStack::PopAndDestroy( &fsSession ); 
    CleanupStack::PopAndDestroy( path ); 
        

    }



// -----------------------------------------------------------------------------
// CIAUpdateQueryHistory::SetDelay
//
// -----------------------------------------------------------------------------
//
void CIAUpdateQueryHistory::SetDelay( TInt aHours )
    {
    iDelay = aHours;
    }



// -----------------------------------------------------------------------------
// CIAUpdateQueryHistory::IsDelayedL
//
// -----------------------------------------------------------------------------
//
TBool CIAUpdateQueryHistory::IsDelayedL( TUint aUid )
    {
    RFs fsSession;
    HBufC* path = HBufC::NewLC( KMaxFileName );
    TPtr pathPtr = path->Des();
    
    ConnectLC( fsSession, pathPtr );
    
    ReadHistoryL( fsSession, *path );
       
    CleanupStack::PopAndDestroy( &fsSession ); 
    CleanupStack::PopAndDestroy( path );
    
    TInt index = ItemIndex( aUid );
    if ( index == KErrNotFound )
        {
        return EFalse; // no history data for this uid
        }

    TIAUpdateQueryHistoryItem& item = Item( index );

    return IsDelayed( item );
    }


// -----------------------------------------------------------------------------
// CIAUpdateQueryHistory::ConnectLC
//
// -----------------------------------------------------------------------------
//
void CIAUpdateQueryHistory::ConnectLC( RFs& aFsSession, TDes& aPath )
    {
    User::LeaveIfError( aFsSession.Connect() );
    CleanupClosePushL( aFsSession );
    User::LeaveIfError( aFsSession.SetSessionToPrivate( KHistoryDrive ) );
    // sessionpath in emulator: '\epoc32\winscw\c\private\2000F85A'
    User::LeaveIfError( aFsSession.SessionPath( aPath ) );
    BaflUtils::EnsurePathExistsL( aFsSession, aPath );
    aPath.Append( KHistoryFile );
    }

// -----------------------------------------------------------------------------
// CIAUpdateQueryHistory::InternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateQueryHistory::InternalizeL( RReadStream& aStream )
	{
	TInt count = aStream.ReadInt32L();
	if ( count < 0 )
	    {
	    User::Leave( KErrCorrupt );
	    }
	
	for (TInt i = 0; i< count; i++ )
		{
		TIAUpdateQueryHistoryItem item;
		item.InternalizeL( aStream );
		iList.AppendL( item );
		}
	}


// -----------------------------------------------------------------------------
// CIAUpdateQueryHistory::ExternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateQueryHistory::ExternalizeL( RWriteStream& aStream )
	{
	TInt count = iList.Count();
	aStream.WriteInt32L( count );
		
	for (TInt i = 0; i < count; i++ )
		{
		TIAUpdateQueryHistoryItem& item = iList[i];
		item.ExternalizeL( aStream );
		}
	}


// -----------------------------------------------------------------------------
// CIAUpdateQueryHistory::ReadHistoryL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateQueryHistory::ReadHistoryL( RFs& aFsSession, const TDesC& aPath )
	{
	iList.Reset();
	RFile file;
    TInt err = file.Open( aFsSession, aPath, EFileRead|EFileShareAny );
    if ( err != KErrNotFound )
    	{
    	User::LeaveIfError( err );
    	CleanupClosePushL( file );
    
        RFileReadStream stream( file, 0 );
        CleanupClosePushL( stream );

        InternalizeL( stream );

        CleanupStack::PopAndDestroy( &stream );
        CleanupStack::PopAndDestroy( &file );
    	}
 	}



// -----------------------------------------------------------------------------
// CIAUpdateQueryHistory::WriteHistoryL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateQueryHistory::WriteHistoryL( RFs& aFsSession, const TDesC& aPath )
	{
	
	TDriveUnit driveUnit( KHistoryDrive );
	if ( SysUtil::DiskSpaceBelowCriticalLevelL( &aFsSession, KHistoryItemSize, driveUnit ) )
	    {
		User::Leave( KErrDiskFull );
	    }
	RFile file;
    User::LeaveIfError( file.Replace( aFsSession, aPath, EFileWrite|EFileShareAny ) );
    CleanupClosePushL( file );
    
    RFileWriteStream stream( file, 0 );
    CleanupClosePushL( stream );

    ExternalizeL( stream );
        
    stream.CommitL();
    
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( &file );
	}
    




// -----------------------------------------------------------------------------
// CIAUpdateQueryHistory::IsDelayed
//
// -----------------------------------------------------------------------------
//
TBool CIAUpdateQueryHistory::IsDelayed( TIAUpdateQueryHistoryItem& aItem )
    {
    TTime currentTime;
    currentTime.UniversalTime();

    TTime lastQueryTime = aItem.Time();
        
    TTimeIntervalHours hours;
    TInt err = currentTime.HoursFrom( lastQueryTime, hours );
    if ( err != KErrNone )
        {
        return EFalse;
        }
    
    TInt hourCount = hours.Int();
    if ( hourCount >= 0 && hourCount < iDelay )
        {
        return ETrue;
        }
        
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CIAUpdateQueryHistory::AddItemL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateQueryHistory::AddItemL( TIAUpdateQueryHistoryItem aItem )
	{
	TInt index = ItemIndex( aItem.iUid );
	if ( index != KErrNotFound )
		{
		iList.Remove( index );
		}

	User::LeaveIfError( iList.Append( aItem ) );
	
	}


// -----------------------------------------------------------------------------
// CIAUpdateQueryHistory::ItemIndex
//
// -----------------------------------------------------------------------------
//
TInt CIAUpdateQueryHistory::ItemIndex( TUint aUid )
	{
	TInt count = iList.Count();
	
	for (TInt i=0; i < count; i++)
		{
		TIAUpdateQueryHistoryItem& item= iList[i];
		if ( item.iUid == aUid )
			{
			return i;
			}
		}
		
	return KErrNotFound;
	}


// -----------------------------------------------------------------------------
// CIAUpdateQueryHistory::Item
//
// -----------------------------------------------------------------------------
//
TIAUpdateQueryHistoryItem& CIAUpdateQueryHistory::Item( TInt aIndex )
	{
	return iList[aIndex];
	}

    
//  End of File  
