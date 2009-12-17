/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "iaupdatefwversionfilehandler.h"
#include "iaupdatedebug.h"


// Constants to create the file.
_LIT( KFile, "fwversion");
const TInt KDrive( EDriveC );

    

// -----------------------------------------------------------------------------
// CIAUpdateFwVersionFileHandler::NewL
//
// -----------------------------------------------------------------------------
//
CIAUpdateFwVersionFileHandler* CIAUpdateFwVersionFileHandler::NewL()
    {
    CIAUpdateFwVersionFileHandler* self =
        CIAUpdateFwVersionFileHandler::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateFwVersionFileHandler::NewLC
//
// -----------------------------------------------------------------------------
//
CIAUpdateFwVersionFileHandler* CIAUpdateFwVersionFileHandler::NewLC()
    {
    CIAUpdateFwVersionFileHandler* self =
        new( ELeave) CIAUpdateFwVersionFileHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }


// -----------------------------------------------------------------------------
// CIAUpdateFwVersionFileHandler::CIAUpdateFwVersionFileHandler
//
// -----------------------------------------------------------------------------
//
CIAUpdateFwVersionFileHandler::CIAUpdateFwVersionFileHandler()
    : iFwVersion( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CIAUpdateFwVersionFileHandler::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateFwVersionFileHandler::ConstructL()
    {
    User::LeaveIfError( iFsSession.Connect() );
    User::LeaveIfError( iFsSession.SetSessionToPrivate( KDrive ) );
    
    User::LeaveIfError( iFsSession.SessionPath( iPath ) );
    BaflUtils::EnsurePathExistsL( iFsSession, iPath );
    iPath.Append( KFile );
       
    }


// -----------------------------------------------------------------------------
// CIAUpdateFwVersionFileHandler::~CIAUpdateFwVersionFileHandler
//
// -----------------------------------------------------------------------------
//
CIAUpdateFwVersionFileHandler::~CIAUpdateFwVersionFileHandler()
    {
    iFsSession.Close();
    delete iFwVersion;
    }


// -----------------------------------------------------------------------------
// CIAUpdateFwVersionFileHandler::ReadControllerDataL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateFwVersionFileHandler::ReadControllerDataL()
    {
    RFile file;
    TInt err = file.Open( iFsSession, iPath, EFileRead|EFileShareAny );
    if ( err == KErrNotFound )
        {
       // File did not exist. 
        // So, nothing to do here anymore.
        return;
        }
    User::LeaveIfError( err );
    CleanupClosePushL( file );
    
    RFileReadStream stream( file, 0 );
    CleanupClosePushL( stream );

    InternalizeL( stream );

    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( &file );
    }


// -----------------------------------------------------------------------------
// CIAUpdateFwVersionFileHandler::WriteControllerDataL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateFwVersionFileHandler::WriteControllerDataL()
    {
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
// CIAUpdateFwVersionFileHandler::FwVersionL
//
// -----------------------------------------------------------------------------
//
HBufC* CIAUpdateFwVersionFileHandler::FwVersionL()
    {
    if ( iFwVersion )
        {
        delete iFwVersion;
        iFwVersion = NULL;
        }
    iFwVersion = HBufC::NewL( KSysVersionInfoTextLength );
    // Read data from the file if the file exists.
    // Otherwise, let default values remain.
    ReadControllerDataL(); 
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateFwVersionFileHandler::FwVersionL() Stored firmware version: %S", iFwVersion );
    return iFwVersion;
    }


// ----------------------------------------------------------
// CIAUpdateFwVersionFileHandler::SetFwVersionL()
// ----------------------------------------------------------
void CIAUpdateFwVersionFileHandler::SetFwVersionL( const TDesC& aFwVersion )
    {
    IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateFwVersionFileHandler::SetFwVersionL() New firmware version: %S", &aFwVersion );
    if ( iFwVersion )
        {
        delete iFwVersion;
        iFwVersion = NULL;
        }
    iFwVersion = HBufC::NewL( aFwVersion.Length() );
    TPtr fwptr = iFwVersion->Des();
    fwptr.Copy( aFwVersion );
    WriteControllerDataL(); 
    }


// -----------------------------------------------------------------------------
// CIAUpdateFwVersionFileHandler::InternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateFwVersionFileHandler::InternalizeL( RReadStream& aStream )
    {
    // If you make changes here, 
       // remember to update ExternalizeL accordingly!!!
    //read the length of fw version string
    TInt fwlength ( aStream.ReadUint8L() );
    
    //read string of fw version
    TPtr fwVer = iFwVersion->Des();
    aStream.ReadL( fwVer, fwlength );
    }


// -----------------------------------------------------------------------------
// CIAUpdateFwVersionFileHandler::ExternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateFwVersionFileHandler::ExternalizeL( RWriteStream& aStream )
    {
    // If you make changes here, 
    // remember to update InternalizeL accordingly!!!
     
    //write length of fw string
    aStream.WriteUint8L( iFwVersion->Length() );
    // write string of fw version
    aStream.WriteL( *iFwVersion );
    }

//EOF
