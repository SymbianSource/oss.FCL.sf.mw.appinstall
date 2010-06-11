/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CSisxUI
*                class member functions.
*
*/


// INCLUDE FILES
#include <bautils.h>
#include <eikenv.h>
#include <data_caging_path_literals.hrh>
#include <SisxUIData.rsg>

#include "SisxUI.h"
#include "SisxUIOperationWatcher.h"
#include "SisxUIAppInfo.h"
#include "SWInstDebug.h"
#include "CUIUtils.h"

using namespace SwiUI;

_LIT( KSisxUIResourceFileName, "sisxuiData.rsc" );
_LIT( KSisxUIResourceDir, "\\Resource\\" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSisxUI::CSisxUI
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSisxUI::CSisxUI()
    {
    }

// -----------------------------------------------------------------------------
// CSisxUI::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSisxUI::ConstructL()
    {
    TFileName fileName;
    fileName.Append( KSisxUIResourceDir );
    fileName.Append( KSisxUIResourceFileName );
    iResourceFileOffset = CommonUI::CUIUtils::LoadResourceFileL( fileName, CEikonEnv::Static() );
    }

// -----------------------------------------------------------------------------
// CSisxUI::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSisxUI* CSisxUI::NewL()
    {
    CSisxUI* self = new( ELeave ) CSisxUI();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }
    
// Destructor
CSisxUI::~CSisxUI()
    {
    delete iWatcher;    
    
    if( iResourceFileOffset )
        {
        CEikonEnv::Static()->DeleteResourceFile( iResourceFileOffset );
        } 
    }

// -----------------------------------------------------------------------------
// CSisxUI::InstallL
// Perform installation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUI::InstallL( RFile& aFile,
                        TInstallReq& aInstallParams, 
                        TBool aIsDRM, 
                        TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( !iWatcher, User::Leave( KErrInUse ) );
    FLOG_2( _L("SisxUI: Install request with chrSet: %d, ap: %d"),       
            aInstallParams.iCharsetId, aInstallParams.iIAP );
    iWatcher = CSisxUIOperationWatcher::NewL();       
    iWatcher->InstallL( aFile, aInstallParams.iMIME, aIsDRM, aStatus );
    }
       
// -----------------------------------------------------------------------------
// CSisxUI::SilentInstallL
// Perform silent installation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CSisxUI::SilentInstallL( RFile& aFile,
                              TInstallReq& aInstallParams, 
                              TBool aIsDRM, 
                              TInstallOptions& aOptions,
                              TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( !iWatcher, User::Leave( KErrInUse ) );
    FLOG_2( _L("SisxUI: SilentInstall request with chrSet: %d, ap: %d"),       
            aInstallParams.iCharsetId, aInstallParams.iIAP );

    iWatcher = CSisxUIOperationWatcher::NewL();       
    iWatcher->SilentInstallL( aFile, aInstallParams.iMIME, aIsDRM, aOptions, aStatus );
    }

// -----------------------------------------------------------------------------
// CSisxUI::UninstallL
// Perform uninstallation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUI::UninstallL( const TUid& aUid, 
                          const TDesC8& /*aMIME*/,
                          TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( !iWatcher, User::Leave( KErrInUse ) );
    FLOG_1( _L("SisxUI: Uninstall request with uid: %x"), aUid.iUid );    

    iWatcher = CSisxUIOperationWatcher::NewL();
    iWatcher->UninstallL( aUid, aStatus );
    }

// -----------------------------------------------------------------------------
// CSisxUI::SilentUninstallL
// Perform silent uninstallation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CSisxUI::SilentUninstallL( const TUid& aUid, 
                                const TDesC8& /*aMIME*/, 
                                TUninstallOptions& aOptions,
                                TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( !iWatcher, User::Leave( KErrInUse ) );
    FLOG_1( _L("SisxUI: SilentUninstall request with uid: %x"), aUid.iUid );    

    iWatcher = CSisxUIOperationWatcher::NewL();
    iWatcher->SilentUninstallL( aUid, aOptions, aStatus );
    }

// -----------------------------------------------------------------------------
// CSisxUI::IsAppShellUpdate
// Indicates if the application shell must be updated.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUI::IsAppShellUpdate()
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSisxUI::CompleteL
// Cancel the current operation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUI::Cancel()
    {    
    if ( iWatcher )
        {
        iWatcher->Cancel();        
        } 
    }

// -----------------------------------------------------------------------------
// CSisxUI::CompleteL
// Informs the plugin about completed operation. Called when all tasks are 
// completed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUI::CompleteL()
    {
    if ( iWatcher )
        {
        iWatcher->FinalCompleteL();        
        }    
    }
        
/**
*
*/
CErrDetails* CSisxUI::GetErrorDetails()
    {
    return NULL;    
    }

// -----------------------------------------------------------------------------
// CSisxUI::CustomUninstallL
// Perform a custom uninstallation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CSisxUI::CustomUninstallL( TOperation aOperation, 
                                const TDesC8& aParams, 
                                TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( !iWatcher, User::Leave( KErrInUse ) );

    if ( aOperation == EOperationUninstallIndex )
        {
        // The client wants to uninstall a specific augmentation

        // We need to get uid and index from the parambuffer
        TOpUninstallIndexParam params;
        TPckg<TOpUninstallIndexParam> pckg( params );
        pckg.Copy( aParams );

        // Launch the uninstall
        iWatcher = CSisxUIOperationWatcher::NewL();
        iWatcher->UninstallAugmentationL( params.iUid, params.iIndex, aStatus );        
        }
    else
        {
        User::Leave( KErrNotSupported );
        }    
    }

// -----------------------------------------------------------------------------
// CSisxUI::SilentCustomUninstallL
// Perform a silent custom uninstallation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CSisxUI::SilentCustomUninstallL( TOperation aOperation, 
                                      TUninstallOptions& aOptions,
                                      const TDesC8& aParams, 
                                      TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( !iWatcher, User::Leave( KErrInUse ) );

    if ( aOperation == EOperationUninstallIndex )
        {
        // The client wants to uninstall a specific augmentation

        // We need to get uid and index from the parambuffer
        TOpUninstallIndexParam params;
        TPckg<TOpUninstallIndexParam> pckg( params );
        pckg.Copy( aParams );

        // Launch the uninstall
        iWatcher = CSisxUIOperationWatcher::NewL();
        iWatcher->SilentUninstallAugmentationL( params.iUid, aOptions, params.iIndex, aStatus );        
        }
    else
        {
        User::Leave( KErrNotSupported );
        }    
    }

//  End of File  
