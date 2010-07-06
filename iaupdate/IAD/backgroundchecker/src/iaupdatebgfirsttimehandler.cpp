/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "iaupdatebgfirsttimehandler.h"

#include <bautils.h>  // bafl.lib 
#include <s32file.h>  // estor.lib

//MACROS
_LIT( KPath, "c:\\private\\2000f85a\\");
_LIT( KIAUpdateFirstTimeFile, "IADFirstTimeState");

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CIAUpdateBGFirstTimeHandler::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIAUpdateBGFirstTimeHandler* CIAUpdateBGFirstTimeHandler::NewL()
    {
    CIAUpdateBGFirstTimeHandler* self = CIAUpdateBGFirstTimeHandler::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateUiController::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateBGFirstTimeHandler* CIAUpdateBGFirstTimeHandler::NewLC()
    {
    CIAUpdateBGFirstTimeHandler* self = new( ELeave ) CIAUpdateBGFirstTimeHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CIAUpdateBGFirstTimeHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CIAUpdateBGFirstTimeHandler::ConstructL()
    {
    User::LeaveIfError( iFsSession.Connect() );   
    iPath.Append( KPath ); 
    BaflUtils::EnsurePathExistsL( iFsSession, iPath );
    iPath.Append( KIAUpdateFirstTimeFile );
    }    

// ---------------------------------------------------------------------------
// CIAUpdateBGFirstTimeHandler::CIAUpdateBGFirstTimeHandler
// constructor
// ---------------------------------------------------------------------------
//
CIAUpdateBGFirstTimeHandler::CIAUpdateBGFirstTimeHandler()  
    {
    }

// ---------------------------------------------------------------------------
// CIAUpdateBGFirstTimeHandler::~CIAUpdateBGFirstTimeHandler
// Destructor
// ---------------------------------------------------------------------------
//
CIAUpdateBGFirstTimeHandler::~CIAUpdateBGFirstTimeHandler()
    {
    iFsSession.Close();
    }

// ---------------------------------------------------------------------------
// CIAUpdateBGFirstTimeHandler::SetAgreementAcceptedL
// Set Nokia agreement as accepted by an user
// ---------------------------------------------------------------------------
//
void CIAUpdateBGFirstTimeHandler::SetAgreementAcceptedL()
    {
    ReadDataL();
    iAgreementAccepted = ETrue;
    WriteDataL();
    }
       
// ---------------------------------------------------------------------------
// CIAUpdateBGFirstTimeHandler::SetAutomaticUpdatesAskedL
// Set automatic update checks as prompted to an use
// ---------------------------------------------------------------------------
//
void CIAUpdateBGFirstTimeHandler::SetAutomaticUpdatesAskedL()
    {
	ReadDataL();
	iAutomaticUpdateChecksAsked = ETrue;
    WriteDataL();
    }

// ---------------------------------------------------------------------------
// CIAUpdateBGFirstTimeHandler::AgreementAcceptedL
// Is Nokia agreement of Application Update accepted by an user
// ---------------------------------------------------------------------------
//
TBool CIAUpdateBGFirstTimeHandler::AgreementAcceptedL()
    {
    ReadDataL();
    return iAgreementAccepted;
    }


// ---------------------------------------------------------------------------
// CIAUpdateBGFirstTimeHandler::AutomaticUpdateChecksAskedL
// Is activation for automatic update cheks from network already asked 
// ---------------------------------------------------------------------------
//    
TBool CIAUpdateBGFirstTimeHandler::AutomaticUpdateChecksAskedL()
    {
    ReadDataL();
    return iAutomaticUpdateChecksAsked;
    }


// -----------------------------------------------------------------------------
// CIAUpdateBGFirstTimeHandler::ReadDataL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGFirstTimeHandler::ReadDataL()
	{
	RFile file;
    TInt err = file.Open( iFsSession, iPath, EFileRead|EFileShareAny );
    if ( err == KErrNotFound )
    	{
    	iAgreementAccepted = EFalse;
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
// CIAUpdateBGFirstTimeHandler::WriteDataL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGFirstTimeHandler::WriteDataL()
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
// CIAUpdateBGFirstTimeHandler::InternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGFirstTimeHandler::InternalizeL( RReadStream& aStream )
	{
	iAgreementAccepted = aStream.ReadInt32L();
	iAutomaticUpdateChecksAsked = aStream.ReadInt32L();
	}


// -----------------------------------------------------------------------------
// CIAUpdateBGFirstTimeHandler::ExternalizeL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateBGFirstTimeHandler::ExternalizeL( RWriteStream& aStream )
	{
	aStream.WriteInt32L( iAgreementAccepted );
	aStream.WriteInt32L( iAutomaticUpdateChecksAsked );
	}
    
// End of File  
