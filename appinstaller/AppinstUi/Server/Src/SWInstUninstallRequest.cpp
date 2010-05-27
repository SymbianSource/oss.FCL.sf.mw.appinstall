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
* Description:   This module contains the implementation of CSWInstUninstallRequest 
*                class member functions.
*
*/


// INCLUDE FILES
#include <SWInstTaskManager.h>

#include "SWInstUninstallRequest.h"
#include "SWInstUIPluginAPI.h"
#include "SWInstServerPanic.h"

using namespace SwiUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSWInstUninstallRequest::CSWInstUninstallRequest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSWInstUninstallRequest::CSWInstUninstallRequest( const RMessage2& aMessage )
    : CSWInstRequestObject( aMessage )
    {
    }

// -----------------------------------------------------------------------------
// CSWInstUninstallRequest::CSWInstUninstallRequest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSWInstUninstallRequest::CSWInstUninstallRequest( TInt aObjectId )
    : CSWInstRequestObject( aObjectId )
    {
    }

// -----------------------------------------------------------------------------
// CSWInstUninstallRequest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSWInstUninstallRequest::ConstructL()
    {
    BaseConstructL();    
    }

// -----------------------------------------------------------------------------
// CSWInstUninstallRequest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSWInstUninstallRequest* CSWInstUninstallRequest::NewL( const RMessage2& aMessage )
    {
    CSWInstUninstallRequest* self = 
        new ( ELeave ) CSWInstUninstallRequest( aMessage );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self; 
    }

// -----------------------------------------------------------------------------
// CSWInstUninstallRequest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSWInstUninstallRequest* CSWInstUninstallRequest::NewL( TInt aObjectId )
    {
    CSWInstUninstallRequest* self = new ( ELeave ) CSWInstUninstallRequest( aObjectId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self; 
    }
    
// Destructor
CSWInstUninstallRequest::~CSWInstUninstallRequest()
    {    
    }

// -----------------------------------------------------------------------------
// CSWInstUninstallRequest::Uninstall
// Perform uninstallation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstUninstallRequest::Uninstall( const TUid& aUid, const TDesC8& aMIME )
    {
    iIsSilent = EFalse;
    iUid = aUid;

    TRAPD( err, DoUninstallL( aMIME ) );
    if ( err != KErrNone )
        {
        CompleteSelf( err );        
        }  
    }

// -----------------------------------------------------------------------------
// CSWInstUninstallRequest::SilentInstall
// Perform silent uninstallation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstUninstallRequest::SilentUninstall( const TUid& aUid, 
                                               const TDesC8& aMIME, 
                                               TUninstallOptions& aOptions )   
    {
    iIsSilent = ETrue;
    iUid = aUid;

    iOptions = aOptions;

    TRAPD( err, DoUninstallL( aMIME ) );
    if ( err != KErrNone )
        {
        CompleteSelf( err );        
        }  
    }

// -----------------------------------------------------------------------------
// CSWInstUninstallRequest::CustomUninstall
// Perform a custom uninstallation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstUninstallRequest::CustomUninstall( TOperation aOperation, 
                                               const TDesC8& aParams,
                                               const TDesC8& aMIME )
    {
    iIsSilent = EFalse;

    TRAPD( err, DoCustomUninstallL( aOperation, aParams, aMIME ) );
    
    if ( err != KErrNone )
        {
        CompleteSelf( err );    
        }
    }

// -----------------------------------------------------------------------------
// CSWInstUninstallRequest::SilentCustomUninstall
// Perform a custom uninstallation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstUninstallRequest::SilentCustomUninstall( TOperation aOperation, 
                                                     TUninstallOptions& aOptions,
                                                     const TDesC8& aParams,
                                                     const TDesC8& aMIME )
    {
    iIsSilent = ETrue;

    TRAPD( err, DoSilentCustomUninstallL( aOperation, aOptions, aParams, aMIME ) );
    
    if ( err != KErrNone )
        {
        CompleteSelf( err );    
        }
    }

// -----------------------------------------------------------------------------
// CSWInstUninstallRequest::CancelRequest
// Cancels the current request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstUninstallRequest::CancelRequest()
    {
    // If we have a pending uninstall request, cancel it
    if ( iUIPlugin )
        {
        iUIPlugin->Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CSWInstUninstallRequest::RequestCompleteL
// Gets called when the request is completed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstUninstallRequest::RequestCompleteL( TInt aResult )
    {
    // First see if we even have a ui plugin
    if ( iUIPlugin )
        {        
        // Run outstanding tasks
        TRAP_IGNORE( iTaskManager->ExecutePendingTasksL() );
        
        // Inform the ui
        iUIPlugin->CompleteL();

        // Delete the plugin and try to run outstanding tasks once again.
        delete iUIPlugin;
        iUIPlugin = NULL;
        TRAP_IGNORE( iTaskManager->ExecutePendingTasksL() );
        }
    
    // Complete the client message
    Complete( aResult );
    }

// -----------------------------------------------------------------------------
// CSWInstUninstallRequest::DoUninstallL
// Performs the actual uninstallation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstUninstallRequest::DoUninstallL( const TDesC8& aMIME )
    {
    __ASSERT_ALWAYS( iUid != TUid::Uid( 0 ), User::Leave( KErrBadDescriptor ) );  

    // Check that no other instances of installer are running
    LeaveIfInUseL();    

    // Try to recover in case of old fatal error
    TRAP_IGNORE( iTaskManager->ExecuteRecoveryTasksL() );

    // Create the plugin based on the mime type
    CreatePluginL( aMIME );    

    if ( !iIsSilent )
        {        
        iUIPlugin->UninstallL( iUid, aMIME, iStatus );
        }
    else
        {
        iUIPlugin->SilentUninstallL( iUid, aMIME, iOptions, iStatus );
        }
    
    SetActive();    
    }

// -----------------------------------------------------------------------------
// CSWInstUninstallRequest::DoCustomUninstall
// Perform a custom uninstallation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstUninstallRequest::DoCustomUninstallL( TOperation aOperation, 
                                               const TDesC8& aParams,
                                               const TDesC8& aMIME )
    {
    // Check that no other instances of installer are running
    LeaveIfInUseL();
    
    iTaskManager->ExecuteRecoveryTasksL();
    
    // Create the plugin based on the mime type
    CreatePluginL( aMIME );    
    iUIPlugin->CustomUninstallL( aOperation, aParams, iStatus );
    SetActive();    
    }

// -----------------------------------------------------------------------------
// CSWInstUninstallRequest::DoSilentCustomUninstall
// Perform a custom uninstallation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstUninstallRequest::DoSilentCustomUninstallL( TOperation aOperation, 
                                                     TUninstallOptions& aOptions,
                                                     const TDesC8& aParams,
                                                     const TDesC8& aMIME )
    {
    // Check that no other instances of installer are running
    LeaveIfInUseL();

    iTaskManager->ExecuteRecoveryTasksL();

    // Create the plugin based on the mime type
    CreatePluginL( aMIME );    
    iUIPlugin->SilentCustomUninstallL( aOperation, aOptions, aParams, iStatus );
    SetActive();
    }

//  End of File  
