/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CIAUpdateFirstTimeInfo class 
*                member functions.
*
*/



//INCLUDES
#include "iaupdatefirsttimeinfo.h"
#include "iaupdatefirsttimedatefile.h"

#include <bautils.h>  // bafl.lib 
#include <s32file.h>  // estor.lib
#include <sysutil.h>

//CONSTANTS
const TInt KIAUpdateFirstTimeDrive( EDriveC );
const TInt KSizeOfFile( 12 );
const TTimeIntervalDays KFirstTimeDelayInDays( 14 );

//MACROS
_LIT( KIAUpdateFirstTimeFile, "IADFirstTimeState");
_LIT( KIAUpdateFirstTimeDateFile, "IADFirstTimeDate");

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CIAUpdateFirstTimeInfo::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIAUpdateFirstTimeInfo* CIAUpdateFirstTimeInfo::NewL()
    {
    CIAUpdateFirstTimeInfo* self = CIAUpdateFirstTimeInfo::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateUiController::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateFirstTimeInfo* CIAUpdateFirstTimeInfo::NewLC()
    {
    CIAUpdateFirstTimeInfo* self = new( ELeave ) CIAUpdateFirstTimeInfo();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CIAUpdateFirstTimeInfo::ConstructL
// ---------------------------------------------------------------------------
//
void CIAUpdateFirstTimeInfo::ConstructL()
    {
    User::LeaveIfError( iFsSession.Connect() );
    User::LeaveIfError( iFsSession.SetSessionToPrivate( KIAUpdateFirstTimeDrive ) );
    
    // sessionpath in emulator: '\epoc32\winscw\c\private\2000F85A'
    User::LeaveIfError( iFsSession.SessionPath( iPath ) );
    BaflUtils::EnsurePathExistsL( iFsSession, iPath );
    iPath.Append( KIAUpdateFirstTimeFile );
    }    

// ---------------------------------------------------------------------------
// CIAUpdateFirstTimeInfo::CIAUpdateFirstTimeInfo
// constructor
// ---------------------------------------------------------------------------
//
CIAUpdateFirstTimeInfo::CIAUpdateFirstTimeInfo()  
    {
    }

// ---------------------------------------------------------------------------
// CIAUpdateFirstTimeInfo::~CIAUpdateFirstTimeInfo
// Destructor
// ---------------------------------------------------------------------------
//
CIAUpdateFirstTimeInfo::~CIAUpdateFirstTimeInfo()
    {
    iFsSession.Close();
    }

// ---------------------------------------------------------------------------
// CIAUpdateFirstTimeInfo::SetAgreementAcceptedL
// Set Nokia agreement as accepted by an user
// ---------------------------------------------------------------------------
//
void CIAUpdateFirstTimeInfo::SetAgreementAcceptedL()
    {
    ReadDataL();
    iAgreementAccepted = ETrue;
    WriteDataL();
    }
    
// ---------------------------------------------------------------------------
// CIAUpdateFirstTimeInfo::SetAgreementAskedL
// Set Nokia agreement as asked (prompted) to an user
// ---------------------------------------------------------------------------
//
void CIAUpdateFirstTimeInfo::SetAgreementAskedL()
    {
    ReadDataL();
    iAgreementAsked = ETrue;
    WriteDataL();
    }
        
// ---------------------------------------------------------------------------
// CIAUpdateFirstTimeInfo::SetAutomaticUpdatesAskedL
// Set automatic update checks as prompted to an use
// ---------------------------------------------------------------------------
//
void CIAUpdateFirstTimeInfo::SetAutomaticUpdatesAskedL()
    {
	ReadDataL();
	iAutomaticUpdateChecksAsked = ETrue;
    WriteDataL();
    }
    
// ---------------------------------------------------------------------------
// CIAUpdateFirstTimeInfo::SetFirstTimeIfNotSetL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateFirstTimeInfo::SetFirstTimeIfNotSetL()
    {
	if ( !AutomaticUpdateChecksAskedL() )
	    {
		CIAUpdateFirstTimeDateFile* firstTimeDateFile = 
		      CIAUpdateFirstTimeDateFile::NewLC( KIAUpdateFirstTimeDateFile );
		if ( firstTimeDateFile->FirstTime().Int64() == 0 )
		    {
			firstTimeDateFile->SetCurrentFirstTime();
			firstTimeDateFile->WriteDataL();
		    }
		CleanupStack::PopAndDestroy( firstTimeDateFile );   
	    }
    }

// ---------------------------------------------------------------------------
// CIAUpdateFirstTimeInfo::AgreementAcceptedL
// Is Nokia agreement of Application Update accepted by an user
// ---------------------------------------------------------------------------
//
TBool CIAUpdateFirstTimeInfo::AgreementAcceptedL()
    {
    ReadDataL();
    return iAgreementAccepted;
    }


// ---------------------------------------------------------------------------
// CIAUpdateFirstTimeInfo::AgreementAskedL
// Is Nokia agreement of Application Update already asked 
// ---------------------------------------------------------------------------
//
TBool CIAUpdateFirstTimeInfo::AgreementAskedL()
    {
	ReadDataL();
	return iAgreementAsked;
    }

// ---------------------------------------------------------------------------
// CIAUpdateFirstTimeInfo::AutomaticUpdateChecksAskedL
// Is activation for automatic update cheks from network already asked 
// ---------------------------------------------------------------------------
//    
TBool CIAUpdateFirstTimeInfo::AutomaticUpdateChecksAskedL()
    {
    ReadDataL();
    return iAutomaticUpdateChecksAsked;
    }

// ---------------------------------------------------------------------------
// CIAUpdateFirstTimeInfo::FirstTimeDelayL
// 
// ---------------------------------------------------------------------------
//    
TBool CIAUpdateFirstTimeInfo::FirstTimeDelayL()
    {
    TBool firstTimeDelay = EFalse;
    if ( !AutomaticUpdateChecksAskedL() )
	    {
		CIAUpdateFirstTimeDateFile* firstTimeDateFile = 
		      CIAUpdateFirstTimeDateFile::NewL( KIAUpdateFirstTimeDateFile );
    
        TTime firstTime( firstTimeDateFile->FirstTime() );
  
        delete firstTimeDateFile;
  
        TTime expireTime( firstTime );

        expireTime += KFirstTimeDelayInDays;
    
        TTime universalTime;
        universalTime.UniversalTime();

        if ( universalTime < expireTime && universalTime >= firstTime )
            {
        	firstTimeDelay = ETrue;
            }
	    }
    return firstTimeDelay;	
    }

// -----------------------------------------------------------------------------
// CIAUpdateFirstTimeInfo::ReadDataL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateFirstTimeInfo::ReadDataL()
	{
	RFile file;
    TInt err = file.Open( iFsSession, iPath, EFileRead|EFileShareAny );
    if ( err == KErrNotFound )
    	{
    	iAgreementAccepted = EFalse;
	    iAgreementAsked = EFalse;
	    iAutomaticUpdateChecksAsked = EFalse; 
    	}
    else
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
// CIAUpdateFirstTimeInfo::WriteDataL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateFirstTimeInfo::WriteDataL()
	{
	TDriveUnit driveUnit( KIAUpdateFirstTimeDrive );
	if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFsSession, KSizeOfFile, driveUnit ) )
	    {
		User::Leave( KErrDiskFull );
	    }
	RFile file;
    User::LeaveIfError( file.Replace( iFsSession, iPath, EFileWrite|EFileShareAny ) );
    CleanupClosePushL( file );
    
    RFileWriteStream stream( file, 0 );
    CleanupClosePushL( stream );

    ExternalizeL( stream );
    stream.CommitL();
        
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( &file );
	}

    
// -----------------------------------------------------------------------------
// CIAUpdateFirstTimeInfo::InternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateFirstTimeInfo::InternalizeL( RReadStream& aStream )
	{
	iAgreementAccepted = aStream.ReadInt32L();
	iAgreementAsked = aStream.ReadInt32L();
	iAutomaticUpdateChecksAsked = aStream.ReadInt32L();
	}


// -----------------------------------------------------------------------------
// CIAUpdateFirstTimeInfo::ExternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateFirstTimeInfo::ExternalizeL( RWriteStream& aStream )
	{
	aStream.WriteInt32L( iAgreementAccepted );
	aStream.WriteInt32L( iAgreementAsked );	
	aStream.WriteInt32L( iAutomaticUpdateChecksAsked );
	}
    
// End of File  
