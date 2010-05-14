/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CFileTaskDelete
*                class member functions.
*
*/


// INCLUDE FILES
#include <f32file.h>
#include "FileTaskDelete.h"

using namespace SwiUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFileTaskDelete::CFileTaskDelete
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFileTaskDelete::CFileTaskDelete()
    {
    }

// -----------------------------------------------------------------------------
// CFileTaskDelete::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFileTaskDelete* CFileTaskDelete::NewL()
    {
    return new(ELeave) CFileTaskDelete();    
    }

// Destructor
CFileTaskDelete::~CFileTaskDelete()
    {
    }

// -----------------------------------------------------------------------------
// CFileTaskDelete::ExecuteL
// Executes the task.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CFileTaskDelete::ExecuteL()
    {
    RFs rfs;
    User::LeaveIfError( rfs.Connect() );
    CleanupClosePushL( rfs );    
    
    CFileMan* fileMan = CFileMan::NewL( rfs );
    CleanupStack::PushL( fileMan );
    
    TInt err( KErrNone );
    
    if ( TParsePtrC( iParam.iFile ).NameOrExtPresent() )
        {
        // We want to delete files only
        err = fileMan->Delete( iParam.iFile );
        }
    else
        {
        // We want to delete a directory
        err = fileMan->RmDir( iParam.iFile );
        }   
    
    CleanupStack::PopAndDestroy( fileMan );
    CleanupStack::PopAndDestroy(); // rfs    
    
    if ( err == KErrInUse )
        {
        User::Leave( err );        
        }    
    }

// -----------------------------------------------------------------------------
// CFileTaskDelete::SetParameterL
// Adds a parameter to the task.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CFileTaskDelete::SetParameterL( const TDesC8& aParam, TInt /*aIndex*/ )
    {
    TFileTaskDeleteParamPckg pckg( iParam );
    pckg.Copy( aParam );    
    }

// -----------------------------------------------------------------------------
// CFileTaskDelete::ExternalizeL
// Externalizes the task.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CFileTaskDelete::DoExternalizeL( RWriteStream& aStream ) const
    {
    aStream << iParam.iFile;    
    }

// -----------------------------------------------------------------------------
// CFileTaskDelete::DoInternalizeL
// Internalizes the task.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CFileTaskDelete::DoInternalizeL( RReadStream& aStream )
    {
    aStream >> iParam.iFile;    
    }

//  End of File  
