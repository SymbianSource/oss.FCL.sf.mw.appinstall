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
* Description:   This file contains the implementation of CCUIDialogs
*                class member functions.
*
*/


// INCLUDE FILES
#include <bautils.h>
#include <eikenv.h>
#include <StringLoader.h>
//#include <AknQueryDialog.h> 
#include <eikprogi.h>
//#include <aknnotewrappers.h>
#include <data_caging_path_literals.hrh>
#include <SWInstCommonUI.rsg>
//#include <aknmessagequerydialog.h>
#include <hlplch.h>
//#include <csxhelp/am.hlp.hrh>
#include <coehelp.h>
#include <featmgr.h>
#include <sysutil.h>
#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif //RD_MULTIPLE_DRIVE
//#include <akninputblock.h>
 
#include "CUIDialogs.h"
#include "CUIDriveUtils.h"
#include "CUIWaitDialog.h"
#include "CUIProgressDialog.h"
#include "CUIDetailsDialog.h"
#include "CUIUtils.h"
#include "SWInstPrivateUid.h"

    using namespace SwiUI::CommonUI;

_LIT( KNextLine, "\n" );
_LIT( KFormat, "%d\t%S\t%S" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCUIDialogs::CCUIDialogs
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCUIDialogs::CCUIDialogs()
    {  
    }

// -----------------------------------------------------------------------------
// CCUIDialogs::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCUIDialogs::ConstructL()
    {
    iCoeEnv = CEikonEnv::Static();    
    if ( !iCoeEnv )
        {
        User::Leave( KErrGeneral );        
        }

    TFileName fileName;
    fileName.Append( KDC_RESOURCE_FILES_DIR );
    fileName.Append( KCUIResourceFileName );
    iResourceFileOffset = CUIUtils::LoadResourceFileL( fileName, iCoeEnv );    

//    iAbsorber = CAknInputBlock::NewLC();
//    CleanupStack::Pop( iAbsorber );
    
    FeatureManager::InitializeLibL(); 
    }

// -----------------------------------------------------------------------------
// CCUIDialogs::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCUIDialogs* CCUIDialogs::NewL()
    {
    CCUIDialogs* self = new ( ELeave ) CCUIDialogs();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self; 
    }

// Destructor
EXPORT_C CCUIDialogs::~CCUIDialogs()
    {
    // Close non-modal dialogs
    TRAP_IGNORE( CloseProgressDialogL() );
    TRAP_IGNORE( CloseWaitDialogL() );

    if( iResourceFileOffset )
        {
        iCoeEnv->DeleteResourceFile( iResourceFileOffset );
        }

//    delete iAbsorber;

    FeatureManager::UnInitializeLib();
    }

// -----------------------------------------------------------------------------
// CCUIDialogs::ShowWaitDialogL
// Show a wait dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCUIDialogs::ShowWaitDialogL( TInt aResourceId, 
                                            MCUIWaitDialogCallback* aCallback,
                                            TInt aSoftkeyResourceId )
    {
    HBufC* displayString = StringLoader::LoadLC( aResourceId );

    if ( iWaitDialog )
        {
        iWaitDialog->SetTextL( *displayString );
        SetWaitDialogCallback( aCallback );
        }
    else
        {   
        iWaitDialog = new (ELeave) CCUIWaitDialog ( reinterpret_cast 
                                                    < CEikDialog** > 
                                                    ( &iWaitDialog ),
                                                    ETrue );
        
        iWaitDialog->PrepareLC( R_SWCOMMON_GENERAL_WAIT_DIALOG );
        iWaitDialog->SetTextL( *displayString );
        SetWaitDialogCallback( aCallback );
        iWaitDialog->ButtonGroupContainer().SetCommandSetL( aSoftkeyResourceId );
        iWaitDialog->RunLD();
        }

    CleanupStack::PopAndDestroy( displayString );
    }

// -----------------------------------------------------------------------------
// CCUIDialogs::CloseWaitDialogL
// Closes the wait dialog
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCUIDialogs::CloseWaitDialogL()
    {
    if ( iWaitDialog )
        {
        // Set callback to NULL as we want no callbacks from this.
        delete iWaitDialog;
        iWaitDialog = NULL; 
        }
    }

// -----------------------------------------------------------------------------
// CCUIDialogs::ShowProgressDialog
// Show a progress dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCUIDialogs::ShowProgressDialogL( TInt aResourceId, 
                                                MCUIWaitDialogCallback* aCallback,
                                                TInt aSoftkeyResourceId )
    {
    HBufC* displayString = StringLoader::LoadLC( aResourceId );
    ShowProgressDialogL( *displayString, aCallback, aSoftkeyResourceId );
    CleanupStack::PopAndDestroy( displayString );
    }

EXPORT_C void CCUIDialogs::ShowProgressDialogL( const TDesC& aText, 
                                                MCUIWaitDialogCallback* aCallback,
                                                TInt aSoftkeyResourceId )
    {
    if ( iProgressDialog )
        {
        iProgressDialog->SetTextL( aText );
        SetProgressDialogCallback( aCallback );      
        }
    else
        {   
        iProgressDialog = new (ELeave) CCUIProgressDialog ( reinterpret_cast 
                                                            < CEikDialog** > 
                                                            ( &iProgressDialog ),
                                                            ETrue );
        
        iProgressDialog->PrepareLC( R_SWCOMMON_GENERAL_PROGRESS_DIALOG );
        iProgressDialog->SetTextL( aText );
        SetProgressDialogCallback( aCallback );
        iProgressDialog->ButtonGroupContainer().SetCommandSetL( aSoftkeyResourceId );
        iProgressDialog->RunLD();
        }   
    }

// -----------------------------------------------------------------------------
// CCUIDialogs::SetProgressDialogFinalValueL
// Set final value for the progress bar in progress dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCUIDialogs::SetProgressDialogFinalValueL( TInt aValue )
    {
    if ( iProgressDialog )
        {        
        CEikProgressInfo* progressInfo = iProgressDialog->GetProgressInfoL();
        progressInfo->SetFinalValue( aValue );
        }  
    }

// -----------------------------------------------------------------------------
// CCUIDialogs::UpdateProgressDialogValuL
// Increments the progress bar in progress dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCUIDialogs::UpdateProgressDialogValueL( TInt aValue )
    {
    if ( iProgressDialog )
        {        
        CEikProgressInfo* progressInfo = iProgressDialog->GetProgressInfoL();
        progressInfo->IncrementAndDraw( aValue );
        User::ResetInactivityTime();    // keep lights on and screensaver disabled
        }   
    }

// -----------------------------------------------------------------------------
// CCUIDialogs::CloseProgressDialogL
// Closes the progress dialog
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCUIDialogs::CloseProgressDialogL()
    {
    if ( iProgressDialog )
        {
        // Set callback to NULL as we want no callbacks from this.
        iProgressDialog->SetCallback( NULL );
        iProgressDialog->ProcessFinishedL();   
        delete iProgressDialog;
        iProgressDialog = NULL;  
        }
    }

// -----------------------------------------------------------------------------
// SwiUI::CSWCUIDialogs::ShowConfirmationQueryL
// Show confirmation dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCUIDialogs::ShowConfirmationQueryL( TInt aResourceId,
                                                    TInt aSoftkeyResourceId ) const
    {
    TBool response( EFalse );

    HBufC* displayString = StringLoader::LoadLC( aResourceId );

    response = ShowConfirmationQueryL( *displayString, aSoftkeyResourceId );

    CleanupStack::PopAndDestroy( displayString ); 
                    
    return response;    
    }
        
// -----------------------------------------------------------------------------
// SwiUI::CSWCUIDialogs::ShowConfirmationQueryL
// Show confirmation dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCUIDialogs::ShowConfirmationQueryL( const TDesC& aDisplayString,
                                                    TInt aSoftkeyResourceId ) const
    {
    TBool response( EFalse );
/*
    CAknQueryDialog* note = 
        CAknQueryDialog::NewL( CAknQueryDialog::EConfirmationTone );

    note->PrepareLC( R_SWCOMMON_GENERAL_CONFIRMATION_QUERY );
    note->SetPromptL( aDisplayString );
    note->ButtonGroupContainer().SetCommandSetL( aSoftkeyResourceId );

    if ( note->RunLD() )    
        {
        response = ETrue;
        }   
*/
    return response;    
    }

// -----------------------------------------------------------------------------
// CCUIDialogs::ShowDriveSelectionDialogL
// Show a drive / memory selection dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCUIDialogs::ShowDriveSelectionDialogL( 
    TInt64 aSize,
    const RArray<TDriveUnit>& aDriveUnits,
    const RArray<TInt64>& aDriveSpaces,
    TInt& aSelectionResult )
    {  
    
    TInt result( KErrNone );
/*
    // Sanity check
    if ( aSize <= 0 )
        {
        aSize = 1;        
        }    

    CCUIDriveUtils* driveUtils = 
        CCUIDriveUtils::NewL( aSize,
                              aDriveUnits,
                              aDriveSpaces );
    CleanupStack::PushL( driveUtils );

    aSelectionResult = -1;

#ifdef RD_MULTIPLE_DRIVE
    TDriveList driveList;
	TInt driveCount = 0;
    DriveInfo::GetUserVisibleDrives( iCoeEnv->FsSession(), 
                                     driveList, 
                                     driveCount ); 
    TInt driveCountWithoutRemoteDrives = driveCount;                                 
                       
    
    TUint driveStatus = 0; 
    TInt firstFoundDrive = 0;                                
    for ( TInt i = 0; i < KMaxDrives; i++ )
        {
        if ( driveList[i] ) 
            {
            User::LeaveIfError( DriveInfo::GetDriveStatus( iCoeEnv->FsSession(), 
                                                           i, 
                                                           driveStatus ) );
            TFileName path;                                               
            TDriveUnit driveUnit( i );                                               
            if ( ( driveStatus & DriveInfo::EDriveRemote ) )     	        
                {
                driveCountWithoutRemoteDrives--;
                }
            else
                {
                if ( !firstFoundDrive )
                   {
                   firstFoundDrive = (TInt) driveUnit; 
                   }
                }
            }
        }
                                    
    if ( driveCountWithoutRemoteDrives ==  1 )
       {
       // Only one drive available, force install to available drive
       for ( TInt index = 0; index < aDriveUnits.Count(); index++ )
           {  
           if ( firstFoundDrive == (TInt)aDriveUnits[index] )
               {
               aSelectionResult = index;
               break;
               } 	
           }
       }
#else
    // See if the whole device has enough memory
    if ( SysUtil::FFSSpaceBelowCriticalLevelL( &iCoeEnv->FsSession(), aSize ) &&
         ( !driveUtils->IsCardDrivePresent() ||
           SysUtil::MMCSpaceBelowCriticalLevelL( &iCoeEnv->FsSession(), aSize ) ) )
        {
        result = KErrNoMemory;        
        }
    
    // See if the memory card is present
    else if ( !driveUtils->IsCardDrivePresent() )
        {
        // No memory card present, force install to phone memory
        aSelectionResult = driveUtils->GetInternalDriveIndex();
        }

    // Show the dialog until user selects valid memory or cancels it
#endif //RD_MULTIPLE_DRIVE        
    else
        {        
        while ( ETrue )
            {        
            if ( !driveUtils->ShowSelectionDialogL( aSelectionResult ) ) 
                {
                // User canceled the query
                result = KErrCancel;                
                aSelectionResult = -1;                
                break;            
                }
#ifdef RD_MULTIPLE_DRIVE
            // See if there is enough space in the selected drive
            // Internal memory selected? 
            if ( !SysUtil::DiskSpaceBelowCriticalLevelL( &iCoeEnv->FsSession(), aSize, aDriveUnits[aSelectionResult] ) )                
                {
                break;                    
                }
#else
            // See if there is enough space in the selected drive
            // Internal memory selected?
            if ( driveUtils->GetInternalDriveIndex() == aSelectionResult )
                {
                if ( !SysUtil::FFSSpaceBelowCriticalLevelL( &iCoeEnv->FsSession(), aSize ) )                
                    {
                    break;                    
                    }
                }            
            // The user selected the card. Check the free space from there
            else if ( !SysUtil::MMCSpaceBelowCriticalLevelL( &iCoeEnv->FsSession(), aSize ) )
                {
                break;                    
                }                     
#endif //RD_MULTIPLE_DRIVE                                  
                
            // Show error and let user try again
            HBufC* string = StringLoader::LoadLC( R_SWCOMMON_NOT_ENOUGH_MEMORY_IN_DRIVE );
    
            CAknErrorNote* note = new( ELeave )CAknErrorNote( ETrue );
            note->ExecuteLD( string->Des() );

            CleanupStack::PopAndDestroy( string );                   
            }
        }    
    
    CleanupStack::PopAndDestroy( driveUtils );
*/
    return result;
    }

// -----------------------------------------------------------------------------
// CCUIDialogs::ShowSecurityWarningDialogL
// Show a security warning dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCUIDialogs::ShowSecurityWarningDialogL()
    { 
    
    TBool result( EFalse );
    /*
    // Load resources
    HBufC* text = StringLoader::LoadLC( R_SWCOMMON_SECURITY_WARNING );
    HBufC* linkText = NULL;
    if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {        
        linkText = StringLoader::LoadLC( R_SWCOMMON_SECURITY_WARNING_MORE_INFO );
        }
    else
        {
        linkText = KNullDesC().AllocLC();        
        }
    
    HBufC* message = HBufC::NewLC( text->Length() + linkText->Length() + 1 );
    TPtr msgPtr( message->Des() );
    msgPtr += *text;
    msgPtr += KNextLine;    
    msgPtr += *linkText;

    // Show the dialog
    CAknMessageQueryDialog* dlg = 
        CAknMessageQueryDialog::NewL( *message, CAknQueryDialog::EWarningTone );
    dlg->PrepareLC( R_SWCOMMON_SECURITY_WARNING_DIALOG );
    CleanupStack::PushL( dlg );        
    if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {        
        dlg->SetLinkTextL( *linkText );
        TCallBack callback( ShowSecurityWarningHelp, this );    
        dlg->SetLink( callback );
        }    
    CleanupStack::Pop( dlg );        

    if ( dlg->RunLD() )
        {
        result = ETrue;        
        }    
    
    CleanupStack::PopAndDestroy( 3, text ); // message, linkText, text
     */
    return result;    
    }
 
// -----------------------------------------------------------------------------
// CCUIDialogs::ShowErrorNoteL
// Show an error note.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCUIDialogs::ShowErrorNoteL( TInt aResourceId ) const
    {
    HBufC* errorString = StringLoader::LoadLC( aResourceId );
    ShowErrorNoteL( *errorString );    
    CleanupStack::PopAndDestroy( errorString ); 
    }

// -----------------------------------------------------------------------------
// CCUIDialogs::ShowErrorNoteL
// Show an error note.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCUIDialogs::ShowErrorNoteL( const TDesC& aErrorText ) const
    {
    /*
    CAknErrorNote* note = new (ELeave) CAknErrorNote( ETrue );
    note->ExecuteLD( aErrorText );
    */
    }

// -----------------------------------------------------------------------------
// CCUIDialogs::ResetProgressDialogL
// Resets the progress bar in the progress dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCUIDialogs::ResetProgressDialogL()
    {
    if ( iProgressDialog )
        {        
        CEikProgressInfo* progressInfo = iProgressDialog->GetProgressInfoL();
        progressInfo->SetAndDraw( 0 );
        }   
    }

// -----------------------------------------------------------------------------
// CCUIDialogs::LaunchHelpL
// Launches a help with the given context.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCUIDialogs::LaunchHelpL( const TDesC& aContext )
    {
    CArrayFix<TCoeHelpContext>* contexts = 
        new(ELeave) CArrayFixFlat<TCoeHelpContext>( 1 );
    CleanupStack::PushL( contexts );
    TCoeHelpContext context;
    context.iMajor = TUid::Uid( KSWInstHelpUid );
    context.iContext = aContext;
    contexts->AppendL( context );
    CleanupStack::Pop(); // contexts
    HlpLauncher::LaunchHelpApplicationL( iCoeEnv->WsSession(), contexts );
    }

// -----------------------------------------------------------------------------
// CCUIDialogs::ShowDelayedWaitDialogL
// Show a wait dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCUIDialogs::ShowDelayedWaitDialogL( TInt aResourceId, 
                                                   MCUIWaitDialogCallback* aCallback,
                                                   TInt aSoftkeyResourceId )
    {
    if ( iWaitDialog )
        {
        ShowWaitDialogL( aResourceId, aCallback, aSoftkeyResourceId );
        }
    else
        {   
        HBufC* displayString = StringLoader::LoadLC( aResourceId );
        iWaitDialog = new (ELeave) CCUIWaitDialog ( reinterpret_cast 
                                                    < CEikDialog** > 
                                                    ( &iWaitDialog ),
                                                    EFalse );
        
        iWaitDialog->PrepareLC( R_SWCOMMON_GENERAL_WAIT_DIALOG );
        iWaitDialog->SetTextL( *displayString );
        SetWaitDialogCallback( aCallback );
        iWaitDialog->ButtonGroupContainer().SetCommandSetL( aSoftkeyResourceId );
        iWaitDialog->RunLD();
        CleanupStack::PopAndDestroy( displayString );
        }
    }

// -----------------------------------------------------------------------------
// CCUIDialogs::ShowInstallationCompleteQueryL
// Show an installation complete query.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCUIDialogs::ShowInstallationCompleteQueryL( 
                                        const MDesCArray& aApplicationArray,
                                        const MDesCArray& aInstalledFolderArray,
                                        const CAknIconArray& aIconArray,
                                        TInt& aIndexOfAppToBeStarted ) const
           
    {
    /*
    aIndexOfAppToBeStarted = KErrNotFound;
    
    // Prepare for showing the installation complete query
    CDesCArray *itemArray = new( ELeave ) CDesCArrayFlat( 1 );
    CleanupStack::PushL( itemArray );
    CAknIconArray* iconArray = new( ELeave ) CAknIconArray( 1 );
    CleanupStack::PushL( iconArray );
  
    for ( TInt i = 0; i <= aApplicationArray.MdcaCount() - 1; i++ )
    	{
    	HBufC* firstLine = aApplicationArray.MdcaPoint( i ).AllocLC();
    	HBufC* secondLine = NULL;
    	if ( !(aInstalledFolderArray.MdcaCount() <  (i + 1 ) ) )
    		{
            if ( aInstalledFolderArray.MdcaPoint( i ).Length() > 0 ) 
        		{
        		secondLine = StringLoader::LoadLC( 
        		                       R_SWCOMMON_INSTALLED_FOLDER_HEAD, 
        		                       aInstalledFolderArray.MdcaPoint( i ) );
        		}
            else
                {
                // show just empty second lines for empty folder names
                secondLine = KNullDesC().AllocLC();
                }
    		}
    	if ( secondLine == NULL )
    		{
    		// default folder is used
    		secondLine = StringLoader::LoadLC( R_SWCOMMON_INSTALLED_FOLDER );
    		}
    	HBufC* listItem = HBufC::NewLC( firstLine->Length() + 
    	                                secondLine->Length() + 10 );	
      	listItem->Des().Format( KFormat, i, firstLine, secondLine );	
    	
    	itemArray->AppendL( listItem->Des() );	
    	CleanupStack::PopAndDestroy( listItem ); 
    	CleanupStack::PopAndDestroy( secondLine ); 
    	CleanupStack::PopAndDestroy( firstLine );
    	    	
        
        // Check first that there is an item in array
        if ( !( aIconArray.Count() <  ( i + 1 ) ) )
        	{
        	CGulIcon* icon = CGulIcon::NewL();
            CleanupStack::PushL( icon );
        	icon->SetBitmapsOwnedExternally( ETrue );
        	icon->SetBitmap( aIconArray.At(i)->Bitmap() );
        	icon->SetMask( aIconArray.At(i)->Mask() );
        	iconArray->AppendL( icon );
        	CleanupStack::Pop( icon );
           	}
       	}
        
    // show list query
    CAknListQueryDialog* dlg = new ( ELeave ) CAknListQueryDialog( 
                                                 &aIndexOfAppToBeStarted );
    dlg->PrepareLC( R_SWCOMMON_START_QUERY );
    dlg->SetItemTextArray( itemArray );
    dlg->SetIconArrayL( iconArray );
    
    dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );

    TInt retQuery = dlg->RunLD();
    CleanupStack::Pop( iconArray );
     
    CleanupStack::PopAndDestroy( itemArray );  
    
    TBool appIsStarted = EFalse;
    if ( retQuery == EAknSoftkeyOk )
        {        
        appIsStarted = ETrue;
        }
    else
    	{
    	aIndexOfAppToBeStarted = KErrNotFound;	
    	}    

    return appIsStarted;
    */
    return EFalse;
    }   
    
// -----------------------------------------------------------------------------
// CCUIDialogs::ShowUninstallConfirmationForSeveralAppsL
// Show an installation complete query.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
EXPORT_C TBool CCUIDialogs::ShowUninstallConfirmationForSeveralAppsL(
                                const TDesC& aApplicationGroup,
                                const MDesCArray& aApplicationArray,
                                TInt aSoftkeyResourceId ) const
	{
    /*
    HBufC* appList = HBufC::NewLC( 0 ); 
    TInt count = aApplicationArray.MdcaCount();   
    for ( TInt i = 0; i < count; i++ )
    	{
    	HBufC* tmp = appList;
    	appList = appList->ReAllocL(
    	                   tmp->Length() + 
    	                   aApplicationArray.MdcaPoint( i ).Length() +
    	                   KNextLine().Length() );
    	CleanupStack::Pop( tmp );
        CleanupStack::PushL( appList ); 
        appList->Des().Append( aApplicationArray.MdcaPoint( i ) ); 
        if ( (i+1) < count )
        	{
        	appList->Des().Append( KNextLine() );
        	}
    	}
    CDesCArray* stringArray = new( ELeave ) CDesCArrayFlat( 2 );
    CleanupStack::PushL( stringArray ); 
    stringArray->AppendL( aApplicationGroup );
    stringArray->AppendL( *appList );
    CleanupStack::Pop( stringArray );
    CleanupStack::PopAndDestroy( appList );
    CleanupStack::PushL( stringArray );	
    HBufC* message = StringLoader::LoadLC( 
                            R_SWCOMMON_QUERY_UNINST_MIDLET_SUITE,
                            *stringArray );	
    CleanupStack::Pop( message );	                    
    CleanupStack::PopAndDestroy( stringArray );	                    
    CleanupStack::PushL( message );
        
    CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL( *message );
    dlg->PrepareLC( R_SWCOMMON_MULTI_UNINSTALL_DIALOG ); 
    dlg->ButtonGroupContainer().SetCommandSetL( aSoftkeyResourceId );
    
    TBool response( EFalse );
    if ( dlg->RunLD() )    
        {
        response = ETrue; 
        }
    CleanupStack::PopAndDestroy( message );
    return response; 
    */
    return EFalse;
	}                                
                                     

//    
// -----------------------------------------------------------------------------                                                
// CCUIDialogs::SetWaitDialogCallback
// Set callback for the wait dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCUIDialogs::SetWaitDialogCallback( MCUIWaitDialogCallback* aCallback )
    {
    if ( aCallback )
        {
        iWaitDialog->SetCallback( aCallback );        
        }
    }

// -----------------------------------------------------------------------------
// CCUIDialogs::SetProgressDialogCallback
// Set callback for the progress dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCUIDialogs::SetProgressDialogCallback( MCUIWaitDialogCallback* aCallback )
    {
    if ( aCallback )
        {
        iProgressDialog->SetCallback( aCallback );        
        }
    }

// -----------------------------------------------------------------------------
// CCUIDialogs::ShowSecurityWarningHelp
// Used as a callback function in message query. Launches help for security
//  warning.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCUIDialogs::ShowSecurityWarningHelp( TAny* aPtr )
    {
    //TRAP_IGNORE( reinterpret_cast<CCUIDialogs*>(aPtr)->LaunchHelpL( KAM_HLP_INSTALL_UNTRUSTED ) );    

    return KErrNone;    
    }





