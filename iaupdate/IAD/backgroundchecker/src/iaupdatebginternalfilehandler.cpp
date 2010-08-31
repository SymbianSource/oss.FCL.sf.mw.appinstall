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
* Description:    
*
*/

#include <bautils.h>
#include <s32file.h>
#include <sysversioninfo.h>
#include "iaupdatebglogger.h"
#include "iaupdatebginternalfilehandler.h"


// Constants to create the file.
_LIT( KFile, "bgcheckerinternalfile");
const TInt KDrive( EDriveC );

    

// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::NewL
//
// -----------------------------------------------------------------------------
//
CIAUpdateBGInternalFileHandler* CIAUpdateBGInternalFileHandler::NewL()
    {
    CIAUpdateBGInternalFileHandler* self =
        CIAUpdateBGInternalFileHandler::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::NewLC
//
// -----------------------------------------------------------------------------
//
CIAUpdateBGInternalFileHandler* CIAUpdateBGInternalFileHandler::NewLC()
    {
    CIAUpdateBGInternalFileHandler* self =
        new( ELeave) CIAUpdateBGInternalFileHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::CIAUpdateBGInternalFileHandler
//
// -----------------------------------------------------------------------------
//
CIAUpdateBGInternalFileHandler::CIAUpdateBGInternalFileHandler()
    : iLastTimeShowNewFeatureDialog( 0 ), iUserRejectNewFeatureDialog( EFalse ), 
      iMode( EFirstTimeMode ), iFwVersion( NULL ), 
      iSNID(0), iRetryTimes(0)
    {
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGInternalFileHandler::ConstructL()
    {
    User::LeaveIfError( iFsSession.Connect() );
    User::LeaveIfError( iFsSession.SetSessionToPrivate( KDrive ) );
    
    User::LeaveIfError( iFsSession.SessionPath( iPath ) );
    BaflUtils::EnsurePathExistsL( iFsSession, iPath );
    iPath.Append( KFile );
    
    iFwVersion = HBufC::NewL( KSysVersionInfoTextLength );
    // Read data from the file if the file exists.
    // Otherwise, let default values remain.
    ReadControllerDataL(); 
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::~CIAUpdateBGInternalFileHandler
//
// -----------------------------------------------------------------------------
//
CIAUpdateBGInternalFileHandler::~CIAUpdateBGInternalFileHandler()
    {
    iFsSession.Close();
    delete iFwVersion;
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::ReadControllerDataL
//
// -----------------------------------------------------------------------------
//
TBool CIAUpdateBGInternalFileHandler::ReadControllerDataL()
    {
    FLOG("[bgchecker] ReadControllerDataL");
    RFile file;
    TInt err = file.Open( iFsSession, iPath, EFileRead );
    if ( err == KErrNotFound )
        {
        FLOG("[bgchecker] file doesn't exist");
        // File did not exist. 
        // So, nothing to do here anymore.
        return EFalse;
        }
    FLOG("[bgchecker] file exist");
    User::LeaveIfError( err );
    FLOG("[bgchecker] file no error");
    CleanupClosePushL( file );
    
    RFileReadStream stream( file, 0 );
    CleanupClosePushL( stream );

    InternalizeL( stream );

    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( &file );

    return ETrue;
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::WriteControllerDataL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGInternalFileHandler::WriteControllerDataL()
    {
    RFile file;
    User::LeaveIfError( file.Replace( iFsSession, iPath, EFileWrite ) );
    CleanupClosePushL( file );
    
    RFileWriteStream stream( file, 0 );
    CleanupClosePushL( stream );

    ExternalizeL( stream );
    
    stream.CommitL();
    
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( &file );
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::LastTimeShowNewFeatureDialog()
//
// -----------------------------------------------------------------------------
//
const TTime& CIAUpdateBGInternalFileHandler::LastTimeShowNewFeatureDialog() const
    {
    return iLastTimeShowNewFeatureDialog;
    }
    

// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::SetLastTimeShowNewFeatureDialog
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGInternalFileHandler::SetLastTimeShowNewFeatureDialog( const TTime& aTime )
    {
    iLastTimeShowNewFeatureDialog = aTime;
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::SetLastTimeShowNewFeatureDialog
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGInternalFileHandler::SetLastTimeShowNewFeatureDialog( TInt64 aTime )
    {
    iLastTimeShowNewFeatureDialog = aTime;
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::UserDecision
//
// -----------------------------------------------------------------------------
//
TBool CIAUpdateBGInternalFileHandler::UserRejectNewFeatureDialog() const
    {
    return iUserRejectNewFeatureDialog;
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::SetLanguage
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGInternalFileHandler::SetUserRejectNewFeatureDialog( TBool aUserDecision )
    {
    iUserRejectNewFeatureDialog = aUserDecision;
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::SetMode
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGInternalFileHandler::SetMode( TIAUpdateBGMode aMode )
    {
    iMode = aMode;
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::Mode
//
// -----------------------------------------------------------------------------
//
TIAUpdateBGMode CIAUpdateBGInternalFileHandler::Mode()
    {
    return iMode;
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::FwVersion
//
// -----------------------------------------------------------------------------
//
HBufC* CIAUpdateBGInternalFileHandler::FwVersion()
    {
    return iFwVersion;
    }

// ----------------------------------------------------------
// CIAUpdateBGInternalFileHandler::SetFwVersionL()
// ----------------------------------------------------------
TInt CIAUpdateBGInternalFileHandler::SetFwVersionL( const TDesC& aFwVersion )
    {
    if ( iFwVersion )
        {
        delete iFwVersion;
        iFwVersion = NULL;
        }
    iFwVersion = HBufC::NewL( aFwVersion.Length() );
    TPtr fwptr = iFwVersion->Des();
    fwptr.Copy( aFwVersion );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::SoftNotificationID
//
// -----------------------------------------------------------------------------
//
TInt CIAUpdateBGInternalFileHandler::SoftNotificationID()
    {
    return iSNID;
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::SetSoftNotifcationID
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGInternalFileHandler::SetSoftNotificationID( TInt aID )
    {
    iSNID = aID;
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::RetryTimes
//
// -----------------------------------------------------------------------------
//
TInt CIAUpdateBGInternalFileHandler::RetryTimes()
    {
    return iRetryTimes;
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::SetRetryTimes
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGInternalFileHandler::SetRetryTimes( TInt aRetry )
    {
    iRetryTimes = aRetry;
    }

// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::NrOfIndicatorEntries
//
// -----------------------------------------------------------------------------
//
TInt CIAUpdateBGInternalFileHandler::NrOfIndicatorEntries()
    {
    return iNrOfIndicatiorEntries;
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::SetNrOfIndicatorEntries
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGInternalFileHandler::SetNrOfIndicatorEntries( TInt aEntries )
    {
    iNrOfIndicatiorEntries = aEntries;
    }
// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::InternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGInternalFileHandler::InternalizeL( RReadStream& aStream )
    {
    // If you make changes here, 
    // remember to update ExternalizeL accordingly!!!
    
    TInt64 lastShowTime( 0 );
    aStream >> lastShowTime;
    SetLastTimeShowNewFeatureDialog( lastShowTime );
    
    TInt userDecision( aStream.ReadUint8L() );
    // Static casting is safe to do here because enum and TInt are the same.
    SetUserRejectNewFeatureDialog( static_cast< TBool >( userDecision ) );
    
    TInt mode( aStream.ReadUint8L() );
    SetMode( static_cast<TIAUpdateBGMode> (mode) );
    
    //read the length of fw version string
    TInt fwlength ( aStream.ReadUint8L() );
    
    //read string of fw version
    TPtr fwVer = iFwVersion->Des();
    aStream.ReadL( fwVer, fwlength );
    
    TInt snid ( aStream.ReadUint16L() );
    SetSoftNotificationID( snid ); 
    
    TInt retry ( aStream.ReadUint8L() );
    SetRetryTimes( retry );
    
    TInt entries ( aStream.ReadUint8L() );
    SetNrOfIndicatorEntries( entries );
    
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGInternalFileHandler::ExternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGInternalFileHandler::ExternalizeL( RWriteStream& aStream )
    {
    // If you make changes here, 
    // remember to update InternalizeL accordingly!!!
    
    TInt64 lastshowTime( LastTimeShowNewFeatureDialog().Int64() );
    aStream << lastshowTime;

    TInt userDecision ( UserRejectNewFeatureDialog() );
    aStream.WriteUint8L( userDecision );
    
    TInt mode( Mode() );
    aStream.WriteUint8L( mode );
    
    //write length of fw string
    aStream.WriteUint8L( iFwVersion->Length() );
    // write string of fw version
    aStream.WriteL( *iFwVersion );
    
    TInt snid ( SoftNotificationID() ); 
    aStream.WriteUint16L( snid );
    
    TInt retry ( RetryTimes() );
    aStream.WriteUint8L( retry ); 
    
    TInt entries ( NrOfIndicatorEntries() );
    aStream.WriteUint8L( entries ); 
    
    }

//EOF
