/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CSisxUIHandler
*                class member functions.
*
*/


// INCLUDE FILES
#include <StringLoader.h>
#include <barsread.h>
#include <eikenv.h>
#include <aknlistquerydialog.h>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <SisxUIData.rsg>
#include <SWInstCommonUI.rsg>
#include <SWInstDefs.h>
#include <featmgr.h>
//#include <csxhelp/am.hlp.hrh>
#include <swi/sisinstallerrors.h>
#include <AknUtils.h>
#include <eikfrlb.h> //for the marquee effect
#include <eikfrlbd.h> //for the marquee effect

#include "SisxUIHandler.h"
#include "SisxUICertificateInfo.h"
#include "SisxUIAppInfo.h"
#include "SisxUISelectionCache.h"
#include "SisxUIResult.h"
#include "SisxUILangName.h"             // CLangName
#include "SisxUICleanupUtils.h"         // CleanupResetAndDestroyPushL
#include "CUIUtils.h"
#include "SWInstDebug.h"

using namespace SwiUI;

_LIT( KDetailedError,"%S (%d)" );
_LIT( KSwiTempFile,"c:\\temp\\swierrors.txt" );
_LIT( KNextLine, "\n" );
_LIT( KLeftParenthes, "(" );
//directional marks needed for Arabic language
_LIT( KLRE, "\x202A" );
_LIT( KPDF, "\x202C" );

const TInt KSpaceForErrorCode = 12;     // space for " (-12345678)"

// ============================ LOCAL FUNCTIONS ================================

TBool HasLangId( const TLanguage* aId, const CLangName& aLang )
    {
    return( *aId == aLang.Id() );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSisxUIHandler::CSisxUIHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSisxUIHandler::CSisxUIHandler( TInstallMode aMode, MSisxUICancelObserver* aCancelObserver )
    : iMode( aMode ), 
      iCancelObserver( aCancelObserver ), 
      iAllowUntrusted( ETrue )
    {    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSisxUIHandler::ConstructL()
    {
    iCommonDialogs = CommonUI::CCUIDialogs::NewL();
    iCache = CSisxUISelectionCache::NewL();
    if ( iMode == EModeInstall )
        {        
        iCommonDialogs->ShowWaitDialogL( R_SISXUI_PREPARING, this, R_AVKON_SOFTKEYS_EMPTY );
        }    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSisxUIHandler* CSisxUIHandler::NewL( TInstallMode aMode, MSisxUICancelObserver* aCancelObserver )
    {
    CSisxUIHandler* self = new( ELeave ) CSisxUIHandler( aMode, aCancelObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }
    
// Destructor
CSisxUIHandler::~CSisxUIHandler()
    {
    // If wait dialog is showing close it.  	
    if ( iShowingWaitDialog )
        {        
        if ( iCommonDialogs )
            {
        	TRAP_IGNORE( iCommonDialogs->CloseWaitDialogL() );	
            }
        }    	    	
    delete iCommonDialogs;
    iCertificateArray.ResetAndDestroy();
    delete iCache;    
    iPackages.ResetAndDestroy();  
    iDrmFile.Close();
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::DisplayInstallL
// Displays a dialog at the beginning of an installation with details about the 
// SISX package being installed. This dialog must be shown in TrustedUI mode.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUIHandler::DisplayInstallL( 
#ifdef RD_COMBINE_INSTALLATION_QUERIES
                              const Swi::CAppInfo& /*aAppInfo*/,
#else
                              const Swi::CAppInfo& aAppInfo,
#endif //RD_COMBINE_INSTALLATION_QUERIES
                              const CApaMaskedBitmap* /*aLogo*/,
                              const RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/ )  
    {
#ifdef RD_COMBINE_INSTALLATION_QUERIES //with new dialog sequence
    
    TBool result( ETrue );
    // Mark the integrity as done
    iIntegrityCheckDone = ETrue;
    iShowingDialog = EFalse;

#else  // show using old dialog sequence 
    iShowingDialog = ETrue;
    
    // Close the preparing wait dialog
    iCommonDialogs->CloseWaitDialogL();

    TBool result( EFalse );

    HBufC* prompt = StringLoader::LoadLC( R_SISXUI_INSTALL_PROMPT, aAppInfo.AppName() );
    if ( iCommonDialogs->ShowConfirmationQueryL( *prompt, R_AVKON_SOFTKEYS_YES_NO__YES ) )
        {        
        result = ETrue;       
        }   
    else
        {
        iResult = TSisxUIResult( EUiResultCancelled );
        // Make sure no notes are shown after this        
        iShownFinalNote = ETrue;        
        }
   
    CleanupStack::PopAndDestroy( prompt );

    // Mark the integrity as done
    iIntegrityCheckDone = ETrue;

    iShowingDialog = EFalse;    
#endif //RD_COMBINE_INSTALLATION_QUERIES    
    return result;
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::DisplayGrantCapabilitiesL
// displays a dialog requesting that the user grants these capabilities to the 
// SISX package being installed. This dialog must be shown in TrustedUI mode.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUIHandler::DisplayGrantCapabilitiesL( const Swi::CAppInfo& /*aAppInfo*/, 
                                                 const TCapabilitySet& aCapabilitySet ) 
    {
    iShowingDialog = ETrue;

    TBool result( ETrue );

    // Add the text heading
    HBufC* messageString = StringLoader::LoadLC( R_SISXUI_CAPAB_HEADING );

    if ( aCapabilitySet.HasCapability( ECapabilityLocalServices ) )
        {
        AddCapabilityL( messageString, R_SISXUI_CAPAB_LOCAL );
        }

    if ( aCapabilitySet.HasCapability( ECapabilityReadUserData ) )
        {
        AddCapabilityL( messageString, R_SISXUI_CAPAB_READ_USER );
        }

    if ( aCapabilitySet.HasCapability( ECapabilityWriteUserData ) )
        {
        AddCapabilityL( messageString, R_SISXUI_CAPAB_WRITE_USER );
        }

    if ( aCapabilitySet.HasCapability( ECapabilityUserEnvironment ) )
        {
        AddCapabilityL( messageString, R_SISXUI_CAPAB_USERENV );
        }

    if ( aCapabilitySet.HasCapability( ECapabilityNetworkServices ) )
        {
        AddCapabilityL( messageString, R_SISXUI_CAPAB_NETWORK );
        }

    if ( aCapabilitySet.HasCapability( ECapabilityLocation ) )
        {
        AddCapabilityL( messageString, R_SISXUI_CAPAB_LOCATION );
        }

    HBufC* linkText = NULL;    
    if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {        
        linkText = StringLoader::LoadLC( R_SISXUI_CAPAB_MORE_INFO );
        }
    else
        {
        linkText = KNullDesC().AllocLC();        
        }
    
    HBufC* finalMessage = HBufC::NewLC( messageString->Length() + linkText->Length() + 1);
    TPtr msgPtr( finalMessage->Des() );
    
    msgPtr += *messageString;
    msgPtr += KNextLine;
    msgPtr += *linkText;    

    // Show the dialog
    CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL( *finalMessage );
    dlg->PrepareLC( R_SISXUI_CAPAB_DIALOG );
    if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {        
        dlg->SetLinkTextL( *linkText );
        TCallBack callback( ShowCapabilitiesHelp, this );    
        dlg->SetLink( callback );
        }    

    if ( !dlg->RunLD() )
        {
        // User cancelled
        iResult = TSisxUIResult( EUiResultCancelled );
        result = EFalse;        
        }    
    
    CleanupStack::PopAndDestroy( 3, messageString ); // finalMessage, linkText, messageString

    iShowingDialog = EFalse;    
    return result;       
    }
										
// -----------------------------------------------------------------------------
// CSisxUIHandler::DisplayLanguageL
// requests preferred language and dialect information from the UI.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSisxUIHandler::DisplayLanguageL( const Swi::CAppInfo& /*aAppInfo*/, 
                                       const RArray<TLanguage>& aLanguages ) 
    {
    TInt languageIndex( 0 );
    TInt index( 0 );
    TLanguage phoneLanguage( User::Language() );    

    // If phones default language is available in the file, do not query the user
    for ( index = 0; index < aLanguages.Count(); index++ )
        {
        // Check if this matches the phone language        
        if( aLanguages[ index ] == phoneLanguage )
            {
            return index;
            }
        
        // Check if user has already selected this language during installation        
        if( iCache->LanguageCached() && aLanguages[index] == iCache->Language() )
            {
            // Do not query user, we have a cached language    
            return index;            
            }        
        }

    // If there is only one language in the list, do not query the user
    if( aLanguages.Count() == 1 )
        {
        return 0;        
        }

    iShowingDialog = ETrue;

    // Prepare for showing the language selection dialog
    CDesCArray *itemArray = new( ELeave ) CDesCArrayFlat(4);
    CleanupStack::PushL( itemArray );
    
    // construct the array of available languages
    TResourceReader reader;
    CEikonEnv::Static()->CreateResourceReaderLC( reader, R_SISXUI_LANGUAGES_ARRAY );
    RPointerArray<CLangName> langNameArray;
    CleanupResetAndDestroyPushL( langNameArray );
    TInt count = reader.ReadInt16();
    while( --count >= 0 )
        {
        CLangName* langName = CLangName::NewL( reader );
        langNameArray.AppendL( langName );
        }

    for ( index = 0; index <= aLanguages.Count() - 1; index++ )
        {
        TLanguage language = aLanguages[ index ];
        TInt nameIndex = langNameArray.Find( language, HasLangId );
        if ( nameIndex == KErrNotFound ) 
            {
            nameIndex = TLanguage( 0 );     // produces qtn_swins_lqi_unknow
            }
        itemArray->AppendL( langNameArray[ nameIndex ]->Name() );
        }
    
    // show list query
    CAknListQueryDialog* dlg = new ( ELeave ) CAknListQueryDialog( &languageIndex );
    dlg->PrepareLC( R_SISXUI_LANGUAGE_QUERY );
    dlg->SetItemTextArray( itemArray );
    dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );

    if( languageIndex != 0 )
        {
        dlg->ListBox()->SetCurrentItemIndex( languageIndex );
        }

    TInt queryOk = dlg->RunLD();
    
    CleanupStack::PopAndDestroy( 3, itemArray ); // langNameArray, reader, itemArray
    
    if ( !queryOk )
        {        
        languageIndex = -1;
        iResult = TSisxUIResult( EUiResultCancelled );       
        }
    
    // Cache only if no previous cache is found
    if( languageIndex != -1 && !iCache->LanguageCached() )
        {
        iCache->SetLanguage( aLanguages[languageIndex] );
        }     
    
    iShowingDialog = EFalse;
    return languageIndex;
    }
	
// -----------------------------------------------------------------------------
// CSisxUIHandler::DisplayDriveL
// Requests the drive to install the package on, from the UI.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSisxUIHandler::DisplayDriveL( const Swi::CAppInfo& /*aAppInfo*/, 
                                    TInt64 aSize,
                                    const RArray<TChar>& aDriveLetters,
                                    const RArray<TInt64>& aDriveSpaces ) 
    {
    if ( aDriveLetters.Count() <= 0 )
        {
        iResult = TSisxUIResult( EUiResultInsufficientMemory );
        return -1;        
        }    

    iShowingDialog = ETrue;

    RArray<TDriveUnit> driveUnits;
    CleanupClosePushL( driveUnits );

    TInt index( 0 );

    // Convert drive letters to TDriveUnits
    for ( index = 0; index < aDriveLetters.Count(); index++ )
        {
        TBuf<2> tmp;
        tmp.Append( aDriveLetters[index] );
        driveUnits.Append( TDriveUnit( tmp ) );
        }
    
    TInt selection( -1 );
    TInt dlgResult( KErrNone );    

    // See if the selection has been cached
    if ( iCache->DriveCached() )
        {
        for ( index = 0; index < aDriveLetters.Count(); index++ )
            {
            if ( (TInt)iCache->Drive() == (TInt)driveUnits[index] )
                {
                // Make sure, we have enough size in the cached drive
                if ( aSize < aDriveSpaces[index] )
                    {
                    selection = index;
                    break;                    
                    }  
                else
                    {
                    // Cached drive has insufficient memory, ask user again
                    dlgResult = iCommonDialogs->ShowDriveSelectionDialogL( aSize, 
                                                                           driveUnits, 
                                                                           aDriveSpaces, 
                                                                           selection );
                                
                    break;
                    }                
                }
            }        
        }
    // Not cached
    else
        {
        dlgResult = iCommonDialogs->ShowDriveSelectionDialogL( aSize, 
                                                               driveUnits, 
                                                               aDriveSpaces, 
                                                               selection );
        }

    if ( dlgResult == KErrNoMemory )
        {                       
        // No memory
        iResult = TSisxUIResult( EUiResultInsufficientMemory );
        selection = -1;                        
        }  

    else if ( dlgResult == KErrCancel )
        {
        // User cancelled
        iResult = TSisxUIResult( EUiResultCancelled );
        selection = -1;                        
        }    

    // If the result is valid, put it to the cache
    else if ( selection >= 0 )
        {            
        iCache->SetDrive( driveUnits[selection] );
        }

    CleanupStack::PopAndDestroy(); // driveUnits

    iShowingDialog = EFalse;
    return selection;    
    }
	
// -----------------------------------------------------------------------------
// CSisxUIHandler::DisplayUpgradeL
// Displays a dialog asking whether we want to upgrade an application present 
// on the device.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUIHandler::DisplayUpgradeL( const Swi::CAppInfo& aAppInfo, 
                                       const Swi::CAppInfo& aExistingAppInfo ) 
    {
    iShowingDialog = ETrue;

    TBool result( EFalse );

    TVersion newVersion = aAppInfo.AppVersion();
    TVersion oldVersion = aExistingAppInfo.AppVersion();

    // Display "Replace?" confirmation query only if the new version is older
    // than the currently installed old version (i.e. when downgrading). See
    // also User::QueryVersionSupported() although it has bit different meaning.
    TBool downgrading = ( newVersion.iMajor < oldVersion.iMajor ||
            ( newVersion.iMajor == oldVersion.iMajor && newVersion.iMinor < oldVersion.iMinor ) );
    if( !downgrading )
        {
        iShowingDialog = EFalse;
        return ETrue;
        }

    // Create strings from the version numbers
    //
    HBufC* tempBuf = 
        CommonUI::CUIUtils::ConstructVersionStringLC( oldVersion.iMajor, 
                                                      oldVersion.iMinor, 
                                                      oldVersion.iBuild );
                                                      
    HBufC* oldVersionBuf = HBufC::NewLC( KLRE().Length() + tempBuf->Length() + KPDF().Length() ); 
    TInt position = tempBuf->Find( KLeftParenthes() );
    if ( position >= 0 )
        {
        oldVersionBuf->Des() = tempBuf->Mid( 0, position );
        oldVersionBuf->Des() += KLRE();
        oldVersionBuf->Des() += tempBuf->Mid( position, tempBuf->Length() - position );
        oldVersionBuf->Des() += KPDF();
        }
    else
        {
        oldVersionBuf->Des() = *tempBuf;	
        }
    CleanupStack::Pop( oldVersionBuf );    
    CleanupStack::PopAndDestroy( tempBuf );
    CleanupStack::PushL( oldVersionBuf );
    tempBuf = NULL;
  
    tempBuf = CommonUI::CUIUtils::ConstructVersionStringLC( newVersion.iMajor, 
                                                            newVersion.iMinor, 
                                                            newVersion.iBuild );
    
    HBufC* newVersionBuf = HBufC::NewLC( KLRE().Length() + tempBuf->Length() + KPDF().Length() );

    position = tempBuf->Find( KLeftParenthes() );
    if ( position >= 0 )
        {
        newVersionBuf->Des() = tempBuf->Mid( 0, position );
        newVersionBuf->Des() += KLRE();
        newVersionBuf->Des() += tempBuf->Mid( position, tempBuf->Length() - position );
        newVersionBuf->Des() += KPDF();
        }
    else
        {
        newVersionBuf->Des() = *tempBuf;	
        }
    CleanupStack::Pop( newVersionBuf );
    CleanupStack::PopAndDestroy( tempBuf );
    CleanupStack::PushL( newVersionBuf );

    CDesCArray* stringArray = new ( ELeave ) CDesCArrayFlat( 4 );
    CleanupStack::PushL( stringArray );
    
    stringArray->AppendL( aAppInfo.AppName() );    
    stringArray->AppendL( *oldVersionBuf );    
    stringArray->AppendL( *newVersionBuf );    

    HBufC* upgradeString = 
        StringLoader::LoadLC( R_SISXUI_UPGRADE_PROMPT, *stringArray );
    TPtr ptr = upgradeString->Des();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr ); 


    if ( iCommonDialogs->ShowConfirmationQueryL( *upgradeString ) )
        {
        result = ETrue;        
        }   
    else
        {
        // User selected cancel. 
        iResult = TSisxUIResult( EUiResultCancelled );
        }
    
    CleanupStack::PopAndDestroy( 4 ); // upgradeString, stringArray, newVersionBuf, oldVersionBuf
    
    iShowingDialog = EFalse;
    return result;
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::DisplayOptionsL
// Displays a dialog box asking which options the user wishes to install.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUIHandler::DisplayOptionsL( const Swi::CAppInfo& /*aAppInfo*/,
                                       const RPointerArray<TDesC>& aOptions,
                                       RArray<TBool>& aSelections ) 
    {
    iShowingDialog = ETrue;

    TBool response;
    TInt index = 0;    

    if ( aOptions.Count() < 1 )
        {
        User::Leave( KErrArgument );        
        }
    
    CDesCArray* itemArray = new ( ELeave ) CDesCArrayFlat( 4 );
    CleanupStack::PushL( itemArray );

    // Construct text array for the dialog
    for( index = 0; index < aOptions.Count(); index++ )
        {
        TPtrC itemLabel( *( aOptions[index] ) ) ;        
        HBufC *itemLine = HBufC::NewLC( itemLabel.Length() + 2 ); // Add 2 for "1\t"
        TPtr itemPtr = itemLine->Des();
        itemPtr.Copy( _L("1\t") );
        itemPtr.Append( itemLabel );
        
        itemArray->AppendL( itemPtr );

        CleanupStack::PopAndDestroy( itemLine );
        }    

    // Create and selection index array for multiselection list
    CListBoxView::CSelectionIndexArray* selections = 
        new ( ELeave ) CArrayFixFlat<TInt>( aSelections.Count() );
    CleanupStack::PushL( selections );    
    // Set all selections marked by default.
    for ( index = 0; index < aSelections.Count(); index++ )
        {
        selections->AppendL( index );        
        }

    // Create multiselection list query
    CAknListQueryDialog* dlg = new ( ELeave ) CAknListQueryDialog( selections );

    dlg->PrepareLC( R_SISXUI_OPTIONS_QUERY );
    dlg->SetItemTextArray( itemArray );
    // Set all selections marked by default.
    dlg->ListBox()->SetSelectionIndexesL( selections );    
    
    dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );
    ((CEikFormattedCellListBox*)(dlg->ListBox()))->ItemDrawer()->FormattedCellData()->EnableMarqueeL( ETrue );
    TInt queryOk = dlg->RunLD();
    
    if( !queryOk )
        {
        response = EFalse;
        iResult = TSisxUIResult( EUiResultCancelled );
        }
    else
        {
        response = ETrue;

        // First set all items in aSelections to EFalse
        for( index = 0; index < aSelections.Count(); index++ )
            {
            aSelections[index] = EFalse;
            }
        // Then copy selections to aSelections
        for( index = 0; index < selections->Count(); index++ )
            {
            TInt selectedItem = selections->At( index );
            aSelections[selectedItem] = ETrue;
            }
        
        }

    CleanupStack::PopAndDestroy( 2 ); // itemArray, selections
 
    iShowingDialog = EFalse;
    return response;
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::HandleInstallEventL
// Informs the UI of events occurring during installation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUIHandler::HandleInstallEventL( const Swi::CAppInfo& aAppInfo,
                                          Swi::TInstallEvent aEvent, 
                                          TInt aValue, 
                                          const TDesC& /*aDesC*/ )
    {
    TBool result( ETrue );    
    iShowingDialog = ETrue;

    switch ( aEvent )
        {
        case Swi::EEventSetProgressBarFinalValue:
            if ( iMode == EModeInstall )
                {                
                // Show installing progress dialog.
                iCommonDialogs->ShowProgressDialogL( R_SISXUI_INSTALLING, this );

                // Here we need to update the name and vendor of the main package.
                // They might have changed when a language has been selected.
                if ( iPackages.Count() > 0 )
                    {                    
                    iPackages[0]->SetNameL( aAppInfo.AppName() );
                    iPackages[0]->SetVendorL( aAppInfo.AppVendor() );
                    }                
                }
            else
                {               
                // Show uninstalling progress dialog.
                iCommonDialogs->ShowProgressDialogL( R_SISXUI_UNINSTALLING, this, R_AVKON_SOFTKEYS_EMPTY );
                }         
            iCommonDialogs->SetProgressDialogFinalValueL( aValue );

            break;  
            
        case Swi::EEventUpdateProgressBar:
            iCommonDialogs->UpdateProgressDialogValueL( aValue );
            break;  
       
        case Swi::EEventOcspCheckEnd:
            iCommonDialogs->CloseWaitDialogL();
            iOcspInProgress = EFalse;            
            break;

        case Swi::EEventDevCert:
            {        
            HBufC* messageString = StringLoader::LoadLC( R_SISXUI_DEVCERT_WARNING );
            CAknMessageQueryDialog* dlg = 
                CAknMessageQueryDialog::NewL( *messageString, CAknQueryDialog::EWarningTone );
            if ( !dlg->ExecuteLD( R_SISXUI_DEVCERT_DIALOG ) )
                {
                // User cancelled
                iResult = TSisxUIResult( EUiResultCancelled );
                result = EFalse;                
                }    
            CleanupStack::PopAndDestroy( messageString );
            }
            break;

            // Ignored
        case Swi::EEventCompletedUnInstall:            
        case Swi::EEventCompletedInstall:                        
        case Swi::EEventAbortedInstall:
        case Swi::EEventAbortedUnInstall:       
        case Swi::EEventLeave:
        default:
            break;
            
        }

    // See if user has requested an cancel and our state is cancellable
    if ( result && iCancelRequested &&  
         ( aEvent == Swi::EEventSetProgressBarFinalValue ||
           aEvent == Swi::EEventUpdateProgressBar ||
           aEvent == Swi::EEventOcspCheckEnd ) )
        {
        iCancelRequested = EFalse;        
        // Show the confirmation prompt
        if ( iCommonDialogs->ShowConfirmationQueryL( R_SWCOMMON_CANCEL_INSTALLATION, 
                                                     R_AVKON_SOFTKEYS_YES_NO__YES ) )
            {
            // Ok to cancel       
            iResult = TSisxUIResult( EUiResultCancelled );
            iCancelObserver->UserCancelL( ETrue );            
            }                
        }    
    
    iShowingDialog = EFalse;
    return result;
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::HandleCancellableInstallEventL
// Informs the UI of cancellable events occurring during installation.  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIHandler::HandleCancellableInstallEventL( const Swi::CAppInfo& /*aAppInfo*/,
                                                     Swi::TInstallCancellableEvent aEvent, 
                                                     Swi::MCancelHandler& /*aCancelHandler*/,
                                                     TInt /*aValue*/, 
                                                     const TDesC& /*aDes*/ )
    {
    switch ( aEvent )
        {
        case Swi::EEventOcspCheckStart:
            {
            // Show ocsp wait dialog
            iCommonDialogs->ShowWaitDialogL( R_SISXUI_DOING_OCSP, this );
            iOcspInProgress = ETrue;            
            break;      
            }

            // These are ignored
        case Swi::EEventRemovingFiles:
        case Swi::EEventCopyingFiles:
        case Swi::EEventShuttingDownApps:
        default:
            break;            
        }    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::DisplaySecurityWarningL
// Displays a dialog indicating that the package is unsigned, or that signature 
// validation has failed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUIHandler::DisplaySecurityWarningL( const Swi::CAppInfo& aAppInfo,
                                               Swi::TSignatureValidationResult aSigValidationResult,
                                               RPointerArray<CPKIXValidationResultBase>& aPkixResults,
                                               RPointerArray<Swi::CCertificateInfo>& aCertificates,
                                               TBool aInstallAnyway )
    {
#ifdef RD_COMBINE_INSTALLATION_QUERIES //with new dialog sequence
    // Close the preparing wait dialog
    iCommonDialogs->CloseWaitDialogL();
#endif //RD_COMBINE_INSTALLATION_QUERIES
    iShowingDialog = ETrue;

    FLOG_1( _L("CSisxUIHandler::DisplaySecurityWarningL: %d"), aSigValidationResult );    
    TBool result( ETrue );
    
    switch ( aSigValidationResult )
        {
        case Swi::EValidationSucceeded:       
            {
            CSisxUIAppInfo* appInfo = CSisxUIAppInfo::NewL( aAppInfo, ETrue );
            iPackages.Append( appInfo );
            
            for ( TInt index = 0; index < aCertificates.Count(); index++ )
                {
                TValidationStatus status = aPkixResults[index]->Error();
                if ( status.iReason == EValidatedOK )
                    {                    
                    CSisxUICertificateInfo* certInfo = CSisxUICertificateInfo::NewL( *aCertificates[index] );
                    CleanupStack::PushL( certInfo );
                    User::LeaveIfError( iCertificateArray.Append( certInfo ));
                    CleanupStack::Pop( certInfo );
                    }                
                }

            appInfo->PrepareIteratorL( aAppInfo );            
#ifdef RD_COMBINE_INSTALLATION_QUERIES //with new dialog sequence
            CommonUI::CCUIDetailsDialog* details = 
                CommonUI::CCUIDetailsDialog::NewL( R_AVKON_SOFTKEYS_OK_CANCEL__OK, ETrue );
#else       //with old dialog sequence            
            CommonUI::CCUIDetailsDialog* details = 
                CommonUI::CCUIDetailsDialog::NewL( R_SWCOMMON_SOFTKEYS_CONTINUE_CANCEL__CONTINUE, ETrue );
#endif //RD_COMBINE_INSTALLATION_QUERIES
            if ( !iIsDrmFileSet )
                {                
                result = details->ExecuteLD( *appInfo, iCertificateArray );
                }
            else
                {
                result = details->ExecuteLD( *appInfo, iCertificateArray, iDrmFile );
                }
            appInfo->FreeIterator();            

            if ( !result )
                {
                // User cancelled
                iResult = TSisxUIResult( EUiResultCancelled );
                }            
            
            iCertificateArray.ResetAndDestroy();            
            
            break;
            }       

            // Non fatals
        case Swi::ESignatureSelfSigned:
        case Swi::ESignatureNotPresent:
        case Swi::ENoCertificate:
        case Swi::ECertificateValidationError:
        case Swi::ENoCodeSigningExtension:
        case Swi::ENoSupportedPolicyExtension:
            // Fatals with aInstallAnyway set as EFalse
        case Swi::ESignatureCouldNotBeValidated:
        case Swi::EMandatorySignatureMissing:
            {
            // Check if we are allowed to continue installation
            if ( aInstallAnyway && iAllowUntrusted )
                {          
#ifdef RD_COMBINE_INSTALLATION_QUERIES //with new dialog sequence

                CSisxUIAppInfo* appInfo = CSisxUIAppInfo::NewL( aAppInfo, EFalse );
                iPackages.Append( appInfo );
                appInfo->PrepareIteratorL( aAppInfo );            
                CommonUI::CCUIDetailsDialog* details = 
                CommonUI::CCUIDetailsDialog::NewL( R_AVKON_SOFTKEYS_OK_CANCEL__OK, ETrue );
                if ( !iIsDrmFileSet )
                    {                
                    result = details->ExecuteLD( *appInfo );
                    }
                else
                    {
                    result = details->ExecuteLD( *appInfo, iDrmFile );
                    }
                appInfo->FreeIterator();
                if ( result )
                    {
                	result = iCommonDialogs->ShowSecurityWarningDialogL();
                    }
                        
#else           //with old dialog sequence   
      
                result = iCommonDialogs->ShowSecurityWarningDialogL();
                
                if ( result )
                    {
                    CSisxUIAppInfo* appInfo = CSisxUIAppInfo::NewL( aAppInfo, EFalse );
                    iPackages.Append( appInfo );
                    appInfo->PrepareIteratorL( aAppInfo );            
                    CommonUI::CCUIDetailsDialog* details = 
                        CommonUI::CCUIDetailsDialog::NewL( R_SWCOMMON_SOFTKEYS_CONTINUE_CANCEL__CONTINUE );
                    if ( !iIsDrmFileSet )
                        {                
                        result = details->ExecuteLD( *appInfo );
                        }
                    else
                        {
                        result = details->ExecuteLD( *appInfo, iDrmFile );
                        }
                    appInfo->FreeIterator();
                    }
                    
#endif //RD_COMBINE_INSTALLATION_QUERIES

                if ( !result )
                    {
                    // User cancelled
                    iResult = TSisxUIResult( EUiResultCancelled );
                    }                
                }
            else
                {
                iResult = TSisxUIResult( aSigValidationResult );
                result = EFalse;

                // Fatal error, not allowed to continue
                // Try to see if the error was due to invalid date in the certificate
                for ( TInt index = 0; index < aPkixResults.Count(); index++ )
                    {
                    TValidationStatus status = aPkixResults[index]->Error();
                    if ( status.iReason == EDateOutOfRange )
                        {
                        // Compare timestamps, use secure time if available
                        TTime now;
                        if( now.UniversalTimeSecure() != KErrNone )
                            {
                            // KErrNoSecureTime or other error
                            now.UniversalTime();
                            }
                        TTime validFrom( aCertificates[index]->ValidFrom() );
                        
                        if ( now < validFrom )
                            {
                            // Certificate valid in future
                            iResult = TSisxUIResult( EUiResultCertificateValidInFuture );
                            }
                        else
                            {
                            // Certificate expired
                            iResult = TSisxUIResult( EUiResultCertificateExpired );
                            }

                        break;                        
                        }                    
                    }
                }            
            break;
            }

        default:
            iResult = TSisxUIResult( aSigValidationResult );
            result = EFalse;
            break;            
        }
    
    iShowingDialog = EFalse;
    return result;    
    }
	
// -----------------------------------------------------------------------------
// CSisxUIHandler::DisplayOcspResultL
// Displays a dialog indicating that there was an error carrying out online 
// certificate status checking.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUIHandler::DisplayOcspResultL( const Swi::CAppInfo& /*aAppInfo*/,
                                          Swi::TRevocationDialogMessage aMessage, 
                                          RPointerArray<TOCSPOutcome>& /*aOutcomes*/, 
                                          RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/,
                                          TBool aWarningOnly )
    {
    iShowingDialog = ETrue;

    FLOG_1( _L("CSisxUIHandler::DisplayOcspResultL: %d"), aMessage );    
    TBool result( ETrue );

    // This will be treated as warning if engine says it's a warning and
    // ocsp setting is on. In case it's must, we abort the installation.
    if ( aWarningOnly && iOcspProc == ESisxUIOcspProcOn )
        {
        result = iCommonDialogs->ShowConfirmationQueryL( R_SISXUI_OCSP_SECURITY_WARNING );
        if ( !result )
            {
            // User cancelled the installation
            iResult = TSisxUIResult( EUiResultCancelled );
            }                
        }
    // error or ocsp is set to must
    else
        {
        iResult = TSisxUIResult( aMessage );
        result = EFalse; 
        }            

    iShowingDialog = EFalse;
    return result;    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::DisplayCannotOverwriteFileL
// Used to inform the user the installation they are attempting is trying to 
// overwrite a file that does not belong to it. Following this message 
// installation is aborted.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIHandler::DisplayCannotOverwriteFileL( const Swi::CAppInfo& /*aAppInfo*/,
                                                  const Swi::CAppInfo& /*aInstalledAppInfo*/,
                                                  const TDesC& aFileName )
    {
    FLOG_1( _L("CSisxUIHandler::DisplayCannotOverwriteFileL: %S"), &aFileName );    
    iResult = TSisxUIResult( EUiResultUnableToOverwrite );
    // Keep compiler happy
    (void)aFileName;    
    }
 
// -----------------------------------------------------------------------------
// CSisxUIHandler::DisplayMissingDependencyL
// Used to inform the user of a missing dependency needed to install the 
// current package. It is up to the UI implementors and/or the user whether
// or not this warning can be overridden.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUIHandler::DisplayMissingDependencyL( const Swi::CAppInfo& /*aAppInfo*/,
                                                 const TDesC& aDependencyName,
                                                 TVersion aWantedVersionFrom,
                                                 TVersion aWantedVersionTo,
                                                 TVersion aInstalledVersion )
    {
    iShowingDialog = ETrue;

    TBool result( EFalse );
    
    // Dependency component not installed    
    if ( ( aInstalledVersion.iMajor == 0 && 
           aInstalledVersion.iMinor == 0 &&
           aInstalledVersion.iBuild == 0 ) || 
         ( aWantedVersionFrom.iMajor == 0 && 
           aWantedVersionFrom.iMinor == 0 &&
           aWantedVersionFrom.iBuild == 0 ) )     
        {
        HBufC* prompt = 
            StringLoader::LoadLC( R_SISXUI_COMPONENT_MISSING, aDependencyName );
        
        if ( iCommonDialogs->ShowConfirmationQueryL( *prompt ) )
            {
            result = ETrue;        
            }

        CleanupStack::PopAndDestroy( prompt );        
        }

    else
        {
        CDesCArray* stringArray = new ( ELeave ) CDesCArrayFlat( 2 );
        CleanupStack::PushL( stringArray );
        
        stringArray->AppendL( aDependencyName );
        stringArray->AppendL( aWantedVersionFrom.Name() );
        
        HBufC* string = NULL;
        
        // Specific version is needed        
        if ( aWantedVersionTo.iMajor == aWantedVersionFrom.iMajor &&
             aWantedVersionTo.iMinor == aWantedVersionFrom.iMinor &&
             aWantedVersionTo.iBuild == aWantedVersionFrom.iBuild )          
            {
            string = StringLoader::LoadLC( R_SISXUI_REQUIRE_VER, *stringArray );
            }

        // Version or greater is needed
        else
            {
            string = StringLoader::LoadLC( R_SISXUI_REQUIRE_VER_OR_GREATER, *stringArray );
            }       

        TPtr ptr = string->Des();
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr ); 

        ShowErrorL( *string );
        
        CleanupStack::PopAndDestroy( 2); // stringArray, string
        }

    if ( !result )
        {        
        iResult = TSisxUIResult( EUiResultCancelled );
        }

    iShowingDialog = EFalse;
    return result;
    }
       
// -----------------------------------------------------------------------------
// CSisxUIHandler::DisplayUninstallL
// Displays a dialog at the start of the uninstallation with information about 
// the application to be uninstalled. The user may cancel the uninstallation at 
// this stage. This dialog must be shown in TrustedUI mode.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUIHandler::DisplayUninstallL( const Swi::CAppInfo& aAppInfo )
    {
    iShowingDialog = ETrue;

    TBool result( EFalse );

    CSisxUIAppInfo* appInfo = CSisxUIAppInfo::NewL( aAppInfo, EFalse );
    iPackages.Append( appInfo );

    HBufC* prompt = StringLoader::LoadLC( R_SISXUI_UNINSTALL_PROMPT, aAppInfo.AppName() );
    if ( iCommonDialogs->ShowConfirmationQueryL( *prompt, R_AVKON_SOFTKEYS_YES_NO__YES ) )
        {
        result = ETrue;
        }   
    else
        {
        iResult = TSisxUIResult( EUiResultCancelled );        
        // Make sure no notes are shown after this
        iShownFinalNote = ETrue;       
        }
    CleanupStack::PopAndDestroy( prompt );

    iShowingDialog = EFalse;
    return result;
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::DisplayTextL
// Displays a dialog containing text. This dialog has only a Continue button if 
// aOption is EInstFileTextOptionContinue, or Yes and No buttons otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUIHandler::DisplayTextL( const Swi::CAppInfo& aAppInfo, 
                                    Swi::TFileTextOption aOption,
                                    const TDesC& aText ) 
    {
    iShowingDialog = ETrue;

    HBufC* text = aText.AllocLC();
    CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL( *text );
    dlg->PrepareLC( R_SISXUI_TEXT_DIALOG );
    CleanupStack::PushL( dlg );    

    switch ( aOption )
        {
        case Swi::EInstFileTextOptionContinue:
            dlg->ButtonGroupContainer().SetCommandSetL( R_SISXUI_SOFTKEYS_CONTINUE_EMPTY__CONTINUE );
            break;            
            
        case Swi::EInstFileTextOptionSkipOneIfNo:
            dlg->ButtonGroupContainer().SetCommandSetL( R_AVKON_SOFTKEYS_YES_NO__YES );
            break;
            
        case Swi::EInstFileTextOptionAbortIfNo:
            dlg->ButtonGroupContainer().SetCommandSetL( R_AVKON_SOFTKEYS_OK_CANCEL__OK );
            break;
            
        case Swi::EInstFileTextOptionExitIfNo:
            dlg->ButtonGroupContainer().SetCommandSetL( R_AVKON_SOFTKEYS_OK_CANCEL__OK );
            break;

        default:
            dlg->ButtonGroupContainer().SetCommandSetL( R_AVKON_SOFTKEYS_OK_EMPTY__OK );
            break;            
        }
    
    dlg->QueryHeading()->SetTextL( aAppInfo.AppName() );        
    CleanupStack::Pop( dlg );    
    TBool result = dlg->RunLD();   
    
    CleanupStack::PopAndDestroy( text );    

    if ( !result )
        {
        iResult = TSisxUIResult( EUiResultCancelled );
        }
    
    iShowingDialog = EFalse;
    return result;    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::DisplayErrorL
// Displays a dialog showing an error message.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIHandler::DisplayErrorL( const Swi::CAppInfo& /*aAppInfo*/,
                                    Swi::TErrorDialog aType, 
                                    const TDesC& aParam ) 
    {
    FLOG_2( _L("CSisxUIHandler::DisplayErrorL: %d, %S"), aType, &aParam );    
    iResult = TSisxUIResult( aType );
    // Keep compiler happy
    (void)aParam;    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::DisplayDependencyBreakL
// Displays a dialog indicating that a component which is being uninstalled is 
// a dependency of another component installed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUIHandler::DisplayDependencyBreakL( const Swi::CAppInfo& /*aAppInfo*/,
                                               const RPointerArray<TDesC>& /*aComponents*/ )
    {
    iShowingDialog = ETrue;

    TBool result( ETrue );
    
    result = iCommonDialogs->ShowConfirmationQueryL( R_SISXUI_DEPENDENCY_BREAK );

    if ( !result )
        {
        iResult = TSisxUIResult( EUiResultCancelled );
        }
    
    iShowingDialog = EFalse;
    return result;
    }
		
// -----------------------------------------------------------------------------
// CSisxUIHandler::DisplayApplicationsInUseL
// Informs user that the applications are in use during an (un)installation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUIHandler::DisplayApplicationsInUseL( const Swi::CAppInfo& /*aAppInfo*/, 
                                                 const RPointerArray<TDesC>& /*aAppNames*/ )
    {
    iShowingDialog = ETrue;

    TBool result( ETrue );
    
    if ( iMode == EModeInstall )
        {
        result = iCommonDialogs->ShowConfirmationQueryL( R_SISXUI_CLOSE_MULTI_INST );        
        }
    else
        {
        result = iCommonDialogs->ShowConfirmationQueryL( R_SISXUI_CLOSE_MULTI_UNINST );        
        }    
    
    if ( !result )
        {
        TRAP_IGNORE( iCommonDialogs->CloseProgressDialogL() );
        iResult = TSisxUIResult( EUiResultCancelled );
        if ( iMode == EModeUninstall )
            {
        	// Display Wait dialog while Symbian SWI is cancelling uninstall process.	
            iCommonDialogs->ShowWaitDialogL( R_SISXUI_CANCELLING, 
                                             this, 
                                             R_AVKON_SOFTKEYS_EMPTY );
            iShowingWaitDialog = ETrue;
            }
        
        }

    iShowingDialog = EFalse;
    return result;
    }
	
// -----------------------------------------------------------------------------
// CSisxUIHandler::DisplayQuestionL
// Displays a generic question dialog with a Yes/No answer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUIHandler::DisplayQuestionL( const Swi::CAppInfo& /*aAppInfo*/, 
                                        Swi::TQuestionDialog aQuestion, 
                                        const TDesC& /*aDes*/ )
    {
    iShowingDialog = ETrue;

    TBool result( ETrue );
    
    if ( aQuestion == Swi::EQuestionIncompatible && !iIncompatibleShown )
        {
        // Incompatible application        
        result = iCommonDialogs->ShowConfirmationQueryL( R_SISXUI_INCOMP_PACKAGE, R_AVKON_SOFTKEYS_YES_NO__YES );
        // Make sure this question is not asked twice.
        iIncompatibleShown = ETrue;        
        }

    if ( !result )
        {
        iResult = TSisxUIResult( EUiResultCancelled );
        }

    iShowingDialog = EFalse;
    return result;    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::HandleDialogExitL
// Called when wait/progress dialog is about to be cancelled.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUIHandler::HandleDialogExitL( TInt aButtonId )
    {
    TBool result( EFalse );
    
    if ( aButtonId == EAknSoftkeyCancel )
        {        
        if ( iOcspInProgress )
            {
            result = ETrue;            
            // We need to cancel immediately            
            iResult = TSisxUIResult( EUiResultCancelled );
            iCancelObserver->UserCancelL( EFalse ); 
            }
        else
            {
            // User will still get confirmation
            iCancelRequested = ETrue;        
            }
        
        }    

    return result;    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::ProcessResult
// Process the result of the (un)installation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIHandler::ProcessResult( TInt aResult )
    {
    // In case we havent got a result from callbacks, we need to use one
    // from the engine complete code.
    if ( iResult.iResult == EUiResultNoResult )
        {
        switch ( aResult )
            {
            case KErrNone:
                 // This means that the result was ok
                iResult = TSisxUIResult( EUiResultOk );
                break;
                
            case KErrAccessDenied:
                iResult = TSisxUIResult( EUiResultAccessDenied );
                break;

            case KSWInstErrUserCancel:
                iResult = TSisxUIResult( EUiResultCancelled );
                break;

            case KSWInstErrFileCorrupted:
                iResult = TSisxUIResult( EUiResultFileCorrupt );
                break;  

            case KErrInvalidUpgrade:
                iResult = TSisxUIResult( EUiResultInvalidUpgrade );
                break; 

            case KErrNoMemory:
            case KErrDiskFull:
                iResult = TSisxUIResult( EUiResultInsufficientMemory );
                break;                

            case KErrCancel:
                iResult = TSisxUIResult( EUiResultCancelled );
                break;                
                
            default:
                if ( iMode == EModeInstall && !iIntegrityCheckDone )
                    {
                    // Sisx integrity check has not been done, report file
                    // corrupt
                    iResult = TSisxUIResult( EUiResultFileCorrupt );                    
                    }    
                else
                    {
                    // Report general error
                    iResult = TSisxUIResult( EUiResultGeneralError );
                    iResult.iDetailedErrorCode = aResult;
                    }                
                break;                  
            } 
        }    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::GetResult
// Return result of the installation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TSisxUIResult CSisxUIHandler::GetResult() const
    {
    return iResult;    
    }


// -----------------------------------------------------------------------------
// CSisxUIHandler::ShowFinalNoteL
// Show the final note.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
void CSisxUIHandler::ShowFinalNoteL()
    {
    switch ( iResult.iResult )
        {      
        case EUiResultOk:
            DisplayCompleteL();
            break;
            
        case EUiResultCancelled:
            DisplayCancelL();
            break;

        case EUiResultUnableToOverwrite:
        case EUiResultInvalidUpgrade:
        case EUiResultBlockingEclipsingFile:
            ShowErrorL( R_SISXUI_UPDATE_ERROR );
            break;
            
        case EUiResultAlreadyInRom:
            ShowErrorL( R_SISXUI_ALREADY_IN_ROM );            
            break;

        case EUiResultUnknownFile:
        case EUiResultFileNotFound:
        case EUiResultInvalidFileName:		
        case EUiResultFileCorrupt:
            ShowErrorL( R_SISXUI_FILE_CORRUPT );
            break;
            
        case EUiResultDiskNotPresent:
            ShowErrorL( R_SISXUI_DISK_NOT_PRESENT );
            break;
            
        case EUiResultCannotRead:  
            ShowErrorL( R_SISXUI_CANNOT_READ );
            break;
            
        case EUiResultCannotDelete:          	
            ShowErrorL( R_SISXUI_CANNOT_DELETE );
            break;

        case EUiResultInsufficientMemory:
        case EUiResultInsufficientSpaceOnDrive:
            ShowErrorL( R_SISXUI_NOT_ENOUGH_MEMORY );
            break;            
            
        case EUiResultCapabilitiesCannotBeGranted:
            ShowErrorL( R_SISXUI_CAPABILITIES_CANNOT_BE_GRANTED );
            break;            

        case EUiResultNoCertificate:
        case EUiResultCertificateValidationError:
        case EUiResultSignatureNotPresent:
        case EUiResultSignatureCouldNotBeValidated:
        case EUiResultNoCodeSigningExtension:
        case EUiResultNoSupportedPolicyExtension:  
        case EUiResultMandatorySignatureMissing:
            ShowErrorL( R_SISXUI_CERTIFICATE_ERROR );
            break;            
        case EUiResultSignatureSelfSigned:    
            ShowErrorL( R_SISXUI_SELFSIGNED_ERROR );              
            break; 
        case EUiResultCertificateExpired:
            ShowErrorL( R_SISXUI_CERT_EXPIRED );
            break;
            
        case EUiResultCertificateValidInFuture:
            ShowErrorL( R_SISXUI_CERT_DATE_INVALID );
            break;

        case EUiResultInvalidRevocationServerResponse:
        case EUiResultInvalidRevocationServerUrl:
        case EUiResultResponseSignatureValidationFailure:
            ShowErrorL( R_SISXUI_OCSP_SETTINGS_ERROR );
            break;

        case EUiResultUnableToObtainCertificateStatus:  
        case EUiResultInvalidCertificateStatusInformation:
        case EUiResultCertificateStatusIsUnknown:
        case EUiResultCertificateStatusIsUnknownSelfSigned:
            ShowErrorL( R_SISXUI_OCSP_GENERAL_ERROR );
            break;            
            
        case EUiResultCertificateStatusIsRevoked:   
            ShowErrorL( R_SISXUI_OCSP_REVOKED );
            break;

        case EUiResultAccessDenied:
        case EUiResultOSExeViolation:
            ShowErrorL( R_SISXUI_ACCESS_DENIED );
            break;            

        case EUiResultMissingBasePackage:
            ShowErrorL( R_SISXUI_BASE_MISSING );
            break;    

        case EUiResultCannotUninstallPartialUpgrade:
            ShowErrorL( R_SISXUI_UNINSTALL_PARTIAL_ERROR );
            break; 

        case EUiResultSIDViolation:
        case EUiResultVIDViolation:
        case EUiResultUIDPackageViolation:
        case EUiResultSIDMismatch:
            ShowErrorL( R_SISXUI_PROTECTED_ERROR );
            break;            

        case EUiResultConstraintsExceeded:
            ShowErrorL( R_SISXUI_CONSTRAINED_ERROR );
            break;            
            
            //These should not come here
        case EUiResultMissingDependency:			
        case EUiResultRequireVer:		
        case EUiResultRequireVerOrGreater:
            //
        case EUiResultGeneralError:
        case EUiResultNoResult:
        default:            
            if ( iMode == EModeInstall )
                {                
                ShowErrorL( R_SISXUI_INSTALL_ERROR );
                }
            else
                {
                ShowErrorL( R_SISXUI_UNINSTALL_ERROR );
                }         
            break;            
        }    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::ProcessedPackages
// Return the package information about this install/uninstall.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIHandler::ProcessedPackages( RPointerArray<CSisxUIAppInfo>& aPackages )
    {
    for ( TInt index = 0; index < iPackages.Count(); index++ )
        {
        aPackages.Append( iPackages[index] );        
        }    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::InstallMode
// Return the install mode of this instance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSisxUIHandler::TInstallMode CSisxUIHandler::InstallMode() const
    {
    return iMode;    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::SetOcspProcedure
// Sets the ocsp procedure for the installation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIHandler::SetOcspProcedure( const TSisxUIOcspProc& aOcspProc )
    {
    iOcspProc = aOcspProc;
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::IsShowingDialog
// Indicates if a dialog is shown to user and expecting some user input.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
TBool CSisxUIHandler::IsShowingDialog()
    {
    return iShowingDialog;    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::ShowCancellingL
// Shows a cancelling wait dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIHandler::ShowCancellingL()
    {
    // We need to show this only in case of ocsp.
    if ( iOcspInProgress )
        {        
        // Show cancelling wait dialog    
        iCommonDialogs->ShowDelayedWaitDialogL( R_SISXUI_CANCELLING, 
                                                this, 
                                                R_AVKON_SOFTKEYS_EMPTY );
        }    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::DoNotShowFinalNote
// Sets the UI not to show final note.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIHandler::DoNotShowFinalNote()
    {
    iShownFinalNote = ETrue;    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::ResetResult
// Resets the result to no result.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIHandler::ResetResult()
    {
    iResult.iResult = EUiResultNoResult;    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::SetFileForDrm
// Sets the file handle for showing drm details.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIHandler::SetFileForDrm( RFile& aFile )  
    {
    iDrmFile.Duplicate( aFile );
    iIsDrmFileSet = ETrue;    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::SetAllowUntrusted
// Sets the policy for allowing the installation of untrusted packages.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIHandler::SetAllowUntrusted( TBool aAllowUntrusted )
    {
    iAllowUntrusted = aAllowUntrusted;    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::DisplayCancelL
// Displays a cancel note.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIHandler::DisplayCancelL()
    {
    if ( !iShownFinalNote )
        {  
        // Make sure that no wait or progress dialog is running
        Finalize();
  
        HBufC* string = NULL;        
        if ( iMode == EModeInstall )
            {            
            string = StringLoader::LoadLC( R_SISXUI_INSTALL_CANCELLED );
            }
        else
            {
            string = StringLoader::LoadLC( R_SISXUI_UNINSTALL_CANCELLED );
            }
        
        CAknInformationNote* note = new (ELeave) CAknInformationNote( ETrue );
        note->ExecuteLD( *string );
        CleanupStack::PopAndDestroy( string ); 
        }

    iShownFinalNote = ETrue;    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::DisplayCompleteL
// Displays a complete note.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIHandler::DisplayCompleteL()
    {
    if ( !iShownFinalNote )
        {        
        // Make sure that no wait or progress dialog is running
        Finalize();        

        HBufC* string = NULL;        
        if ( iMode == EModeInstall )
            {            
            string = StringLoader::LoadLC( R_SISXUI_INSTALL_COMPLETED );
            }
        else
            {
            string = StringLoader::LoadLC( R_SISXUI_UNINSTALL_COMPLETED );
            }
        
        CAknConfirmationNote* note = new (ELeave) CAknConfirmationNote( ETrue );
        note->ExecuteLD( *string );
        CleanupStack::PopAndDestroy( string ); 
        }   

    iShownFinalNote = ETrue;    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::ShowErrorL
// Convenience function for showing a Sisx specific error.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIHandler::ShowErrorL( TInt aResourceString )
    {
    if ( !iShownFinalNote )
        {
        Finalize();

        HBufC* errorString = StringLoader::LoadLC( aResourceString );

        RFs fs;
        User::LeaveIfError( fs.Connect() );
        CleanupClosePushL( fs );

        CAknErrorNote* note = new (ELeave) CAknErrorNote( ETrue );

        TEntry entry;
        TInt status = fs.Entry( KSwiTempFile, entry );

        if ( status == KErrNone )
            {
            // File exists. Status code will be appended to the message
            HBufC* detailedMessage = HBufC::NewLC( errorString->Length() + KSpaceForErrorCode );
            if( iResult.iResult != EUiResultGeneralError ) {
                detailedMessage->Des().Format( KDetailedError, errorString, iResult.iResult );
            } else {
                detailedMessage->Des().Format( KDetailedError, errorString,
                        iResult.iDetailedErrorCode );
            }
            note->ExecuteLD( *detailedMessage );
            CleanupStack::PopAndDestroy( detailedMessage );
            }
        else
            {
            // File doesn't exist. Show the message without status code
            note->ExecuteLD( *errorString );
            }

        CleanupStack::PopAndDestroy( 2 );  // fs, errorString

        // No notes after error
        iShownFinalNote = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::ShowErrorL
// Convenience function for showing a Sisx specific error.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIHandler::ShowErrorL( const TDesC& aDes )
    {
    if ( !iShownFinalNote )
        { 
        Finalize();    
        
        CAknErrorNote* note = new (ELeave) CAknErrorNote( ETrue );
        note->ExecuteLD( aDes );
        
        // No notes after error
        iShownFinalNote = ETrue;   
        }    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::AddCapabilityL
// Adds the given capability string to the descriptor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIHandler::AddCapabilityL( HBufC*& aString, TInt aResourceId )
    {
    HBufC* tmpString = StringLoader::LoadLC( aResourceId );
    HBufC* newString;
    
    if ( aString->Length() > 0 )
        {
        newString = HBufC::NewL( tmpString->Length() + aString->Length() + 1 ); // + \n
        
        TPtr ptr( newString->Des() );
        ptr += *aString;        
        ptr += KNextLine;        
        ptr += *tmpString;
        }
    else
        {
        newString = HBufC::NewL( tmpString->Length() );

        TPtr ptr( newString->Des() );                
        ptr += *tmpString;
        }

    CleanupStack::PopAndDestroy( tmpString );
    CleanupStack::PopAndDestroy( aString );  
  
    aString = newString;    
    CleanupStack::PushL( aString );    
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::Finalize
// Finalizes the installation by closing all open dialogs.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIHandler::Finalize()
    {
    TRAP_IGNORE( iCommonDialogs->CloseWaitDialogL() );
    TRAP_IGNORE( iCommonDialogs->CloseProgressDialogL() );   
    // No notes after this
    iShownFinalNote = ETrue;        
    }

// -----------------------------------------------------------------------------
// CSisxUIHandler::ShowCapabilitiesHelp
// Callback used in message query. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSisxUIHandler::ShowCapabilitiesHelp( TAny* aPtr )
    {
    //TRAP_IGNORE( 
    //    reinterpret_cast<CSisxUIHandler*>(aPtr)->iCommonDialogs->LaunchHelpL( KAM_HLP_INSTALL_CAPAB )    
    //    );    

    return KErrNone;    
    }


//  End of File  
