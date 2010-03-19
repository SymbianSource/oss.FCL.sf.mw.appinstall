/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of 
*                CSWInstInstallRequest class member functions.
*
*/


// INCLUDE FILES

#include <DRMHelper.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <SWInstSvrUI.rsg>
#include <SWInstCommonUI.rsg>
#include <SWInstTaskManager.h>
#include <caf/caf.h>
#include <coemain.h>
#include <sysutil.h>

#include "SWInstInstallRequest.h"
#include "SWInstUIPluginAPI.h"
#include "SWInstServerPanic.h"
#include "CUIDialogs.h"

using namespace SwiUI;

const TInt KSystemFileRequirement = 4096; // 4 KB

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSWInstInstallRequest::CSWInstInstallRequest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSWInstInstallRequest::CSWInstInstallRequest( const RMessage2& aMessage )
    : CSWInstRequestObject( aMessage )
    {
    }

// -----------------------------------------------------------------------------
// CSWInstInstallRequest::CSWInstInstallRequest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSWInstInstallRequest::CSWInstInstallRequest( TInt aObjectId )
    : CSWInstRequestObject( aObjectId )
    {
    }

// -----------------------------------------------------------------------------
// CSWInstInstallRequest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSWInstInstallRequest::ConstructL()
    {
    BaseConstructL();    
    User::LeaveIfError( iFs.Connect() );    
    iFs.ShareProtected();    
    }

// -----------------------------------------------------------------------------
// CSWInstInstallRequest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSWInstInstallRequest* CSWInstInstallRequest::NewL( const RMessage2& aMessage )
    {
    CSWInstInstallRequest* self = new ( ELeave ) CSWInstInstallRequest( aMessage );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self; 
    }

// -----------------------------------------------------------------------------
// CSWInstInstallRequest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSWInstInstallRequest* CSWInstInstallRequest::NewL( TInt aObjectId )
    {
    CSWInstInstallRequest* self = new ( ELeave ) CSWInstInstallRequest( aObjectId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self; 
    }
    
// Destructor
CSWInstInstallRequest::~CSWInstInstallRequest()
    {
    iFile.Close();    
    iFs.Close();    
    }

// -----------------------------------------------------------------------------
// CSWInstInstallRequest::Install
// Perform installation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstInstallRequest::Install( RFile& aFile, const TInstallReq& aParams )
    {
    iIsSilent = EFalse;
    iParams = aParams;
    iFile = aFile;

    TRAPD( err, DoInstallL( aFile ) );

    if ( err != KErrNone )
        {
        CompleteSelf( err );        
        }
    }

// -----------------------------------------------------------------------------
// CSWInstInstallRequest::Install
// Perform installation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstInstallRequest::Install( const TDesC& aFileName, const TInstallReq& aParams )
    {
    iIsSilent = EFalse;
    iParams = aParams;

    // Copy the package path to source url
    if ( !iParams.iSourceURL.Length() )
        {
        iParams.iSourceURL = TParsePtrC( aFileName ).DriveAndPath();        
        }   

    // Open file handle to the package
    TInt err = iFile.Open( iFs, aFileName, EFileRead | EFileShareReadersOnly );
    if ( err == KErrNone )
        {
        TRAP( err, DoInstallL( iFile ) );
        }    

    if ( err != KErrNone )
        {
        CompleteSelf( err );    
        }   
    }

// -----------------------------------------------------------------------------
// CSWInstInstallRequest::SilentInstall
// Perform silent installation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstInstallRequest::SilentInstall( RFile& aFile,
                                           const TInstallReq& aParams, 
                                           const TInstallOptions& aOptions )
    {
    iIsSilent = ETrue;
    iParams = aParams;
    iOptions = aOptions;
    iFile = aFile;    

    TRAPD( err, DoInstallL( aFile ) );

    if ( err != KErrNone )
        {
        CompleteSelf( err );        
        }
    }

// -----------------------------------------------------------------------------
// CSWInstInstallRequest::SilentInstall
// Perform silent installation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstInstallRequest::SilentInstall( const TDesC& aFileName,
                                           const TInstallReq& aParams, 
                                           const TInstallOptions& aOptions )
    {
    iIsSilent = ETrue;
    iParams = aParams;
    iOptions = aOptions;

    // Copy the package path to source url
    if ( !iParams.iSourceURL.Length() )
        {
        iParams.iSourceURL = TParsePtrC( aFileName ).DriveAndPath();        
        }    
    
    // Open file handle to the package
    TInt err = iFile.Open( iFs, aFileName, EFileRead | EFileShareReadersOnly );
    if ( err == KErrNone )
        {
        TRAP( err, DoInstallL( iFile ) );   
        }    
    
    if ( err != KErrNone )
        {
        CompleteSelf( err );    
        }   
    }

// -----------------------------------------------------------------------------
// CSWInstInstallRequest::CancelRequest
// Cancels the current request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstInstallRequest::CancelRequest()
    {
    // If we have a pending install request, cancel it
    if ( iUIPlugin )
        {
        iUIPlugin->Cancel();
        }
    }
      
// -----------------------------------------------------------------------------
// CSWInstInstallRequest::RequestCompleteL
// Gets called when the request is completed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstInstallRequest::RequestCompleteL( TInt aResult )
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

    iFile.Close();
    
    // Complete the client message
    Complete( aResult );        
    }
      
// -----------------------------------------------------------------------------
// CSWInstInstallRequest::HandleNotSupportedL
// Handles the error when installation of the given package is not supported.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstInstallRequest::HandleNotSupportedL( const TDesC& aFileName ) const
    {    
    if ( !iIsSilent )
        {        
        // Display an error to the user
        HBufC* errorString = StringLoader::LoadLC( R_SWINSTSVRUI_NOT_SUPP, 
                                                   TParsePtrC( aFileName ).NameAndExt() );
        CommonUI::CCUIDialogs* commonDlgs = CommonUI::CCUIDialogs::NewL();  
        CleanupStack::PushL( commonDlgs );
        commonDlgs->ShowErrorNoteL( *errorString );
        CleanupStack::PopAndDestroy( 2, errorString );
        }    
    User::Leave( KSWInstErrPackageNotSupported );
    }

// -----------------------------------------------------------------------------
// CSWInstInstallRequest::DoInstallL
// Performs the actual installation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstInstallRequest::DoInstallL( RFile& aFile )
    {
    // Check that no instances of installer is running
    LeaveIfInUseL();    

    // Try to recover in case of old fatal error
    TRAP_IGNORE( iTaskManager->ExecuteRecoveryTasksL() );

    // If there is no mime type in the parameters try to get it based on content.
    // If mime is supplied in parameters, it is always used first!
    if ( !iParams.iMIME.Length() )
        {        
        ContentAccess::CContent* content = ContentAccess::CContent::NewLC( aFile );
        GetMIMETypeL( *content, iParams.iMIME );        
        CleanupStack::PopAndDestroy( content );
        }

    // If the mime could not be detected, cancel the installation.
    if ( !iParams.iMIME.Length() )
        {        
        TFileName name;
        aFile.Name( name );        
        HandleNotSupportedL( name );
        }        
    else
        {        
        // Make sure that we are not running out of disk space. 
        // We need to make sure that there is enough space at least for possible
        // persistent tasks.
        if ( SysUtil::FFSSpaceBelowCriticalLevelL( &iFs, KSystemFileRequirement ) )
            {
            if ( !iIsSilent )
                {                
                CommonUI::CCUIDialogs* commonDlgs = CommonUI::CCUIDialogs::NewL();  
                CleanupStack::PushL( commonDlgs );
                commonDlgs->ShowErrorNoteL( R_SWCOMMON_NOT_ENOUGH_MEMORY );
                CleanupStack::PopAndDestroy( commonDlgs );
                }
            User::Leave( KSWInstErrInsufficientMemory );            
            }        
        TInt err = 0; 
        // Try to create the plugin based on the MIME type   
        TRAP( err, CreatePluginL( iParams.iMIME ) );
        if ( err == KErrNotFound ) //lets try again by asking mime type from the content
            {
            ContentAccess::CContent* content = ContentAccess::CContent::NewLC( aFile );
            GetMIMETypeL( *content, iParams.iMIME );        
            CleanupStack::PopAndDestroy( content );	
            TRAP( err, CreatePluginL( iParams.iMIME ) );
            }
        if ( err == KErrNotFound )
            {
            TFileName name;
            aFile.Name( name );            
            HandleNotSupportedL( name ); 
            }    
        else if ( err != KErrNone )
            {
            User::Leave( err );        
            }
        else
            {   
            TInt isProtected( EFalse );
            
            // See if this is protected content    
            ContentAccess::CContent* content = ContentAccess::CContent::NewLC( aFile );
            content->GetAttribute( ContentAccess::EIsProtected, isProtected );
            CleanupStack::PopAndDestroy( content );

            // We can launch the installation if this is not protected or
            // rights exist for the protected content.
            if ( !isProtected || HandleProtectedContentL( aFile ) )
                {     
                if ( !iIsSilent )
                    {            
                    iUIPlugin->InstallL( aFile, iParams, isProtected, iStatus );
                    }
                else
                    {
                    iUIPlugin->SilentInstallL( aFile, iParams, isProtected, iOptions, iStatus );
                    }       
 
                SetActive();
                }
            else
                {
                User::Leave( KSWInstErrNoRights );              
                }  
            }    
        }    
    }

// -----------------------------------------------------------------------------
// CSWInstInstallRequest::GetMIMETypeL
// Gets MIME type of the package.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSWInstInstallRequest::GetMIMETypeL( const ContentAccess::CContent& aContent, 
                                          TDes8& aMIME )
    {
    HBufC* tmpMIME = HBufC::NewLC( KMaxDataTypeLength );
    TPtr mimePtr( tmpMIME->Des() );            
    User::LeaveIfError( aContent.GetStringAttribute( ContentAccess::EMimeType, mimePtr ) );
    aMIME.Copy( *tmpMIME );
    CleanupStack::PopAndDestroy( tmpMIME );
    }

// -----------------------------------------------------------------------------
// CSWInstInstallRequest::HandleProtectedContentL
// Helper to handle protected content.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSWInstInstallRequest::HandleProtectedContentL( RFile& aFile )
    {
    TBool result( EFalse );
    
    // Try to open the content, if we get an error, let the DRMHelper handle it
    ContentAccess::CContent* content = ContentAccess::CContent::NewLC( aFile );
    CData* pkgData = NULL;
    TRAPD( err, pkgData = content->OpenContentL( ContentAccess::EExecute ) ); // Install needs EExecute
    delete pkgData;
    pkgData = NULL;        
    CleanupStack::PopAndDestroy( content );    
    
    if ( err != KErrNone && !iIsSilent )
        {      
        CDRMHelper* drmHelper = CDRMHelper::NewL( *CCoeEnv::Static() );
        TRAPD( helperErr, drmHelper->HandleErrorL( err, aFile ) );
        delete drmHelper;
        drmHelper = NULL;
        
        if ( helperErr != KErrNone )
            {
            // Some error from helper
            CommonUI::CCUIDialogs* commonDlgs = CommonUI::CCUIDialogs::NewL();  
            CleanupStack::PushL( commonDlgs );
            commonDlgs->ShowErrorNoteL( R_SWINSTSVRUI_NO_RIGHTS );
            CleanupStack::PopAndDestroy( commonDlgs );
            }      
        }
    else if ( err == KErrNone )
        {
        // We have rights
        result = ETrue;        
        }

    return result;   
    }

//  End of File  
